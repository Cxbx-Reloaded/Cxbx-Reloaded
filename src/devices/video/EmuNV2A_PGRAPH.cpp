// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ******************************************************************
// *
// *  This file is part of the Cxbx project.
// *
// *  Cxbx and Cxbe are free software; you can redistribute them
// *  and/or modify them under the terms of the GNU General Public
// *  License as published by the Free Software Foundation; either
// *  version 2 of the license, or (at your option) any later version.
// *
// *  This program is distributed in the hope that it will be useful,
// *  but WITHOUT ANY WARRANTY; without even the implied warranty of
// *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// *  GNU General Public License for more details.
// *
// *  You should have recieved a copy of the GNU General Public License
// *  along with this program; see the file COPYING.
// *  If not, write to the Free Software Foundation, Inc.,
// *  59 Temple Place - Suite 330, Bostom, MA 02111-1307, USA.
// *
// *  This file is heavily based on code from XQEMU
// *  https://github.com/xqemu/xqemu/blob/master/hw/xbox/nv2a/nv2a_pgraph.c
// *  Copyright (c) 2012 espes
// *  Copyright (c) 2015 Jannik Vogel
// *  Copyright (c) 2018 Matt Borgerson
// *
// *  Contributions for Cxbx-Reloaded
// *  Copyright (c) 2017-2018 Luke Usher <luke.usher@outlook.com>
// *  Copyright (c) 2018 Patrick van Logchem <pvanlogchem@gmail.com>
// *
// *  All rights reserved
// *
// ******************************************************************

#include "core\hle\D3D8\XbD3D8Types.h" // For X_D3DFORMAT
#include "core\hle\D3D8\XbVertexShader.h"

// FIXME
#define qemu_mutex_lock_iothread()
#define qemu_mutex_unlock_iothread()

// Xbox uses 4 KiB pages
#define TARGET_PAGE_BITS 12
#define TARGET_PAGE_SIZE (1 << TARGET_PAGE_BITS)
#define TARGET_PAGE_MASK ~(TARGET_PAGE_SIZE - 1)
#define TARGET_PAGE_ALIGN(addr) (((addr) + TARGET_PAGE_SIZE - 1) & TARGET_PAGE_MASK)

static const GLenum pgraph_texture_min_filter_map[] = {
    0,
    GL_NEAREST,
    GL_LINEAR,
    GL_NEAREST_MIPMAP_NEAREST,
    GL_LINEAR_MIPMAP_NEAREST,
    GL_NEAREST_MIPMAP_LINEAR,
    GL_LINEAR_MIPMAP_LINEAR,
    GL_LINEAR, /* TODO: Convolution filter... */
};

static const GLenum pgraph_texture_mag_filter_map[] = {
    0,
    GL_NEAREST,
    GL_LINEAR,
    0,
    GL_LINEAR /* TODO: Convolution filter... */
};

static const GLenum pgraph_texture_addr_map[] = {
    0,
    GL_REPEAT,
    GL_MIRRORED_REPEAT,
    GL_CLAMP_TO_EDGE,
    GL_CLAMP_TO_BORDER,
    // GL_CLAMP
};

static const GLenum pgraph_blend_factor_map[] = {
    GL_ZERO,
    GL_ONE,
    GL_SRC_COLOR,
    GL_ONE_MINUS_SRC_COLOR,
    GL_SRC_ALPHA,
    GL_ONE_MINUS_SRC_ALPHA,
    GL_DST_ALPHA,
    GL_ONE_MINUS_DST_ALPHA,
    GL_DST_COLOR,
    GL_ONE_MINUS_DST_COLOR,
    GL_SRC_ALPHA_SATURATE,
    0,
    GL_CONSTANT_COLOR,
    GL_ONE_MINUS_CONSTANT_COLOR,
    GL_CONSTANT_ALPHA,
    GL_ONE_MINUS_CONSTANT_ALPHA,
};

static const GLenum pgraph_blend_equation_map[] = {
    GL_FUNC_SUBTRACT,
    GL_FUNC_REVERSE_SUBTRACT,
    GL_FUNC_ADD,
    GL_MIN,
    GL_MAX,
    GL_FUNC_REVERSE_SUBTRACT,
    GL_FUNC_ADD,
};

static const GLenum pgraph_blend_logicop_map[] = {
    GL_CLEAR,
    GL_AND,
    GL_AND_REVERSE,
    GL_COPY,
    GL_AND_INVERTED,
    GL_NOOP,
    GL_XOR,
    GL_OR,
    GL_NOR,
    GL_EQUIV,
    GL_INVERT,
    GL_OR_REVERSE,
    GL_COPY_INVERTED,
    GL_OR_INVERTED,
    GL_NAND,
    GL_SET,
};

static const GLenum pgraph_cull_face_map[] = {
    0,
    GL_FRONT,
    GL_BACK,
    GL_FRONT_AND_BACK
};

static const GLenum pgraph_depth_func_map[] = {
    GL_NEVER,
    GL_LESS,
    GL_EQUAL,
    GL_LEQUAL,
    GL_GREATER,
    GL_NOTEQUAL,
    GL_GEQUAL,
    GL_ALWAYS,
};

static const GLenum pgraph_stencil_func_map[] = {
    GL_NEVER,
    GL_LESS,
    GL_EQUAL,
    GL_LEQUAL,
    GL_GREATER,
    GL_NOTEQUAL,
    GL_GEQUAL,
    GL_ALWAYS,
};

static const GLenum pgraph_stencil_op_map[] = {
    0,
    GL_KEEP,
    GL_ZERO,
    GL_REPLACE,
    GL_INCR,
    GL_DECR,
    GL_INVERT,
    GL_INCR_WRAP,
    GL_DECR_WRAP,
};

enum FormatEncoding {
    linear = 0,
    swizzled, // for all NV097_SET_TEXTURE_FORMAT_*_SZ_*
    compressed // for all NV097_SET_TEXTURE_FORMAT_*_DXT*
};

typedef struct ColorFormatInfo {
    unsigned int bytes_per_pixel; // Derived from the total number of channel bits
    FormatEncoding encoding;
    GLint gl_internal_format;
    GLenum gl_format; // == 0 for compressed formats
    GLenum gl_type;
    GLint *gl_swizzle_mask; // == nullptr when gl_internal_format, gl_format and gl_type are sufficient
} ColorFormatInfo;

// Resulting gl_internal_format, gl_format and gl_type values, for formats handled by convert_texture_data()
#define GL_CONVERT_TEXTURE_DATA_RESULTING_FORMAT GL_RGBA8, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV

static GLint gl_swizzle_mask_0RG1[4] = { GL_ZERO, GL_RED, GL_GREEN, GL_ONE };
static GLint gl_swizzle_mask_111R[4] = { GL_ONE, GL_ONE, GL_ONE, GL_RED };
static GLint gl_swizzle_mask_ARGB[4] = { GL_ALPHA, GL_RED, GL_GREEN, GL_BLUE };
static GLint gl_swizzle_mask_BGRA[4] = { GL_BLUE, GL_GREEN, GL_RED, GL_ALPHA };
static GLint gl_swizzle_mask_GGGR[4] = { GL_GREEN, GL_GREEN, GL_GREEN, GL_RED };
static GLint gl_swizzle_mask_R0G1[4] = { GL_RED, GL_ZERO, GL_GREEN, GL_ONE };
static GLint gl_swizzle_mask_RRR1[4] = { GL_RED, GL_RED, GL_RED, GL_ONE };
static GLint gl_swizzle_mask_RRRG[4] = { GL_RED, GL_RED, GL_RED, GL_GREEN };
static GLint gl_swizzle_mask_RRRR[4] = { GL_RED, GL_RED, GL_RED, GL_RED };

// Note : Avoid designated initializers to facilitate C++ builds
static const ColorFormatInfo kelvin_color_format_map[256] = {
    //0x00 [NV097_SET_TEXTURE_FORMAT_COLOR_SZ_Y8] =
        {1, swizzled, GL_R8, GL_RED, GL_UNSIGNED_BYTE,
         gl_swizzle_mask_RRR1},
    //0x01 [NV097_SET_TEXTURE_FORMAT_COLOR_SZ_AY8] =
        {1, swizzled, GL_R8, GL_RED, GL_UNSIGNED_BYTE,
         gl_swizzle_mask_RRRR},
    //0x02 [NV097_SET_TEXTURE_FORMAT_COLOR_SZ_A1R5G5B5] =
        {2, swizzled, GL_RGB5_A1, GL_BGRA, GL_UNSIGNED_SHORT_1_5_5_5_REV},
    //0x03 [NV097_SET_TEXTURE_FORMAT_COLOR_SZ_X1R5G5B5] =
        {2, swizzled, GL_RGB5, GL_BGRA, GL_UNSIGNED_SHORT_1_5_5_5_REV},
    //0x04 [NV097_SET_TEXTURE_FORMAT_COLOR_SZ_A4R4G4B4] =
        {2, swizzled, GL_RGBA4, GL_BGRA, GL_UNSIGNED_SHORT_4_4_4_4_REV},
    //0x05 [NV097_SET_TEXTURE_FORMAT_COLOR_SZ_R5G6B5] =
        {2, swizzled, GL_RGB565, GL_RGB, GL_UNSIGNED_SHORT_5_6_5},
    //0x06 [NV097_SET_TEXTURE_FORMAT_COLOR_SZ_A8R8G8B8] =
        {4, swizzled, GL_RGBA8, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV},
    //0x07 [NV097_SET_TEXTURE_FORMAT_COLOR_SZ_X8R8G8B8] =
        {4, swizzled, GL_RGB8, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV},
    //0x08 [?] =
        {},
    //0x09 [?] =
        {},
    //0x0A [?] =
        {},

    /* paletted texture */
    //0x0B [NV097_SET_TEXTURE_FORMAT_COLOR_SZ_I8_A8R8G8B8] = // See convert_texture_data
        {1, swizzled, GL_CONVERT_TEXTURE_DATA_RESULTING_FORMAT},

    //0x0C [NV097_SET_TEXTURE_FORMAT_COLOR_L_DXT1_A1R5G5B5] =
        {4, compressed, GL_COMPRESSED_RGBA_S3TC_DXT1_EXT, 0, GL_RGBA},
    //0x0D [?] =
        {},
    //0x0E [NV097_SET_TEXTURE_FORMAT_COLOR_L_DXT23_A8R8G8B8] =
        {4, compressed, GL_COMPRESSED_RGBA_S3TC_DXT3_EXT, 0, GL_RGBA},
    //0x0F [NV097_SET_TEXTURE_FORMAT_COLOR_L_DXT45_A8R8G8B8] =
        {4, compressed, GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, 0, GL_RGBA},
    //0x10 [NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A1R5G5B5] =
        {2, linear, GL_RGB5_A1, GL_BGRA, GL_UNSIGNED_SHORT_1_5_5_5_REV},
    //0x11 [NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_R5G6B5] =
        {2, linear, GL_RGB565, GL_RGB, GL_UNSIGNED_SHORT_5_6_5},
    //0x12 [NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A8R8G8B8] =
        {4, linear, GL_RGBA8, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV},
    //0x13 [NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_Y8] =
        {1, linear, GL_R8, GL_RED, GL_UNSIGNED_BYTE,
         gl_swizzle_mask_RRR1},
    //0x14 [NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_SY8] =
        {1, linear, GL_R8_SNORM, GL_RED, GL_BYTE,
         gl_swizzle_mask_RRR1}, // TODO : Verify
    //0x15 [NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_X7SY9] = // See convert_texture_data FIXME
        {2, linear, GL_CONVERT_TEXTURE_DATA_RESULTING_FORMAT}, // TODO : Verify
    //0x16 [NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_R8B8] =
        {2, linear, GL_RG8, GL_RG, GL_UNSIGNED_BYTE,
         gl_swizzle_mask_R0G1}, // TODO : Verify
    //0x17 [NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_G8B8] =
        {2, linear, GL_RG8, GL_RG, GL_UNSIGNED_BYTE,
         gl_swizzle_mask_0RG1}, // TODO : Verify
    //0x18 [NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_SG8SB8] =
        {2, linear, GL_RG8_SNORM, GL_RG, GL_BYTE,
         gl_swizzle_mask_0RG1}, // TODO : Verify

    //0x19 [NV097_SET_TEXTURE_FORMAT_COLOR_SZ_A8] =
        {1, swizzled, GL_R8, GL_RED, GL_UNSIGNED_BYTE,
         gl_swizzle_mask_111R},
    //0x1A [NV097_SET_TEXTURE_FORMAT_COLOR_SZ_A8Y8] =
        {2, swizzled, GL_RG8, GL_RG, GL_UNSIGNED_BYTE,
         gl_swizzle_mask_GGGR},
    //0x1B [NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_AY8] =
        {1, linear, GL_R8, GL_RED, GL_UNSIGNED_BYTE,
         gl_swizzle_mask_RRRR},
    //0x1C [NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_X1R5G5B5] =
        {2, linear, GL_RGB5, GL_BGRA, GL_UNSIGNED_SHORT_1_5_5_5_REV},
    //0x1D [NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A4R4G4B4] =
        {2, linear, GL_RGBA4, GL_BGRA, GL_UNSIGNED_SHORT_4_4_4_4_REV}, // TODO : Verify this is truely linear
    //0x1E [NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_X8R8G8B8] =
        {4, linear, GL_RGB8, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV},
    //0x1F [NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A8] =
        {1, linear, GL_R8, GL_RED, GL_UNSIGNED_BYTE,
         gl_swizzle_mask_111R},
    //0x20 [NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A8Y8] =
        {2, linear, GL_RG8, GL_RG, GL_UNSIGNED_BYTE,
         gl_swizzle_mask_GGGR},
    //0x21 [?] =
        {},
    //0x22 [?] =
        {},
    //0x23 [?] =
        {},
    //0x24 [NV097_SET_TEXTURE_FORMAT_COLOR_LC_IMAGE_CR8YB8CB8YA8] = // See convert_texture_data calling ____UYVYToARGBRow_C
        {2, linear, GL_CONVERT_TEXTURE_DATA_RESULTING_FORMAT}, // TODO : Verify
    //0x25 [NV097_SET_TEXTURE_FORMAT_COLOR_LC_IMAGE_YB8CR8YA8CB8] = // See convert_texture_data calling ____YUY2ToARGBRow_C
        {2, linear, GL_CONVERT_TEXTURE_DATA_RESULTING_FORMAT}, // TODO : Verify
    //0x26 [NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A8CR8CB8Y8] = // See convert_texture_data FIXME
        {2, linear, GL_CONVERT_TEXTURE_DATA_RESULTING_FORMAT}, // TODO : Verify

    //0x27 [NV097_SET_TEXTURE_FORMAT_COLOR_SZ_R6G5B5] = // See convert_texture_data calling __R6G5B5ToARGBRow_C
        {2, swizzled, GL_CONVERT_TEXTURE_DATA_RESULTING_FORMAT}, // TODO : Verify
    //0x28 [NV097_SET_TEXTURE_FORMAT_COLOR_SZ_G8B8] =
        {2, swizzled, GL_RG8, GL_RG, GL_UNSIGNED_BYTE,
         gl_swizzle_mask_0RG1},
    //0x29 [NV097_SET_TEXTURE_FORMAT_COLOR_SZ_R8B8] =
        {2, swizzled, GL_RG8, GL_RG, GL_UNSIGNED_BYTE,
         gl_swizzle_mask_R0G1},
    //0x2A [NV097_SET_TEXTURE_FORMAT_COLOR_SZ_DEPTH_X8_Y24_FIXED] =
        {4, swizzled, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8}, // TODO : Verify
    //0x2B [NV097_SET_TEXTURE_FORMAT_COLOR_SZ_DEPTH_X8_Y24_FLOAT] =
        {4, swizzled, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL, GL_FLOAT_32_UNSIGNED_INT_24_8_REV}, // TODO : Verify
    //0x2C [NV097_SET_TEXTURE_FORMAT_COLOR_SZ_DEPTH_Y16_FIXED] =
        {2, swizzled, GL_DEPTH_COMPONENT16, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT}, // TODO : Verify
    //0x2D [NV097_SET_TEXTURE_FORMAT_COLOR_SZ_DEPTH_Y16_FLOAT] =
        {2, swizzled, GL_DEPTH_COMPONENT16, GL_DEPTH_COMPONENT, GL_FLOAT}, // TODO : Verify


    /* TODO: format conversion */
    //0x2E [NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_DEPTH_X8_Y24_FIXED] =
        {4, linear, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8},
    //0x2F [?] =
        {},
    //0x30 [NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_DEPTH_Y16_FIXED] =
        {2, linear, GL_DEPTH_COMPONENT16, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT},
    //0x31 [NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_DEPTH_Y16_FLOAT] =
        {2, linear, GL_DEPTH_COMPONENT16, GL_DEPTH_COMPONENT, GL_FLOAT}, // TODO : Verify
    //0x32 [NV097_SET_TEXTURE_FORMAT_COLOR_SZ_Y16] =
        {2, swizzled, GL_R16, GL_RED, GL_UNSIGNED_SHORT, // TODO : Verify
         gl_swizzle_mask_RRR1},
    //0x33 [NV097_SET_TEXTURE_FORMAT_COLOR_SZ_YB16YA16] =
        {4, swizzled, GL_RG16, GL_RG, GL_UNSIGNED_SHORT, // TODO : Verify
         gl_swizzle_mask_RRRG},
    //0x34 [NV097_SET_TEXTURE_FORMAT_COLOR_LC_IMAGE_A4V6YB6A4U6YA6] = // TODO : handle in convert_texture_data
        {2, linear, GL_CONVERT_TEXTURE_DATA_RESULTING_FORMAT}, // TODO : Verify
    //0x35 [NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_Y16] =
        {2, linear, GL_R16, GL_RED, GL_UNSIGNED_SHORT,
         gl_swizzle_mask_RRR1},
    //0x36 [NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_YB16YA16] =
        {4, linear, GL_RG16, GL_RG, GL_UNSIGNED_SHORT, // TODO : Verify
         gl_swizzle_mask_RRRG},
    //0x37 [NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_R6G5B5] = // See convert_texture_data calling __R6G5B5ToARGBRow_C
        {2, linear, GL_CONVERT_TEXTURE_DATA_RESULTING_FORMAT}, // TODO : Verify
    //0x38 [NV097_SET_TEXTURE_FORMAT_COLOR_SZ_R5G5B5A1] =
        {2, swizzled, GL_RGB5_A1, GL_RGBA, GL_UNSIGNED_SHORT_5_5_5_1}, // TODO : Verify
    //0x39 [NV097_SET_TEXTURE_FORMAT_COLOR_SZ_R4G4B4A4] =
        {2, swizzled, GL_RGBA, GL_RGBA, GL_UNSIGNED_SHORT_4_4_4_4}, // TODO : Verify
    //0x3A [NV097_SET_TEXTURE_FORMAT_COLOR_SZ_A8B8G8R8] =
        {4, swizzled, GL_RGBA8, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV}, // TODO : Verify
    //0x3B [NV097_SET_TEXTURE_FORMAT_COLOR_SZ_B8G8R8A8] =
        {4, swizzled, GL_RGBA8, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8}, // TODO : Verify

    //0x3C [NV097_SET_TEXTURE_FORMAT_COLOR_SZ_R8G8B8A8] =
        {4, swizzled, GL_RGBA8, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8},
    //0x3D [NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_R5G5B5A1] =
        {2, linear, GL_RGB5_A1, GL_RGBA, GL_UNSIGNED_SHORT_5_5_5_1}, // TODO : Verify
    //0x3E [NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_R4G4B4A4] =
        {2, linear, GL_RGBA, GL_RGBA, GL_UNSIGNED_SHORT_4_4_4_4}, // TODO : Verify

    //0x3F [NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A8B8G8R8] =
        {4, linear, GL_RGBA8, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV}, // TODO : Verify
    //0x40 [NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_B8G8R8A8] =
        {4, linear, GL_RGBA8, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8}, // TODO : Verify
    //0x41 [NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_R8G8B8A8] =
        {4, linear, GL_RGBA8, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8}, // TODO : Verify
};

typedef struct SurfaceColorFormatInfo {
    unsigned int bytes_per_pixel;
    GLint gl_internal_format;
    GLenum gl_format;
    GLenum gl_type;
} SurfaceColorFormatInfo;

// Note : Avoid designated initializers to facilitate C++ builds
static const SurfaceColorFormatInfo kelvin_surface_color_format_map[16] = {
    //0x00 [?] = 
        {},
    //0x01 [NV097_SET_SURFACE_FORMAT_COLOR_LE_X1R5G5B5_Z1R5G5B5] =
        {2, GL_RGB5_A1, GL_BGRA, GL_UNSIGNED_SHORT_1_5_5_5_REV},
    //0x02 [NV097_SET_SURFACE_FORMAT_COLOR_LE_X1R5G5B5_O1R5G5B5] =
        {},
    //0x03 [NV097_SET_SURFACE_FORMAT_COLOR_LE_R5G6B5] =
        {2, GL_RGB565, GL_RGB, GL_UNSIGNED_SHORT_5_6_5},
    //0x04 [NV097_SET_SURFACE_FORMAT_COLOR_LE_X8R8G8B8_Z8R8G8B8] =
        {4, GL_RGBA8, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV},
    //0x05 [NV097_SET_SURFACE_FORMAT_COLOR_LE_X8R8G8B8_O8R8G8B8] =
        {},
    //0x06 [NV097_SET_SURFACE_FORMAT_COLOR_LE_X1A7R8G8B8_Z1A7R8G8B8] =
        {},
    //0x07 [NV097_SET_SURFACE_FORMAT_COLOR_LE_X1A7R8G8B8_O1A7R8G8B8] =
        {},
    //0x08 [NV097_SET_SURFACE_FORMAT_COLOR_LE_A8R8G8B8] =
        {4, GL_RGBA8, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV},
    //0x09 [NV097_SET_SURFACE_FORMAT_COLOR_LE_B8] =
        {}, // TODO : {1, GL_R8, GL_RED, GL_UNSIGNED_BYTE}, // PatrickvL guesstimate
    //0x0A [NV097_SET_SURFACE_FORMAT_COLOR_LE_G8B8] =
        {}, // TODO : {2, GL_RG8, GL_RG, GL_UNSIGNED_BYTE}, // PatrickvL guesstimate
    //0x0B [?] = 
        {},
    //0x0C [?] = 
        {},
    //0x0D [?] = 
        {},
    //0x0E [?] = 
        {},
    //0x0F [?] = 
        {}
};

void (*pgraph_draw_arrays)(NV2AState *d);
void (*pgraph_draw_inline_buffer)(NV2AState *d);
void (*pgraph_draw_inline_array)(NV2AState *d);
void (*pgraph_draw_inline_elements)(NV2AState *d);
void (*pgraph_draw_state_update)(NV2AState *d);
void (*pgraph_draw_clear)(NV2AState *d);

//static void pgraph_set_context_user(NV2AState *d, uint32_t value);
//void pgraph_handle_method(NV2AState *d, unsigned int subchannel, unsigned int method, uint32_t parameter);
static void pgraph_log_method(unsigned int subchannel, unsigned int graphics_class, unsigned int method, uint32_t parameter);
static void pgraph_allocate_inline_buffer_vertices(PGRAPHState *pg, unsigned int attr);
static void pgraph_finish_inline_buffer_vertex(PGRAPHState *pg);
static void pgraph_update_shader_constants(PGRAPHState *pg, ShaderBinding *binding, bool binding_changed, bool vertex_program, bool fixed_function);
static void pgraph_bind_shaders(PGRAPHState *pg);
static bool pgraph_get_framebuffer_dirty(PGRAPHState *pg);
static bool pgraph_get_color_write_enabled(PGRAPHState *pg);
static bool pgraph_get_zeta_write_enabled(PGRAPHState *pg);
static void pgraph_set_surface_dirty(PGRAPHState *pg, bool color, bool zeta);
static void pgraph_update_surface_part(NV2AState *d, bool upload, bool color);
static void pgraph_update_surface(NV2AState *d, bool upload, bool color_write, bool zeta_write);
static void pgraph_bind_textures(NV2AState *d);
static void pgraph_apply_anti_aliasing_factor(PGRAPHState *pg, unsigned int *width, unsigned int *height);
static void pgraph_get_surface_dimensions(PGRAPHState *pg, unsigned int *width, unsigned int *height);
static void pgraph_update_memory_buffer(NV2AState *d, hwaddr addr, hwaddr size, bool f);
static void pgraph_bind_vertex_attributes(NV2AState *d, unsigned int num_elements, bool inline_data, unsigned int inline_stride);
static unsigned int pgraph_bind_inline_array(NV2AState *d);

static float convert_f16_to_float(uint16_t f16);
static float convert_f24_to_float(uint32_t f24);
static uint8_t* convert_texture_data(const unsigned int color_format, const uint8_t *data, const uint8_t *palette_data, const unsigned int width, const unsigned int height, const unsigned int depth, const unsigned int row_pitch, const unsigned int slice_pitch);
static int upload_gl_texture(GLenum gl_target, const TextureShape s, const uint8_t *texture_data, const uint8_t *palette_data);
static TextureBinding* generate_texture(const TextureShape s, const uint8_t *texture_data, const uint8_t *palette_data);
static guint texture_key_hash(gconstpointer key);
static gboolean texture_key_equal(gconstpointer a, gconstpointer b);
static gpointer texture_key_retrieve(gpointer key, gpointer user_data, GError **error);
static void texture_key_destroy(gpointer data);
static void texture_binding_destroy(gpointer data);
static guint shader_hash(gconstpointer key);
static gboolean shader_equal(gconstpointer a, gconstpointer b);
static unsigned int kelvin_map_stencil_op(uint32_t parameter);
static unsigned int kelvin_map_polygon_mode(uint32_t parameter);
static unsigned int kelvin_map_texgen(uint32_t parameter, unsigned int channel);
static uint64_t fnv_hash(const uint8_t *data, size_t len);
static uint64_t fast_hash(const uint8_t *data, size_t len, unsigned int samples);

/* PGRAPH - accelerated 2d/3d drawing engine */

static uint32_t pgraph_rdi_read(PGRAPHState *pg,
                                unsigned int select, unsigned int address)
{
    uint32_t r = 0;
    switch(select) {
    case RDI_INDEX_VTX_CONSTANTS0:
    case RDI_INDEX_VTX_CONSTANTS1:
        assert((address / 4) < NV2A_VERTEXSHADER_CONSTANTS);
        r = pg->vsh_constants[address / 4][3 - address % 4];
        break;
    default:
        fprintf(stderr, "nv2a: unknown rdi read select 0x%x address 0x%x\n",
                select, address);
        assert(false);
        break;
    }
    return r;
}

static void pgraph_rdi_write(PGRAPHState *pg,
                             unsigned int select, unsigned int address,
                             uint32_t val)
{
    switch(select) {
    case RDI_INDEX_VTX_CONSTANTS0:
    case RDI_INDEX_VTX_CONSTANTS1:
        assert(false); /* Untested */
        assert((address / 4) < NV2A_VERTEXSHADER_CONSTANTS);
        pg->vsh_constants_dirty[address / 4] |=
            (val != pg->vsh_constants[address / 4][3 - address % 4]);
        pg->vsh_constants[address / 4][3 - address % 4] = val;
        break;
    default:
        NV2A_DPRINTF("unknown rdi write select 0x%x, address 0x%x, val 0x%08x\n",
                     select, address, val);
        break;
    }
}

DEVICE_READ32(PGRAPH)
{
    qemu_mutex_lock(&d->pgraph.pgraph_lock);

    PGRAPHState *pg = &d->pgraph;
    DEVICE_READ32_SWITCH() {
    case NV_PGRAPH_INTR:
        result = pg->pending_interrupts;
        break;
    case NV_PGRAPH_INTR_EN:
        result = pg->enabled_interrupts;
        break;
    case NV_PGRAPH_RDI_DATA: {
        unsigned int select = GET_MASK(pg->pgraph_regs[NV_PGRAPH_RDI_INDEX/4],
                                       NV_PGRAPH_RDI_INDEX_SELECT);
        int address = GET_MASK(pg->pgraph_regs[NV_PGRAPH_RDI_INDEX/4],
                                        NV_PGRAPH_RDI_INDEX_ADDRESS);

        result = pgraph_rdi_read(pg, select, address);

        /* FIXME: Overflow into select? */
        assert(address < GET_MASK(NV_PGRAPH_RDI_INDEX_ADDRESS,
                                  NV_PGRAPH_RDI_INDEX_ADDRESS));
        SET_MASK(pg->pgraph_regs[NV_PGRAPH_RDI_INDEX/4],
                 NV_PGRAPH_RDI_INDEX_ADDRESS, address + 1);
        break;
    }
    default:
        DEVICE_READ32_REG(pgraph); // Was : DEBUG_READ32_UNHANDLED(PGRAPH);
    }

    qemu_mutex_unlock(&pg->pgraph_lock);

//    reg_log_read(NV_PGRAPH, addr, r);

    DEVICE_READ32_END(PGRAPH);
}

DEVICE_WRITE32(PGRAPH)
{
    PGRAPHState *pg = &d->pgraph;
//    reg_log_write(NV_PGRAPH, addr, val);

    qemu_mutex_lock(&pg->pgraph_lock);

    switch (addr) {
    case NV_PGRAPH_INTR:
        pg->pending_interrupts &= ~value;
        qemu_cond_broadcast(&pg->interrupt_cond);
        break;
    case NV_PGRAPH_INTR_EN:
        pg->enabled_interrupts = value;
        break;
    case NV_PGRAPH_INCREMENT:
        if (value & NV_PGRAPH_INCREMENT_READ_3D) {
			pg->KelvinPrimitive.SetFlipRead=
                (pg->KelvinPrimitive.SetFlipRead + 1)
                % pg->KelvinPrimitive.SetFlipModulo;
            qemu_cond_broadcast(&pg->flip_3d);
        }
        break;
    case NV_PGRAPH_RDI_DATA: {
        unsigned int select = GET_MASK(pg->pgraph_regs[NV_PGRAPH_RDI_INDEX / 4],
                                       NV_PGRAPH_RDI_INDEX_SELECT);
        int address = GET_MASK(pg->pgraph_regs[NV_PGRAPH_RDI_INDEX / 4],
                                        NV_PGRAPH_RDI_INDEX_ADDRESS);

        pgraph_rdi_write(pg, select, address, value);

        /* FIXME: Overflow into select? */
        assert(address < GET_MASK(NV_PGRAPH_RDI_INDEX_ADDRESS/4,
                                  NV_PGRAPH_RDI_INDEX_ADDRESS));
        SET_MASK(pg->pgraph_regs[NV_PGRAPH_RDI_INDEX / 4],
                 NV_PGRAPH_RDI_INDEX_ADDRESS, address + 1);
        break;
    }
    case NV_PGRAPH_CHANNEL_CTX_TRIGGER: {
        xbox::addr_xt context_address =
			//warning pg->pgraph_regs[NV_PGRAPH_CHANNEL_CTX_POINTER / 4] was never set.
			GET_MASK(pg->pgraph_regs[NV_PGRAPH_CHANNEL_CTX_POINTER / 4],
                NV_PGRAPH_CHANNEL_CTX_POINTER_INST) << 4;

        if (value & NV_PGRAPH_CHANNEL_CTX_TRIGGER_READ_IN) {
            unsigned pgraph_channel_id =
                GET_MASK(pg->pgraph_regs[NV_PGRAPH_CTX_USER / 4], NV_PGRAPH_CTX_USER_CHID);

            NV2A_DPRINTF("PGRAPH: read channel %d context from %" HWADDR_PRIx "\n",
                pgraph_channel_id, context_address);

            uint8_t *context_ptr = d->pramin.ramin_ptr + context_address;
            uint32_t context_user = ldl_le_p((uint32_t*)context_ptr);

            NV2A_DPRINTF("    - CTX_USER = 0x%08X\n", context_user);

            pg->pgraph_regs[NV_PGRAPH_CTX_USER / 4] = context_user;
            // pgraph_set_context_user(d, context_user);
        }
        if (value & NV_PGRAPH_CHANNEL_CTX_TRIGGER_WRITE_OUT) {
            /* do stuff ... */
        }

        break;
    }
    default: 
        DEVICE_WRITE32_REG(pgraph); // Was : DEBUG_WRITE32_UNHANDLED(PGRAPH);
        break;
    }

    // events
    switch (addr) {
    case NV_PGRAPH_FIFO:
        qemu_cond_broadcast(&pg->fifo_access_cond);
        break;
    }

    qemu_mutex_unlock(&pg->pgraph_lock);

    DEVICE_WRITE32_END(PGRAPH);
}

void OpenGL_draw_end(NV2AState *d); // forward declaration

void OpenGL_draw_arrays(NV2AState *d)
{
    PGRAPHState *pg = &d->pgraph;

    assert(pg->opengl_enabled);
    assert(pg->shader_binding);

    pgraph_bind_vertex_attributes(d, pg->draw_arrays_max_count,
        false, 0);
    glMultiDrawArrays(pg->shader_binding->gl_primitive_mode,
        pg->gl_draw_arrays_start,
        pg->gl_draw_arrays_count,
        pg->draw_arrays_length);

    OpenGL_draw_end(d);
}

void OpenGL_draw_inline_buffer(NV2AState *d)
{
    PGRAPHState *pg = &d->pgraph;

    assert(pg->opengl_enabled);
    assert(pg->shader_binding);

    for (unsigned int i = 0; i < NV2A_VERTEXSHADER_ATTRIBUTES; i++) {
        VertexAttribute *vertex_attribute = &pg->vertex_attributes[i];

        if (vertex_attribute->inline_buffer) {

            glBindBuffer(GL_ARRAY_BUFFER,
                vertex_attribute->gl_inline_buffer);
            glBufferData(GL_ARRAY_BUFFER,
                pg->inline_buffer_length
                * sizeof(float) * 4,
                vertex_attribute->inline_buffer,
                GL_DYNAMIC_DRAW);

            /* Clear buffer for next batch */
            g_free(vertex_attribute->inline_buffer);
            vertex_attribute->inline_buffer = NULL;

            glVertexAttribPointer(i, 4, GL_FLOAT, GL_FALSE, 0, 0);
            glEnableVertexAttribArray(i);
        }
        else {
            glDisableVertexAttribArray(i);
            glVertexAttrib4fv(i, vertex_attribute->inline_value);
        }
    }

    glDrawArrays(pg->shader_binding->gl_primitive_mode,
        0, pg->inline_buffer_length);

    OpenGL_draw_end(d);
}

void OpenGL_draw_inline_array(NV2AState *d)
{
    PGRAPHState *pg = &d->pgraph;

    assert(pg->opengl_enabled);
    assert(pg->shader_binding);

    unsigned int index_count = pgraph_bind_inline_array(d);
    glDrawArrays(pg->shader_binding->gl_primitive_mode,
        0, index_count);

    OpenGL_draw_end(d);
}

void OpenGL_draw_inline_elements(NV2AState *d)
{
    PGRAPHState *pg = &d->pgraph;

    assert(pg->opengl_enabled);
    assert(pg->shader_binding);

    uint32_t max_element = 0;
    uint32_t min_element = (uint32_t)-1;
    for (unsigned int i = 0; i < pg->inline_elements_length; i++) {
        max_element = MAX(pg->inline_elements[i], max_element);
        min_element = MIN(pg->inline_elements[i], min_element);
    }
    pgraph_bind_vertex_attributes(d, max_element + 1, false, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pg->gl_element_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        pg->inline_elements_length * sizeof(pg->inline_elements[0]),
        pg->inline_elements,
        GL_DYNAMIC_DRAW);

    glDrawRangeElements(pg->shader_binding->gl_primitive_mode,
        min_element, max_element,
        pg->inline_elements_length,
        GL_UNSIGNED_SHORT, // Cxbx-Reloaded TODO : Restore GL_UNSIGNED_INT once HLE_draw_inline_elements can draw using uint32_t
        (void*)0);

    OpenGL_draw_end(d);
}

static void CxbxImGui_RenderOpenGL(ImGuiUI* m_imgui, std::nullptr_t unused)
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    m_imgui->DrawMenu();
    m_imgui->DrawWidgets();

    ImGui::Render();

    ImDrawData* drawData = ImGui::GetDrawData();
    if (drawData->TotalVtxCount > 0) {
        ImGui_ImplOpenGL3_RenderDrawData(drawData);
    }
}

void OpenGL_draw_state_update(NV2AState *d)
{
    PGRAPHState *pg = &d->pgraph;

    assert(pg->opengl_enabled);

    NV2A_GL_DGROUP_BEGIN("NV097_SET_BEGIN_END: 0x%x", pg->primitive_mode);

    //uint32_t control_0 = pg->pgraph_regs[NV_PGRAPH_CONTROL_0 / 4];
    //uint32_t control_1 = pg->pgraph_regs[NV_PGRAPH_CONTROL_1 / 4];

    bool depth_test = pg->KelvinPrimitive.SetCullFaceEnable!=0;
    bool stencil_test = pg->KelvinPrimitive.SetStencilTestEnable!=0;

    pgraph_update_surface(d, true, true, depth_test || stencil_test);

    bool alpha = pg->KelvinPrimitive.SetColorMask & NV097_SET_COLOR_MASK_ALPHA_WRITE_ENABLE;
    bool red = pg->KelvinPrimitive.SetColorMask & NV097_SET_COLOR_MASK_RED_WRITE_ENABLE;
    bool green = pg->KelvinPrimitive.SetColorMask & NV097_SET_COLOR_MASK_GREEN_WRITE_ENABLE;
    bool blue = pg->KelvinPrimitive.SetColorMask & NV097_SET_COLOR_MASK_BLUE_WRITE_ENABLE;
    glColorMask(red, green, blue, alpha);
    glDepthMask(!!(pg->KelvinPrimitive.SetDepthMask&0x1!=0));
    glStencilMask(pg->KelvinPrimitive.SetStencilMask&0xFF);

    //uint32_t blend = pg->pgraph_regs[NV_PGRAPH_BLEND / 4];
    if (pg->KelvinPrimitive.SetBlendEnable!=0) {
        glEnable(GL_BLEND);
        uint32_t sfactor = pg->KelvinPrimitive.SetBlendFuncSfactor;
        uint32_t dfactor = pg->KelvinPrimitive.SetBlendFuncDfactor;
        assert(sfactor < ARRAY_SIZE(pgraph_blend_factor_map));
        assert(dfactor < ARRAY_SIZE(pgraph_blend_factor_map));
        glBlendFunc(pgraph_blend_factor_map[sfactor],
            pgraph_blend_factor_map[dfactor]);

        uint32_t equation = pg->KelvinPrimitive.SetBlendEquation;
        assert(equation < ARRAY_SIZE(pgraph_blend_equation_map));
        glBlendEquation(pgraph_blend_equation_map[equation]);

        uint32_t blend_color = pg->KelvinPrimitive.SetBlendColor;
        glBlendColor(((blend_color >> 16) & 0xFF) / 255.0f, /* red */
            ((blend_color >> 8) & 0xFF) / 255.0f,  /* green */
            (blend_color & 0xFF) / 255.0f,         /* blue */
            ((blend_color >> 24) & 0xFF) / 255.0f);/* alpha */
    }
    else {
        glDisable(GL_BLEND);
    }

    /* Face culling */
    //uint32_t setupraster = pg->pgraph_regs[NV_PGRAPH_SETUPRASTER / 4];
    if (pg->KelvinPrimitive.SetBlendEnable!=0) {
        uint32_t cull_face = pg->KelvinPrimitive.SetCullFace;
        assert(cull_face < ARRAY_SIZE(pgraph_cull_face_map));
        glCullFace(pgraph_cull_face_map[cull_face]);
        glEnable(GL_CULL_FACE);
    }
    else {
        glDisable(GL_CULL_FACE);
    }

    /* Front-face select */
    glFrontFace(pg->KelvinPrimitive.SetFrontFace & 1
        ? GL_CCW : GL_CW);

    /* Polygon offset */
    /* FIXME: GL implementation-specific, maybe do this in VS? */
    if (pg->KelvinPrimitive.SetPolyOffsetFillEnable!=0) {
        glEnable(GL_POLYGON_OFFSET_FILL);
    }
    else {
        glDisable(GL_POLYGON_OFFSET_FILL);
    }
    if (pg->KelvinPrimitive.SetPolyOffsetLineEnable!=0) {
        glEnable(GL_POLYGON_OFFSET_LINE);
    }
    else {
        glDisable(GL_POLYGON_OFFSET_LINE);
    }
    if (pg->KelvinPrimitive.SetPolyOffsetPointEnable!=0) {
        glEnable(GL_POLYGON_OFFSET_POINT);
    }
    else {
        glDisable(GL_POLYGON_OFFSET_POINT);
    }

    if ((  pg->KelvinPrimitive.SetPolyOffsetPointEnable!=0)
        || pg->KelvinPrimitive.SetPolyOffsetLineEnable!=0
        || pg->KelvinPrimitive.SetPolyOffsetPointEnable!=0) {
        GLfloat zfactor = *(float*)&pg->KelvinPrimitive.SetPolygonOffsetScaleFactor;
        GLfloat zbias = *(float*)&pg->KelvinPrimitive.SetPolygonOffsetBias;
        glPolygonOffset(zfactor, zbias);
    }

    /* Depth testing */
    if (depth_test) {
        glEnable(GL_DEPTH_TEST);

        uint32_t depth_func = pg->KelvinPrimitive.SetDepthFunc;
        assert(depth_func < ARRAY_SIZE(pgraph_depth_func_map));
        glDepthFunc(pgraph_depth_func_map[depth_func]);
    }
    else {
        glDisable(GL_DEPTH_TEST);
    }

    if (stencil_test) {
        glEnable(GL_STENCIL_TEST);

        uint32_t stencil_func = pg->KelvinPrimitive.SetStencilFunc &0x0F;
        uint32_t stencil_ref = pg->KelvinPrimitive.SetStencilFuncRef &0xFF;
        uint32_t func_mask = pg->KelvinPrimitive.SetStencilFuncMask;
        //uint32_t control2 = pg->pgraph_regs[NV_PGRAPH_CONTROL_2/4];
        uint32_t op_fail = pg->KelvinPrimitive.SetStencilOpFail &0x0F;
        uint32_t op_zfail = pg->KelvinPrimitive.SetStencilOpZfail;
        uint32_t op_zpass = pg->KelvinPrimitive.SetStencilOpZpass;

        assert(stencil_func < ARRAY_SIZE(pgraph_stencil_func_map));
        assert(op_fail < ARRAY_SIZE(pgraph_stencil_op_map));
        assert(op_zfail < ARRAY_SIZE(pgraph_stencil_op_map));
        assert(op_zpass < ARRAY_SIZE(pgraph_stencil_op_map));

        glStencilFunc(
            pgraph_stencil_func_map[stencil_func],
            stencil_ref,
            func_mask);

        glStencilOp(
            pgraph_stencil_op_map[op_fail],
            pgraph_stencil_op_map[op_zfail],
            pgraph_stencil_op_map[op_zpass]);

    }
    else {
        glDisable(GL_STENCIL_TEST);
    }

    /* Dither */
    /* FIXME: GL implementation dependent */
    if (pg->KelvinPrimitive.SetDitherEnable!=0) {
        glEnable(GL_DITHER);
    }
    else {
        glDisable(GL_DITHER);
    }

    pgraph_bind_shaders(pg);
    pgraph_bind_textures(d);

    //glDisableVertexAttribArray(NV2A_VERTEX_ATTR_DIFFUSE);
    //glVertexAttrib4f(NV2A_VERTEX_ATTR_DIFFUSE, 1.0f, 1.0f, 1.0f, 1.0f);

    unsigned int width, height;
    pgraph_get_surface_dimensions(pg, &width, &height);
    pgraph_apply_anti_aliasing_factor(pg, &width, &height);
    glViewport(0, 0, width, height);

    /* Visibility testing */
    if (pg->KelvinPrimitive.SetZpassPixelCountEnable!=0) {
        GLuint gl_query;
        glGenQueries(1, &gl_query);
        pg->gl_zpass_pixel_count_query_count++;
        pg->gl_zpass_pixel_count_queries = (GLuint*)g_realloc(
            pg->gl_zpass_pixel_count_queries,
            sizeof(GLuint) * pg->gl_zpass_pixel_count_query_count);
        pg->gl_zpass_pixel_count_queries[
            pg->gl_zpass_pixel_count_query_count - 1] = gl_query;
        glBeginQuery(GL_SAMPLES_PASSED, gl_query);
    }

    // Render ImGui
    static std::function<void(ImGuiUI*, std::nullptr_t)> internal_render = &CxbxImGui_RenderOpenGL;
    g_renderbase->Render(internal_render, nullptr);
}

void OpenGL_draw_end(NV2AState *d)
{
    PGRAPHState *pg = &d->pgraph;

    assert(pg->opengl_enabled);

    /* End of visibility testing */
    if (pg->KelvinPrimitive.SetZpassPixelCountEnable !=0) {
        glEndQuery(GL_SAMPLES_PASSED);
    }

    NV2A_GL_DGROUP_END();
}

void OpenGL_draw_clear(NV2AState *d)
{
    PGRAPHState *pg = &d->pgraph;

    assert(pg->opengl_enabled);

    NV2A_DPRINTF("---------PRE CLEAR ------\n");
    GLbitfield gl_mask = 0;

    bool write_color = (pg->KelvinPrimitive.ClearSurface & NV097_CLEAR_SURFACE_COLOR);
    bool write_zeta =
        (pg->KelvinPrimitive.ClearSurface & (NV097_CLEAR_SURFACE_Z | NV097_CLEAR_SURFACE_STENCIL));

    if (write_zeta) {
        uint32_t clear_zstencil =
            d->pgraph.KelvinPrimitive.SetZStencilClearValue;
        GLint gl_clear_stencil;
        GLfloat gl_clear_depth;

        /* FIXME: Put these in some lookup table */
        const float f16_max = 511.9375f;
        /* FIXME: 7 bits of mantissa unused. maybe use full buffer? */
        const float f24_max = 3.4027977E38f;

        switch (pg->surface_shape.zeta_format) {
        case NV097_SET_SURFACE_FORMAT_ZETA_Z16: {
            if (pg->KelvinPrimitive.ClearSurface & NV097_CLEAR_SURFACE_Z) {
                gl_mask |= GL_DEPTH_BUFFER_BIT;
                uint16_t z = clear_zstencil & 0xFFFF;
                if (pg->surface_shape.z_format) {
                    gl_clear_depth = convert_f16_to_float(z) / f16_max;
                    assert(false); /* FIXME: Untested */
                }
                else {
                    gl_clear_depth = z / (float)0xFFFF;
                }
            }
            break;
        }
        case NV097_SET_SURFACE_FORMAT_ZETA_Z24S8: {
            if (pg->KelvinPrimitive.ClearSurface & NV097_CLEAR_SURFACE_STENCIL) {
                gl_mask |= GL_STENCIL_BUFFER_BIT;
                gl_clear_stencil = clear_zstencil & 0xFF;
            }
            if (pg->KelvinPrimitive.ClearSurface & NV097_CLEAR_SURFACE_Z) {
                gl_mask |= GL_DEPTH_BUFFER_BIT;
                uint32_t z = clear_zstencil >> 8;
                if (pg->surface_shape.z_format) {
                    gl_clear_depth = convert_f24_to_float(z) / f24_max;
                    assert(false); /* FIXME: Untested */
                }
                else {
                    gl_clear_depth = z / (float)0xFFFFFF;
                }
            }
            break;
        }
        default:
            fprintf(stderr, "Unknown zeta surface format: 0x%x\n", pg->surface_shape.zeta_format);
            assert(false);
            break;
        }

        if (gl_mask & GL_DEPTH_BUFFER_BIT) {
            glDepthMask(GL_TRUE);
            glClearDepth(gl_clear_depth);
        }

        if (gl_mask & GL_STENCIL_BUFFER_BIT) {
            glStencilMask(0xff);
            glClearStencil(gl_clear_stencil);
        }
    }

    if (write_color) {
        gl_mask |= GL_COLOR_BUFFER_BIT;
        glColorMask((pg->KelvinPrimitive.ClearSurface & NV097_CLEAR_SURFACE_R)
            ? GL_TRUE : GL_FALSE,
            (pg->KelvinPrimitive.ClearSurface & NV097_CLEAR_SURFACE_G)
            ? GL_TRUE : GL_FALSE,
            (pg->KelvinPrimitive.ClearSurface & NV097_CLEAR_SURFACE_B)
            ? GL_TRUE : GL_FALSE,
            (pg->KelvinPrimitive.ClearSurface & NV097_CLEAR_SURFACE_A)
            ? GL_TRUE : GL_FALSE);
        uint32_t clear_color = d->pgraph.KelvinPrimitive.SetColorClearValue;

        /* Handle RGB */
        GLfloat red, green, blue;
        switch (pg->surface_shape.color_format) {
        case NV097_SET_SURFACE_FORMAT_COLOR_LE_X1R5G5B5_Z1R5G5B5:
        case NV097_SET_SURFACE_FORMAT_COLOR_LE_X1R5G5B5_O1R5G5B5:
            red = ((clear_color >> 10) & 0x1F) / 31.0f;
            green = ((clear_color >> 5) & 0x1F) / 31.0f;
            blue = (clear_color & 0x1F) / 31.0f;
            assert(false); /* Untested */
            break;
        case NV097_SET_SURFACE_FORMAT_COLOR_LE_R5G6B5:
            red = ((clear_color >> 11) & 0x1F) / 31.0f;
            green = ((clear_color >> 5) & 0x3F) / 63.0f;
            blue = (clear_color & 0x1F) / 31.0f;
            break;
        case NV097_SET_SURFACE_FORMAT_COLOR_LE_X8R8G8B8_Z8R8G8B8:
        case NV097_SET_SURFACE_FORMAT_COLOR_LE_X8R8G8B8_O8R8G8B8:
        case NV097_SET_SURFACE_FORMAT_COLOR_LE_X1A7R8G8B8_Z1A7R8G8B8:
        case NV097_SET_SURFACE_FORMAT_COLOR_LE_X1A7R8G8B8_O1A7R8G8B8:
        case NV097_SET_SURFACE_FORMAT_COLOR_LE_A8R8G8B8:
            red = ((clear_color >> 16) & 0xFF) / 255.0f;
            green = ((clear_color >> 8) & 0xFF) / 255.0f;
            blue = (clear_color & 0xFF) / 255.0f;
            break;
        case NV097_SET_SURFACE_FORMAT_COLOR_LE_B8:
        case NV097_SET_SURFACE_FORMAT_COLOR_LE_G8B8:
            /* Xbox D3D doesn't support clearing those */
        default:
            red = 1.0f;
            green = 0.0f;
            blue = 1.0f;
            fprintf(stderr, "CLEAR_SURFACE for color_format 0x%x unsupported",
                pg->surface_shape.color_format);
            assert(false);
            break;
        }

        /* Handle alpha */
        GLfloat alpha;
        switch (pg->surface_shape.color_format) {
            /* FIXME: CLEAR_SURFACE seems to work like memset, so maybe we
            *        also have to clear non-alpha bits with alpha value?
            *        As GL doesn't own those pixels we'd have to do this on
            *        our own in xbox memory.
            */
        case NV097_SET_SURFACE_FORMAT_COLOR_LE_X1A7R8G8B8_Z1A7R8G8B8:
        case NV097_SET_SURFACE_FORMAT_COLOR_LE_X1A7R8G8B8_O1A7R8G8B8:
            alpha = ((clear_color >> 24) & 0x7F) / 127.0f;
            assert(false); /* Untested */
            break;
        case NV097_SET_SURFACE_FORMAT_COLOR_LE_A8R8G8B8:
            alpha = ((clear_color >> 24) & 0xFF) / 255.0f;
            break;
        default:
            alpha = 1.0f;
            break;
        }

        glClearColor(red, green, blue, alpha);
    }

    if (gl_mask) {
        pgraph_update_surface(d, true, write_color, write_zeta);

        glEnable(GL_SCISSOR_TEST);

        unsigned int xmin = GET_MASK(pg->KelvinPrimitive.SetClearRectHorizontal,
            NV_PGRAPH_CLEARRECTX_XMIN);
        unsigned int xmax = GET_MASK(pg->KelvinPrimitive.SetClearRectHorizontal,
            NV_PGRAPH_CLEARRECTX_XMAX);
        unsigned int ymin = GET_MASK(pg->KelvinPrimitive.SetClearRectVertical,
            NV_PGRAPH_CLEARRECTY_YMIN);
        unsigned int ymax = GET_MASK(pg->KelvinPrimitive.SetClearRectVertical,
            NV_PGRAPH_CLEARRECTY_YMAX);

        unsigned int scissor_x = xmin;
        unsigned int scissor_y = pg->surface_shape.clip_height - ymax - 1;

        unsigned int scissor_width = xmax - xmin + 1;
        unsigned int scissor_height = ymax - ymin + 1;

        pgraph_apply_anti_aliasing_factor(pg, &scissor_x, &scissor_y);
        pgraph_apply_anti_aliasing_factor(pg, &scissor_width, &scissor_height);

        /* FIXME: Should this really be inverted instead of ymin? */
        glScissor(scissor_x, scissor_y, scissor_width, scissor_height);

        /* FIXME: Respect window clip?!?! */

        NV2A_DPRINTF("------------------CLEAR 0x%x %d,%d - %d,%d  %x---------------\n",
            parameter, xmin, ymin, xmax, ymax, d->pgraph.KelvinPrimitive.SetColorClearValue);

        /* Dither */
        /* FIXME: Maybe also disable it here? + GL implementation dependent */
        if (pg->KelvinPrimitive.SetDitherEnable!=0) {
            glEnable(GL_DITHER);
        }
        else {
            glDisable(GL_DITHER);
        }

        glClear(gl_mask);

        glDisable(GL_SCISSOR_TEST);
    }

    pgraph_set_surface_dirty(pg, write_color, write_zeta);
}

void OpenGL_init_pgraph_plugins()
{
    pgraph_draw_arrays = OpenGL_draw_arrays;
    pgraph_draw_inline_buffer = OpenGL_draw_inline_buffer;
    pgraph_draw_inline_array = OpenGL_draw_inline_array;
    pgraph_draw_inline_elements = OpenGL_draw_inline_elements;
    pgraph_draw_state_update = OpenGL_draw_state_update;
    pgraph_draw_clear = OpenGL_draw_clear;
}

//calsulate vertex stride by accumulating the size of each attribute.
int pgraph_get_NV2A_vertex_stride(PGRAPHState *pg)
{
	int stride = 0;
	for(int slot=0;slot< NV2A_VERTEXSHADER_ATTRIBUTES;slot++){
		if(pg->vertex_attributes[slot].count>0)
			stride+= pg->vertex_attributes[slot].count*pg->vertex_attributes[slot].size;
	}
	return stride;
}

//Contex Handle Define
#define NV_DD_DMA_PUSHER_SYNC_NOTIFIER_CONTEXT_DMA_TO_MEMORY    2 
#define NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY                   3 
#define NV_DD_DMA_CONTEXT_DMA_TO_VIDEO_MEMORY                   4 
#define NV_DD_DMA_CONTEXT_DMA_FROM_VIDEO_MEMORY                 5 
#define NV_DD_DMA_PUSHER_CONTEXT_DMA_FROM_MEMORY                6 
#define D3D_MEMCOPY_NOTIFIER_CONTEXT_DMA_TO_MEMORY              7 
#define D3D_SEMAPHORE_CONTEXT_DMA_IN_MEMORY                     8
#define D3D_COLOR_CONTEXT_DMA_IN_VIDEO_MEMORY                   9
#define D3D_ZETA_CONTEXT_DMA_IN_VIDEO_MEMORY                   10
#define D3D_COPY_CONTEXT_DMA_IN_VIDEO_MEMORY                   11
#define D3D_CONTEXT_IN_CACHED_MEMORY                           12

#define D3D_KELVIN_PRIMITIVE                                   13 
#define D3D_MEMORY_TO_MEMORY_COPY                              14 
#define D3D_SCALED_IMAGE_FROM_MEMORY                           15   
#define D3D_RECTANGLE_COPY                                     16
#define D3D_RECTANGLE_COPY_SURFACES                            17

#define D3D_RECTANGLE_COPY_PATTERN                             18
#define D3D_RECTANGLE_COPY_COLOR_KEY                           19
#define D3D_RECTANGLE_COPY_ROP                                 20
#define D3D_RECTANGLE_COPY_BETA1                               21
#define D3D_RECTANGLE_COPY_BETA4                               22
#define D3D_RECTANGLE_COPY_CLIP_RECTANGLE                      24

//extern xbox::X_VERTEXATTRIBUTEFORMAT g_NV2AInlineArrayVertexBuffer_AttributeFormat = {};//tmp glue

static uint32_t subchannel_to_graphic_class[8] = { NV_KELVIN_PRIMITIVE,NV_MEMORY_TO_MEMORY_FORMAT,NV_IMAGE_BLIT,NV_CONTEXT_SURFACES_2D,0,NV_CONTEXT_PATTERN,0,0, };
extern IDirect3DDevice9Ex *g_pD3DDevice;
extern VertexShaderMode g_Xbox_VertexShaderMode;
extern void CxbxUpdateHostVertexShader();
//extern int g_InlineVertexBuffer_DeclarationOverride ;
//starting address of vertex shader user program
extern xbox::dword_xt g_Xbox_VertexShader_FunctionSlots_StartAddress;
//xbox vertex shader attributes slots. set by SetVertexShaderInput(). try to set it directly before set vertex shader or draw prmitives.
extern xbox::X_VERTEXATTRIBUTEFORMAT g_Xbox_SetVertexShaderInput_Attributes;
extern DWORD ABGR_to_ARGB(const uint32_t color);
xbox::X_VERTEXATTRIBUTEFORMAT g_NV2AInlineArrayVertexBuffer_AttributeFormat = {};

//get Host FVF with pVAF, not implement yet.
/*
const DWORD GetHostFVFfromXboxVertexAttributes(xbox::X_VERTEXATTRIBUTEFORMAT *pVAF)
{
	DWORD HostFVF = 0;
	for (int slot = 0; slot < X_VSH_MAX_ATTRIBUTES; slot++) {
		switch (slot) {
		case xbox::X_D3DVSDE_POSITION:
			if (pVAF->Slots[slot].Format == 0);
			break;
		case xbox::X_D3DVSDE_BLENDWEIGHT:
			break;
		case xbox::X_D3DVSDE_NORMAL:
			break;
		case xbox::X_D3DVSDE_DIFFUSE:
			
			;
			break;
		case xbox::X_D3DVSDE_SPECULAR:
			
			;
			break;
		case xbox::X_D3DVSDE_FOG:
			break;
		case xbox::X_D3DVSDE_POINTSIZE:
			break;
		case xbox::X_D3DVSDE_BACKDIFFUSE:
			break;
		case xbox::X_D3DVSDE_BACKSPECULAR:
			break;
		case xbox::X_D3DVSDE_TEXCOORD0:
			break;
		case xbox::X_D3DVSDE_TEXCOORD1:
			break;
		case xbox::X_D3DVSDE_TEXCOORD2:
			break;
		case xbox::X_D3DVSDE_TEXCOORD3:
			break;
		}
	}
	return HostFVF;
}
*/
//method count always represnt total dword needed as the arguments following the method.
//caller must ensure there are enough argements available in argv.
int pgraph_handle_method(
    NV2AState *d,
    unsigned int subchannel,
    unsigned int method,
    uint32_t arg0,
    uint32_t *argv,
    uint32_t method_count,
    uint32_t max_lookahead_words)

{
    int num_processed = 1;			//num_processed default to 1, which represent the first parameter passed in this call.
    int num_words_consumed = 1;		//actual word consumed here in method processing.
    int num_words_available = MIN(method_count, max_lookahead_words);
    size_t arg_count = 0;

    unsigned int i;
    unsigned int slot;

    PGRAPHState *pg = &d->pgraph;

    bool channel_valid =
        d->pgraph.pgraph_regs[NV_PGRAPH_CTX_CONTROL / 4] & NV_PGRAPH_CTX_CONTROL_CHID;
    assert(channel_valid);

    unsigned channel_id = GET_MASK(pg->pgraph_regs[NV_PGRAPH_CTX_USER / 4], NV_PGRAPH_CTX_USER_CHID);

    ContextSurfaces2DState *context_surfaces_2d = &pg->context_surfaces_2d;
    ImageBlitState *image_blit = &pg->image_blit;
    KelvinState *kelvin = &pg->kelvin;

    assert(subchannel < 8);
    //xbox d3d binds subchannel with graphic class in CDeivice_CreateDevice(). in the beginning,
    //set_object can be used to recognize the binding between graphic class and subchannel.
    //the arg0 is object handle,rather than an address here. need to use a function called ramht_look() to lookup the object entry.
    //quesiton is that the object was created and binded using xbx d3d routine which we didn't patch. need further study.
    if (method == NV_SET_OBJECT) {
		/*during init, arg0 = Handle of Miniport GraphicObject Handle,
		we must setup links between subchannel used here with the handle and the graphic class associate with that handle.
		the link could be changed dynamicaly using the NV_SET_OBJECT method.
		*/
		/*
		assert(arg0 < d->pramin.ramin_size);
        uint8_t *obj_ptr = d->pramin.ramin_ptr + arg0;

        uint32_t ctx_1 = ldl_le_p((uint32_t*)obj_ptr);
        uint32_t ctx_2 = ldl_le_p((uint32_t*)(obj_ptr+4));
        uint32_t ctx_3 = ldl_le_p((uint32_t*)(obj_ptr+8));
        uint32_t ctx_4 = ldl_le_p((uint32_t*)(obj_ptr+12));
        uint32_t ctx_5 = arg0;
        //these code below doesn't make sense, each NV_PGRAPH_CTX_CACHE only has 0x20 bytes, we have 8 subchannel, 0x20/8=4 bytes, that means 1 NV_PGRAPH_CTX_CACHE can only provide 1 DWORD to 1 subchannel.
        //somehow the original code pg->pgraph_regs[NV_PGRAPH_CTX_CACHE1 + subchannel *4  ] = ctx_1; really works, but in this new code it no longer works. need further study.
        pg->pgraph_regs[NV_PGRAPH_CTX_CACHE1 / 4 + subchannel ] = ctx_1;
        pg->pgraph_regs[NV_PGRAPH_CTX_CACHE2 / 4 + subchannel ] = ctx_2;
        pg->pgraph_regs[NV_PGRAPH_CTX_CACHE3 / 4 + subchannel ] = ctx_3;
        pg->pgraph_regs[NV_PGRAPH_CTX_CACHE4 / 4 + subchannel ] = ctx_4;
        pg->pgraph_regs[NV_PGRAPH_CTX_CACHE5 / 4 + subchannel ] = ctx_5;
		*/
        switch (arg0) {
        case D3D_KELVIN_PRIMITIVE:
            subchannel_to_graphic_class[subchannel]= NV_KELVIN_PRIMITIVE;
            break;
        case D3D_MEMORY_TO_MEMORY_COPY:
            subchannel_to_graphic_class[subchannel] = NV_MEMORY_TO_MEMORY_FORMAT;//should be copy, supposed to dma copy an image rect and change the pixel format in the same time. need further study.
            break;
        case D3D_RECTANGLE_COPY:
            subchannel_to_graphic_class[subchannel] = NV_IMAGE_BLIT;
            break;
        case D3D_RECTANGLE_COPY_SURFACES:
            subchannel_to_graphic_class[subchannel] = NV_CONTEXT_SURFACES_2D;
            break;
        case D3D_RECTANGLE_COPY_PATTERN:
            subchannel_to_graphic_class[subchannel] = NV_CONTEXT_PATTERN;
            break;
        default:
            assert(0);
            break;
        }

    }

    // is this right? needs double check.
    pg->pgraph_regs[NV_PGRAPH_CTX_SWITCH1/4] = pg->pgraph_regs[NV_PGRAPH_CTX_CACHE1 + subchannel ];
    pg->pgraph_regs[NV_PGRAPH_CTX_SWITCH2/4] = pg->pgraph_regs[NV_PGRAPH_CTX_CACHE2 + subchannel ];
    pg->pgraph_regs[NV_PGRAPH_CTX_SWITCH3/4] = pg->pgraph_regs[NV_PGRAPH_CTX_CACHE3 + subchannel ];
    pg->pgraph_regs[NV_PGRAPH_CTX_SWITCH4/4] = pg->pgraph_regs[NV_PGRAPH_CTX_CACHE4 + subchannel ];
    pg->pgraph_regs[NV_PGRAPH_CTX_SWITCH5/4] = pg->pgraph_regs[NV_PGRAPH_CTX_CACHE5 + subchannel ];

    //if the graphics_class doesn't work, we have to switch to use subchannel instead.
	uint32_t graphics_class = GET_MASK(pg->pgraph_regs[NV_PGRAPH_CTX_SWITCH1/4],
                                       NV_PGRAPH_CTX_SWITCH1_GRCLASS);
	graphics_class = subchannel_to_graphic_class[subchannel];
    //if graphic_class not set correctly, use subchannel instead.
    //xbox d3d binds subchannel with specific graphic class, this binding could change, need more verificaiton. or we could implement the set_object
    //the binding here are reverse engineered from xdk pushbuffer sample.
	/*
	if (graphics_class == 0) {
        switch (subchannel) {
            case 0:
                graphics_class = NV_KELVIN_PRIMITIVE;
                break;
            case 1:
                graphics_class = NV_MEMORY_TO_MEMORY_FORMAT;//should be copy, supposed to dma copy an image rect and change the pixel format in the same time. need further study.
                break;
            case 2:
                graphics_class = NV_IMAGE_BLIT;
                break;
            case 3:
                graphics_class = NV_CONTEXT_SURFACES_2D;
                break;
            case 4:
                break;
            case 5:
                graphics_class = NV_CONTEXT_PATTERN;
                break;
            case 6:
                break;
            case 7:
                break;
        }
    }
	*/
    // Logging is slow.. disable for now..
    //pgraph_log_method(subchannel, graphics_class, method, parameter);

    //if (subchannel != 0) {
        // catches context switching issues on xbox d3d
        // assert(graphics_class != 0x97); // no need to assert, this is normap for graphic classes other than KELVIN_PRIMITIVE
    //}

    /* ugly switch for now */
    //we shall switch the sub_channel instead of graphics_class.
    switch (graphics_class) {

        case NV_MEMORY_TO_MEMORY_FORMAT:
            switch (method) {
            case NV039_SET_OBJECT:
                ;//check whether we should add object instance for this class or not.
                break;
            }
            break;

        case NV_CONTEXT_PATTERN: {
            switch (method) {
            case NV044_SET_OBJECT:
                ;//check whether we should add object instance for this class or not.
                break;

            case NV044_SET_MONOCHROME_COLOR0:
                pg->pgraph_regs[NV_PGRAPH_PATT_COLOR0/4] = arg0;//why not using NV044_SET_MONOCHROME_COLOR0?
                break;

            }
        }   break;

        case NV_CONTEXT_SURFACES_2D: {
            switch (method) {
            case NV062_SET_OBJECT:
                context_surfaces_2d->object_instance = arg0;
                break;
            case NV062_SET_CONTEXT_DMA_IMAGE_SOURCE:
                context_surfaces_2d->dma_image_source = arg0;
                break;
            case NV062_SET_CONTEXT_DMA_IMAGE_DESTIN:
                context_surfaces_2d->dma_image_dest = arg0;
                break;
            case NV062_SET_COLOR_FORMAT:
                context_surfaces_2d->color_format = arg0;
                break;
            case NV062_SET_PITCH:
                context_surfaces_2d->source_pitch = arg0 & 0xFFFF;
                context_surfaces_2d->dest_pitch = arg0 >> 16;
                break;
            case NV062_SET_OFFSET_SOURCE:
                context_surfaces_2d->source_offset = arg0 & 0x07FFFFFF;
                break;
            case NV062_SET_OFFSET_DESTIN:
                context_surfaces_2d->dest_offset = arg0 & 0x07FFFFFF;
                break;
            default:
                EmuLog(LOG_LEVEL::WARNING, "Unknown NV_CONTEXT_SURFACES_2D Method: 0x%08X", method);
            }
    
            break; 
        }
    
        case NV_IMAGE_BLIT: {
            switch (method) {
                case NV09F_SET_OBJECT:
                    image_blit->object_instance = arg0;
                    break;
                case NV09F_SET_CONTEXT_SURFACES:
                    image_blit->context_surfaces = arg0;
                    break;
                case NV09F_SET_OPERATION:
                    image_blit->operation = arg0;
                    break;
                case NV09F_CONTROL_POINT_IN:
                    image_blit->in_x = arg0 & 0xFFFF;
                    image_blit->in_y = arg0 >> 16;
                    break;
                case NV09F_CONTROL_POINT_OUT:
                    image_blit->out_x = arg0 & 0xFFFF;
                    image_blit->out_y = arg0 >> 16;
                    break;
                case NV09F_SIZE:
                    image_blit->width = arg0 & 0xFFFF;
                    image_blit->height = arg0 >> 16;

                    /* I guess this kicks it off? */
                    if (image_blit->operation == NV09F_SET_OPERATION_SRCCOPY) {

                        NV2A_GL_DPRINTF(true, "NV09F_SET_OPERATION_SRCCOPY");

                        ContextSurfaces2DState *context_surfaces = context_surfaces_2d;
                        assert(context_surfaces->object_instance
                            == image_blit->context_surfaces);

                        unsigned int bytes_per_pixel;
                        switch (context_surfaces->color_format) {
                        case NV062_SET_COLOR_FORMAT_LE_Y8:
                            bytes_per_pixel = 1;
                            break;
                        case NV062_SET_COLOR_FORMAT_LE_R5G6B5:
                            bytes_per_pixel = 2;
                            break;
                        case NV062_SET_COLOR_FORMAT_LE_A8R8G8B8:
                            bytes_per_pixel = 4;
                            break;
                        default:
                            printf("Unknown blit surface format: 0x%x\n", context_surfaces->color_format);
                            assert(false);
                            break;
                        }

                        xbox::addr_xt source_dma_len, dest_dma_len;
                        uint8_t *source, *dest;

                        source = (uint8_t*)nv_dma_map(d, context_surfaces->dma_image_source,
                                                        &source_dma_len);
                        assert(context_surfaces->source_offset < source_dma_len);
                        source += context_surfaces->source_offset;

                        dest = (uint8_t*)nv_dma_map(d, context_surfaces->dma_image_dest,
                                                        &dest_dma_len);
                        assert(context_surfaces->dest_offset < dest_dma_len);
                        dest += context_surfaces->dest_offset;

                        NV2A_DPRINTF("  - 0x%tx -> 0x%tx\n", source - d->vram_ptr,
                                                                dest - d->vram_ptr);

                        unsigned int y;
                        for (y = 0; y<image_blit->height; y++) {
                            uint8_t *source_row = source
                                + (image_blit->in_y + y) * context_surfaces->source_pitch
                                + image_blit->in_x * bytes_per_pixel;

                            uint8_t *dest_row = dest
                                + (image_blit->out_y + y) * context_surfaces->dest_pitch
                                + image_blit->out_x * bytes_per_pixel;

                            memmove(dest_row, source_row,
                                image_blit->width * bytes_per_pixel);
                        }

                    } else {
                        assert(false);
                    }

                    break;
                default:
                    EmuLog(LOG_LEVEL::WARNING, "Unknown NV_IMAGE_BLIT Method: 0x%08X", method);
            }
            break;
        }

        //test case:xdk pushbuffer sample.
 
        case NV_KELVIN_PRIMITIVE: {

            //update struct KelvinPrimitive/array regs[] in first round, skip special cases. then we process those state variables if necessary in 2nd round.
            switch (method) {
                //list all special cases here.
                case NV097_SET_OBJECT:
                case NV097_NO_OPERATION:	//this is used as short jump or interrupt, padding in front of fixups in order to make sure fixup will be applied before the instruction enter cache.
                
                case NV097_SET_BEGIN_END:   //enclave subset of drawing instructions. need special handling.
                case NV097_SET_TRANSFORM_CONSTANT://this sets the vertex constant register/slots using index from NV097_SET_TRANSFORM_CONSTANT_LOAD, not the transform constants in KelvinPrime.
                case NV097_SET_TRANSFORM_PROGRAM://this sets the vertex shader using index from NV097_SET_TRANSFORM_PROGRAM_LOAD, not the transform program in KelvinPrime.

                case NV097_ARRAY_ELEMENT16: //PUSH_INSTR_IMM_NOINC
                case NV097_ARRAY_ELEMENT32: //PUSH_INSTR_IMM_NOINC
				case NV097_DRAW_ARRAYS:		//PUSH_INSTR_IMM_NOINC
				case NV097_INLINE_ARRAY:	//PUSH_INSTR_IMM_NOINC
                    break;

                default:
                    for (int argc = 0; argc < method_count; argc++) {
                        pg->regs[ method/4 + argc] = argv[argc];
                    }
                    break;
            }

            //2nd round, handle special cases, setup bit mask flags, setup pgraph internal state vars, 
            switch (method) {
                case NV097_SET_OBJECT://done
                    kelvin->object_instance = arg0;
                    break;

                case NV097_NO_OPERATION://done
                    /* The bios uses nop as a software method call -
                     * it seems to expect a notify interrupt if the parameter isn't 0.
                     * According to a nouveau guy it should still be a nop regardless
                     * of the parameter. It's possible a debug register enables this,
                     * but nothing obvious sticks out. Weird.

					// when arg0 !=0, this is an interrupt/call back operation
					// arg0=operation
					// WRITE_REGISTER_VALUE(9), from Otogi CommonSetDebugRegisters()
					//     NV097_SET_ZSTENCIL_CLEAR_VALUE = pushbuffer method (register, method_cout=1)
					//     NV097_SET_COLOR_CLEAR_VALUE = value to write
					// 
					// READ_CALLBACK(7), WRITE_CALLBACK(6), from Otogi D3DDevice_InsertCallback()
					//     NV097_SET_ZSTENCIL_CLEAR_VALUE 0x00001D8C = NVX_READ_CALLBACK Data 
					//     NV097_SET_COLOR_CLEAR_VALUE 0x00001D90 = NVX_READ_CALLBACK Context, 
					// 
					// xboe d3d instruction pattern
					// (NV097_SET_ZSTENCIL_CLEAR_VALUE, method_cout=2)
					// argv[arg0, arg1]
					// (NV097_NO_OPERATION,method_cout=1 )
					// argv[Operation]


					 */
                    if (arg0 != 0) {
                        assert(!(pg->pending_interrupts & NV_PGRAPH_INTR_ERROR));

                        SET_MASK(pg->pgraph_regs[NV_PGRAPH_TRAPPED_ADDR / 4],
                            NV_PGRAPH_TRAPPED_ADDR_CHID, channel_id);
                        SET_MASK(pg->pgraph_regs[NV_PGRAPH_TRAPPED_ADDR / 4],
                            NV_PGRAPH_TRAPPED_ADDR_SUBCH, subchannel);
                        SET_MASK(pg->pgraph_regs[NV_PGRAPH_TRAPPED_ADDR / 4],
                            NV_PGRAPH_TRAPPED_ADDR_MTHD, method);
                        pg->pgraph_regs[NV_PGRAPH_TRAPPED_DATA_LOW / 4] = arg0;
                        pg->pgraph_regs[NV_PGRAPH_NSOURCE / 4] = NV_PGRAPH_NSOURCE_NOTIFICATION; /* TODO: check this */
                        pg->pending_interrupts |= NV_PGRAPH_INTR_ERROR;

                        qemu_mutex_unlock(&pg->pgraph_lock);
                        qemu_mutex_lock_iothread();
                        update_irq(d);
                        qemu_mutex_lock(&pg->pgraph_lock);
                        qemu_mutex_unlock_iothread();

                        while (pg->pending_interrupts & NV_PGRAPH_INTR_ERROR) {
                            qemu_cond_wait(&pg->interrupt_cond, &pg->pgraph_lock);
                        }
                    }
                    num_words_consumed = method_count; //test case: xdk pushbuffer sample. 3rd method from file is NOP with method count 0x81.
                    break;

                case NV097_WAIT_FOR_IDLE://done  //this method is used to wait for NV2A state machine to sync to pushbuffer.
                    pgraph_update_surface(d, false, true, true);
                    break;

                case NV097_SET_FLIP_READ://done  //pg->KelvinPrimitive.SetFlipRead
                    //SET_MASK(pg->pgraph_regs[NV_PGRAPH_SURFACE / 4], NV_PGRAPH_SURFACE_READ_3D,
                    //    arg0);
                    break;
                case NV097_SET_FLIP_WRITE://done  //pg->KelvinPrimitive.SetFlipWrite
                    //SET_MASK(pg->pgraph_regs[NV_PGRAPH_SURFACE / 4], NV_PGRAPH_SURFACE_WRITE_3D,
                    //    arg0);
                    break; 
                case NV097_SET_FLIP_MODULO://done  //pg->KelvinPrimitive.SetFlipModulo
                    //SET_MASK(pg->pgraph_regs[NV_PGRAPH_SURFACE / 4], NV_PGRAPH_SURFACE_MODULO_3D,
                    //    arg0);
                    break;
                case NV097_FLIP_INCREMENT_WRITE: {//done
                    NV2A_DPRINTF("flip increment write %d -> ",
						pg->KelvinPrimitive.SetFlipWrite);
					pg->KelvinPrimitive.SetFlipWrite=
                        (pg->KelvinPrimitive.SetFlipWrite + 1)
                        % pg->KelvinPrimitive.SetFlipModulo;
                    NV2A_DPRINTF("%d\n",
						pg->KelvinPrimitive.SetFlipWrite);
        #ifdef __APPLE__
                    if (glFrameTerminatorGREMEDY) {
                        glFrameTerminatorGREMEDY();
						}
        #endif // __APPLE__
						break;
					}
				case NV097_FLIP_STALL://done
                    pgraph_update_surface(d, false, true, true);
                    // TODO: Fix this (why does it hang?)
                    /* while (true) */ {
                        //uint32_t surface = pg->pgraph_regs[NV_PGRAPH_SURFACE / 4];
                        NV2A_DPRINTF("flip stall read: %d, write: %d, modulo: %d\n",
							pg->KelvinPrimitive.SetFlipRead,
							pg->KelvinPrimitive.SetFlipWrite,
							pg->KelvinPrimitive.SetFlipModulo);

                        if (pg->KelvinPrimitive.SetFlipRead
                            != pg->KelvinPrimitive.SetFlipWrite) {
                            break;
                        }
                    }
                    // TODO: Remove this when the AMD crash is solved in vblank_thread
                    NV2ADevice::UpdateHostDisplay(d);
                    NV2A_DPRINTF("flip stall done\n");
                    break;

                case NV097_SET_CONTEXT_DMA_NOTIFIES://done
                    //use pg->KelvinPrimitive.SetContextDmaNotifies
                    //pg->dma_notifies = arg0;
                    break;
                case NV097_SET_CONTEXT_DMA_A://done
                    //pg->KelvinPrimitive.SetContextDmaA
                    //pg->dma_a = arg0;
                    break;
                case NV097_SET_CONTEXT_DMA_B://done
                    //pg->KelvinPrimitive.SetContextDmaB
                    //pg->dma_b = arg0;
                    break;
                case NV097_SET_CONTEXT_DMA_STATE://done
                    //KelvinPrimitive.SetContextDmaState
                    //pg->dma_state = arg0;
                    break;
                case NV097_SET_CONTEXT_DMA_COLOR://done
                    /* try to get any straggling draws in before the surface's changed :/ */
                    pgraph_update_surface(d, false, true, true);
                    //KelvinPrimitive.SetContextDmaColor
                    //pg->dma_color = arg0;
                    break;
                case NV097_SET_CONTEXT_DMA_ZETA://done
                    //KelvinPrimitive.SetContextDmaZeta
                    //pg->dma_zeta = arg0;
                    break;
                case NV097_SET_CONTEXT_DMA_VERTEX_A://done
                    //KelvinPrimitive.SetContextDmaVertexA
                    //pg->dma_vertex_a = arg0;
                    break;
                case NV097_SET_CONTEXT_DMA_VERTEX_B://done
                    //KelvinPrimitive.SetContextDmaVertexB
                    //pg->dma_vertex_b = arg0;
                    break;
                case NV097_SET_CONTEXT_DMA_SEMAPHORE://done
                    //KelvinPrimitive.SetContextDmaSemaphore
                    //pg->dma_semaphore = arg0;
                    break;
                case NV097_SET_CONTEXT_DMA_REPORT://done
                    //KelvinPrimitive.SetContextDmaReport
                    //pg->dma_report = arg0;
                    break;

                case NV097_SET_SURFACE_CLIP_HORIZONTAL://done KelvinPrimitive.SetSurfaceClipHorizontal could use union with word ClipX, ClipWidth
                    pgraph_update_surface(d, false, true, true);
                    pg->surface_shape.clip_x =
                        GET_MASK(arg0, NV097_SET_SURFACE_CLIP_HORIZONTAL_X);
                    pg->surface_shape.clip_width =
                        GET_MASK(arg0, NV097_SET_SURFACE_CLIP_HORIZONTAL_WIDTH);
                    break;
                case NV097_SET_SURFACE_CLIP_VERTICAL://done KelvinPrimitive.SetSurfaceClipVertical could use union with word ClipY, ClipHeight
                    pgraph_update_surface(d, false, true, true);
                    pg->surface_shape.clip_y =
                        GET_MASK(arg0, NV097_SET_SURFACE_CLIP_VERTICAL_Y);
                    pg->surface_shape.clip_height =
                        GET_MASK(arg0, NV097_SET_SURFACE_CLIP_VERTICAL_HEIGHT);
                    break;
                case NV097_SET_SURFACE_FORMAT://done
                    pgraph_update_surface(d, false, true, true);

                    pg->surface_shape.color_format =
                        GET_MASK(arg0, NV097_SET_SURFACE_FORMAT_COLOR);
                    pg->surface_shape.zeta_format =
                        GET_MASK(arg0, NV097_SET_SURFACE_FORMAT_ZETA);
                    pg->surface_type =
                        GET_MASK(arg0, NV097_SET_SURFACE_FORMAT_TYPE);
                    pg->surface_shape.anti_aliasing =
                        GET_MASK(arg0, NV097_SET_SURFACE_FORMAT_ANTI_ALIASING);
                    pg->surface_shape.log_width =
                        GET_MASK(arg0, NV097_SET_SURFACE_FORMAT_WIDTH);
                    pg->surface_shape.log_height =
                        GET_MASK(arg0, NV097_SET_SURFACE_FORMAT_HEIGHT);
                    break;
                case NV097_SET_SURFACE_PITCH://done
                    pgraph_update_surface(d, false, true, true);

                    pg->surface_color.pitch =
                        GET_MASK(arg0, NV097_SET_SURFACE_PITCH_COLOR);
                    pg->surface_zeta.pitch =
                        GET_MASK(arg0, NV097_SET_SURFACE_PITCH_ZETA);
                    pg->surface_color.buffer_dirty = true;
                    pg->surface_zeta.buffer_dirty = true;
                    break;
                case NV097_SET_SURFACE_COLOR_OFFSET://done
                    pgraph_update_surface(d, false, true, true);

                    pg->surface_color.offset = arg0;
                    pg->surface_color.buffer_dirty = true;
                    break;
                case NV097_SET_SURFACE_ZETA_OFFSET://done
                    pgraph_update_surface(d, false, true, true);

                    pg->surface_zeta.offset = arg0;
                    pg->surface_zeta.buffer_dirty = true;
                    break;

                CASE_8(NV097_SET_COMBINER_ALPHA_ICW, 4) ://done
                        //slot = (method - NV097_SET_COMBINER_ALPHA_ICW) / 4;
                    //pg->pgraph_regs[NV_PGRAPH_COMBINEALPHAI0/4 + slot * 4] = parameter;
                    break;

                case NV097_SET_COMBINER_SPECULAR_FOG_CW0://done
                    //pg->pgraph_regs[NV_PGRAPH_COMBINESPECFOG0/4] = parameter;
                    break;

                case NV097_SET_COMBINER_SPECULAR_FOG_CW1://done
                    //pg->pgraph_regs[NV_PGRAPH_COMBINESPECFOG1/4] = parameter;
                    break;

                case NV097_SET_CONTROL0: {//done  //pg->KelvinPrimitive.SetControl0& NV097_SET_CONTROL0_COLOR_SPACE_CONVERT GET_MASK(pg->KelvinPrimitive.SetControl0, NV097_SET_CONTROL0_COLOR_SPACE_CONVERT)
                    pgraph_update_surface(d, false, true, true);

                    //bool stencil_write_enable =
                    //	arg0 & NV097_SET_CONTROL0_STENCIL_WRITE_ENABLE;
                    //SET_MASK(pg->pgraph_regs[NV_PGRAPH_CONTROL_0 / 4],
                    //	NV_PGRAPH_CONTROL_0_STENCIL_WRITE_ENABLE,
                    //	stencil_write_enable);

                    //uint32_t z_format = GET_MASK(arg0, NV097_SET_CONTROL0_Z_FORMAT);//GET_MASK(pg->KelvinPrimitive.SetControl0, NV097_SET_CONTROL0_Z_FORMAT)
                    //SET_MASK(pg->pgraph_regs[NV_PGRAPH_SETUPRASTER / 4],
                    //	NV_PGRAPH_SETUPRASTER_Z_FORMAT, z_format);

                    //bool z_perspective =
                    //	arg0 & NV097_SET_CONTROL0_Z_PERSPECTIVE_ENABLE;
                    //SET_MASK(pg->pgraph_regs[NV_PGRAPH_CONTROL_0 / 4],
                    //	NV_PGRAPH_CONTROL_0_Z_PERSPECTIVE_ENABLE,
                    //	z_perspective);

                    //int color_space_convert =
                    //	GET_MASK(arg0, NV097_SET_CONTROL0_COLOR_SPACE_CONVERT);
                    //SET_MASK(pg->pgraph_regs[NV_PGRAPH_CONTROL_0 / 4],
                    //	NV_PGRAPH_CONTROL_0_CSCONVERT,
                    //	color_space_convert);
                    break;
                }

                case NV097_SET_COLOR_MATERIAL: {//done
                    //SET_MASK(pg->pgraph_regs[NV_PGRAPH_CSV0_C / 4], NV_PGRAPH_CSV0_C_EMISSION,  //(pg->KelvinPrimitive.SetColorMaterial >> 0) & 3)
                    //	(arg0 >> 0) & 3);
                    //SET_MASK(pg->pgraph_regs[NV_PGRAPH_CSV0_C / 4], NV_PGRAPH_CSV0_C_AMBIENT,  //(pg->KelvinPrimitive.SetColorMaterial >> 2) & 3)
                    //	(arg0 >> 2) & 3);
                    //SET_MASK(pg->pgraph_regs[NV_PGRAPH_CSV0_C / 4], NV_PGRAPH_CSV0_C_DIFFUSE,  //(pg->KelvinPrimitive.SetColorMaterial >> 4) & 3)
                    //	(arg0 >> 4) & 3);
                    //SET_MASK(pg->pgraph_regs[NV_PGRAPH_CSV0_C / 4], NV_PGRAPH_CSV0_C_SPECULAR,  //(pg->KelvinPrimitive.SetColorMaterial >> 6) & 3
                    //	(arg0 >> 6) & 3);
                    //break;
                }

                case NV097_SET_FOG_MODE: {//done //pg->KelvinPrimitive.SetFogMode
                    /* FIXME: There is also NV_PGRAPH_CSV0_D_FOG_MODE */
                    unsigned int mode;
                    switch (arg0) {
                    case NV097_SET_FOG_MODE_V_LINEAR:
                        mode = NV_PGRAPH_CONTROL_3_FOG_MODE_LINEAR; break;
                    case NV097_SET_FOG_MODE_V_EXP:
                        mode = NV_PGRAPH_CONTROL_3_FOG_MODE_EXP; break;
                    case NV097_SET_FOG_MODE_V_EXP2:
                        mode = NV_PGRAPH_CONTROL_3_FOG_MODE_EXP2; break;
                    case NV097_SET_FOG_MODE_V_EXP_ABS:
                        mode = NV_PGRAPH_CONTROL_3_FOG_MODE_EXP_ABS; break;
                    case NV097_SET_FOG_MODE_V_EXP2_ABS:
                        mode = NV_PGRAPH_CONTROL_3_FOG_MODE_EXP2_ABS; break;
                    case NV097_SET_FOG_MODE_V_LINEAR_ABS:
                        mode = NV_PGRAPH_CONTROL_3_FOG_MODE_LINEAR_ABS; break;
                    default:
                        assert(false);
                        mode = NV_PGRAPH_CONTROL_3_FOG_MODE_LINEAR;
                        break;
                    }
                    //SET_MASK(pg->pgraph_regs[NV_PGRAPH_CONTROL_3 / 4], NV_PGRAPH_CONTROL_3_FOG_MODE,
                    //	mode);
                    pg->KelvinPrimitive.SetFogMode = mode;
                    break;
                }
                case NV097_SET_FOG_GEN_MODE: {//done //pg->KelvinPrimitive.SetFogGenMode
                    unsigned int mode; 
                    switch (arg0) {
                    case NV097_SET_FOG_GEN_MODE_V_SPEC_ALPHA:
                        mode = FOG_MODE_LINEAR; break;
                    case NV097_SET_FOG_GEN_MODE_V_RADIAL:
                        mode = FOG_MODE_EXP; break;
                    case NV097_SET_FOG_GEN_MODE_V_PLANAR:
                        mode = FOG_MODE_ERROR2; break;
                    case NV097_SET_FOG_GEN_MODE_V_ABS_PLANAR:
                        mode = FOG_MODE_EXP2; break;
                    case NV097_SET_FOG_GEN_MODE_V_FOG_X:
                        mode = FOG_MODE_LINEAR_ABS; break;
                    default:
                        assert(false);
                        mode = FOG_MODE_LINEAR;
                        break;
                    }
                    // SET_MASK(pg->pgraph_regs[NV_PGRAPH_CSV0_D / 4], NV_PGRAPH_CSV0_D_FOGGENMODE, mode);
                    pg->KelvinPrimitive.SetFogGenMode = mode;
                    break;
                }
                case NV097_SET_FOG_ENABLE://done //pg->KelvinPrimitive.SetFogEnable
                    /*
                    FIXME: There is also:
                    SET_MASK(pg->pgraph_regs[NV_PGRAPH_CSV0_D/4], NV_PGRAPH_CSV0_D_FOGENABLE,
                    parameter);
                    */
                    //SET_MASK(pg->pgraph_regs[NV_PGRAPH_CONTROL_3 / 4], NV_PGRAPH_CONTROL_3_FOGENABLE,
                    //	arg0);
                    break;
                case NV097_SET_FOG_COLOR: {//done //pg->KelvinPrimitive.SetFogColor
                    /* parameter channels are ABGR, PGRAPH channels are ARGB */
                    uint8_t alpha = GET_MASK(arg0, NV097_SET_FOG_COLOR_ALPHA);
                    uint8_t blue = GET_MASK(arg0, NV097_SET_FOG_COLOR_BLUE);
                    uint8_t green = GET_MASK(arg0, NV097_SET_FOG_COLOR_GREEN);
                    uint8_t red = GET_MASK(arg0, NV097_SET_FOG_COLOR_RED);
					uint32_t fgcolor;
					SET_MASK(fgcolor, NV_PGRAPH_FOGCOLOR_ALPHA, alpha);
                    SET_MASK(fgcolor, NV_PGRAPH_FOGCOLOR_RED, red);
                    SET_MASK(fgcolor, NV_PGRAPH_FOGCOLOR_GREEN, green);
                    SET_MASK(fgcolor, NV_PGRAPH_FOGCOLOR_BLUE, blue);
					pg->KelvinPrimitive.SetFogColor = fgcolor;
					break;
                }

                case NV097_SET_WINDOW_CLIP_TYPE://done //pg->KelvinPrimitive.SetWindowClipType
                    //SET_MASK(pg->pgraph_regs[NV_PGRAPH_SETUPRASTER / 4],
                        //NV_PGRAPH_SETUPRASTER_WINDOWCLIPTYPE, arg0);
                    pg->KelvinPrimitive.SetWindowClipType &= 0x1;
                    break;

                case NV097_SET_ALPHA_TEST_ENABLE://D3DRS_CULLMODE //pg->KelvinPrimitive.SetAlphaTestEnable
                    //SET_MASK(pg->pgraph_regs[NV_PGRAPH_CONTROL_0 / 4],
                    //	NV_PGRAPH_CONTROL_0_ALPHATESTENABLE, arg0);
					g_pD3DDevice->SetRenderState(D3DRS_ALPHATESTENABLE, pg->KelvinPrimitive.SetAlphaTestEnable != 0);
					break;

                case NV097_SET_BLEND_ENABLE://D3DRS_ALPHABLENDENABLE //pg->KelvinPrimitive.SetBlendEnable
                    //SET_MASK(pg->pgraph_regs[NV_PGRAPH_BLEND / 4], NV_PGRAPH_BLEND_EN, arg0);
					g_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, pg->KelvinPrimitive.SetBlendEnable != 0);
					break;

                case NV097_SET_CULL_FACE_ENABLE://D3DRS_CULLMODE //pg->KelvinPrimitive.SetCullFaceEnable
                    //SET_MASK(pg->pgraph_regs[NV_PGRAPH_SETUPRASTER / 4],
                    //	NV_PGRAPH_SETUPRASTER_CULLENABLE,
                    //	arg0);
					g_pD3DDevice->SetRenderState(D3DRS_CULLMODE, pg->KelvinPrimitive.SetCullFaceEnable != 0);
					break;
                case NV097_SET_DEPTH_TEST_ENABLE://done //pg->KelvinPrimitive.SetDepthTestEnable
                    // Test-case : Whiplash
                    //SET_MASK(pg->pgraph_regs[NV_PGRAPH_CONTROL_0 / 4], NV_PGRAPH_CONTROL_0_ZENABLE,
                    //	arg0);
                    break;
                case NV097_SET_DITHER_ENABLE://done //pg->KelvinPrimitive.SetDitherEnable
                    //SET_MASK(pg->pgraph_regs[NV_PGRAPH_CONTROL_0 / 4],
                    //	NV_PGRAPH_CONTROL_0_DITHERENABLE, arg0);
                    break;
                case NV097_SET_LIGHTING_ENABLE://done //pg->KelvinPrimitive.SetLightingEnable
                    //SET_MASK(pg->pgraph_regs[NV_PGRAPH_CSV0_C / 4], NV_PGRAPH_CSV0_C_LIGHTING,
                    //	arg0);
					g_pD3DDevice->SetRenderState(D3DRS_LIGHTING, pg->KelvinPrimitive.SetLightingEnable != 0);
					break;
                case NV097_SET_POINT_PARAMS_ENABLE://done //pg->KelvinPrimitive.SetPointParamsEnable
                    //this state is not used yet.
                    break;
                case NV097_SET_POINT_SMOOTH_ENABLE://done //pg->KelvinPrimitive.SetPointSmoothEnable
                    //this state is not used yet.
                    break;
                case NV097_SET_LINE_SMOOTH_ENABLE://done //pg->KelvinPrimitive.SetLineSmoothEnable
                    //SET_MASK(pg->pgraph_regs[NV_PGRAPH_SETUPRASTER / 4],
                    //	NV_PGRAPH_SETUPRASTER_LINESMOOTHENABLE, arg0);
                    break;
                case NV097_SET_POLY_SMOOTH_ENABLE://done //pg->KelvinPrimitive.SetPolySmoothEnable
                    //SET_MASK(pg->pgraph_regs[NV_PGRAPH_SETUPRASTER / 4],
                    //	NV_PGRAPH_SETUPRASTER_POLYSMOOTHENABLE, arg0);
                    break;
                case NV097_SET_SKIN_MODE://done //pg->KelvinPrimitive.SetSkinMode
                    //SET_MASK(pg->pgraph_regs[NV_PGRAPH_CSV0_D / 4], NV_PGRAPH_CSV0_D_SKIN,
                    //	arg0);
                    break;
                case NV097_SET_STENCIL_TEST_ENABLE://done //pg->KelvinPrimitive.SetStencilTestEnable
                    //SET_MASK(pg->pgraph_regs[NV_PGRAPH_CONTROL_1 / 4],
                    //	NV_PGRAPH_CONTROL_1_STENCIL_TEST_ENABLE, arg0);
                    break;
                case NV097_SET_POLY_OFFSET_POINT_ENABLE://done //pg->KelvinPrimitive.SetPolyOffsetPointEnable
                    //SET_MASK(pg->pgraph_regs[NV_PGRAPH_SETUPRASTER / 4],
                    //	NV_PGRAPH_SETUPRASTER_POFFSETPOINTENABLE, arg0);
                    break;
                case NV097_SET_POLY_OFFSET_LINE_ENABLE://done //pg->KelvinPrimitive.SetPolyOffsetLineEnable
                    //SET_MASK(pg->pgraph_regs[NV_PGRAPH_SETUPRASTER / 4],
                    //	NV_PGRAPH_SETUPRASTER_POFFSETLINEENABLE, arg0);
                    break;
                case NV097_SET_POLY_OFFSET_FILL_ENABLE://done //pg->KelvinPrimitive.SetPolyOffsetFillEnable
                    //SET_MASK(pg->pgraph_regs[NV_PGRAPH_SETUPRASTER / 4],
                    //	NV_PGRAPH_SETUPRASTER_POFFSETFILLENABLE, arg0);
                    break;
                case NV097_SET_ALPHA_FUNC://done //pg->KelvinPrimitive.SetAlphaFunc
                    //SET_MASK(pg->pgraph_regs[NV_PGRAPH_CONTROL_0 / 4],
                    //	NV_PGRAPH_CONTROL_0_ALPHAFUNC, arg0 & 0xF);
                    pg->KelvinPrimitive.SetAlphaFunc &= 0xF;
                    break;
                case NV097_SET_ALPHA_REF://done //pg->KelvinPrimitive.SetAlphaRef
                    //SET_MASK(pg->pgraph_regs[NV_PGRAPH_CONTROL_0 / 4],
                    //	NV_PGRAPH_CONTROL_0_ALPHAREF, arg0);
                    pg->KelvinPrimitive.SetAlphaRef &= 0xFF;
                    break;
                case NV097_SET_BLEND_FUNC_SFACTOR: {//done //pg->KelvinPrimitive.SetBlendFuncSfactor
                    unsigned int factor=arg0;
                    if (factor > 15) {
                        fprintf(stderr, "Unknown blend source factor: 0x%x, reset to NV_PGRAPH_BLEND_SFACTOR_ZERO\n", arg0);
                        //assert(false);
                        //set factor a default value, even this is not supposed to happen.
						// pushbuffer sample using method 304 with arg0 0x302.
						pg->KelvinPrimitive.SetBlendFuncSfactor= factor & 0x0F;
                    }
                    //SET_MASK(pg->pgraph_regs[NV_PGRAPH_BLEND / 4], NV_PGRAPH_BLEND_SFACTOR, factor);
                    break;
                }

                case NV097_SET_BLEND_FUNC_DFACTOR: {//done //pg->KelvinPrimitive.SetBlendFuncDfactor
                    unsigned int factor=arg0;
                    switch (arg0) {
                    case NV097_SET_BLEND_FUNC_DFACTOR_V_ZERO:
                        factor = NV_PGRAPH_BLEND_DFACTOR_ZERO; break;
                    case NV097_SET_BLEND_FUNC_DFACTOR_V_ONE:
                        factor = NV_PGRAPH_BLEND_DFACTOR_ONE; break;
                    case NV097_SET_BLEND_FUNC_DFACTOR_V_SRC_COLOR:
                        factor = NV_PGRAPH_BLEND_DFACTOR_SRC_COLOR; break;
                    case NV097_SET_BLEND_FUNC_DFACTOR_V_ONE_MINUS_SRC_COLOR:
                        factor = NV_PGRAPH_BLEND_DFACTOR_ONE_MINUS_SRC_COLOR; break;
                    case NV097_SET_BLEND_FUNC_DFACTOR_V_SRC_ALPHA:
                        factor = NV_PGRAPH_BLEND_DFACTOR_SRC_ALPHA; break;
                    case NV097_SET_BLEND_FUNC_DFACTOR_V_ONE_MINUS_SRC_ALPHA:
                        factor = NV_PGRAPH_BLEND_DFACTOR_ONE_MINUS_SRC_ALPHA; break;
                    case NV097_SET_BLEND_FUNC_DFACTOR_V_DST_ALPHA:
                        factor = NV_PGRAPH_BLEND_DFACTOR_DST_ALPHA; break;
                    case NV097_SET_BLEND_FUNC_DFACTOR_V_ONE_MINUS_DST_ALPHA:
                        factor = NV_PGRAPH_BLEND_DFACTOR_ONE_MINUS_DST_ALPHA; break;
                    case NV097_SET_BLEND_FUNC_DFACTOR_V_DST_COLOR:
                        factor = NV_PGRAPH_BLEND_DFACTOR_DST_COLOR; break;
                    case NV097_SET_BLEND_FUNC_DFACTOR_V_ONE_MINUS_DST_COLOR:
                        factor = NV_PGRAPH_BLEND_DFACTOR_ONE_MINUS_DST_COLOR; break;
                    case NV097_SET_BLEND_FUNC_DFACTOR_V_SRC_ALPHA_SATURATE:
                        factor = NV_PGRAPH_BLEND_DFACTOR_SRC_ALPHA_SATURATE; break;
                    case NV097_SET_BLEND_FUNC_DFACTOR_V_CONSTANT_COLOR:
                        factor = NV_PGRAPH_BLEND_DFACTOR_CONSTANT_COLOR; break;
                    case NV097_SET_BLEND_FUNC_DFACTOR_V_ONE_MINUS_CONSTANT_COLOR:
                        factor = NV_PGRAPH_BLEND_DFACTOR_ONE_MINUS_CONSTANT_COLOR; break;
                    case NV097_SET_BLEND_FUNC_DFACTOR_V_CONSTANT_ALPHA:
                        factor = NV_PGRAPH_BLEND_DFACTOR_CONSTANT_ALPHA; break;
                    case NV097_SET_BLEND_FUNC_DFACTOR_V_ONE_MINUS_CONSTANT_ALPHA:
                        factor = NV_PGRAPH_BLEND_DFACTOR_ONE_MINUS_CONSTANT_ALPHA; break;
                    default:
                        factor=NV_PGRAPH_BLEND_DFACTOR_ZERO;
                        fprintf(stderr, "Unknown blend destination factor: 0x%x\n", arg0);
                        assert(false);
                        break;
                    }
                    //SET_MASK(pg->pgraph_regs[NV_PGRAPH_BLEND / 4], NV_PGRAPH_BLEND_DFACTOR, factor);
                    pg->KelvinPrimitive.SetBlendFuncDfactor = factor;
                    break;
                }

                case NV097_SET_BLEND_COLOR://done //pg->KelvinPrimitive.SetBlendColor
                    //pg->pgraph_regs[NV_PGRAPH_BLENDCOLOR/4] = parameter;
                    break;

                case NV097_SET_BLEND_EQUATION: {//done //pg->KelvinPrimitive.SetBlendEquation
                    unsigned int equation;
                    switch (arg0) {
                    case NV097_SET_BLEND_EQUATION_V_FUNC_SUBTRACT:
                        equation = 0; break;
                    case NV097_SET_BLEND_EQUATION_V_FUNC_REVERSE_SUBTRACT:
                        equation = 1; break;
                    case NV097_SET_BLEND_EQUATION_V_FUNC_ADD:
                        equation = 2; break;
                    case NV097_SET_BLEND_EQUATION_V_MIN:
                        equation = 3; break;
                    case NV097_SET_BLEND_EQUATION_V_MAX:
                        equation = 4; break;
                    case NV097_SET_BLEND_EQUATION_V_FUNC_REVERSE_SUBTRACT_SIGNED:
                        equation = 5; break;
                    case NV097_SET_BLEND_EQUATION_V_FUNC_ADD_SIGNED:
                        equation = 6; break;
                    default:
                        assert(false);
                        equation = 0;
                        break;
                    }
                    pg->KelvinPrimitive.SetBlendEquation = equation;
                    //SET_MASK(pg->pgraph_regs[NV_PGRAPH_BLEND / 4], NV_PGRAPH_BLEND_EQN, equation);

                    break;
                }

                case NV097_SET_DEPTH_FUNC://done //pg->KelvinPrimitive.SetDepthFunc
                    // Test-case : Whiplash
                    //SET_MASK(pg->pgraph_regs[NV_PGRAPH_CONTROL_0 / 4], NV_PGRAPH_CONTROL_0_ZFUNC,
                    //	arg0 & 0xF);
                    pg->KelvinPrimitive.SetDepthFunc &= 0x0F;
                    break;

                case NV097_SET_COLOR_MASK: {//done //pg->KelvinPrimitive.SetColorMask
                    pg->surface_color.write_enabled_cache |= pgraph_get_color_write_enabled(pg);

                    bool alpha = arg0 & NV097_SET_COLOR_MASK_ALPHA_WRITE_ENABLE;
                    bool red = arg0 & NV097_SET_COLOR_MASK_RED_WRITE_ENABLE;
                    bool green = arg0 & NV097_SET_COLOR_MASK_GREEN_WRITE_ENABLE;
                    bool blue = arg0 & NV097_SET_COLOR_MASK_BLUE_WRITE_ENABLE;
                    //SET_MASK(pg->pgraph_regs[NV_PGRAPH_CONTROL_0 / 4],
                    //	NV_PGRAPH_CONTROL_0_ALPHA_WRITE_ENABLE, alpha);
                    //SET_MASK(pg->pgraph_regs[NV_PGRAPH_CONTROL_0 / 4],
                    //	NV_PGRAPH_CONTROL_0_RED_WRITE_ENABLE, red);
                    //SET_MASK(pg->pgraph_regs[NV_PGRAPH_CONTROL_0 / 4],
                    //	NV_PGRAPH_CONTROL_0_GREEN_WRITE_ENABLE, green);
                    //SET_MASK(pg->pgraph_regs[NV_PGRAPH_CONTROL_0 / 4],
                    //	NV_PGRAPH_CONTROL_0_BLUE_WRITE_ENABLE, blue);
                    break;
                }
                case NV097_SET_DEPTH_MASK://done //pg->KelvinPrimitive.SetDepthMask
                    pg->surface_zeta.write_enabled_cache |= pgraph_get_zeta_write_enabled(pg);

                    //SET_MASK(pg->pgraph_regs[NV_PGRAPH_CONTROL_0 / 4],
                    //	NV_PGRAPH_CONTROL_0_ZWRITEENABLE, arg0);
                    break;
                case NV097_SET_STENCIL_MASK://done //pg->KelvinPrimitive.SetStencilMask
                    //SET_MASK(pg->pgraph_regs[NV_PGRAPH_CONTROL_1 / 4],
                    //	NV_PGRAPH_CONTROL_1_STENCIL_MASK_WRITE, arg0);
                    pg->KelvinPrimitive.SetStencilMask &= 0xFF;
                    break;
                case NV097_SET_STENCIL_FUNC://done //pg->KelvinPrimitive.SetStencilFunc
                    //SET_MASK(pg->pgraph_regs[NV_PGRAPH_CONTROL_1 / 4],
                        //NV_PGRAPH_CONTROL_1_STENCIL_FUNC, arg0 & 0xF);
                        pg->KelvinPrimitive.SetStencilFunc &= 0x0F;
                    break;
                case NV097_SET_STENCIL_FUNC_REF://done //pg->KelvinPrimitive.SetStencilFuncRef
                    //SET_MASK(pg->pgraph_regs[NV_PGRAPH_CONTROL_1 / 4],
                    //	NV_PGRAPH_CONTROL_1_STENCIL_REF, arg0);
                    break;
                case NV097_SET_STENCIL_FUNC_MASK://done //pg->KelvinPrimitive.SetStencilFuncMask
                    //SET_MASK(pg->pgraph_regs[NV_PGRAPH_CONTROL_1 / 4],
                        //NV_PGRAPH_CONTROL_1_STENCIL_MASK_READ, arg0);
                    pg->KelvinPrimitive.SetStencilFuncMask & 0xFF;
                    break;
                case NV097_SET_STENCIL_OP_FAIL://done //pg->KelvinPrimitive.SetStencilOpFail
                    //SET_MASK(pg->pgraph_regs[NV_PGRAPH_CONTROL_2 / 4],
                        //NV_PGRAPH_CONTROL_2_STENCIL_OP_FAIL,
                        pg->KelvinPrimitive.SetStencilOpFail &= 0x0F;
                        kelvin_map_stencil_op(pg->KelvinPrimitive.SetStencilOpFail);
                    break;
                case NV097_SET_STENCIL_OP_ZFAIL://done //pg->KelvinPrimitive.SetStencilOpZfail
                    //SET_MASK(pg->pgraph_regs[NV_PGRAPH_CONTROL_2 / 4],
                        //NV_PGRAPH_CONTROL_2_STENCIL_OP_ZFAIL,
                        pg->KelvinPrimitive.SetStencilOpZfail &= 0xF;
                        kelvin_map_stencil_op(pg->KelvinPrimitive.SetStencilOpZfail);
                    break;
                case NV097_SET_STENCIL_OP_ZPASS://done //pg->KelvinPrimitive.SetStencilOpZpass
                    //SET_MASK(pg->pgraph_regs[NV_PGRAPH_CONTROL_2 / 4],
                        //NV_PGRAPH_CONTROL_2_STENCIL_OP_ZPASS,
                        pg->KelvinPrimitive.SetStencilOpZpass&= 0xF;
                        kelvin_map_stencil_op(pg->KelvinPrimitive.SetStencilOpZpass);
                    break;

                case NV097_SET_SHADE_MODE:break;//done //pg->KelvinPrimitive.SetShadeMode

                case NV097_SET_LINE_WIDTH:break;//done //pg->KelvinPrimitive.SetLineWidth

                case NV097_SET_POLYGON_OFFSET_SCALE_FACTOR:break;//done //pg->KelvinPrimitive.SetPolygonOffsetScaleFactor

                case NV097_SET_POLYGON_OFFSET_BIAS: break;//done //pg->KelvinPrimitive.SetPolygonOffsetBias

                case NV097_SET_FRONT_POLYGON_MODE://done //pg->KelvinPrimitive.SetFrontPolygonMode
                    //SET_MASK(pg->pgraph_regs[NV_PGRAPH_SETUPRASTER / 4],
                    //	NV_PGRAPH_SETUPRASTER_FRONTFACEMODE,
                    //	kelvin_map_polygon_mode(arg0));
                    pg->KelvinPrimitive.SetFrontPolygonMode = kelvin_map_polygon_mode(pg->KelvinPrimitive.SetFrontPolygonMode);
                    break;
                case NV097_SET_BACK_POLYGON_MODE://done //pg->KelvinPrimitive.SetBackPolygonMode
                    //SET_MASK(pg->pgraph_regs[NV_PGRAPH_SETUPRASTER / 4],
                    //	NV_PGRAPH_SETUPRASTER_BACKFACEMODE,
                    //	kelvin_map_polygon_mode(arg0));
                    pg->KelvinPrimitive.SetBackPolygonMode = kelvin_map_polygon_mode(pg->KelvinPrimitive.SetBackPolygonMode);
                    break;
                case NV097_SET_CLIP_MIN:break;//done //pg->KelvinPrimitive.SetClipMin

                case NV097_SET_CLIP_MAX:break;//done //pg->KelvinPrimitive.SetClipMax

                case NV097_SET_CULL_FACE: {//done //pg->KelvinPrimitive.SetCullFace
                    unsigned int face=arg0;
                    switch (arg0) {
                    case NV097_SET_CULL_FACE_V_FRONT:
                        face = NV_PGRAPH_SETUPRASTER_CULLCTRL_FRONT; break;
                    case NV097_SET_CULL_FACE_V_BACK:
                        face = NV_PGRAPH_SETUPRASTER_CULLCTRL_BACK; break;
                    case NV097_SET_CULL_FACE_V_FRONT_AND_BACK:
                        face = NV_PGRAPH_SETUPRASTER_CULLCTRL_FRONT_AND_BACK; break;
                    default:
                        assert(false);
                        face = NV097_SET_CULL_FACE_V_FRONT;
                        break;
                    }
                    //SET_MASK(pg->pgraph_regs[NV_PGRAPH_SETUPRASTER / 4],
                    //	NV_PGRAPH_SETUPRASTER_CULLCTRL,
                    //	face);
                    pg->KelvinPrimitive.SetCullFace = face;
                    break;
                }
                case NV097_SET_FRONT_FACE: {//done //pg->KelvinPrimitive.SetFrontFace
                    bool ccw;
                    switch (arg0) {
                    case NV097_SET_FRONT_FACE_V_CW:
                        ccw = false; break;
                    case NV097_SET_FRONT_FACE_V_CCW:
                        ccw = true; break;
                    default:
                        ccw = false; 
                        fprintf(stderr, "Unknown front face: 0x%x\n", arg0);
                        assert(false);
                        break;
                    }
                    //SET_MASK(pg->pgraph_regs[NV_PGRAPH_SETUPRASTER / 4],
                    //	NV_PGRAPH_SETUPRASTER_FRONTFACE,
                    //	ccw ? 1 : 0);
                    pg->KelvinPrimitive.SetFrontFace = ccw ? 1 : 0;
                    break;
                }
                case NV097_SET_NORMALIZATION_ENABLE://done //pg->KelvinPrimitive.SetNormalizationEnable
                    //SET_MASK(pg->pgraph_regs[NV_PGRAPH_CSV0_C / 4],
                    //	NV_PGRAPH_CSV0_C_NORMALIZATION_ENABLE,
                    //	arg0);
                    break;

                case NV097_SET_MATERIAL_EMISSION : break;//done //pg->KelvinPrimitive.SetMaterialEmission[3]
                case NV097_SET_MATERIAL_ALPHA : break;//done //pg->KelvinPrimitive.SetMaterialAlpha
                case NV097_SET_SPECULAR_ENABLE : break;//done //pg->KelvinPrimitive.SetSpecularEnable

                case NV097_SET_LIGHT_ENABLE_MASK://done //pg->KelvinPrimitive.SetLightEnableMask
                    //SET_MASK(pg->pgraph_regs[NV_PGRAPH_CSV0_D / 4], NV_PGRAPH_CSV0_D_LIGHTS, arg0);
                    break;

                CASE_4(NV097_SET_TEXGEN_S, 16) : {//done //pg->KelvinPrimitive.SetTexgen[2].S  {S,T,R,Q}
                        slot = (method - NV097_SET_TEXGEN_S) / 16; //slot is 0 or 1
                        //unsigned int reg = (slot <2) ? NV_PGRAPH_CSV1_A / 4
                        //	: NV_PGRAPH_CSV1_B / 4;
                        //unsigned int mask = (slot % 2) ? NV_PGRAPH_CSV1_A_T1_S
                        //	: NV_PGRAPH_CSV1_A_T0_S;
                        //SET_MASK(pg->pgraph_regs[NV_PGRAPH_CSV1_A / 4], mask, kelvin_map_texgen(arg0, 0));
                        pg->KelvinPrimitive.SetTexgen[slot].S = kelvin_map_texgen(arg0, 0);
                        break;
                    }
                CASE_4(NV097_SET_TEXGEN_T, 16) : {//done //pg->KelvinPrimitive.SetTexgen[2].T  {S,T,R,Q}
                        slot = (method - NV097_SET_TEXGEN_T) / 16;
                        //unsigned int reg = (slot < 2) ? NV_PGRAPH_CSV1_A / 4
                        //	: NV_PGRAPH_CSV1_B / 4;
                        //unsigned int mask = (slot % 2) ? NV_PGRAPH_CSV1_A_T1_T
                        //	: NV_PGRAPH_CSV1_A_T0_T;
                        //SET_MASK(pg->pgraph_regs[NV_PGRAPH_CSV1_A / 4], mask, kelvin_map_texgen(arg0, 1));
                        pg->KelvinPrimitive.SetTexgen[slot].T = kelvin_map_texgen(arg0, 0);
                        break;
                    }
                CASE_4(NV097_SET_TEXGEN_R, 16) : {//done //pg->KelvinPrimitive.SetTexgen[2].R  {S,T,R,Q}
                        slot = (method - NV097_SET_TEXGEN_R) / 16;
                        //unsigned int reg = (slot < 2) ? NV_PGRAPH_CSV1_A / 4
                        //	: NV_PGRAPH_CSV1_B / 4;
                        //unsigned int mask = (slot % 2) ? NV_PGRAPH_CSV1_A_T1_R
                        //	: NV_PGRAPH_CSV1_A_T0_R;
                        //SET_MASK(pg->pgraph_regs[NV_PGRAPH_CSV1_A / 4], mask, kelvin_map_texgen(arg0, 2));
                        pg->KelvinPrimitive.SetTexgen[slot].R = kelvin_map_texgen(arg0, 0);
                        break;
                    }
                CASE_4(NV097_SET_TEXGEN_Q, 16) : {//done //pg->KelvinPrimitive.SetTexgen[2].Q  {S,T,R,Q}
                        slot = (method - NV097_SET_TEXGEN_Q) / 16;
                        //original code uses condition slot < 2 , then NV_PGRAPH_CSV1_A will always be used.
                        //unsigned int reg = (slot < 2) ? NV_PGRAPH_CSV1_A / 4
                        //	: NV_PGRAPH_CSV1_B / 4;
                        //unsigned int mask = (slot % 2) ? NV_PGRAPH_CSV1_A_T1_Q
                        //	: NV_PGRAPH_CSV1_A_T0_Q;
                        //SET_MASK(pg->pgraph_regs[NV_PGRAPH_CSV1_A / 4], mask, kelvin_map_texgen(arg0, 3));
                        pg->KelvinPrimitive.SetTexgen[slot].Q = kelvin_map_texgen(arg0, 0);
                        break;
                    }
                CASE_4(NV097_SET_TEXTURE_MATRIX_ENABLE, 4) ://done //pg->KelvinPrimitive.SetTextureMatrix0Enable[4]
                    //slot = (method - NV097_SET_TEXTURE_MATRIX_ENABLE) / 4;
                    //pg->texture_matrix_enable[slot] = arg0;
                    break;

                case NV097_SET_POINT_SIZE: break;//done //pg->KelvinPrimitive.SetPointSize

                CASE_16(NV097_SET_PROJECTION_MATRIX, 4) : {//done //pg->KelvinPrimitive.SetProjectionMatrix[16]
                    //KelvinPrimitive.SetProjectionMatrix[] is update already. we update the vertex shader contant as well.
					slot = (method - NV097_SET_PROJECTION_MATRIX) / 4;
					for (int argc = 0; argc < method_count; argc++, slot++) {
                        arg0 = argv[argc];
                        // pg->projection_matrix[slot] = *(float*)&parameter;
                        unsigned int row = NV_IGRAPH_XF_XFCTX_PMAT0 + slot / 4;
                        pg->vsh_constants[row][slot % 4] = arg0;
                        pg->vsh_constants_dirty[row] = true;
                    }
                    break;
                }
				//Matrix transposed before pushed, always matrix 0, method count 16
                CASE_64(NV097_SET_MODEL_VIEW_MATRIX, 4) : {//done //pg->KelvinPrimitive.SetModelViewMatrix0[16] SetModelViewMatrix1[16] SetModelViewMatrix2[16] SetModelViewMatrix3[16]
                     //KelvinPrimitive.SetModelViewMatrix?[] is update already. we update the vertex shader contant as well.
					slot = (method - NV097_SET_MODEL_VIEW_MATRIX) / 4;
					for (int argc = 0; argc < method_count; argc++, slot++) {
                        arg0 = argv[argc];
                        //matnum = argc / 16;
						unsigned int matnum = slot / 16;
						unsigned int entry = slot % 16;
                        unsigned int row = NV_IGRAPH_XF_XFCTX_MMAT0 + matnum * 4 + entry / 4;
                        pg->vsh_constants[row][entry % 4] = arg0;
                        pg->vsh_constants_dirty[row] = true;
                    }
                    break;
                }
				//Matrix not transposed before pushed, always matrix 0, method count 12
                CASE_64(NV097_SET_INVERSE_MODEL_VIEW_MATRIX, 4) : {//done //pg->KelvinPrimitive.SetInverseModelViewMatrix0[16] SetInverseModelViewMatrix1[16] SetInverseModelViewMatrix2[16] SetInverseModelViewMatrix3[16]
                    //KelvinPrimitive.SetModelViewMatrix?[] is update already. we update the vertex shader contant as well.
					slot = (method - NV097_SET_INVERSE_MODEL_VIEW_MATRIX) / 4;
					for (int argc = 0; argc < method_count; argc++, slot++) {
                        arg0 = argv[argc];
						unsigned int matnum = slot / 16;
                        unsigned int entry = slot % 16;
                        unsigned int row = NV_IGRAPH_XF_XFCTX_IMMAT0 + matnum * 4 + entry / 4;
                        pg->vsh_constants[row][entry % 4] = arg0;
                        pg->vsh_constants_dirty[row] = true;
                    }
                    break;
                }
				//Matrix transposed before pushed, always matrix 0, method count 16
                CASE_16(NV097_SET_COMPOSITE_MATRIX, 4) : {//done //pg->KelvinPrimitive.SetCompositeMatrix[16]
                        //KelvinPrimitive.SetCompositeMatrix[16] is update already. we update the vertex shader contant as well.
					slot = (method - NV097_SET_COMPOSITE_MATRIX) / 4;
					for (int argc = 0; argc < method_count; argc++, slot++) {
                        arg0 = argv[argc];
                        unsigned int row = NV_IGRAPH_XF_XFCTX_CMAT0 + slot / 4;
                        pg->vsh_constants[row][slot % 4] = arg0;
                        pg->vsh_constants_dirty[row] = true;
                    }
                    break;
                }

                CASE_64(NV097_SET_TEXTURE_MATRIX, 4) : {//done //pg->KelvinPrimitive.SetInverseModelViewMatrix0[16] SetInverseModelViewMatrix1[16] SetInverseModelViewMatrix2[16] SetInverseModelViewMatrix3[16]
                    //KelvinPrimitive.SetCompositeMatrix[16] is update already. we update the vertex shader contant as well.
					slot = (method - NV097_SET_TEXTURE_MATRIX) / 4;
					for (int argc = 0; argc < method_count; argc++, slot++) {
                        arg0 = argv[argc];
                        unsigned int tex = slot / 16;
                        unsigned int entry = slot % 16;
                        unsigned int row = NV_IGRAPH_XF_XFCTX_T0MAT + tex * 8 + entry / 4;
                        pg->vsh_constants[row][entry % 4] = arg0;
                        pg->vsh_constants_dirty[row] = true;
                    }
                    break;
                }
                /* Handles NV097_SET_TEXGEN_PLANE_S,T,R,Q */ //KelvinPrimitive.SetTexgenPlane[4]::{S[4],T[4],R[4],Q[4]}
                CASE_64(NV097_SET_TEXGEN_PLANE_S, 4) : {//done //pg->KelvinPrimitive.SetTexgenPlane[i].S[j] .T[j] .R[j] .Q[j]  tex=i, entry=j
                    //KelvinPrimitive.SetCompositeMatrix[16] is update already. we update the vertex shader contant as well.
					slot = (method - NV097_SET_TEXGEN_PLANE_S) / 4;
					for (int argc = 0; argc < method_count; argc++, slot++) {
                        arg0 = argv[argc];
                        unsigned int tex = slot / 16;
                        unsigned int entry = slot % 16;
                        unsigned int row = NV_IGRAPH_XF_XFCTX_TG0MAT + tex * 8 + entry / 4;
                        pg->vsh_constants[row][entry % 4] = arg0;
                        pg->vsh_constants_dirty[row] = true;
                    }
                    break;
                }

                CASE_3(NV097_SET_FOG_PARAMS, 4) ://done //pg->KelvinPrimitive.SetFogParams[3]
                    //KelvinPrimitive.SetCompositeMatrix[16] is update already. we update the vertex shader contant as well.
					slot = (method - NV097_SET_FOG_PARAMS) / 4;
					for (int argc = 0; argc < method_count; argc++, slot++) {
                        arg0 = argv[argc];
                        
                        //pg->pgraph_regs[NV_PGRAPH_FOGPARAM0 / 4 + slot ] = arg0; //NV_PGRAPH_FOGPARAM0 remapped to NV097_SET_FOG_PARAMS
                        /* Cxbx note: slot = 2 is right after slot = 1 */
                        pg->ltctxa[NV_IGRAPH_XF_LTCTXA_FOG_K][slot] = arg0;
                        pg->ltctxa_dirty[NV_IGRAPH_XF_LTCTXA_FOG_K] = true;
                    }
                    break;

                case NV097_SET_TEXGEN_VIEW_MODEL://done //pg->KelvinPrimitive.SetTexgenViewModel
                    //SET_MASK(pg->pgraph_regs[NV_PGRAPH_CSV0_D / 4], NV_PGRAPH_CSV0_D_TEXGEN_REF,
                    //	arg0);
                    break;

                CASE_4(NV097_SET_FOG_PLANE, 4) ://done //pg->KelvinPrimitive.SetFogPlane[4]
                    //KelvinPrimitive.SetFogPlane[4] is update already. we update the vertex shader contant as well.
					slot = (method - NV097_SET_FOG_PLANE) / 4;
					for (int argc = 0; argc < method_count; argc++, slot++) {
                        arg0 = argv[argc];
                        
                        pg->vsh_constants[NV_IGRAPH_XF_XFCTX_FOG][slot] = arg0;
                        pg->vsh_constants_dirty[NV_IGRAPH_XF_XFCTX_FOG] = true;
                    }
                    break;

                CASE_6(NV097_SET_SPECULAR_PARAMS, 4) ://done //pg->KelvinPrimitive.SetSpecularParams[6]
                    //KelvinPrimitive.SetSpecularParams[6] is update already. we update the vertex shader contant as well.
					slot = (method - NV097_SET_SPECULAR_PARAMS) / 4;
					for (int argc = 0; argc < method_count; argc++, slot++) {
                        arg0 = argv[argc];
                    // this state is not implemented yet.
                        //pg->ltctxa[NV_IGRAPH_XF_LTCTXA_FR_AMB][slot] = arg0;
                        //pg->ltctxa_dirty[NV_IGRAPH_XF_LTCTXA_FR_AMB] = true;
                    }
                    break;

                case NV097_SET_SWATH_WIDTH: break;//not implement //pg->KelvinPrimitive.SetSwathWidth

                case NV097_SET_FLAT_SHADE_OP: break;//not implement //pg->KelvinPrimitive.SetFlatShadeOp

                CASE_3(NV097_SET_SCENE_AMBIENT_COLOR, 4) ://done //pg->KelvinPrimitive.SetSceneAmbientColor[3]
                    //KelvinPrimitive.SetSceneAmbientColor[3] is update already. we update the vertex shader contant as well.
					slot = (method - NV097_SET_SCENE_AMBIENT_COLOR) / 4;
					for (int argc = 0; argc < method_count; argc++, slot++) {
                        arg0 = argv[argc];
                        pg->ltctxa[NV_IGRAPH_XF_LTCTXA_FR_AMB][slot] = arg0;
                        pg->ltctxa_dirty[NV_IGRAPH_XF_LTCTXA_FR_AMB] = true;
                    }
                    break;

                CASE_4(NV097_SET_VIEWPORT_OFFSET, 4) ://done //pg->KelvinPrimitive.SetViewportOffset[4]
                    //KelvinPrimitive.SetViewportOffset[4] is update already. we update the vertex shader contant as well.
					slot = (method - NV097_SET_VIEWPORT_OFFSET) / 4;
					for (int argc = 0; argc < method_count; argc++, slot++) {
                        arg0 = argv[argc];
                        pg->vsh_constants[NV_IGRAPH_XF_XFCTX_VPOFF][slot] = arg0;
                        pg->vsh_constants_dirty[NV_IGRAPH_XF_XFCTX_VPOFF] = true;
                    }
                    break;

                CASE_8(NV097_SET_POINT_PARAMS, 4) ://done //pg->KelvinPrimitive.SetPointParams[8]
                    //KelvinPrimitive.SetPointParams[8] is update already. we update the vertex shader contant as well.
					slot = (method - NV097_SET_VIEWPORT_OFFSET) / 4;
					for (int argc = 0; argc < method_count; argc++, slot++) {
                        arg0 = argv[argc];
                    //this state is not implement yet.
                        //pg->vsh_constants[NV_IGRAPH_XF_XFCTX_VPOFF][slot] = arg0;
                        //pg->vsh_constants_dirty[NV_IGRAPH_XF_XFCTX_VPOFF] = true;
                    }
                break;

                CASE_4(NV097_SET_EYE_POSITION, 4) ://done //pg->KelvinPrimitive.SetEyePosition[4]
                    //KelvinPrimitive.SetEyePosition[4] is update already. we update the vertex shader contant as well.
					slot = (method - NV097_SET_EYE_POSITION) / 4;
					for (int argc = 0; argc < method_count; argc++, slot++) {
                        arg0 = argv[argc];
                        pg->vsh_constants[NV_IGRAPH_XF_XFCTX_EYEP][slot] = arg0;
                        pg->vsh_constants_dirty[NV_IGRAPH_XF_XFCTX_EYEP] = true;
                    }
                    break;

                CASE_8(NV097_SET_COMBINER_FACTOR0, 4) ://done //pg->KelvinPrimitive.SetCombinerFactor0[8]
                    //KelvinPrimitive.SetCombinerFactor0[8] is update already. we update the vertex shader contant as well.
                    //for (int argc = 0; argc < method_count; argc++,slot++) {
                    //	arg0 = argv[argc];
                    //	slot = (method - NV097_SET_COMBINER_FACTOR0) / 4;
                    //	pg->pgraph_regs[NV_PGRAPH_COMBINEFACTOR0/ 4 + slot * 4] = arg0;
                    //}
                    break;

                CASE_8(NV097_SET_COMBINER_FACTOR1, 4) ://done //pg->KelvinPrimitive.SetCombinerFactor1[8]
                    //	slot = (method - NV097_SET_COMBINER_FACTOR1) / 4;
                    //pg->pgraph_regs[NV_PGRAPH_COMBINEFACTOR1/ 4 + slot * 4] = arg0;
                    break;

                CASE_8(NV097_SET_COMBINER_ALPHA_OCW, 4) ://done //pg->KelvinPrimitive.SetCombinerAlphaOCW[8]
                    //	slot = (method - NV097_SET_COMBINER_ALPHA_OCW) / 4;
                    //pg->pgraph_regs[NV_PGRAPH_COMBINEALPHAO0/ 4 + slot * 4] = arg0;
                    break;

                CASE_8(NV097_SET_COMBINER_COLOR_ICW, 4) ://done //pg->KelvinPrimitive.SetCombinerColorICW[8]
                    //	slot = (method - NV097_SET_COMBINER_COLOR_ICW) / 4;
                    //pg->pgraph_regs[NV_PGRAPH_COMBINECOLORI0/ 4 + slot * 4] = arg0;
                    break;

                CASE_4(NV097_SET_COLOR_KEY_COLOR, 4) ://done //pg->KelvinPrimitive.SetColorKeyColor[4]
                    //	slot = (method - NV097_SET_COMBINER_COLOR_ICW) / 4;
                    //pg->pgraph_regs[NV_PGRAPH_COMBINECOLORI0/ 4 + slot * 4] = arg0;
                //this state is not implement yet.
                    break;

                CASE_4(NV097_SET_VIEWPORT_SCALE, 4) ://done //pg->KelvinPrimitive.SetViewportScale[4]
                    //KelvinPrimitive.SetViewportScale[4] is update already. we update the vertex shader contant as well.
					slot = (method - NV097_SET_VIEWPORT_SCALE) / 4;
					for (int argc = 0; argc < method_count; argc++, slot++) {
                        arg0 = argv[argc];
                        pg->vsh_constants[NV_IGRAPH_XF_XFCTX_VPSCL][slot] = arg0;
                        pg->vsh_constants_dirty[NV_IGRAPH_XF_XFCTX_VPSCL] = true;
                    }
                    break;
//*****************************************************************************
//NV097_SET_TRANSFORM_PROGRAM require further verification with xbox d3d setvertexshader() with actual vertex data push in pushbuffer.

				/*
				D3DDevice_SetVertexShader(Handle)
				{
					if Program Shader
						D3DDevice_LoadVertexShader(Handle, 0);
						D3DDevice_SelectVertexShader(Handle, 0);
					else //FVF fixed function
						if D3DFVF_DIFFUSE //Flags & VERTEXSHADER_HASDIFFUSE 0x400
							(NV097_SET_VERTEX_DATA4UB(SLOT_DIFFUSE), -1, method count 1) 
						if D3DFVF_SPECULAR  //Flags & VERTEXSHADER_HASSPECULAR 0x800
							(NV097_SET_VERTEX_DATA4UB(SLOT_SPECULAR), 0, method count 1)
						if D3DFVF_BACKDIFFUSE  //Flags & VERTEXSHADER_HASBACKDIFFUSE 0x1000
							(NV097_SET_VERTEX_DATA4UB(SLOT_BACK_DIFFUSE), -1, method count 1)
						if D3DFVF_BACKSPECULAR  //Flags & VERTEXSHADER_HASBACKSPECULAR 0x2000
							(NV097_SET_VERTEX_DATA4UB(SLOT_BACK_SPECULAR), 0, method count 1)
						if D3DFVF_PASSTHROUGH  //Flags & VERTEXSHADER_PASSTHROUGH 0x2
							(NV097_SET_TRANSFORM_PROGRAM_START, 0, method count 1)
							(NV097_SET_TRANSFORM_EXECUTION_MODE,
							 NV097_SET_TRANSFORM_EXECUTION_MODE_MODE_PROGRAM|NV097_SET_TRANSFORM_EXECUTION_MODE_RANGE_MODE_PRIV,
							 NV097_SET_TRANSFORM_PROGRAM_CXT_WRITE_EN_V_READ_ONLY,
							 method count 2)
							 SetupPasstrhouProgram()
								(NV097_SET_TRANSFORM_CONSTANT_LOAD, 0,method count 1)
								(NV097_SET_TRANSFORM_CONSTANT(0),method count  8)
								    if (D3DRenderState.SetFogTableMode == D3DFOG_NONE)
									{
										prog_size_dwords = sizeof(FVFPassthruProgramSpecularFog) / sizeof(DWORD) //48 dwords
										pProgram = &FVFPassthruProgramSpecularFog;
									}
									else if (pDevice->m_StateFlags & STATE_FOGSOURCEZ)
									{
										prog_size_dwords = sizeof(FVFPassthruProgramZFog) / sizeof(DWORD); //44 dwords
										pProgram = &FVFPassthruProgramZFog;
									}
									else
									{
										prog_size_dwords = sizeof(FVFPassthruProgramWFog) / sizeof(DWORD); //48 dwords
										pProgram = &FVFPassthruProgramWFog;
									}
									Scale=?(D3DRenderState.SetZEnable == D3DZB_USEW )pDevice->SetInverseWFar * pDevice->SetZScale  : 1.0f
									argv[0]=pDevice->SetSuperSampleScaleX
									argv[1]=pDevice->SetSuperSampleScaleY
									argv[2]=pDevice->SetZScale
									argv[3]=Scale
									m_off=?(D3DRenderState.SetMultiSampleAntiAlias))0.5f:0.0f
									argv[5]=pDevice->SetScreenSpaceOffsetX - m_off
									argv[6]=pDevice->SetScreenSpaceOffsetY - m_off
									argv[7]=0.0f;
									argv[8]=0.0f;
									(NV097_SET_TRANSFORM_PROGRAM_LOAD, 0, method count 1)
									prog_ind=0
									do
									    batch=MIN(32,prog_size_dwords)
									    (NV097_SET_TRANSFORM_PROGRAM(0), method count batch)
										    arg[0...batch-1]=pProgram[prog_ind+0...prog_ind+batch-1]
										prog_size_dwords - =batch
										prog_ind + =batch

									while (prog_size_dwords  !=0)


						else //not pass through
							setviewport()
								if(! FVF)
									(NV097_SET_VIEWPORT_OFFSET(0), A,   B,   C,   0.0f, method count 4)
									(NV097_SET_VIEWPORT_SCALE(0), X,Y,Z,0.0f, method count 4)
								else //FVF
									(NV097_SET_VIEWPORT_OFFSET(0),X,Y,0.0f,0.0f,method count 4)
								(NV097_SET_CLIP_MIN, ClipMin, ClipMax, method count 2)
				}

				D3DDevice_LoadVertexShader(Handle, addr=0)
				{
					(NV097_SET_TRANSFORM_PROGRAM_LOAD, addr ,method count 1)
					vertexshader.programdata_and_constants; //this data includes the push buffer command NV097_SET_TRANSFORM_PROGRAM to load following program data.
				}

				D3DDevice_SelectVertexShader(Handle, addr=0)
				{
					(NV097_SET_LIGHTING_ENABLE,0, method count 1)
				    (NV097_SET_TRANSFORM_EXECUTION_MODE,
              // NV097_SET_TRANSFORM_EXECUTION_MODE:
						(DRF_DEF(097, _SET_TRANSFORM_EXECUTION_MODE, _MODE, _PROGRAM) |  DRF_DEF(097, _SET_TRANSFORM_EXECUTION_MODE, _RANGE_MODE, _PRIV)),
              // NV097_SET_TRANSFORM_PROGRAM_CXT_WRITE_EN:
			              pVertexShader->Flags & VERTEXSHADER_WRITE (0x3),   method count 2);
					setviewport()
						if(! FVF)
							(NV097_SET_VIEWPORT_OFFSET(0), A,   B,   C,   0.0f, method count 4)
							(NV097_SET_VIEWPORT_SCALE(0), X,Y,Z,0.0f, method count 4)
						else //FVF
							(NV097_SET_VIEWPORT_OFFSET(0),X,Y,0.0f,0.0f,method count 4)
						(NV097_SET_CLIP_MIN, ClipMin, ClipMax, method count 2)

					(NV097_SET_TRANSFORM_PROGRAM_START, addr , method count 1)

				}

				try to create a vertex shader handle with feasible structure and set
				    g_Xbox_VertexShader_Handle = Handle; should work.
				HLE g_Xbox_VertexShader_Handle = xbox vertex shader handle.
				    vertex shader address = vertex shader handle - 0x01.
				HLE g_Xbox_VertexShaderMode == VertexShaderMode::FixedFunction/Passthrough/ShaderProgram
				HLE XbVertexShader.cpp::CxbxUpdateHostVertexShader()
				HLE XbVertexShader.cpp::CxbxUpdateHostVertexShaderConstants();
				HLE XbVertexShader.cpp::CxbxSetVertexShaderSlots(DWORD* pTokens, DWORD Address, DWORD NrInstructions)
				    DWORD * HLE_get_NV2A_vertex_program_register_ptr(unsigned const_index)
				    GetCxbxVertexShaderSlotPtr(Address)  // remapped to DWORD * HLE_get_NV2A_vertex_program_register_ptr(unsigned const_index)
					    return &g_Xbox_VertexShader_FunctionSlots[SlotIndexAddress * X_VSH_INSTRUCTION_SIZE];
						//remapped to return xbox::dword_xt * HLE_get_NV2A_vertex_program_register_ptr(unsigned const_index)
				HLE XbVertexShader.cpp::CxbxSetVertexShaderPassthroughProgram()
				HLE XbVertexShader.cpp::CxbxImpl_SetVertexShaderConstant(INT Register, PVOID pConstantData, DWORD ConstantCount)
					HLE_get_NV2A_vertex_constant_float4_ptr(Register)
						return &pg->vsh_constants[Register][0]

				HLE vertex program slots stored in
				    DWORD g_Xbox_VertexShader_FunctionSlots[137*4];
					pg->vsh_program_slots[136][4]
				for vertex shader program:
					HLE uses g_Xbox_VertexShader_FunctionSlots[137*4], remapped to pg->vsh_program_slots
					PGRAPH uses pg->vsh_program_slots[136][4] //extend to [137][1] and set last slot [1,1,1,1] FLD_FINAL 
				for vertex shder constants
					HLE uses pg->vsh_constants[192][4]
					PGRAPH use pg->vsh_constants[192][4]

				whenever the vertex constants are update by either side, both side see the same register.
				for program set via pushbuffer directly. we need to update to HLE side and set g_Xbox_VertexShader_Handle and g_Xbox_VertexShaderMode
				*/

				CASE_32(NV097_SET_TRANSFORM_PROGRAM, 4) : {//not done yet //pg->KelvinPrimitive.SetTransformProgram[32]
                        //KelvinPrimitive.SetTransformProgram[32] is update already. we update the extra vertex shader program as well.
						//KelvinPrimitive.SetTransformProgram[32] holds only 32 slots. for program larger than 32 slots, must be split to batches.
					    //before the first batch, NV097_SET_TRANSFORM_PROGRAM_LOAD must be called to set the beginning slot of loading.
					    //NV097_SET_TRANSFORM_PROGRAM will advanced after each execution of NV097_SET_TRANSFORM_PROGRAM.
					    //for continuous batch NV097_SET_TRANSFORM_PROGRAM methods, it will not have NV097_SET_TRANSFORM_PROGRAM_LOAD in between.
					slot = (method - NV097_SET_TRANSFORM_PROGRAM) / 4;
                    pg->KelvinPrimitive.SetTransformProgramLoad+= slot/4;
                    for (int argc = 0; argc < method_count; argc++, slot++) {
                        arg0 = argv[argc];
                        //target program register address is prestored in KelvinPrimitive.SetTransformProgramLoad
                        assert(pg->KelvinPrimitive.SetTransformProgramLoad < NV2A_MAX_TRANSFORM_PROGRAM_LENGTH);
                        //pg->KelvinPrimitive.SetTransformProgram[32] is not enough for xbox d3d, pgraph uses vsh_program_slots[136][4] to store vertex shader program
                        pg->vsh_program_slots[pg->KelvinPrimitive.SetTransformProgramLoad][argc % 4] = arg0;
                        if (argc % 4 == 3) {
                            pg->KelvinPrimitive.SetTransformProgramLoad+= + 1; //KelvinPrimitive.SetTransformProgramLoad must be advanced
                            //for single time method program set, we may leave the SET_TRANSFORM_PROGRAM_LOAD along,
                            //but if the program is large and requires multiple times of method set, the SET_TRANSFORM_PROGRAM_LOAD might need to be update.
                            //need to check the actual vertex shader data put transfered in the pushbuffer to verify.
                        }
                    }
                    break;
                }

                CASE_32(NV097_SET_TRANSFORM_CONSTANT, 4) : {//done //pg->KelvinPrimitive.SetTransformConstant[32]
                    //KelvinPrimitive.SetTransformConstant[32] is update already. we update the extra vertex shader contant array as well.
                    //because KelvinPrimitive.SetTransformConstant[32] can only hold 32 constant slots, so the max. constant slots can be set in one method is 32.
                    //for constants more than 32, must be split in batches. that is hardware limit.
                    //before calling the first NV097_SET_TRANSFORM_CONSTANT, NV097_SET_TRANSFORM_CONSTANT_LOAD must be called, or it will be used with the last set value,
                    //NV097_SET_TRANSFORM_CONSTANT_LOAD will be advanced when NV097_SET_TRANSFORM_CONSTANT was called with the the constant slots set.
                    //for continuous batch NV097_SET_TRANSFORM_CONSTANT_LOAD operation, NV097_SET_TRANSFORM_CONSTANT_LOAD only has to be set in the very first beginning.
                    slot = (method - NV097_SET_TRANSFORM_CONSTANT) / 4;
                    //slot is sopposed to be 0 here.
                    pg->KelvinPrimitive.SetTransformConstantLoad+=slot/4;
                    for (int argc = 0; argc < method_count; argc++) {
                        arg0 = argv[argc];
                        //the target constant register address is prestored in NV097_SET_TRANSFORM_CONSTANT_LOAD  KelvinPrimitive.SetTransformConstantLoad
                        assert(pg->KelvinPrimitive.SetTransformConstantLoad < NV2A_VERTEXSHADER_CONSTANTS);
                        // VertexShaderConstant *vsh_constant = &pg->vsh_constants[const_load];
                        if ((arg0 != pg->vsh_constants[pg->KelvinPrimitive.SetTransformConstantLoad][argc % 4])) {
                            pg->vsh_constants_dirty[pg->KelvinPrimitive.SetTransformConstantLoad] |= 1;
                            pg->vsh_constants[pg->KelvinPrimitive.SetTransformConstantLoad][argc % 4] = arg0;
                        }
                        //pg->KelvinPrimitive.SetTransformConstant[32] is not enough for xbox d3d, pgraph uses vsh_constants[192][4] to store vertex shader program
                        if (argc % 4 == 3) {
                            pg->KelvinPrimitive.SetTransformConstantLoad+= 1; //pg->KelvinPrimitive.SetTransformConstantLoad must be advanced.
                        }
                    }
					break;
                }

                /* Handles NV097_SET_BACK_LIGHT* */
                CASE_128(NV097_SET_BACK_LIGHT_AMBIENT_COLOR, 4): { //done  //pg->KelvinPrimitive.SetBackLight[8]. {AmbientColor[3],DiffuseColor[3],SpecularColor[3],Rev_0c24[7]}
					slot = (method - NV097_SET_BACK_LIGHT_AMBIENT_COLOR) / 4;
					for (size_t arg_c = 0; arg_c < method_count; arg_c++,slot++) {
                        arg0 = argv[arg_c];
                        unsigned int part =  slot % 16;
                        unsigned int light_index = arg_count / 16; /* [Light index], we have 8 lights, each light holds 16 dwords */
                        assert(light_index < 8);
                        switch(part) {//check the definition of pg->ltctxb, then correlate to KelvinPrimitive.SetBackLight.???
                            //CASE_3(NV097_SET_BACK_LIGHT_AMBIENT_COLOR, 4):
                            CASE_3(0, 1) ://NV097_SET_BACK_LIGHT_AMBIENT_COLOR
                                //part -= NV097_SET_BACK_LIGHT_AMBIENT_COLOR / 4;
                                pg->ltctxb[NV_IGRAPH_XF_LTCTXB_L0_BAMB + light_index *6][part] = arg0;
                                pg->ltctxb_dirty[NV_IGRAPH_XF_LTCTXB_L0_BAMB + light_index *6] = true;
                                break;
                            //CASE_3(NV097_SET_BACK_LIGHT_DIFFUSE_COLOR, 4):
                            CASE_3(3, 1) ://NV097_SET_BACK_LIGHT_DIFFUSE_COLOR
                                //part -= NV097_SET_BACK_LIGHT_DIFFUSE_COLOR / 4;
                                pg->ltctxb[NV_IGRAPH_XF_LTCTXB_L0_BDIF + light_index *6][part] = arg0;
                                pg->ltctxb_dirty[NV_IGRAPH_XF_LTCTXB_L0_BDIF + light_index *6] = true;
                                break;
                            //CASE_3(NV097_SET_BACK_LIGHT_SPECULAR_COLOR, 4):
                            CASE_3(6, 1):
                                //part -= NV097_SET_BACK_LIGHT_SPECULAR_COLOR / 4;
                                pg->ltctxb[NV_IGRAPH_XF_LTCTXB_L0_BSPC + light_index *6][part] = arg0;
                                pg->ltctxb_dirty[NV_IGRAPH_XF_LTCTXB_L0_BSPC + light_index *6] = true;
                                break;
                            default:
                                assert(false);
                                break;
                        }
                    }
                    break;
                }
                /* Handles all the light source props except for NV097_SET_BACK_LIGHT_* */
                CASE_256(NV097_SET_LIGHT_AMBIENT_COLOR, 4): {//pg->KelvinPrimitive.SetLight[8].{AmbientColor[3],DiffuseColor[3],SpecularColor[3],LocalRange,InfiniteHalfVector[3],InfiniteDirection[3],SpotFalloff[3],SpotDirection[4],LocalPosition[3],LocalAttenuation[3],Rev_1074[3]}
					slot = (method - NV097_SET_LIGHT_AMBIENT_COLOR) / 4;

                    for (size_t arg_count = 0; arg_count < method_count; arg_count++,slot++) {
                        arg0 = ldl_le_p(argv + arg_count);

                        unsigned int part = arg_count % 32;
                        unsigned int light_index = arg_count / 32; /* [Light index] */ //each light holds 32 dwords 
                        assert(light_index < 8);
                        switch(part) {//check the definition of pg->ltctxb, then correlate to KelvinPrimitive.SetBackLight.???
                            CASE_3(0, 1)://NV097_SET_LIGHT_AMBIENT_COLOR
                                //part -= NV097_SET_LIGHT_AMBIENT_COLOR / 4;
                                pg->ltctxb[NV_IGRAPH_XF_LTCTXB_L0_AMB + light_index *6][part] = arg0;
                                pg->ltctxb_dirty[NV_IGRAPH_XF_LTCTXB_L0_AMB + light_index *6] = true;
                                break;
                            CASE_3(3, 1)://NV097_SET_LIGHT_DIFFUSE_COLOR
                                //part -= NV097_SET_LIGHT_DIFFUSE_COLOR / 4;
                                pg->ltctxb[NV_IGRAPH_XF_LTCTXB_L0_DIF + light_index *6][part] = arg0;
                                pg->ltctxb_dirty[NV_IGRAPH_XF_LTCTXB_L0_DIF + light_index *6] = true;
                                break;
                            CASE_3(6, 1)://NV097_SET_LIGHT_SPECULAR_COLOR
                                //part -= NV097_SET_LIGHT_SPECULAR_COLOR / 4;
                                pg->ltctxb[NV_IGRAPH_XF_LTCTXB_L0_SPC + light_index *6][part] = arg0;
                                pg->ltctxb_dirty[NV_IGRAPH_XF_LTCTXB_L0_SPC + light_index *6] = true;
                                break;
                            case 9://NV097_SET_LIGHT_LOCAL_RANGE:
                                pg->ltc1[NV_IGRAPH_XF_LTC1_r0 + light_index][0] = arg0;
                                pg->ltc1_dirty[NV_IGRAPH_XF_LTC1_r0 + light_index] = true;
                                break;
                            CASE_3(10,1)://NV097_SET_LIGHT_INFINITE_HALF_VECTOR
                                //part -= NV097_SET_LIGHT_INFINITE_HALF_VECTOR / 4;
                                //KelvinPrimitive.SetLight[8].InfiniteHalfVector[3]
                                //pg->light_infinite_half_vector[light_index][part] = *(float*)&arg0;
                                break;
                            CASE_3(13,1)://NV097_SET_LIGHT_INFINITE_DIRECTION
                                //part -= NV097_SET_LIGHT_INFINITE_DIRECTION / 4;
                                //KelvinPrimitive.SetLight[8].InfiniteDirection[3]
                                //pg->light_infinite_direction[light_index][part] = *(float*)&arg0;
                                break;
                            CASE_3(16,1)://NV097_SET_LIGHT_SPOT_FALLOFF
                                //part -= NV097_SET_LIGHT_SPOT_FALLOFF / 4;
                                pg->ltctxa[NV_IGRAPH_XF_LTCTXA_L0_K + light_index *2][part] = arg0;
                                pg->ltctxa_dirty[NV_IGRAPH_XF_LTCTXA_L0_K + light_index *2] = true;
                                break;
                            CASE_4(19,1)://NV097_SET_LIGHT_SPOT_DIRECTION
                                //part -= NV097_SET_LIGHT_SPOT_DIRECTION / 4;
                                //KelvinPrimitive.SetLight[8].SpotDirection[4]
                                pg->ltctxa[NV_IGRAPH_XF_LTCTXA_L0_SPT + light_index *2][part] = arg0;
                                pg->ltctxa_dirty[NV_IGRAPH_XF_LTCTXA_L0_SPT + light_index *2] = true;
                                break;
                            CASE_3(23,1)://NV097_SET_LIGHT_LOCAL_POSITION
                                //part -= NV097_SET_LIGHT_LOCAL_POSITION / 4;
                                //KelvinPrimitive.SetLight[8].LocalPosition[]
                                //pg->light_local_position[light_index][part] = *(float*)&arg0;
                                break;
                            CASE_3(26,1)://NV097_SET_LIGHT_LOCAL_ATTENUATION
                                //part -= NV097_SET_LIGHT_LOCAL_ATTENUATION / 4;
                                //pg->KelvinPrimitive.SetLight[8].LocalAttenuation[3]
                                //pg->light_local_attenuation[light_index][part] = *(float*)&arg0;
                                break;
                            default:
                                //assert(false);
								//Rev_1074[3]
								break;
                        }

                    }
                    break;
                }
            //this state is not implement yet. may not be used in xbox
                case NV097_SET_STIPPLE_CONTROL:break;//not implement //pg->KelvinPrimitive.SetStippleControl
                    
            //this state is not implement yet. may not be used in xbox
                CASE_32(NV097_SET_STIPPLE_PATTERN,4):

                    break;

                CASE_3(NV097_SET_VERTEX3F, 4) : { //pg->KelvinPrimitive.SetVertex3f[3]: 
					if (pg->KelvinPrimitive.SetBeginEnd == NV097_SET_BEGIN_END_OP_END) {
						assert(0);
						break;
					}
					slot = (method - NV097_SET_VERTEX3F) / 4;
					VertexAttribute *vertex_attribute =
					&pg->vertex_attributes[slot];
					//allocate attribute.inline_buffer if it's not allocated yet.
					//pgraph_allocate_inline_buffer_vertices(pg, slot);
                    vertex_attribute->inline_value[0] = pg->KelvinPrimitive.SetVertex3f[0];
					vertex_attribute->inline_value[1] = pg->KelvinPrimitive.SetVertex3f[1];
					vertex_attribute->inline_value[2] = pg->KelvinPrimitive.SetVertex3f[2];
					vertex_attribute->inline_value[3] = 1.0f;
                    break;
                }

				CASE_4(NV097_SET_VERTEX4F, 4) :{ //pg->KelvinPrimitive.SetVertex4f[4]
					if (pg->KelvinPrimitive.SetBeginEnd == NV097_SET_BEGIN_END_OP_END) {
						assert(0);
						break;
					}
					slot = (method - NV097_SET_VERTEX4F) / 4;
					VertexAttribute *vertex_attribute =
						&pg->vertex_attributes[slot];
					//allocate attribute.inline_buffer if it's not allocated yet.
					//pgraph_allocate_inline_buffer_vertices(pg, slot);
					vertex_attribute->inline_value[0] = pg->KelvinPrimitive.SetVertex4f[0];
					vertex_attribute->inline_value[1] = pg->KelvinPrimitive.SetVertex4f[1];
					vertex_attribute->inline_value[2] = pg->KelvinPrimitive.SetVertex4f[2];
					vertex_attribute->inline_value[3] = pg->KelvinPrimitive.SetVertex4f[3];
					//set flag so when each vertex is completed, it can know which attribute is set and require to be pushed to vertex buffer.
					vertex_attribute->set_by_inline_buffer = true;
					if (slot == 0) {//D3DVSDE_POSITION =0     //slot ==0 only happended in NV097_SET_VERTEX_DATA4F
						//vertex completed, push all attributes to vertex buffer.
						pgraph_finish_inline_buffer_vertex(pg);
						pg->inline_buffer_length++;
					}
					break;
				}
#if(0)  //missing state implememtations, need verifications.

#   define NV097_SET_VERTEX4S                                 0x00001528 // [2]
#   define NV097_SET_NORMAL3F                                 0x00001530 // [3]
#   define NV097_SET_NORMAL3S                                 0x00001540 // [2]
#   define NV097_SET_DIFFUSE_COLOR4F                          0x00001550 // [4]
#   define NV097_SET_DIFFUSE_COLOR3F                          0x00001560 // [3]
#   define NV097_SET_DIFFUSE_COLOR4UB                         0x0000156C
#   define NV097_SET_SPECULAR_COLOR4F                         0x00001570 // [4]
#   define NV097_SET_SPECULAR_COLOR3F                         0x00001580 // [3]
#   define NV097_SET_SPECULAR_COLOR4UB                        0x0000158C
#   define NV097_SET_TEXCOORD0_2F                             0x00001590 // [2]
#   define NV097_SET_TEXCOORD0_2S                             0x00001598
#   define NV097_SET_TEXCOORD0_4F                             0x000015A0 // [4]
#   define NV097_SET_TEXCOORD0_4S                             0x000015B0 // [2]
#   define NV097_SET_TEXCOORD1_2F                             0x000015B8 // [2]
#   define NV097_SET_TEXCOORD1_2S                             0x000015C0
#   define NV097_SET_TEXCOORD1_4F                             0x000015C8 // [4]
#   define NV097_SET_TEXCOORD1_4S                             0x000015D8 // [2]
#   define NV097_SET_TEXCOORD2_2F                             0x000015E0 // [2]
#   define NV097_SET_TEXCOORD2_2S                             0x000015E8
#   define NV097_SET_TEXCOORD2_4F                             0x000015F0 // [4]
#   define NV097_SET_TEXCOORD2_4S                             0x00001600 // [2]
#   define NV097_SET_TEXCOORD3_2F                             0x00001608 // [2]
#   define NV097_SET_TEXCOORD3_2S                             0x00001610
#   define NV097_SET_TEXCOORD3_4F                             0x00001620 // [4]
#   define NV097_SET_TEXCOORD3_4S                             0x00001630 // [2]
#   define NV097_SET_FOG1F                                    0x00001698
#   define NV097_SET_WEIGHT1F                                 0x0000169C
#   define NV097_SET_WEIGHT2F                                 0x000016A0 // [2]
#   define NV097_SET_WEIGHT3F                                 0x000016B0 // [3]
#   define NV097_SET_EDGE_FLAG                                0x000016BC
#   define NV097_SET_WEIGHT4F                                 0x000016C0 // [4]
#   define NV097_SET_TRANSFORM_FIXED_CONST3                   0x000016D0 // [4]
#   define NV097_SET_TRANSFORM_FIXED_CONST0                   0x000016E0 // [4]
#   define NV097_SET_TRANSFORM_FIXED_CONST1                   0x000016F0 // [4]
#   define NV097_SET_TRANSFORM_FIXED_CONST2                   0x00001700 // [4]
#   define NV097_INVALIDATE_VERTEX_CACHE_FILE                 0x00001710
#   define NV097_INVALIDATE_VERTEX_FILE                       0x00001714
#   define NV097_TL_NOP                                       0x00001718
#   define NV097_TL_SYNC                                      0x0000171C


#endif
				/*
				

				HRESULT SetVertexShaderInputDirect(
					D3DVERTEXATTRIBUTEFORMAT *pVAF,
					UINT StreamCount,
					D3DSTREAM_INPUT *pStreamInputs
				);
				typedef struct{
					D3DVERTEXSHADERINPUT Input[16];
				} D3DVERTEXATTRIBUTEFORMAT;

				typedef struct{
					DWORD StreamIndex;
					DWORD Offset;
					DWORD Format;
					BYTE TessType;
					BYTE TessSource;
				} D3DVERTEXSHADERINPUT;


				typedef struct _D3DSTREAM_INPUT{  // Xbox extension
				  IDirect3DVertexBuffer8 *VertexBuffer;
				  UINT Stride;
				  UINT Offset;
				} D3DSTREAM_INPUT;


				D3DDevice_SetVertexShaderInputDirect() calls:

                (NV097_SET_VERTEX_DATA_ARRAY_OFFSET(0), 16)
                for i=0~15
                    offset[i] = pStreamInputs[pVAF.Input[i].StreamIndex].VertexBuffer->Data
                     + pStreamInputs[pVAF.Input[i].StreamIndex].Offset
                     + pVAF[i].Offset;

                    pg->KelvinPrimitive.SetVertexDataArrayOffset[i] = offset[i];

                    Format[i] = (pStreamInputs[pVAF.Input[i].StreamIndex].Stride << 8) + pVAF.Input[i].Format;

                 then calls 

                 NV097_SET_VERTEX_DATA_ARRAY_FORMAT(0), 16)
                 for i=0~15
                    pg->KelvinPrimitive.SetVertexDataFormat[i]=Format[i];

				for D3D_Draw_UP() calls, vertex are pushed to pushed buffer. there are no stream set.
				no NV097_SET_VERTEX_DATA_ARRAY_OFFSET is called, only NV097_SET_VERTEX_DATA_ARRAY_FORMAT, and no Stride set in the Format[i]

                 */

				CASE_16(NV097_SET_VERTEX_DATA_ARRAY_OFFSET, 4) : //pg->KelvinPrimitive.SetVertexDataArrayOffset[16]
				//pg->KelvinPrimitive.SetVertexDataArrayOffset[i] = vertex buffer address start + offset of Attribute [i]
					slot = (method - NV097_SET_VERTEX_DATA_ARRAY_OFFSET) / 4;
                    for (size_t argc = 0; argc < method_count; argc++, slot ++) {
                            arg0 = argv[argc];

                            pg->vertex_attributes[slot].dma_select =
                                arg0 & 0x80000000;
                            pg->vertex_attributes[slot].offset =
                                arg0 & 0x7fffffff;

                            pg->vertex_attributes[slot].converted_elements = 0;

                    }

					break;

                CASE_16(NV097_SET_VERTEX_DATA_ARRAY_FORMAT, 4):{ //done //pg->KelvinPrimitive.SetVertexDataArrayFormat[16]
					//pg->KelvinPrimitive.SetVertexDataArrayFormat[i] = Attribute [i].Format (SizeAndType) &0xFF + if (draw up method?)Stride << 8 : 0
					slot = (method - NV097_SET_VERTEX_DATA_ARRAY_FORMAT) / 4;
					int uiStride = 0;
					for (size_t argc = 0; argc < method_count; argc++,slot++) {
                        arg0 = argv[argc];
						//set out own vertex attribute format and offset here.
						g_NV2AInlineArrayVertexBuffer_AttributeFormat.Slots[argc].Format = argv[argc] & 0xFF;
						//Attribute.Offset is the offset from start of vertex to start of attribute
						g_NV2AInlineArrayVertexBuffer_AttributeFormat.Slots[argc].Offset = uiStride;
						g_NV2AInlineArrayVertexBuffer_AttributeFormat.Slots[argc].StreamIndex = 0;

						VertexAttribute *vertex_attribute = &pg->vertex_attributes[slot];

                        vertex_attribute->format =
                            GET_MASK(arg0, NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE);
                        vertex_attribute->count =
                            GET_MASK(arg0, NV097_SET_VERTEX_DATA_ARRAY_FORMAT_SIZE);
                        vertex_attribute->stride =
                            GET_MASK(arg0, NV097_SET_VERTEX_DATA_ARRAY_FORMAT_STRIDE);

                        NV2A_DPRINTF("vertex data array format=%d, count=%d, stride=%d\n",
                            vertex_attribute->format,
                            vertex_attribute->count,
                            vertex_attribute->stride);

                        vertex_attribute->gl_count = vertex_attribute->count;

                        switch (vertex_attribute->format) {
                            case NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_UB_D3D:
                                vertex_attribute->gl_type = GL_UNSIGNED_BYTE;
                                vertex_attribute->gl_normalize = GL_TRUE;
                                vertex_attribute->size = 1;
                                assert(vertex_attribute->count == 4);
                                // https://www.opengl.org/registry/specs/ARB/vertex_array_bgra.txt
                                vertex_attribute->gl_count = GL_BGRA;
                                vertex_attribute->needs_conversion = false;
                                break;
                            case NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_UB_OGL:
                                vertex_attribute->gl_type = GL_UNSIGNED_BYTE;
                                vertex_attribute->gl_normalize = GL_TRUE;
                                vertex_attribute->size = 1;
                                vertex_attribute->needs_conversion = false;
                                break;
                            case NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_S1:
                                vertex_attribute->gl_type = GL_SHORT;
                                vertex_attribute->gl_normalize = GL_TRUE;
                                vertex_attribute->size = 2;
                                vertex_attribute->needs_conversion = false;
                                break;
                            case NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_F:
                                vertex_attribute->gl_type = GL_FLOAT;
                                vertex_attribute->gl_normalize = GL_FALSE;
                                vertex_attribute->size = 4;
                                vertex_attribute->needs_conversion = false;
                                break;
                            case NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_S32K:
                                vertex_attribute->gl_type = GL_SHORT;
                                vertex_attribute->gl_normalize = GL_FALSE;
                                vertex_attribute->size = 2;
                                vertex_attribute->needs_conversion = false;
                                break;
                            case NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_CMP:
                                /* 3 signed, normalized components packed in 32-bits. (11,11,10) */
                                vertex_attribute->size = 4;
                                vertex_attribute->gl_type = GL_FLOAT;
                                vertex_attribute->gl_normalize = GL_FALSE;
                                vertex_attribute->needs_conversion = true;
                                vertex_attribute->converted_size = sizeof(float);
                                vertex_attribute->converted_count = 3 * vertex_attribute->count;
                                break;
                            default:
                                fprintf(stderr, "Unknown vertex type: 0x%x\n", vertex_attribute->format);
                                assert(false);
                                break;
                        }
						if (vertex_attribute->needs_conversion) {
                            vertex_attribute->converted_elements = 0;
                        } else {
                            if (vertex_attribute->converted_buffer) {
                                g_free(vertex_attribute->converted_buffer);
                                vertex_attribute->converted_buffer = NULL;
                            }
                        }
						//advance out vertex attribute offset
						if (vertex_attribute->count > 0) {
							uiStride += vertex_attribute->converted_size*vertex_attribute->count;
							switch (argc) {
							case xbox::X_D3DVSDE_POSITION:
								pg->HostFVF |= D3DFVF_XYZ;
								break;
							case xbox::X_D3DVSDE_BLENDWEIGHT:
								if (vertex_attribute->size = 1) {
									switch (vertex_attribute->count) {
									case 1:pg->HostFVF |= D3DFVF_XYZB1; break;
									case 2:pg->HostFVF |= D3DFVF_XYZB2; break;
									case 3:pg->HostFVF |= D3DFVF_XYZB3; break;
									case 4:pg->HostFVF |= D3DFVF_XYZB4; break;
									}
								}
								else {
									pg->HostFVF |= D3DFVF_XYZRHW;
								}
								break;
							case xbox::X_D3DVSDE_NORMAL:
								pg->HostFVF |= D3DFVF_NORMAL;
								break;
							case xbox::X_D3DVSDE_DIFFUSE:
								pg->HostFVF |= D3DFVF_DIFFUSE;
								break;
							case xbox::X_D3DVSDE_SPECULAR:
								pg->HostFVF |= D3DFVF_SPECULAR;
								break;
							case xbox::X_D3DVSDE_FOG:
								//pg->HostFVF |= D3DFVF_FOG; //no feasible ENUM
								break;
							case xbox::X_D3DVSDE_POINTSIZE:
								pg->HostFVF |= D3DFVF_PSIZE;
								break;
							case xbox::X_D3DVSDE_BACKDIFFUSE:
								//pg->HostFVF |= D3DFVF_NORMAL; //no feasible ENUM
								break;
							case xbox::X_D3DVSDE_BACKSPECULAR:
								//pg->HostFVF |= D3DFVF_NORMAL; //no feasible ENUM
								break;
							case xbox::X_D3DVSDE_TEXCOORD0:
								pg->HostFVF |= D3DFVF_TEX0;
								break;
							case xbox::X_D3DVSDE_TEXCOORD1:
								pg->HostFVF |= D3DFVF_TEX1;
								break;
							case xbox::X_D3DVSDE_TEXCOORD2:
								pg->HostFVF |= D3DFVF_TEX2;
								break;
							case xbox::X_D3DVSDE_TEXCOORD3:
								pg->HostFVF |= D3DFVF_TEX3;
								break;
							}

						}
                    }
                    break;
				}
                CASE_3(NV097_SET_BACK_SCENE_AMBIENT_COLOR,4):break;//not implement //pg->KelvinPrimitive.SetBackSceneAmbientColor[3]
                case NV097_SET_BACK_MATERIAL_ALPHA:break;          //not implement //pg->KelvinPrimitive.SetBackMaterialAlpha
                CASE_3(NV097_SET_BACK_MATERIAL_EMISSIONR,4):break; //not implement //pg->KelvinPrimitive.SetBackMaterialEmission[3]

                case NV097_SET_LOGIC_OP_ENABLE://done, not used //pg->KelvinPrimitive.SetLogicOpEnable
                    //SET_MASK(pg->pgraph_regs[NV_PGRAPH_BLEND / 4],
                    //		 NV_PGRAPH_BLEND_LOGICOP_ENABLE, arg0);
                    break;

                case NV097_SET_LOGIC_OP://done, not used //pg->KelvinPrimitive.SetLogicOp
                    //SET_MASK(pg->pgraph_regs[NV_PGRAPH_BLEND / 4],
                    //		 NV_PGRAPH_BLEND_LOGICOP, arg0 & 0xF);
                    break;

                case NV097_SET_TWO_SIDED_LIGHT_EN:break;  //not implement //pg->KelvinPrimitive.SetTwoSidedLightEn

                case NV097_CLEAR_REPORT_VALUE://done //pg->KelvinPrimitive.ClearReportValue

                    /* FIXME: Does this have a value in parameter? Also does this (also?) modify
                     *        the report memory block?
                     */
                    if (pg->gl_zpass_pixel_count_query_count) {
                        if (pg->opengl_enabled) {
                            glDeleteQueries(pg->gl_zpass_pixel_count_query_count,
                                            pg->gl_zpass_pixel_count_queries);
                        }
                        pg->gl_zpass_pixel_count_query_count = 0;
                    }
                    pg->zpass_pixel_count_result = 0;

                    break;

                case NV097_SET_ZPASS_PIXEL_COUNT_ENABLE://done //pg->KelvinPrimitive.SetZpassPixelCountEnable
                    //pg->KelvinPrimitive.SetZpassPixelCountEnable
                    //pg->zpass_pixel_count_enable = arg0;
                    break;

                case NV097_GET_REPORT: {//done //pg->KelvinPrimitive.GetReport
                    /* FIXME: This was first intended to be watchpoint-based. However,
                     *        qemu / kvm only supports virtual-address watchpoints.
                     *        This'll do for now, but accuracy and performance with other
                     *        approaches could be better
                     */
                    uint8_t type = GET_MASK(arg0, NV097_GET_REPORT_TYPE);
                    assert(type == NV097_GET_REPORT_TYPE_ZPASS_PIXEL_CNT);
                    hwaddr offset = GET_MASK(arg0, NV097_GET_REPORT_OFFSET);

                    uint64_t timestamp = 0x0011223344556677; /* FIXME: Update timestamp?! */
                    uint32_t done = 0;

                    if (pg->opengl_enabled) {
                        /* FIXME: Multisampling affects this (both: OGL and Xbox GPU),
                         *        not sure if CLEARs also count
                         */
                        /* FIXME: What about clipping regions etc? */
                        for(i = 0; i < pg->gl_zpass_pixel_count_query_count; i++) {
                            GLuint gl_query_result;
                            glGetQueryObjectuiv(pg->gl_zpass_pixel_count_queries[i],
                                                GL_QUERY_RESULT,
                                                &gl_query_result);
                            pg->zpass_pixel_count_result += gl_query_result;
                        }
                        if (pg->gl_zpass_pixel_count_query_count) {
                            glDeleteQueries(pg->gl_zpass_pixel_count_query_count,
                                            pg->gl_zpass_pixel_count_queries);
                        }
                        pg->gl_zpass_pixel_count_query_count = 0;

                        hwaddr report_dma_len;
                        uint8_t *report_data = (uint8_t*)nv_dma_map(d, pg->KelvinPrimitive.SetContextDmaReport,
                                                                    &report_dma_len);
                        assert(offset < report_dma_len);
                        report_data += offset;

                        stq_le_p((uint64_t*)&report_data[0], timestamp);
                        stl_le_p((uint32_t*)&report_data[8], pg->zpass_pixel_count_result);
                        stl_le_p((uint32_t*)&report_data[12], done);
                    }

                    break;
                }

                CASE_3(NV097_SET_TL_CONST_ZERO, 4) :break;//not implement yet //pg->KelvinPrimitive.SetTLConstZero[3]

                CASE_3(NV097_SET_EYE_DIRECTION, 4)://done //pg->KelvinPrimitive.SetEyeDirection[3]
					slot = (method - NV097_SET_EYE_DIRECTION) / 4;
                    for (size_t argc = 0; argc < method_count; argc++, slot += 4) {
                        arg0 = argv[argc];

                        pg->ltctxa[NV_IGRAPH_XF_LTCTXA_EYED][slot] = arg0;
                        pg->ltctxa_dirty[NV_IGRAPH_XF_LTCTXA_EYED] = true;
                    }
                    break;

                CASE_3(NV097_SET_LINEAR_FOG_CONST, 4) :break;//not implement yet //pg->KelvinPrimitive.SetLinearFogConst[3]
                case NV097_SET_SHADER_CLIP_PLANE_MODE:break;  //not implement //pg->KelvinPrimitive.SetShaderClipPlaneMode

//**********************this case is very complicate, need more time to verify it.
                case NV097_SET_BEGIN_END: {//consider done  //pg->KelvinPrimitive.SetBeginEnd
        //this case is critical, xbox d3d calls this case twice to enclose the DrawVerticesUp/DrawVert.... calls.
                    //uint32_t control_0 = pg->pgraph_regs[NV_PGRAPH_CONTROL_0 / 4];
                    //uint32_t control_1 = pg->pgraph_regs[NV_PGRAPH_CONTROL_1 / 4];

                    bool depth_test = pg->KelvinPrimitive.SetCullFaceEnable!=0;
                    bool stencil_test = pg->KelvinPrimitive.SetStencilTestEnable!=0;

                    if (arg0 == NV097_SET_BEGIN_END_OP_END) {//the DrawXXX call completes the pushbuffer operation. we can process the rendering.

                        if (pg->draw_arrays_length>0) {

                            NV2A_GL_DPRINTF(false, "Draw Arrays");

                            assert(pg->inline_buffer_length == 0);
                            assert(pg->inline_array_length == 0);
                            assert(pg->inline_elements_length == 0);

                            if (pgraph_draw_arrays != nullptr) {
							    //shall we calculate the input vertes stride = pgraph_get_NV2A_vertex_stride(PGRAPHState *pg)?
								//DWORD dwVertexStride = pgraph_get_NV2A_vertex_stride(pg);

									pgraph_draw_arrays(d);
                            }
                        } else if (pg->inline_buffer_length) {//for draw calls using SET_BEGIN_ENG(primitive)/SET_VERTEX_DATAXXX ... /SET_BEGIN_ENG(0)

                            NV2A_GL_DPRINTF(false, "Inline Buffer");

                            assert(pg->draw_arrays_length == 0);
                            assert(pg->inline_array_length == 0);
                            assert(pg->inline_elements_length == 0);
							unsigned int uiStride = 0;

							for (int i = 0; i < NV2A_VERTEXSHADER_ATTRIBUTES; i++) {

								//setup attribute Format and Offset
								if (pg->vertex_attributes[i].set_by_inline_buffer) {
									pg->vertex_attributes[i].format= xbox::X_D3DVSDT_FLOAT4;
									pg->vertex_attributes[i].offset = uiStride;
									uiStride += 4 * sizeof(float);
								}

								//reset the attribute flag for next draw call.
								pg->vertex_attributes[i].set_by_inline_buffer = false;
							}

							DWORD dwVertexStride = pgraph_get_NV2A_vertex_stride(pg);
							for (int i = 0; i < NV2A_VERTEXSHADER_ATTRIBUTES; i++) {
								//apply stride to attributes.
								pg->vertex_attributes[i].format+= dwVertexStride<<8;
							}

							if (pgraph_draw_inline_buffer != nullptr) {
								//stride needs special re-calc

								//xbox vertex shader attributes slots. set by SetVertexShaderInput(). try to set it directly before set vertex shader or draw prmitives.
								//extern xbox::X_VERTEXATTRIBUTEFORMAT g_Xbox_SetVertexShaderInput_Attributes; g_Xbox_SetVertexShaderInput_Attributes = g_NV2AInlineArrayVertexBuffer_AttributeFormat;

								if (dwVertexStride > 0) {

									//calculate the input vertes stride = pgraph_get_NV2A_vertex_stride(PGRAPHState *pg) in HLE_pgraph_draw_inline_array(d)
									pgraph_draw_inline_buffer(d);
								}
								//shall we calculate the input vertes stride = pgraph_get_NV2A_vertex_stride(PGRAPHState *pg)?
								//pgraph_draw_inline_buffer(d);
                            }
							
                        } else if (pg->inline_array_length) {

                            NV2A_GL_DPRINTF(false, "Inline Array");

                            assert(pg->draw_arrays_length == 0);
                            assert(pg->inline_buffer_length == 0);
                            assert(pg->inline_elements_length == 0);

							DWORD dwVertexStride = pgraph_get_NV2A_vertex_stride(pg);
							for (int i = 0; i < NV2A_VERTEXSHADER_ATTRIBUTES; i++) {
								//apply stride to attributes.
								pg->vertex_attributes[i].format += dwVertexStride << 8;
							}

							if (pgraph_draw_inline_array != nullptr) {
								//calculate the input vertes stride = pgraph_get_NV2A_vertex_stride(PGRAPHState *pg) in HLE_pgraph_draw_inline_array(d)
								pgraph_draw_inline_array(d);
                            }
                        } else if (pg->inline_elements_length) {

                            NV2A_GL_DPRINTF(false, "Inline Elements");

                            assert(pg->draw_arrays_length == 0);
                            assert(pg->inline_buffer_length == 0);
                            assert(pg->inline_array_length == 0);

                            if (pgraph_draw_inline_elements != nullptr) {

								//shall we calculate the input vertes stride = pgraph_get_NV2A_vertex_stride(PGRAPHState *pg)?
								pgraph_draw_inline_elements(d);
                            }
                        } else {
                            NV2A_GL_DPRINTF(true, "EMPTY NV097_SET_BEGIN_END");
                            assert(false);
                        }

                    } else {

                        assert(arg0 <= NV097_SET_BEGIN_END_OP_POLYGON);

                        pg->primitive_mode = arg0;

                        if (pgraph_draw_state_update != nullptr) {
                            pgraph_draw_state_update(d);
                        }

                        pg->inline_elements_length = 0;
                        pg->inline_array_length = 0;
                        pg->draw_arrays_length = 0;
                        //pg->draw_arrays_min_start = -1;
                        pg->draw_arrays_max_count = 0;

						pg->inline_buffer_length = 0;
						//reset attribute flag for in_line_buffer
						for (int i = 0; i < NV2A_VERTEXSHADER_ATTRIBUTES; i++) {
							//reset the attribute flag for next draw call.
							pg->vertex_attributes[i].set_by_inline_buffer = false;
						}

                    }

                    pgraph_set_surface_dirty(pg, true, depth_test || stencil_test);
                    break;
                }

                case NV097_ARRAY_ELEMENT16://xbox D3DDevice_DrawIndexedVertices() calls this 
					//NV2A hardware limit 2048 pair of index16 max., xbox d3d block size 511 dword (1022 vertices)max.verified with Otogi
					//  (NV097_SET_BEGIN_END, PrimitiveType)
					//->if(alignment required) (NV097_ARRAY_ELEMENT16,alignment)
					//->if(block transfer required)loop block transfer (NV097_ARRAY_ELEMENT16,511)
					//->if(vertices remained) (NV097_ARRAY_ELEMENT16,vertices_remained)
					//->if(last_odd_vertex) (NV097_ARRAY_ELEMENT32,vertices_remained)
					//NV097_ARRAY_ELEMENT16 must always send out vertices in pair. it can't send single vertex witl mask 0x0000FFFF, that will introduce bug.
					//LOG_TEST_CASE("NV2A_VB_ELEMENT_U16");	
                    // Test-case : Turok (in main menu)	
                    // Test-case : Hunter Redeemer	
                    // Test-case : Otogi (see https://github.com/Cxbx-Reloaded/Cxbx-Reloaded/pull/1113#issuecomment-385593814)
                    for (size_t argc = 0; argc < method_count; argc++) {
                        arg0 = argv[argc];
                        assert(pg->inline_elements_length < NV2A_MAX_BATCH_LENGTH);
                        pg->inline_elements[
                            pg->inline_elements_length++] = arg0 & 0xFFFF;
                        pg->inline_elements[
                            pg->inline_elements_length++] = arg0 >> 16;
                    }
					break;

                case NV097_ARRAY_ELEMENT32://xbox D3DDevice_DrawIndexedVertices() calls this
					// xbox d3d uses NV097_ARRAY_ELEMENT32 to send the very last odd vertex of index vertex stream.
	                //LOG_TEST_CASE("NV2A_VB_ELEMENT_U32");	
                    // Test-case : Turok (in main menu)
                    for (size_t argc = 0; argc < method_count; argc++) {
                        arg0 = argv[argc];
                        assert(pg->inline_elements_length < NV2A_MAX_BATCH_LENGTH);
                        pg->inline_elements[
                            pg->inline_elements_length++] = arg0;

                    }

                    break;
                case NV097_DRAW_ARRAYS: {
					/*
					D3DDevice_DrawVertices(
						D3DPRIMITIVETYPE PrimitiveType,
						UINT StartVertex,
						UINT VertexCount)
					(NV097_SET_BEGIN_END, PrimitiveType)1
					BlockSize=256
					ArraysCount= total vertex count / BlockSize  + 1, min. 1 array
					(PUSHER_NOINC(NV097_DRAW_ARRAYS), ArraysCount) 1
					while (VertexCount > BlockSize){
						(PUSHER_NOINC(NV097_DRAW_ARRAYS), BlockSize - 1) 1
						VertexCount -= BlockSize
					}
					(PUSHER_NOINC(NV097_DRAW_ARRAYS), VertexCount - 1) 1
					(NV097_SET_BEGIN_END, NV097_SET_BEGIN_END_OP_END)1

					*/

					/* strange case xdk push buffer sample uses 
					//set_vertex_data_array_foramt with stride 0, then begin(triangle_list)/NV097_INLINE_ARRAY with 3 vertex with stride 0x10/end,
					//then set_vertex_data_array_offset, set_vertex_data_array_foramt with stride 0x10,begin(triangle_list)/NV097_DRAW_ARRAYS with arg0=0x02000000, method count=1, count=2, start offset =0/end.
					//the actual proper result should be 2 exact triangles reandered side by side.
					//used in xbox D3DDevice_DrawVertices
					//  (NV097_SET_BEGIN_END, PrimitiveType)
					//->(NV097_DRAW_ARRAYS,Method_Count=block_count)
					//-> loop append each block to pushbuffer
					//->(NV097_SET_BEGIN_END, NV097_SET_BEGIN_END_OP_END)
                     */
					//start is supposed to be the start vertex index
                    unsigned int start = GET_MASK(arg0, NV097_DRAW_ARRAYS_START_INDEX);
					//count is supposed to be the block count of vertex to be drawed. NV2A can draw up to 256 vertices in one submission.
					//for vertices more then 256, it must be processed with sequencial blocks.
					//each args in the argv[] represnt each block. the count stands for how many block left. the start means the starting vertex of each block.
					//for very last block, the count becomes vertices left in the final block.
					//we simply loops the argv[] with method_count, then we can know how many vertices we have to process.

					unsigned int block_count = method_count;//method_count is supposed to be block count staring from 1, including final block //GET_MASK(arg0, NV097_DRAW_ARRAYS_COUNT)+1;
					
					//each block submits 256 vertices. not sure we need to add start here or not.
					unsigned int last_block_vertex_count = GET_MASK(argv[method_count - 1], NV097_DRAW_ARRAYS_COUNT)+1;//?? shall we add 1, or leave it just as? currently keep it as is original code.
					unsigned int total_vertex_count = (block_count - 1) * 256 + last_block_vertex_count;
					pg->draw_arrays_max_count = MAX(pg->draw_arrays_max_count, start + total_vertex_count);

                    assert(pg->draw_arrays_length < ARRAY_SIZE(pg->gl_draw_arrays_start));
					if (pg->draw_arrays_length > 0) {
						unsigned int last_start =
						pg->gl_draw_arrays_start[pg->draw_arrays_length - 1];
						GLsizei* plast_count =
							&pg->gl_draw_arrays_count[pg->draw_arrays_length - 1];
						if (start == (last_start + *plast_count)) {
							*plast_count += total_vertex_count;
							break;
						}
					}
					pg->gl_draw_arrays_start[pg->draw_arrays_length] = start;
					pg->gl_draw_arrays_count[pg->draw_arrays_length] = total_vertex_count;
					pg->draw_arrays_length  ++;
					break;
                }

				case NV097_INLINE_ARRAY://xbox D3DDevice_DrawVerticesUP() D3DDevice_DrawIndexedVerticesUP calls this
					//we only know how many DWORDs of data is coming.
					//D3DDevice_DrawVerticesUP() Otogi: max. 16 vertices each batch, but data per vertex is not fixed.
					//D3DDevice_DrawIndexedVerticesUP:
					//  (NV097_SET_BEGIN_END, PrimitiveType)
					//->if(remained vertex count >16) loop batch (NV097_INLINE_ARRAY, count), count= 16* dwords/vertex.
					//->last batch (NV097_INLINE_ARRAY, count), count= last remained vertex count * dwords/vertex.
					//->(NV097_SET_BEGIN_END, NV097_SET_BEGIN_END_OP_END)
                    for (size_t argc = 0; argc < method_count; argc++) {
                        arg0 = argv[argc];
                        assert(pg->inline_array_length < NV2A_MAX_BATCH_LENGTH);
                        pg->inline_array[
                            pg->inline_array_length++] = arg0;
                    }

                    break;

                CASE_3(NV097_SET_EYE_VECTOR, 4) :break;//not implement //pg->KelvinPrimitive.SetEyeVector[3]

                case NV097_INLINE_VERTEX_REUSE:break;//not implement //pg->KelvinPrimitive.InlineVertexReuse

				/*

                D3DVSDE_POSITION
                Register 0. This is used to kick off vertex to NV2A GPU. This register should only be used inside NV097_SET_BEGIN_END block.
                D3DVSDE_BLENDWEIGHT
                Register 1.
                D3DVSDE_NORMAL
                Register 2.
                D3DVSDE_DIFFUSE
                Register 3. Color is specified as (red, green, blue, alpha), where red, green, blue, and alpha are between 0.0f and 1.0f.
                D3DVSDE_SPECULAR
                Register 4. Color is specified as (red, green, blue, alpha), where red, green, blue, and alpha are between 0.0f and 1.0f.
                D3DVSDE_FOG
                Register 5. Xbox extension.
                D3DVSDE_BACKDIFFUSE
                Register 7. Color is specified as (red, green, blue, alpha), where red, green, blue, and alpha are between 0.0f and 1.0f. Xbox extension.
                D3DVSDE_BACKSPECULAR
                Register 8. Color is specified as (red, green, blue, alpha), where red, green, blue, and alpha are between 0.0f and 1.0f. Xbox extension.
                D3DVSDE_TEXCOORD0
                Register 9.
                D3DVSDE_TEXCOORD1
                Register 10.
                D3DVSDE_TEXCOORD2
                Register 11.
                D3DVSDE_TEXCOORD3
                Register 12.
                    */

                    /*
                HRESULT IDirect3DDevice8::SetVertexData2f(
                   INT  	Register,
                   FLOAT 	a,
                   FLOAT 	b
                   );
                    */
				
                CASE_32(NV097_SET_VERTEX_DATA2F_M, 4): {//done //pg->KelvinPrimitive.SetVertexData2f[16].M[2]
					if (pg->KelvinPrimitive.SetBeginEnd == NV097_SET_BEGIN_END_OP_END) {
						assert(0);
						break;
					}
					//register is set one at a time per method, for loop should be redundant.
					slot = (method - NV097_SET_VERTEX_DATA2F_M) / 4;
					VertexAttribute *vertex_attribute = &pg->vertex_attributes[slot];
                    unsigned int part = slot % 2;// 0:a or 1:b
                    slot /= 2;//register
                    //pgraph_allocate_inline_buffer_vertices(pg, slot);
                    //M[a,b] are sent in the same time. shall be processed together.
                    //vertex_attribute->inline_value[part] = *(float*)&arg0;
                    vertex_attribute->inline_value[0] = pg->KelvinPrimitive.SetVertexData2f[slot].M[0];
                    vertex_attribute->inline_value[1] = pg->KelvinPrimitive.SetVertexData2f[slot].M[1];
                    /* FIXME: Should these really be set to 0.0 and 1.0 ? Conditions? */
                    vertex_attribute->inline_value[2] = 0.0f;
                    vertex_attribute->inline_value[3] = 1.0f;
                    break;
                }

                /*
                HRESULT IDirect3DDevice8::SetVertexData4f(
                   INT Register,
                   FLOAT a,
                   FLOAT b,
                   FLOAT c,
                   FLOAT d
                   );
                */
                CASE_64(NV097_SET_VERTEX_DATA4F_M, 4): {//done //pg->KelvinPrimitive.SetVertexData4f[16].M[4]
					if (pg->KelvinPrimitive.SetBeginEnd == NV097_SET_BEGIN_END_OP_END) {
						assert(0);
						break;
					}
				    //register is set one at a time per method, for loop should be redundant.
					slot = (method - NV097_SET_VERTEX_DATA4F_M) / 4;
					//for (size_t argc = 0; argc < method_count; argc++) {
                    //    arg0 = argv[argc];

                        unsigned int part = slot % 4;//index in M[]
                        slot /= 4;//register
                        VertexAttribute *vertex_attribute = &pg->vertex_attributes[slot];
						//allocate attribute.inline_buffer if it's not allocated yet.
						//pgraph_allocate_inline_buffer_vertices(pg, slot);
                        //vertex_attribute->inline_value[part] = *(float*)&arg0;

						//this is redundant. we're going to copy the inline_value[] to inline_buffer[]
						vertex_attribute->inline_value[0] = pg->KelvinPrimitive.SetVertexData4f[slot].M[0];
                        vertex_attribute->inline_value[1] = pg->KelvinPrimitive.SetVertexData4f[slot].M[1];
                        vertex_attribute->inline_value[2] = pg->KelvinPrimitive.SetVertexData4f[slot].M[2];
                        vertex_attribute->inline_value[3] = pg->KelvinPrimitive.SetVertexData4f[slot].M[3];
						vertex_attribute->inline_buffer[pg->inline_buffer_length];
						//copy the data to attribute.inline_buffer
						//memcpy(&vertex_attribute->inline_buffer[pg->inline_buffer_length],
						//	vertex_attribute->inline_value,
						//	sizeof(float) * 4);
                    //}
					//if ((slot == 0) && (part == 3)) {
					//if ((slot == 0)) {//D3DVSDE_POSITION   //slot ==0 only happended in NV097_SET_VERTEX_DATA4F
						//shall we consider the color state setting in NV097_SET_VERTEX_DATA4UB? we should, to be done.
					//	pgraph_finish_inline_buffer_vertex(pg);
					//}
					break;
                }
            /*
            HRESULT IDirect3DDevice8::SetVertexData2s(
               INT Register,
               SHORT a,
               SHORT b
               );
            */
            CASE_16(NV097_SET_VERTEX_DATA2S, 4): {//done //pg->KelvinPrimitive.SetVertexData2s[16]
				if (pg->KelvinPrimitive.SetBeginEnd == NV097_SET_BEGIN_END_OP_END) {
					assert(0);
					break;
				}
				//register is set one at a time per method, for loop should be redundant.
				slot = (method - NV097_SET_VERTEX_DATA2S) / 4;
                //assert(false); /* FIXME: Untested! */
                VertexAttribute *vertex_attribute = &pg->vertex_attributes[slot];
                //pgraph_allocate_inline_buffer_vertices(pg, slot);
                vertex_attribute->inline_value[0] = (float)(int16_t)(arg0 & 0xFFFF);
                vertex_attribute->inline_value[1] = (float)(int16_t)(arg0 >> 16);
                vertex_attribute->inline_value[2] = 0.0f;
                vertex_attribute->inline_value[3] = 1.0f;
				break;
            }
                /*
                HRESULT IDirect3DDevice8::SetVertexData4ub(
                   INT Register,
                   BYTE a,
                   BYTE b,
                   BYTE c,
                   BYTE d
                   );
                */

				//if pg->KelvinPrimitive.SetBeginEnd !=0, we're in the Begin/End block, data passed in shall be vertex data.
			    //else we're outside of Begin/End block, set in here indicate fix function vertexfunciton selection.
				/*
				D3DDevice_SetVertexShader(Handle)
				{
					if Program Shader
						D3DDevice_LoadVertexShader(Handle, 0);
						D3DDevice_SelectVertexShader(Handle, 0);
					else //FVF fixed function
						if D3DFVF_DIFFUSE //Flags & VERTEXSHADER_HASDIFFUSE 0x400
							(NV097_SET_VERTEX_DATA4UB(SLOT_DIFFUSE), -1, method count 1)
						if D3DFVF_SPECULAR  //Flags & VERTEXSHADER_HASSPECULAR 0x800
							(NV097_SET_VERTEX_DATA4UB(SLOT_SPECULAR), 0, method count 1)
						if D3DFVF_BACKDIFFUSE  //Flags & VERTEXSHADER_HASBACKDIFFUSE 0x1000
							(NV097_SET_VERTEX_DATA4UB(SLOT_BACK_DIFFUSE), -1, method count 1)
						if D3DFVF_BACKSPECULAR  //Flags & VERTEXSHADER_HASBACKSPECULAR 0x2000
							(NV097_SET_VERTEX_DATA4UB(SLOT_BACK_SPECULAR), 0, method count 1)
						if D3DFVF_PASSTHROUGH  //Flags & VERTEXSHADER_PASSTHROUGH 0x2
							(NV097_SET_TRANSFORM_PROGRAM_START, 0, method count 1)
							(NV097_SET_TRANSFORM_EXECUTION_MODE,
							 NV097_SET_TRANSFORM_EXECUTION_MODE_MODE_PROGRAM|NV097_SET_TRANSFORM_EXECUTION_MODE_RANGE_MODE_PRIV,
							 NV097_SET_TRANSFORM_PROGRAM_CXT_WRITE_EN_V_READ_ONLY,
							 method count 2)
							 SetupPasstrhouProgram()
								(NV097_SET_TRANSFORM_CONSTANT_LOAD, 0,method count 1)
								(NV097_SET_TRANSFORM_CONSTANT(0),method count  8)
									if (D3DRenderState.SetFogTableMode == D3DFOG_NONE)
									{
										prog_size_dwords = sizeof(FVFPassthruProgramSpecularFog) / sizeof(DWORD) //48 dwords
										pProgram = &FVFPassthruProgramSpecularFog;
									}
									else if (pDevice->m_StateFlags & STATE_FOGSOURCEZ)
									{
										prog_size_dwords = sizeof(FVFPassthruProgramZFog) / sizeof(DWORD); //44 dwords
										pProgram = &FVFPassthruProgramZFog;
									}
									else
									{
										prog_size_dwords = sizeof(FVFPassthruProgramWFog) / sizeof(DWORD); //48 dwords
										pProgram = &FVFPassthruProgramWFog;
									}
									Scale=?(D3DRenderState.SetZEnable == D3DZB_USEW )pDevice->SetInverseWFar * pDevice->SetZScale  : 1.0f
									argv[0]=pDevice->SetSuperSampleScaleX
									argv[1]=pDevice->SetSuperSampleScaleY
									argv[2]=pDevice->SetZScale
									argv[3]=Scale
									m_off=?(D3DRenderState.SetMultiSampleAntiAlias))0.5f:0.0f
									argv[5]=pDevice->SetScreenSpaceOffsetX - m_off
									argv[6]=pDevice->SetScreenSpaceOffsetY - m_off
									argv[7]=0.0f;
									argv[8]=0.0f;
									(NV097_SET_TRANSFORM_PROGRAM_LOAD, 0, method count 1)
									prog_ind=0
									do
										batch=MIN(32,prog_size_dwords)
										(NV097_SET_TRANSFORM_PROGRAM(0), method count batch)
											arg[0...batch-1]=pProgram[prog_ind+0...prog_ind+batch-1]
										prog_size_dwords - =batch
										prog_ind + =batch

									while (prog_size_dwords  !=0)


						else //not pass through
							setviewport()
								if(! FVF)
									(NV097_SET_VIEWPORT_OFFSET(0), A,   B,   C,   0.0f, method count 4)
									(NV097_SET_VIEWPORT_SCALE(0), X,Y,Z,0.0f, method count 4)
								else //FVF
									(NV097_SET_VIEWPORT_OFFSET(0),X,Y,0.0f,0.0f,method count 4)
								(NV097_SET_CLIP_MIN, ClipMin, ClipMax, method count 2)
							(NV097_SET_TRANSFORM_EXECUTION_MODE,
							 NV097_SET_TRANSFORM_EXECUTION_MODE_MODE_FIXED|NV097_SET_TRANSFORM_EXECUTION_MODE_RANGE_MODE_PRIV,
							 method count 1)
				}
				*/
				CASE_16(NV097_SET_VERTEX_DATA4UB, 4) : {//pg->KelvinPrimitive.SetVertexData4ub[16]
                    //pg->KelvinPrimitive.SetVertexData4ub[16] seems to be also holding state setting for NV2A fixed function for registers other then D3DVSDE_POSITION 
                    /*
                    IDirect3DDevice8::SetVertexDataColor
                        Sets the initial values of a given vertex attribute. This function sets persistent vertex attributes that may be used in vertex shaders and Begin/End blocks.
                    the following two lines are equivalent:

                    pDevice->SetVertexData4ub( register, r, g, b, a ); //notice that R and B are swapped before sent to NV2A.
                    pDevice->SetVertexDataColor( register, D3DCOLOR_ARGB( a, r, g, b ) );
                    Vertex attributes will remain set unless overwritten by the vertex buffer data stream.
                    */
                    //register is set one at a time per method, for loop should be redundant.
					slot = (method - NV097_SET_VERTEX_DATA4UB) / 4;
					if (pg->KelvinPrimitive.SetBeginEnd== NV097_SET_BEGIN_END_OP_END) {//we're out side of Begin/End block, should be setting fix function vertex shader
						//set fix fuction handle. where to set X_D3DFVF_XYZ for X_D3DVSDE_POSITION?
						if (slot== xbox::X_D3DVSDE_DIFFUSE && arg0 == -1)pg->vsh_FVF_handle |= X_D3DFVF_DIFFUSE;
						if (slot == xbox::X_D3DVSDE_SPECULAR && arg0 == 0)pg->vsh_FVF_handle |= X_D3DFVF_SPECULAR;
						//not implement in SetVertexShader
						//if (slot == xbox::X_D3DVSDE_DIFFUSE && arg0 == -1)pg->vsh_FVF_handle |= 0;//can't find corresponded ENUM
						//not implement in SetVertexShader
						//if (slot == xbox::X_D3DVSDE_SPECULAR && arg0 == 0)pg->vsh_FVF_handle |= 0;//can't find corresponded ENUM
						/*
							(NV097_SET_TRANSFORM_EXECUTION_MODE,
							NV097_SET_TRANSFORM_EXECUTION_MODE_MODE_FIXED | NV097_SET_TRANSFORM_EXECUTION_MODE_RANGE_MODE_PRIV, 	method count 1)
							will be called once these setting are completed.
							we shall create/setup fix function shader there.
						*/
					}
					else {//in Begin/End block. data transferred are vertices.
						VertexAttribute *vertex_attribute = &pg->vertex_attributes[slot];
						//do we need to swap the color ARGB/ABGR if the slor is D3DVSDE_DIFFUSE or D3DVSDE_SPECULAR ?
						//don't swap now. let's make the decision of when to swap/whther to swap later.
						//if (slot == xbox::X_D3DVSDE_DIFFUSE || slot == xbox::X_D3DVSDE_SPECULAR||slot == xbox::X_D3DVSDE_BACKDIFFUSE || slot == xbox::X_D3DVSDE_BACKSPECULAR) {
						//	arg0 = ABGR_to_ARGB(arg0);
						//}
						vertex_attribute->inline_value[0] = (arg0 & 0xFF) / 255.0f;
						vertex_attribute->inline_value[1] = ((arg0 >> 8) & 0xFF) / 255.0f;
						vertex_attribute->inline_value[2] = ((arg0 >> 16) & 0xFF) / 255.0f;
						vertex_attribute->inline_value[3] = ((arg0 >> 24) & 0xFF) / 255.0f;
					}
					   break;
				}
					/*
					HRESULT IDirect3DDevice8::SetVertexData4s(
					   INT Register,
					   SHORT a,
					   SHORT b,
					   SHORT c,
					   SHORT d
					   );
					*/
				CASE_32(NV097_SET_VERTEX_DATA4S_M, 4) : {//done //pg->KelvinPrimitive.SetVertexData4s[16].M[2]
					if (pg->KelvinPrimitive.SetBeginEnd == NV097_SET_BEGIN_END_OP_END) {
						assert(0);
						break;
					}
					//register is set one at a time per method, for loop should be redundant.
					slot = (method - NV097_SET_VERTEX_DATA4S_M) / 4;
					slot /= 2;//register
					VertexAttribute *vertex_attribute = &pg->vertex_attributes[slot];
					/* FIXME: Is mapping to [-1,+1] correct? */
					vertex_attribute->inline_value[0] = ((int16_t)(arg0 & 0xFFFF)
						* 2.0f + 1) / 65535.0f;
					vertex_attribute->inline_value[1] = ((int16_t)(arg0 >> 16)
						* 2.0f + 1) / 65535.0f;
					vertex_attribute->inline_value[2] = ((int16_t)(argv[1] & 0xFFFF)
						* 2.0f + 1) / 65535.0f;
					vertex_attribute->inline_value[3] = ((int16_t)(argv[1] >> 16)
						* 2.0f + 1) / 65535.0f;
					break;
				}

				/* begin of SetTexture**************************************************** */

				CASE_4(NV097_SET_TEXTURE_OFFSET, 64) ://KelvinPrimitive.SetTexture[4].Offset , sizeof(SetTexture[])==64
					//get texture[] index
					slot = (method - NV097_SET_TEXTURE_OFFSET) / 64;
					//pg->pgraph_regs[NV_PGRAPH_TEXOFFSET0 / 4 + slot * 4] = arg0;
					pg->texture_dirty[slot] = true;
					break;
				CASE_4(NV097_SET_TEXTURE_FORMAT, 64) : {//KelvinPrimitive.SetTexture[4].Format , sizeof(SetTexture[])==64
					//get texture[] index
					slot = (method - NV097_SET_TEXTURE_FORMAT) / 64;
					bool dma_select =
					GET_MASK(arg0, NV097_SET_TEXTURE_FORMAT_CONTEXT_DMA) == 2;
                    bool cubemap =
                        arg0 & NV097_SET_TEXTURE_FORMAT_CUBEMAP_ENABLE;
                    unsigned int border_source =
                        arg0 & NV097_SET_TEXTURE_FORMAT_BORDER_SOURCE;
                    unsigned int dimensionality =
                        GET_MASK(arg0, NV097_SET_TEXTURE_FORMAT_DIMENSIONALITY);
                    unsigned int color_format =
                        GET_MASK(arg0, NV097_SET_TEXTURE_FORMAT_COLOR);
                    unsigned int levels =
                        GET_MASK(arg0, NV097_SET_TEXTURE_FORMAT_MIPMAP_LEVELS);
                    unsigned int log_width =
                        GET_MASK(arg0, NV097_SET_TEXTURE_FORMAT_BASE_SIZE_U);
                    unsigned int log_height =
                        GET_MASK(arg0, NV097_SET_TEXTURE_FORMAT_BASE_SIZE_V);
                    unsigned int log_depth =
                        GET_MASK(arg0, NV097_SET_TEXTURE_FORMAT_BASE_SIZE_P);

                    uint32_t *reg = &pg->KelvinPrimitive.SetTexture[slot].Format;
                    SET_MASK(*reg, NV_PGRAPH_TEXFMT0_CONTEXT_DMA, dma_select);
                    SET_MASK(*reg, NV_PGRAPH_TEXFMT0_CUBEMAPENABLE, cubemap);
                    SET_MASK(*reg, NV_PGRAPH_TEXFMT0_BORDER_SOURCE, border_source);
                    SET_MASK(*reg, NV_PGRAPH_TEXFMT0_DIMENSIONALITY, dimensionality);
                    SET_MASK(*reg, NV_PGRAPH_TEXFMT0_COLOR, color_format);
                    SET_MASK(*reg, NV_PGRAPH_TEXFMT0_MIPMAP_LEVELS, levels);
                    SET_MASK(*reg, NV_PGRAPH_TEXFMT0_BASE_SIZE_U, log_width);
                    SET_MASK(*reg, NV_PGRAPH_TEXFMT0_BASE_SIZE_V, log_height);
                    SET_MASK(*reg, NV_PGRAPH_TEXFMT0_BASE_SIZE_P, log_depth);
                    //each texture contents 16 dowrds
                    pg->texture_dirty[slot] = true;
                    //double check required.
                    break;
				}

				CASE_4(NV097_SET_TEXTURE_ADDRESS, 64) :////KelvinPrimitive.SetTexture[4].Address , sizeof(SetTexture[])==64
                    //get texture[] index
                    slot = (method - NV097_SET_TEXTURE_OFFSET) / 64;
					//pg->pgraph_regs[NV_PGRAPH_TEXOFFSET0 / 4 + slot * 4] = arg0;
					pg->texture_dirty[slot] = true;
					break;

                CASE_4(NV097_SET_TEXTURE_CONTROL0, 64) :////KelvinPrimitive.SetTexture[4].Control0 , sizeof(SetTexture[])==64
                    //get texture[] index
                    slot = (method - NV097_SET_TEXTURE_OFFSET) / 64;
					//pg->pgraph_regs[NV_PGRAPH_TEXOFFSET0 / 4 + slot * 4] = arg0;
					pg->texture_dirty[slot] = true;
					break;

                CASE_4(NV097_SET_TEXTURE_CONTROL1, 64) :////KelvinPrimitive.SetTexture[4].Control1 , sizeof(SetTexture[])==64
                    //get texture[] index
                    slot = (method - NV097_SET_TEXTURE_OFFSET) / 64;
					//pg->pgraph_regs[NV_PGRAPH_TEXOFFSET0 / 4 + slot * 4] = arg0;
					pg->texture_dirty[slot] = true;
					break;

                CASE_4(NV097_SET_TEXTURE_FILTER, 64) :////KelvinPrimitive.SetTexture[4].Filter , sizeof(SetTexture[])==64
                    //get texture[] index
                    slot = (method - NV097_SET_TEXTURE_OFFSET) / 64;
					//pg->pgraph_regs[NV_PGRAPH_TEXOFFSET0 / 4 + slot * 4] = arg0;
					pg->texture_dirty[slot] = true;
					break;

                CASE_4(NV097_SET_TEXTURE_IMAGE_RECT, 64) :////KelvinPrimitive.SetTexture[4].ImageRect , sizeof(SetTexture[])==64
                    //get texture[] index
                    slot = (method - NV097_SET_TEXTURE_OFFSET) / 64;
                    //pg->pgraph_regs[NV_PGRAPH_TEXOFFSET0 / 4 + slot * 4] = arg0;
                    pg->texture_dirty[slot] = true;
					break;


				CASE_4(NV097_SET_TEXTURE_PALETTE, 64) :{ //KelvinPrimitive.SetTexture[4].Palette , sizeof(SetTexture[])==64
					//get texture[] index
					slot = (method - NV097_SET_TEXTURE_PALETTE) / 64;

					/*
					bool dma_select =
						GET_MASK(arg0, NV097_SET_TEXTURE_PALETTE_CONTEXT_DMA) == 1;
					unsigned int length =
						GET_MASK(arg0, NV097_SET_TEXTURE_PALETTE_LENGTH);
					unsigned int offset =
						GET_MASK(arg0, NV097_SET_TEXTURE_PALETTE_OFFSET);

					uint32_t *reg = &pg->KelvinPrimitive.SetTexture[slot].Palette;
					SET_MASK(*reg, NV_PGRAPH_TEXPALETTE0_CONTEXT_DMA, dma_select);
					SET_MASK(*reg, NV_PGRAPH_TEXPALETTE0_LENGTH, length);
					SET_MASK(*reg, NV_PGRAPH_TEXPALETTE0_OFFSET, offset);
					*/
					pg->texture_dirty[slot] = true;
					//double check required.
					break;
				}
                
                //these NV097_SET_TEXTURE_SET_BUMP_ENV_MAT are no longer needed. leave these code here for short term reference only. shall be cleared later.
                CASE_4(NV097_SET_TEXTURE_SET_BUMP_ENV_MAT + 0x0, 64) ://KevlinPrimitive.SetTexture[4].SetBumpEnvMat00 SetBumpEnvMat01 SetBumpEnvMat10 SetBumpEnvMat11 SetBumpEnvScale SetBumpEnvOffset
                    CASE_4(NV097_SET_TEXTURE_SET_BUMP_ENV_MAT + 0x4, 64) :
                    CASE_4(NV097_SET_TEXTURE_SET_BUMP_ENV_MAT + 0x8, 64) :
                    CASE_4(NV097_SET_TEXTURE_SET_BUMP_ENV_MAT + 0xc, 64) :
                        //xbox d3d update 6 bump state vars in the same time, including SetBumpEnvScale SetBumpEnvOffset
                        //pg->bump_env_matrix[4][4] was remapped to KevlinPrimitive.SetTexture[4].SetBumpEnvMat00~11
                {
                    //find which SetTexture[] element we're setting.
                    //int tex_index = (method - NV097_SET_TEXTURE_SET_BUMP_ENV_MAT) / 64;

                    //bump_env_matrix[tex_index][], there are 4 elements
                    //for (size_t argc = 0; argc < 4; argc++) {
                        //arg0 = ldl_le_p(argv + argc);
                        //slot = (method - NV097_SET_TEXTURE_SET_BUMP_ENV_MAT) / 4;
                        //assert((argc / 16) > 0);
                        //argc -= 16;

                        //this bump_env_matrix[][] is redundat. use pg->KelvinPrimitive.SetTexture[tex_index].SetBumpEnvMat00
                        //pg->bump_env_matrix[tex_index][argc % 4] = *(float*)&pg->KelvinPrimitive.SetTexture[tex_index].SetBumpEnvMat00;

                    //}
                    break;
                }
                //xbox d3d won't update NV097_SET_TEXTURE_SET_BUMP_ENV_SCALE independently, but we leave the case here.
                CASE_4( NV097_SET_TEXTURE_SET_BUMP_ENV_SCALE,64) :
                    //get texture[] index
                    slot = (method - NV097_SET_TEXTURE_OFFSET) / 64;
                    //pg->pgraph_regs[NV_PGRAPH_TEXOFFSET0 / 4 + slot * 4] = arg0;
                    pg->texture_dirty[slot] = true;
                    break;
                //xbox d3d won't update NV097_SET_TEXTURE_SET_BUMP_ENV_OFFSET independently, but we leave the case here.
                CASE_4( NV097_SET_TEXTURE_SET_BUMP_ENV_OFFSET,64):
                    //get texture[] index
                    slot = (method - NV097_SET_TEXTURE_OFFSET) / 64;
                    //pg->pgraph_regs[NV_PGRAPH_TEXOFFSET0 / 4 + slot * 4] = arg0;
                    pg->texture_dirty[slot] = true;
                    break;

        /* end of SetTexture**************************************************** */


#if (0) //state not implement yet
#   define NV097_PARK_ATTRIBUTE                               0x00001D64
#   define NV097_UNPARK_ATTRIBUTE                             0x00001D68
#   define NV097_SET_SEMAPHORE_OFFSET                         0x00001D6C
#endif


                case NV097_BACK_END_WRITE_SEMAPHORE_RELEASE: {
                    pgraph_update_surface(d, false, true, true);

                    //qemu_mutex_unlock(&pg->pgraph_lock);
                    //qemu_mutex_lock_iothread();
					//we're not emulating SEMAPHORE, disable this code for now.
					/*
                    uint32_t semaphore_offset = pg->pgraph_regs[NV_PGRAPH_SEMAPHOREOFFSET/4];

                    xbox::addr_xt semaphore_dma_len;
                    uint8_t *semaphore_data = (uint8_t*)nv_dma_map(d, pg->KelvinPrimitive.SetContextDmaSemaphore,
                        &semaphore_dma_len);
                    assert(semaphore_offset < semaphore_dma_len);
                    semaphore_data += semaphore_offset;

                    stl_le_p((uint32_t*)semaphore_data, arg0);
					*/
                    //qemu_mutex_lock(&pg->pgraph_lock);
                    //qemu_mutex_unlock_iothread();

                    break;
                }
#if (0) //state not implement yet
#   define NV097_TEXTURE_READ_SEMAPHORE_RELEASE               0x00001D74
#   define NV097_SET_ZMIN_MAX_CONTROL                         0x00001D78
#   define NV097_SET_ANTI_ALIASING_CONTROL                    0x00001D7C
#   define NV097_SET_COMPRESS_ZBUFFER_EN                      0x00001D80
#   define NV097_SET_OCCLUDE_ZSTENCIL_EN                      0x00001D84
#   define NV097_SET_ZSTENCIL_CLEAR_VALUE                     0x00001D8C

#endif
                case NV097_SET_COLOR_CLEAR_VALUE:break;//done //pg->KelvinPrimitive.SetColorClearValue

                case NV097_CLEAR_SURFACE: {//done //pg->KelvinPrimitive.ClearSurface
                    //
                    //pg->KelvinPrimitive.ClearSurface = arg0;
                    if (pgraph_draw_clear != nullptr) {
                        pgraph_draw_clear(d);
                    }
                    break;
                }

                case NV097_SET_CLEAR_RECT_HORIZONTAL:break;//done //pg->KelvinPrimitive.SetClearRectHorizontal
                case NV097_SET_CLEAR_RECT_VERTICAL:break;//done //pg->KelvinPrimitive.SetClearRectVertical
#if(0)
#   define NV097_SET_BEGIN_PATCH0                             0x00001DE0
#   define NV097_SET_BEGIN_PATCH1                             0x00001DE4
#   define NV097_SET_BEGIN_PATCH2                             0x00001DE8
#   define NV097_SET_BEGIN_PATCH3                             0x00001DEC
#   define NV097_SET_END_PATCH                                0x00001DF0
#   define NV097_SET_BEGIN_END_SWATCH                         0x00001DF4
#   define NV097_SET_BEGIN_END_CURVE                          0x00001DF8
#   define NV097_SET_CURVE_COEFFICIENTS                       0x00001E00 // [4]
#   define NV097_SET_BEGIN_TRANSITION0                        0x00001E10
#   define NV097_SET_BEGIN_TRANSITION1                        0x00001E14
#   define NV097_SET_BEGIN_TRANSITION2                        0x00001E18
#   define NV097_SET_END_TRANSITION                           0x00001E1C
#   define NV097_SET_SPECULAR_FOG_FACTOR                      0x00001E20 // [2]
#   define NV097_SET_BACK_SPECULAR_PARAMS                     0x00001E28 // [6]
#   define NV097_SET_COMBINER_COLOR_OCW                       0x00001E40 // [8]
#   define NV097_SET_COMBINER_CONTROL                         0x00001E60										 
#endif
                case NV097_SET_SHADOW_ZSLOPE_THRESHOLD://done //pg->KelvinPrimitive.SetShadowZSlopeThreshold
                    //pg->pgraph_regs[NV_PGRAPH_SHADOWZSLOPETHRESHOLD/4] = arg0;
                    assert(arg0 == 0x7F800000); /* FIXME: Unimplemented */
                    break;

                case NV097_SET_SHADOW_DEPTH_FUNC:break;//not implement //pg->KelvinPrimitive.SetShadowDepthFunc

				/*
				D3DDevice_SetPixelShader(Handle)
				if(Handle==0)//restore to fix function
				    
				    (NV097_SET_COMBINER_??,?? ,method count 1 )
				    (NV097_SET_SHADER_STAGE_PROGRAM,??,  method count 1 )
					update bump env for each texture stage [1]~[3]
					(NV097_SET_SHADER_OTHER_STAGE_INPUT,
					(NV097_SET_SHADER_OTHER_STAGE_INPUT_STAGE1_INSTAGE_0 | NV097_SET_SHADER_OTHER_STAGE_INPUT_STAGE2_INSTAGE_1 | NV097_SET_SHADER_OTHER_STAGE_INPUT_STAGE3_INSTAGE_2, method count 1); //arg0 0x210000
				else //set user pixel shader
				    pDevice->SetShaderUsesSpecFog = Handle.pPSDef->PSFinalCombinerInputsABCD | Handle.pPSDef->PSFinalCombinerInputsEFG; //pPSDef[32]|pPSDef[36]
					pDevice->m_ShaderAdjustsTexMode=pPSDef->PSC0Mapping&0x100;

				    pDevice->SetPSShaderStageProgram = Handle.PSDef->PSTextureModes;
					D3D__DirtyFlags |= D3DDIRTYFLAG_SHADER_STAGE_PROGRAM; //this shall trigger (NV097_SET_SHADER_STAGE_PROGRAM,pDevice->SetPSShaderStageProgram,method count 1)

					if(previous pixel shader handle == null) //switch from fix function to user program
					    update bump env for each texture stage [0]~[2]
					(NV097_SET_COMBINER_ALPHA_ICW(0), , method count  8)
					    memcpy((argv, &( Handle.PSDef->PSAlphaInputs[0]), 8 * sizeof(DWORD))  //&pPSDef[0]
					(NV097_SET_COMBINER_FACTOR0(0), , method count   32)
					    memcpy((argv, &( Handle.PSDef->PSConstant0), 32 * sizeof(DWORD))      ////&pPSDef[10]
					(NV097_SET_SHADER_CLIP_PLANE_MODE, Handle.pPSDef->PSCompareMode)
					(NV097_SET_SPECULAR_FOG_FACTOR(0),  method count 2)
					    memcpy((argv, &( Handle.PSDef->PSFinalCombinerConstant0), 2 * sizeof(DWORD))//&pPSDef[43]
					(NV097_SET_COMBINER_COLOR_OCW(0), , method count   9)
					    memcpy((argv, &( Handle.PSDef->PSRGBOutputs[0]), 9 * sizeof(DWORD))//&pPSDef[45]
					(NV097_SET_DOT_RGBMAPPING,  method count 2)
						memcpy((argv, &( Handle.PSDef->PSDotMapping), 2 * sizeof(DWORD))//&pPSDef[55]
					if(pDevice->SetShaderUsesSpecFog != 0)
					    (NV097_SET_COMBINER_SPECULAR_FOG_CW0,  method count 2)
						memcpy((argv, &( Handle.PSDef->PSFinalCombinerInputsABCD), 2 * sizeof(DWORD))//&pPSDef[8]
					
					   
				*/
				case NV097_SET_SHADER_STAGE_PROGRAM:
					if (pg->KelvinPrimitive.SetShaderStageProgram == 0) {
						arg0 = 0;
					}

					break;//done //pg->KelvinPrimitive.SetShaderStageProgram


                case NV097_SET_DOT_RGBMAPPING:{//not implement  //pg->KelvinPrimitive.SetDotRGBMapping
                    break;
                }

                case NV097_SET_SHADER_OTHER_STAGE_INPUT://done  //pg->KelvinPrimitive.SetShaderOtherStageInput
                    //pg->pgraph_regs[NV_PGRAPH_SHADERCTL/4] = arg0;
                    break;

                CASE_4(NV097_SET_TRANSFORM_DATA,4):break;//not implement //pg->KelvinPrimitive.SetTransformData[4]

                case NV097_LAUNCH_TRANSFORM_PROGRAM:break;//not implement //pg->KelvinPrimitive.LaunchTransformProgram

				extern bool g_bUsePassthroughHLSL;//TMP glue

                case NV097_SET_TRANSFORM_EXECUTION_MODE://done //pg->KelvinPrimitive.SetTransformExecutionMode
                    // Test-case : Whiplash
                    //SET_MASK(pg->pgraph_regs[NV_PGRAPH_CSV0_D / 4], NV_PGRAPH_CSV0_D_MODE,  //GET_MASK(pg->KelvinPrimitive.SetTransformExecutionMode,NV097_SET_TRANSFORM_EXECUTION_MODE_MODE)
                    //	GET_MASK(arg0,
                    //		NV097_SET_TRANSFORM_EXECUTION_MODE_MODE));
                    //SET_MASK(pg->pgraph_regs[NV_PGRAPH_CSV0_D / 4], NV_PGRAPH_CSV0_D_RANGE_MODE,  ////GET_MASK(pg->KelvinPrimitive.SetTransformExecutionMode,NV097_SET_TRANSFORM_EXECUTION_MODE_RANGE_MODE)
                    //	GET_MASK(arg0,
                    //		NV097_SET_TRANSFORM_EXECUTION_MODE_RANGE_MODE));
					/*
					user program start program using
					push (NV097_SET_TRANSFORM_PROGRAM_START, addr , method count 1)
					after program setup. we don't start user program here.

					*/
					slot=arg0 & NV097_SET_TRANSFORM_EXECUTION_MODE_MODE;
					if (slot == NV097_SET_TRANSFORM_EXECUTION_MODE_MODE_PROGRAM ) {//program mode, user program or pass through program
						//set shader program mode
						//g_Xbox_VertexShaderMode = VertexShaderMode::ShaderProgram;
						//disable fix function program
						//g_UseFixedFunctionVertexShader = false;
						//reset fix function vertex shader fvf handle when existing FVF
						pg->vsh_FVF_handle = 0;
						//Pass through program starts after finishing setup by calling NV097_SET_TRANSFORM_EXECUTION_MODE_MODE_PROGRAM with method count=2,argv[1]=NV097_SET_TRANSFORM_PROGRAM_CXT_WRITE_EN_V_READ_ONLY (0)
						//Starting pass through program, without pushing(NV097_SET_TRANSFORM_PROGRAM_START, Address)
						/*
						update, user program also calls NV097_SET_TRANSFORM_EXECUTION_MODE with method count =2, 
						         (NV097_SET_TRANSFORM_EXECUTION_MODE,
								  // NV097_SET_TRANSFORM_EXECUTION_MODE:
									(DRF_DEF(097, _SET_TRANSFORM_EXECUTION_MODE, _MODE, _PROGRAM) |  DRF_DEF(097, _SET_TRANSFORM_EXECUTION_MODE, _RANGE_MODE, _PRIV)),
								 // NV097_SET_TRANSFORM_PROGRAM_CXT_WRITE_EN:
								      VertexShader.Flags & VERTEXSHADER_WRITE, method count 2)
							somehow in prerecorded pushbuffer, the user program vertex shader will have argv[1]==0 in calling NV097_SET_TRANSFORM_EXECUTION_MODE
							this is causing trouble. we're firing up the program as pass through program,
							 and then fire it up again when (NV097_SET_TRANSFORM_PROGRAM_START, Address) was pushed. test case pushbuffer sample.
						*/
						if (method_count==2){
							if (argv[1] == NV097_SET_TRANSFORM_PROGRAM_CXT_WRITE_EN_V_READ_ONLY) {
								//g_Xbox_VertexShaderMode = VertexShaderMode::Passthrough;
								//figure out a way to treat the case where pre-recorded pushbuffer user program will be here as well.
								//xbox d3d shall load the pass through program into program slots.
								//but our HLE intercept those calls. so during hybring stage, we have to load the pass through program ourselves
								//there are different pass through programs depending on the KelvinPrimitive states.
								//check KelvinPrimitiveStates and load corresponded pass through programs.
								//load pass through program here.
								//pass through program starts at address 0;
								//xbox::dword_xt g_Xbox_VertexShader_FunctionSlots_StartAddress = 0;
								//this is required to enable CxbxUpdateHostVertexShader()
								//g_bUsePassthroughHLSL = true;
								//g_NV2AInlineArrayVertexBuffer_AttributeFormat was set and updata every time NV097_SET_VERTEX_DATA_ARRAY_FORMAT was called.
								//int old_override = g_InlineVertexBuffer_DeclarationOverride;
								//override might not be needed for vertex shader creation.
								//g_InlineVertexBuffer_DeclarationOverride = 2;
								//create pass through vertex shader
								//CxbxUpdateHostVertexShader();
								//do we really have to restore the override?
								//g_InlineVertexBuffer_DeclarationOverride = old_override;
							}
						}
						/*
						fix function setup finished using 
						push (NV097_SET_TRANSFORM_EXECUTION_MODE,
								 NV097_SET_TRANSFORM_EXECUTION_MODE_MODE_FIXED|NV097_SET_TRANSFORM_EXECUTION_MODE_RANGE_MODE_PRIV, method count 1)
						to start fix function vertex shader program
						*/
					}else if (slot== NV097_SET_TRANSFORM_EXECUTION_MODE_MODE_FIXED){//fix function mode

						//Call CxbxImpl_SetVertexShaderInput(pg->vsh_FVF_handle) here? or set the global xbox vertex attribute []
						//or g_Xbox_SetVertexShaderInput_Attributes = *CxbxGetVertexShaderAttributes(pXboxVertexShader); ??
						//to set vertex format info, but wihthout stream info.

						//set shader program mode
						//g_Xbox_VertexShaderMode = VertexShaderMode::FixedFunction;
					    //set fix function vertex shader handle position FVF and weight FVF
						if (pg->vertex_attributes[xbox::X_D3DVSDE_POSITION].count ==3
							&& pg->vertex_attributes[xbox::X_D3DVSDE_POSITION].size == 4)
							pg->vsh_FVF_handle |= X_D3DFVF_XYZ;
						//weight FVF to be implement.

						//for fix function, we need to call selectVertexShader(FVF) first
						//it sets the g_Xbox_VertexShaderMode = VertexShaderMode::FixedFunction or Passthrough in side,
						//and setup g_Xbox_VertexShader_ForFVF{} for fix function.
						//CxbxImpl_SetVertexShader(pg->vsh_FVF_handle);
						//CxbxImpl_LoadVertexShader(pg->vsh_FVF_handle,0);
						//this global is required for CxbxUpdateHostVertexShader() to parse fix function program
						//g_UseFixedFunctionVertexShader = true;
						//create fix function vertex shader
						//CxbxUpdateHostVertexShader();
					}
					break;


			    //this is set when Starting pass through vertex shaders.
				//it's not called directly, it set by calling NV097_SET_TRANSFORM_EXECUTION_MODE with method count=2, argv[1]=NV097_SET_TRANSFORM_PROGRAM_CXT_WRITE_EN_V_READ_ONLY

				case NV097_SET_TRANSFORM_PROGRAM_CXT_WRITE_EN://done //pg->KelvinPrimitive.SetTransformProgramCxtWriteEn
                    // Test-case : Whiplash
                    //pg->KelvinPrimitive.SetTransformProgramCxtWriteEn
                    //pg->enable_vertex_program_write = arg0;
					if (arg0 == NV097_SET_TRANSFORM_PROGRAM_CXT_WRITE_EN_V_READ_ONLY) {//Starting pass through program
						//g_Xbox_VertexShaderMode = VertexShaderMode::Passthrough;

						//xbox d3d shall load the pass through program into program slots.
						//but our HLE intercept those calls. so during hybring stage, we have to load the pass through program ourselves
						//there are different pass through programs depending on the KelvinPrimitive states.
						//check KelvinPrimitiveStates and load corresponded pass through programs.

						//this is required to enable CxbxUpdateHostVertexShader()
						//g_bUsePassthroughHLSL = true;

						//create pass through vertex shader
						//CxbxUpdateHostVertexShader();
					}

					break;
                case NV097_SET_TRANSFORM_PROGRAM_LOAD://done //pg->KelvinPrimitive.SetTransformProgramLoad
                    assert(arg0 < NV2A_MAX_TRANSFORM_PROGRAM_LENGTH);
                    //SET_MASK(pg->pgraph_regs[NV_PGRAPH_CHEOPS_OFFSET / 4],
                    //	NV_PGRAPH_CHEOPS_OFFSET_PROG_LD_PTR, arg0);
                    break;
				/*
			    this is where xbox d3d SetVertexShader(Handle, address) kick off shader, argv[0]=address, starting program slot.
				we can call
				set g_Xbox_VertexShaderMode == VertexShaderMode::FixedFunction/Passthrough/ShaderProgram
				then call CxbxUpdateHostVertexShader(), which will generate host vertex function program and create/set host vertex shader.
				but we need to set vertex attribute format first. using KelvinPrimitive.SetVertexDataArrayFormat[16]& 0xFF
				user program start program using
				push (NV097_SET_TRANSFORM_PROGRAM_START, addr , method count 1)
				after program setup.
				*/

				case NV097_SET_TRANSFORM_PROGRAM_START: {//done //pg->KelvinPrimitive.SetTransformProgramStart
					//assert(parameter < NV2A_MAX_TRANSFORM_PROGRAM_LENGTH);  //this is a starting address to load the program, shouldn't be checked with the max. program length.
					//SET_MASK(pg->pgraph_regs[NV_PGRAPH_CSV0_C / 4],
					//	NV_PGRAPH_CSV0_C_CHEOPS_PROGRAM_START, arg0);

					/*
					CxbxDrawContext DrawContext = {};

					DrawContext.pXboxIndexData = false;
					DrawContext.XboxPrimitiveType = (xbox::X_D3DPRIMITIVETYPE)pg->primitive_mode;
					DrawContext.dwVertexCount = VertexCount;
					DrawContext.pXboxVertexStreamZeroData = pg->inline_array;
					DrawContext.uiXboxVertexStreamZeroStride = dwVertexStride;
					*/
					/*
					what else do I need to do?
					1. to set g_Xbox_VertexShader_Handle = Handle;? need a dummy Vertex Structure, then create a Vertex Handle.
					2. find a way to update vertex attribute info to HLE.
					   setup FVF using g_NV2AInlineArrayVertexBuffer_AttributeFormat, then call IDirect3DDevice9::SetFVF(FVF) shall work.
					g_d3dDevice->SetFVF(VertexFVF);
					g_d3dDevice->SetStreamSource(0, pBigSquareVB, 0, sizeof(LVertex));
					g_d3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0 ,2);
					*/

					//don't set host FVF here, dangerous. need a HLE vertex attribute setting interface.
					//or an routine to kickoff the g_InlineVertexBuffer_DeclarationOverride
					//g_pD3DDevice->SetFVF(pg->HostFVF);

					//this is a work around for pre-recorded user program, which will get into pass through mode
					//shader is alreay update to host in NV097_SET_TRANSFORM_EXECUTION_MODE, so we skip the start here.
					//really shouldn't do it this way, unless we give up the pass through? or we detect whether we're in prerecorded pushbuffer?
					//if (g_Xbox_VertexShaderMode == VertexShaderMode::Passthrough) {
					//	break;
					//}


					//reset fix fuction handle.
					pg->vsh_FVF_handle = 0;

					//set starting program slot
					//xbox::dword_xt g_Xbox_VertexShader_FunctionSlots_StartAddress = arg0;

					// set vertes attributes and Formats

					//g_NV2AInlineArrayVertexBuffer_AttributeFormat was set and updata every time NV097_SET_VERTEX_DATA_ARRAY_FORMAT was called.
					//int old_override = g_InlineVertexBuffer_DeclarationOverride;
                    //override might not be needed for vertex shader creation.
					//g_InlineVertexBuffer_DeclarationOverride = 2;
					
					//set HLE vertex shader mode global
					//shader program mode shall be set as ShaderProgram  in NV097_SET_TRANSFORM_EXECUTION_MODE
					//for now we set it as Passthrough  in NV097_SET_TRANSFORM_EXECUTION_MODE,
					// and in set it as ShaderProgram here to distinguish user program and pass through program.
					//g_Xbox_VertexShaderMode = VertexShaderMode::ShaderProgram;
					//create and set host vertex shader from NV2A shader program slots. for now, just call it.
					//CxbxUpdateHostVertexShader();

					//g_InlineVertexBuffer_DeclarationOverride = old_override;
					break;
				}
                case NV097_SET_TRANSFORM_CONSTANT_LOAD://done //pg->KelvinPrimitive.SetTransformConstantLoad
                    assert(arg0 < NV2A_VERTEXSHADER_CONSTANTS);
                    //SET_MASK(pg->pgraph_regs[NV_PGRAPH_CHEOPS_OFFSET / 4],
                    //	NV_PGRAPH_CHEOPS_OFFSET_CONST_LD_PTR, arg0);
                    NV2A_DPRINTF("load to %d\n", arg0);
                    break;

                default://default case of KELVIN_PRIME method
                    //reset num_words_consumed indicates the method is not handled.
                    num_words_consumed = 0;
                    NV2A_GL_DPRINTF(true, "    unhandled  (0x%02x 0x%08x)",
                            graphics_class, method);
                    break;
			}//end of KELVIN_PRIMITIVE switch(method)

			//normal code flow of case NV_KELVIN_PRIMITIVE
			//check if the method was handled.
			//if (num_words_consumed > 0){
			//	;
			//}
			//num_processed = method_count;//num_processed should always be method_count. if not, then must be something wrong.
                
        }//	end of graphic_class KELVIN_PRIMITIVE case

        default://graphics_class default case
            NV2A_GL_DPRINTF(true, "Unknown Graphics Class/Method 0x%08X/0x%08X",
                            graphics_class, method);
            break;
    }//end of switch(graphics_class)

    return num_processed;	//return the words processed here so the caller can advance the dma_get pointer of the pushbuffer
                            //num_processed default to 1, which represent the first parameter passed in this call.
                            //but that word is advanced by the caller already. it's the caller's duty to subtract that word from the num_processed;
}

static void pgraph_switch_context(NV2AState *d, unsigned int channel_id)
{
    bool channel_valid =
        d->pgraph.pgraph_regs[NV_PGRAPH_CTX_CONTROL / 4] & NV_PGRAPH_CTX_CONTROL_CHID;
    unsigned pgraph_channel_id = GET_MASK(d->pgraph.pgraph_regs[NV_PGRAPH_CTX_USER / 4], NV_PGRAPH_CTX_USER_CHID);
    // Cxbx Note : This isn't present in xqemu / OpenXbox : d->pgraph.pgraph_lock.lock();
    bool valid = channel_valid && pgraph_channel_id == channel_id;
    if (!valid) {
        SET_MASK(d->pgraph.pgraph_regs[NV_PGRAPH_TRAPPED_ADDR / 4],
                 NV_PGRAPH_TRAPPED_ADDR_CHID, channel_id);

        NV2A_DPRINTF("pgraph switching to ch %d\n", channel_id);

        /* TODO: hardware context switching */
        //assert(!(d->pgraph.pgraph_regs[NV_PGRAPH_DEBUG_3 / 4]
        //        & NV_PGRAPH_DEBUG_3_HW_CONTEXT_SWITCH));

        qemu_mutex_unlock(&d->pgraph.pgraph_lock);
        qemu_mutex_lock_iothread();
        d->pgraph.pending_interrupts |= NV_PGRAPH_INTR_CONTEXT_SWITCH; // TODO : Should this be done before unlocking pgraph_lock?
        update_irq(d);

        qemu_mutex_lock(&d->pgraph.pgraph_lock);
        qemu_mutex_unlock_iothread();

        // wait for the interrupt to be serviced
        while (d->pgraph.pending_interrupts & NV_PGRAPH_INTR_CONTEXT_SWITCH) {
            qemu_cond_wait(&d->pgraph.interrupt_cond, &d->pgraph.pgraph_lock);
        }
    }
}

static void pgraph_wait_fifo_access(NV2AState *d) {
    while (!(d->pgraph.pgraph_regs[NV_PGRAPH_FIFO / 4] & NV_PGRAPH_FIFO_ACCESS)) {
        qemu_cond_wait(&d->pgraph.fifo_access_cond, &d->pgraph.pgraph_lock);
    }
}

static void pgraph_log_method(unsigned int subchannel,
                                unsigned int graphics_class,
                                unsigned int method, uint32_t parameter)
{
    static unsigned int last = 0;
    static unsigned int count = 0;

    extern const char *NV2AMethodToString(DWORD dwMethod); // implemented in PushBuffer.cpp

    if (last == 0x1800 && method != last) {
        const char* method_name = NV2AMethodToString(last); // = 'NV2A_VB_ELEMENT_U16'
        NV2A_GL_DPRINTF(true, "d->pgraph method (%d) 0x%08X %s * %d",
                        subchannel, last, method_name, count);
    }
    if (method != 0x1800) {
        // const char* method_name = NV2AMethodToString(method);
        // unsigned int nmethod = 0;
        // switch (graphics_class) {
        // case NV_KELVIN_PRIMITIVE:
        // 	nmethod = method | (0x5c << 16);
        // 	break;
        // case NV_CONTEXT_SURFACES_2D:
        // 	nmethod = method | (0x6d << 16);
        // 	break;
        // case NV_CONTEXT_PATTERN:
        // 	nmethod = method | (0x68 << 16);
        // 	break;
        // default:
        // 	break;
        // }
        // if (method_name) {
        // 	NV2A_DPRINTF("d->pgraph method (%d): %s (0x%x)\n",
        // 		subchannel, method_name, parameter);
        // } else {
            NV2A_DPRINTF("pgraph method (%d): 0x%08X -> 0x%04x (0x%x)\n",
                subchannel, graphics_class, method, parameter);
        // }

    }
    if (method == last) { count++; }
    else { count = 0; }
    last = method;
}

static void pgraph_allocate_inline_buffer_vertices(PGRAPHState *pg,
                                                   unsigned int attr)
{
    unsigned int i;
    VertexAttribute *vertex_attribute = &pg->vertex_attributes[attr];

    //if (vertex_attribute->inline_buffer || pg->inline_buffer_length == 0) {
	//return if the buffer is already allocated.
	if (vertex_attribute->inline_buffer) {
        return;
    }

    
	//allocate the inline buffer for vertex attribute,
    vertex_attribute->inline_buffer = (float*)g_malloc(NV2A_MAX_BATCH_LENGTH
                                                  * sizeof(float) * 4);

	/* Now upload the previous vertex_attribute value */
	/* don't upload the whole inline buffer of attribute here. this routine is only for buffer allocation.
	for (i = 0; i < pg->inline_buffer_length; i++) {
        memcpy(&vertex_attribute->inline_buffer[i * 4],
               vertex_attribute->inline_value,
               sizeof(float) * 4);
    }
	*/
}

static void pgraph_finish_inline_buffer_vertex(PGRAPHState *pg)
{
    unsigned int i;

    assert(pg->inline_buffer_length < NV2A_MAX_BATCH_LENGTH);

    for (i = 0; i < NV2A_VERTEXSHADER_ATTRIBUTES; i++) {
        VertexAttribute *vertex_attribute = &pg->vertex_attributes[i];
		//process the attribute data if it's been set
		if (vertex_attribute->set_by_inline_buffer ) {
            memcpy(&vertex_attribute->inline_buffer[
                      pg->inline_buffer_length * 4],
                   vertex_attribute->inline_value,
                   sizeof(float) * 4);
        }
    }


}

void pgraph_init(NV2AState *d)
{
    int i;

    PGRAPHState *pg = &d->pgraph;

    qemu_mutex_init(&pg->pgraph_lock);
    qemu_cond_init(&pg->interrupt_cond);
    qemu_cond_init(&pg->fifo_access_cond);
    qemu_cond_init(&pg->flip_3d);
	pg->opengl_enabled = true;
    if (!(pg->opengl_enabled))
        return;

    /* attach OpenGL render plugins */
    OpenGL_init_pgraph_plugins();

    /* fire up opengl */

    pg->gl_context = glo_context_create();
    assert(pg->gl_context);

#ifdef DEBUG_NV2A_GL
    glEnable(GL_DEBUG_OUTPUT);
#endif

    glextensions_init();

    // Set up ImGui's render backend
    //ImGui_ImplSDL2_InitForOpenGL(window, pg->gl_context);
    ImGui_ImplOpenGL3_Init();
    g_renderbase->SetDeviceRelease([] {
        ImGui_ImplOpenGL3_Shutdown();
    });

    /* DXT textures */
    assert(glo_check_extension("GL_EXT_texture_compression_s3tc"));
    /*  Internal RGB565 texture format */
    assert(glo_check_extension("GL_ARB_ES2_compatibility"));

    GLint max_vertex_attributes;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &max_vertex_attributes);
    assert(max_vertex_attributes >= NV2A_VERTEXSHADER_ATTRIBUTES);

    glGenFramebuffers(1, &pg->gl_framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, pg->gl_framebuffer);

    /* need a valid framebuffer to start with */
    glGenTextures(1, &pg->gl_color_buffer);
    glBindTexture(GL_TEXTURE_2D, pg->gl_color_buffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 640, 480,
                 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D, pg->gl_color_buffer, 0);

    assert(glCheckFramebufferStatus(GL_FRAMEBUFFER)
            == GL_FRAMEBUFFER_COMPLETE);

    //glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

#ifdef USE_TEXTURE_CACHE
    pg->texture_cache = g_lru_cache_new_full(
        0,
        NULL,
        texture_key_destroy,
        0,
        NULL,
        texture_binding_destroy,
        texture_key_hash,
        texture_key_equal,
        texture_key_retrieve,
        NULL,
        NULL
        );

    g_lru_cache_set_max_size(pg->texture_cache, 512);
#endif

#ifdef USE_SHADER_CACHE
    pg->shader_cache = g_hash_table_new(shader_hash, shader_equal);
#endif

    for (i=0; i<NV2A_VERTEXSHADER_ATTRIBUTES; i++) {
        glGenBuffers(1, &pg->vertex_attributes[i].gl_converted_buffer);
        glGenBuffers(1, &pg->vertex_attributes[i].gl_inline_buffer);
    }
    glGenBuffers(1, &pg->gl_inline_array_buffer);
    glGenBuffers(1, &pg->gl_element_buffer);

    glGenBuffers(1, &pg->gl_memory_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, pg->gl_memory_buffer);
    glBufferData(GL_ARRAY_BUFFER,
                 d->vram_size,
                 NULL,
                 GL_DYNAMIC_DRAW);

    glGenVertexArrays(1, &pg->gl_vertex_array);
    glBindVertexArray(pg->gl_vertex_array);

//    assert(glGetError() == GL_NO_ERROR);

    glo_set_current(NULL);
}

void pgraph_destroy(PGRAPHState *pg)
{

    qemu_mutex_destroy(&pg->pgraph_lock);
    qemu_cond_destroy(&pg->interrupt_cond);
    qemu_cond_destroy(&pg->fifo_access_cond);
    qemu_cond_destroy(&pg->flip_3d);

    if (pg->opengl_enabled) {
        glo_set_current(pg->gl_context);

        if (pg->gl_color_buffer) {
            glDeleteTextures(1, &pg->gl_color_buffer);
        }
        if (pg->gl_zeta_buffer) {
            glDeleteTextures(1, &pg->gl_zeta_buffer);
        }
        glDeleteFramebuffers(1, &pg->gl_framebuffer);

        // TODO: clear out shader cached
        // TODO: clear out texture cache

        glo_set_current(NULL);

        glo_context_destroy(pg->gl_context);
    }
}

static void pgraph_update_shader_constants(PGRAPHState *pg,
                                           ShaderBinding *binding,
                                           bool binding_changed,
                                           bool vertex_program,
                                           bool fixed_function)
{
    assert(pg->opengl_enabled);

    unsigned int i, j;

    /* update combiner constants */
    for (i = 0; i<= 8; i++) {
        uint32_t constant[2];
        if (i == 8) {
            /* final combiner */
            constant[0] = pg->KelvinPrimitive.SetSpecularFogFactor[0];
            constant[1] = pg->KelvinPrimitive.SetSpecularFogFactor[1];
        } else {
            constant[0] = pg->KelvinPrimitive.SetSpecularFogFactor[0];//+ i?
            constant[1] = pg->KelvinPrimitive.SetSpecularFogFactor[1];//+ i?
        }

        for (j = 0; j < 2; j++) {
            GLint loc = binding->psh_constant_loc[i][j];
            if (loc != -1) {
                float value[4];
                value[0] = (float) ((constant[j] >> 16) & 0xFF) / 255.0f;
                value[1] = (float) ((constant[j] >> 8) & 0xFF) / 255.0f;
                value[2] = (float) (constant[j] & 0xFF) / 255.0f;
                value[3] = (float) ((constant[j] >> 24) & 0xFF) / 255.0f;

                glUniform4fv(loc, 1, value);
            }
        }
    }
    if (binding->alpha_ref_loc != -1) {
        float alpha_ref = pg->KelvinPrimitive.SetAlphaRef / 255.0f;
        glUniform1f(binding->alpha_ref_loc, alpha_ref);
    }


    /* For each texture stage */
    for (i = 0; i < NV2A_MAX_TEXTURES; i++) {
        // char name[32];
        GLint loc;

        /* Bump luminance only during stages 1 - 3 */
        if (i > 0) {
            loc = binding->bump_mat_loc[i];
            if (loc != -1) {
                glUniformMatrix2fv(loc, 1, GL_FALSE, &pg->KelvinPrimitive.SetTexture[i].SetBumpEnvMat00); //KelvinPrimitive has 4 SetTexture[], no need to use i-1
            }
            loc = binding->bump_scale_loc[i];
            if (loc != -1) {
                glUniform1f(loc, pg->KelvinPrimitive.SetTexture[i].SetBumpEnvScale);//KelvinPrimitive has 4 SetTexture[], no need to use i-1
            }
            loc = binding->bump_offset_loc[i];
            if (loc != -1) {
                glUniform1f(loc, pg->KelvinPrimitive.SetTexture[i].SetBumpEnvOffset);//KelvinPrimitive has 4 SetTexture[], no need to use i-1
            }
        }

    }

    if (binding->fog_color_loc != -1) {
        uint32_t fog_color = pg->KelvinPrimitive.SetFogColor;
        glUniform4f(binding->fog_color_loc,
                    GET_MASK(fog_color, NV_PGRAPH_FOGCOLOR_RED) / 255.0f,
                    GET_MASK(fog_color, NV_PGRAPH_FOGCOLOR_GREEN) / 255.0f,
                    GET_MASK(fog_color, NV_PGRAPH_FOGCOLOR_BLUE) / 255.0f,
                    GET_MASK(fog_color, NV_PGRAPH_FOGCOLOR_ALPHA) / 255.0f);
    }
    if (binding->fog_param_loc[0] != -1) {
        glUniform1f(binding->fog_param_loc[0],
			pg->KelvinPrimitive.SetFogParams[0] / 4);
    }
    if (binding->fog_param_loc[1] != -1) {
        glUniform1f(binding->fog_param_loc[1],
			pg->KelvinPrimitive.SetFogParams[1] / 4);
    }

    float zclip_max = pg->KelvinPrimitive.SetClipMax;
    float zclip_min = pg->KelvinPrimitive.SetClipMin;

    if (fixed_function) {
        /* update lighting constants */
        struct {
            uint32_t* v;
            bool* dirty;
            GLint* locs;
            size_t len;
        } lighting_arrays[] = {
            // TODO : Change pointers into offset_of(), so this variable can become static
            {&pg->ltctxa[0][0], &pg->ltctxa_dirty[0], binding->ltctxa_loc, NV2A_LTCTXA_COUNT},
            {&pg->ltctxb[0][0], &pg->ltctxb_dirty[0], binding->ltctxb_loc, NV2A_LTCTXB_COUNT},
            {&pg->ltc1[0][0], &pg->ltc1_dirty[0], binding->ltc1_loc, NV2A_LTC1_COUNT},
        };

        for (i=0; i<ARRAY_SIZE(lighting_arrays); i++) {
            uint32_t *lighting_v = lighting_arrays[i].v;
            bool *lighting_dirty = lighting_arrays[i].dirty;
            GLint *lighting_locs = lighting_arrays[i].locs;
            size_t lighting_len = lighting_arrays[i].len;
            for (j=0; j<lighting_len; j++) {
                if (!lighting_dirty[j] && !binding_changed) continue;
                GLint loc = lighting_locs[j];
                if (loc != -1) {
                    glUniform4fv(loc, 1, (const GLfloat*)&lighting_v[j*4]);
                }
                lighting_dirty[j] = false;
            }
        }


        for (i = 0; i < NV2A_MAX_LIGHTS; i++) {
            GLint loc;
            loc = binding->light_infinite_half_vector_loc[i];
            if (loc != -1) {
                glUniform3fv(loc, 1, pg->KelvinPrimitive.SetLight[i].InfiniteHalfVector);
            }
            loc = binding->light_infinite_direction_loc[i];
            if (loc != -1) {
                glUniform3fv(loc, 1, pg->KelvinPrimitive.SetLight[i].InfiniteDirection);
            }

            loc = binding->light_local_position_loc[i];
            if (loc != -1) {
                glUniform3fv(loc, 1, pg->KelvinPrimitive.SetLight[i].LocalPosition);
            }
            loc = binding->light_local_attenuation_loc[i];
            if (loc != -1) {
                glUniform3fv(loc, 1, pg->KelvinPrimitive.SetLight[i].LocalAttenuation);
            }
        }

        /* estimate the viewport by assuming it matches the surface ... */
        //FIXME: Get surface dimensions?
        float m11 = 0.5f * pg->surface_shape.clip_width;
        float m22 = -0.5f * pg->surface_shape.clip_height;
        float m33 = zclip_max - zclip_min;
        //float m41 = m11;
        //float m42 = -m22;
        float m43 = zclip_min;
        //float m44 = 1.0f;

        if (m33 == 0.0f) {
            m33 = 1.0f;
        }
        float invViewport[16] = {
            1.0f/m11, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f/m22, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f/m33, 0.0f,
            -1.0f, 1.0f, -m43/m33, 1.0f
        };

        if (binding->inv_viewport_loc != -1) {
            glUniformMatrix4fv(binding->inv_viewport_loc,
                               1, GL_FALSE, &invViewport[0]);
        }

    }

    /* update vertex program constants */
    for (i=0; i<NV2A_VERTEXSHADER_CONSTANTS; i++) {
        if (!pg->vsh_constants_dirty[i] && !binding_changed) continue;

        GLint loc = binding->vsh_constant_loc[i];
        //assert(loc != -1);
        if (loc != -1) {
            glUniform4fv(loc, 1, (const GLfloat*)pg->vsh_constants[i]);
        }
        pg->vsh_constants_dirty[i] = false;
    }

    if (binding->surface_size_loc != -1) {
        glUniform2f(binding->surface_size_loc, (GLfloat)pg->surface_shape.clip_width,
                    (GLfloat)pg->surface_shape.clip_height);
    }

    if (binding->clip_range_loc != -1) {
        glUniform2f(binding->clip_range_loc, zclip_min, zclip_max);
    }

}

static void pgraph_bind_shaders(PGRAPHState *pg)
{
    assert(pg->opengl_enabled);

    unsigned int i, j;

    //uint32_t csv0_d = pg->pgraph_regs[NV_PGRAPH_CSV0_D / 4];
    bool vertex_program = GET_MASK(pg->KelvinPrimitive.SetTransformExecutionMode, NV097_SET_TRANSFORM_EXECUTION_MODE_MODE) == 2;

    bool fixed_function = GET_MASK(pg->KelvinPrimitive.SetTransformExecutionMode, NV097_SET_TRANSFORM_EXECUTION_MODE_MODE) == 0;

    //uint32_t csv0_c = pg->pgraph_regs[NV_PGRAPH_CSV0_C / 4];
    int program_start = pg->KelvinPrimitive.SetTransformProgramStart;

    NV2A_GL_DGROUP_BEGIN("%s (VP: %s FFP: %s)", __func__,
                         vertex_program ? "yes" : "no",
                         fixed_function ? "yes" : "no");

    ShaderBinding* old_binding = pg->shader_binding;

    ShaderState state;
    /* register combiner stuff */
    state.psh.window_clip_exclusive = pg->KelvinPrimitive.SetWindowClipType,
    state.psh.combiner_control = pg->KelvinPrimitive.SetCombinerControl;
    state.psh.shader_stage_program = pg->KelvinPrimitive.SetShaderStageProgram;
    state.psh.other_stage_input = pg->KelvinPrimitive.SetShaderOtherStageInput;
    state.psh.final_inputs_0 = pg->KelvinPrimitive.SetFogParams[0];
    state.psh.final_inputs_1 = pg->KelvinPrimitive.SetFogParams[1];
    //uint32_t control0 = pg->pgraph_regs[NV_PGRAPH_CONTROL_0 / 4];

    state.psh.alpha_test = pg->KelvinPrimitive.SetAlphaTestEnable !=0;
    state.psh.alpha_func = (enum PshAlphaFunc)pg->KelvinPrimitive.SetAlphaFunc;

    /* fixed function stuff */
    state.skinning = (enum VshSkinning)pg->KelvinPrimitive.SetSkinMode;
    state.lighting = pg->KelvinPrimitive.SetDitherEnable!=0;
    state.normalization = pg->KelvinPrimitive.SetNormalizationEnable!=0;

    state.fixed_function = fixed_function;

    /* vertex program stuff */
    state.vertex_program = vertex_program;
    state.z_perspective = pg->KelvinPrimitive.SetControl0 & NV097_SET_CONTROL0_Z_PERSPECTIVE_ENABLE;

    /* geometry shader stuff */
    state.primitive_mode = (enum ShaderPrimitiveMode)pg->primitive_mode;
    state.polygon_front_mode = (enum ShaderPolygonMode)pg->KelvinPrimitive.SetFrontPolygonMode;
    state.polygon_back_mode = (enum ShaderPolygonMode)pg->KelvinPrimitive.SetBackPolygonMode;

    state.program_length = 0;
    memset(state.vsh_program_copy, 0, sizeof(state.vsh_program_copy));

    if (vertex_program) {
        // copy in vertex program tokens
        for (i = program_start; i < NV2A_MAX_TRANSFORM_PROGRAM_LENGTH; i++) {
            uint32_t *cur_token = (uint32_t*)&pg->vsh_program_slots[i];
            memcpy(&state.vsh_program_copy[state.program_length],
                   cur_token,
                   VSH_TOKEN_SIZE * sizeof(uint32_t));
            state.program_length++;

            if (vsh_get_field(cur_token, FLD_FINAL)) {
                break;
            }
        }
    }

    /* Texgen */
    for (i = 0; i < 4; i++) {
        //NV_PGRAPH_CSV1_B was never used because there are only 2 Texgen in KelvinPrimitive, which are used as T0_S T R Q, T1_S T R Q
        //unsigned int reg = NV_PGRAPH_CSV1_A / 4;//(i < 2) ? NV_PGRAPH_CSV1_A / 4 : NV_PGRAPH_CSV1_B / 4;
        for (j = 0; j < 4; j++) {
            unsigned int masks[] = {
                // NOTE: For some reason, Visual Studio thinks NV_PGRAPH_xxxx is signed integer. (possible bug?)
                (i % 2U) ? (unsigned int)pg->KelvinPrimitive.SetTexgen[1].S : (unsigned int)pg->KelvinPrimitive.SetTexgen[0].S,
                (i % 2U) ? (unsigned int)pg->KelvinPrimitive.SetTexgen[1].T : (unsigned int)pg->KelvinPrimitive.SetTexgen[0].T,
                (i % 2U) ? (unsigned int)pg->KelvinPrimitive.SetTexgen[1].R : (unsigned int)pg->KelvinPrimitive.SetTexgen[0].R,
                (i % 2U) ? (unsigned int)pg->KelvinPrimitive.SetTexgen[1].Q : (unsigned int)pg->KelvinPrimitive.SetTexgen[0].Q
            };
            state.texgen[i][j] = (enum VshTexgen) masks[j];
        }
    }

    /* Fog */
    state.fog_enable = pg->KelvinPrimitive.SetFogEnable!=0;
    if (state.fog_enable) {
        /*FIXME: Use CSV0_D? */
        state.fog_mode = (enum VshFogMode)pg->KelvinPrimitive.SetFogMode;
        state.foggen = (enum VshFoggen)pg->KelvinPrimitive.SetFogGenMode;
    } else {
        /* FIXME: Do we still pass the fogmode? */
        state.fog_mode = (enum VshFogMode)0;
        state.foggen = (enum VshFoggen)0;
    }

    /* Texture matrices */
    for (i = 0; i < 4; i++) {
        state.texture_matrix_enable[i] = pg->KelvinPrimitive.SetTextureMatrixEnable[4];
    }

    /* Lighting */
    if (state.lighting) {
        for (i = 0; i < NV2A_MAX_LIGHTS; i++) {
            state.light[i] = (enum VshLight)GET_MASK(pg->KelvinPrimitive.SetLightEnableMask,
                                      NV_PGRAPH_CSV0_D_LIGHT0 << (i * 2));
        }
    }

    /* Window clip
     *
     * Optimization note: very quickly check to ignore any repeated or zero-size
     * clipping regions. Note that if region number 7 is valid, but the rest are
     * not, we will still add all of them. Clip regions seem to be typically
     * front-loaded (meaning the first one or two regions are populated, and the
     * following are zeroed-out), so let's avoid adding any more complicated
     * masking or copying logic here for now unless we discover a valid case.
     */
    assert(!state.psh.window_clip_exclusive); /* FIXME: Untested */
    state.psh.window_clip_count = 0;
    uint32_t last_x = 0, last_y = 0;

    for (i = 0; i < 8; i++) {
        const uint32_t x = pg->KelvinPrimitive.SetWindowClipHorizontal[i];
        const uint32_t y = pg->KelvinPrimitive.SetWindowClipVertical[i];
        const uint32_t x_min = GET_MASK(x, NV_PGRAPH_WINDOWCLIPX0_XMIN);
        const uint32_t x_max = GET_MASK(x, NV_PGRAPH_WINDOWCLIPX0_XMAX);
        const uint32_t y_min = GET_MASK(y, NV_PGRAPH_WINDOWCLIPY0_YMIN);
        const uint32_t y_max = GET_MASK(y, NV_PGRAPH_WINDOWCLIPY0_YMAX);

        /* Check for zero width or height clipping region */
        if ((x_min == x_max) || (y_min == y_max)) {
            continue;
        }

        /* Check for in-order duplicate regions */
        if ((x == last_x) && (y == last_y)) {
            continue;
        }

        NV2A_DPRINTF("Clipping Region %d: min=(%d, %d) max=(%d, %d)\n",
            i, x_min, y_min, x_max, y_max);

        state.psh.window_clip_count = i + 1;
        last_x = x;
        last_y = y;
    }

    /* FIXME: We should memset(state, 0x00, sizeof(state)) instead */
    memset(state.psh.rgb_inputs, 0, sizeof(state.psh.rgb_inputs));
    memset(state.psh.rgb_outputs, 0, sizeof(state.psh.rgb_outputs));
    memset(state.psh.alpha_inputs, 0, sizeof(state.psh.alpha_inputs));
    memset(state.psh.alpha_outputs, 0, sizeof(state.psh.alpha_outputs));

    /* Copy content of enabled combiner stages */
    unsigned int num_stages = pg->KelvinPrimitive.SetCombinerControl & 0xFF;
    for (i = 0; i < num_stages; i++) {
        state.psh.rgb_inputs[i] = pg->KelvinPrimitive.SetCombinerColorICW[i];
        state.psh.rgb_outputs[i] = pg->KelvinPrimitive.SetCombinerColorOCW[i];
        state.psh.alpha_inputs[i] = pg->KelvinPrimitive.SetCombinerAlphaICW[i];
        state.psh.alpha_outputs[i] = pg->KelvinPrimitive.SetCombinerAlphaOCW[i];
        //constant_0[i] = pg->pgraph_regs[NV_PGRAPH_COMBINEFACTOR0 + i * 4];
        //constant_1[i] = pg->pgraph_regs[NV_PGRAPH_COMBINEFACTOR1 + i * 4];
    }

    for (i = 0; i < 4; i++) {
        state.psh.rect_tex[i] = false;
        bool enabled = pg->KelvinPrimitive.SetTexture[i].Control0
                         & NV_PGRAPH_TEXCTL0_0_ENABLE;
        unsigned int color_format =
            GET_MASK(pg->KelvinPrimitive.SetTexture[i].Format,
                     NV_PGRAPH_TEXFMT0_COLOR);

        if (enabled && kelvin_color_format_map[color_format].encoding == linear) {
            state.psh.rect_tex[i] = true;
        }

        for (j = 0; j < 4; j++) {
            state.psh.compare_mode[i][j] =
                (pg->KelvinPrimitive.SetShaderClipPlaneMode >> (4 * i + j)) & 1;
        }
        state.psh.alphakill[i] = pg->KelvinPrimitive.SetTexture[i].Control0
                               & NV_PGRAPH_TEXCTL0_0_ALPHAKILLEN;
    }

#ifdef USE_SHADER_CACHE
    ShaderBinding* cached_shader = (ShaderBinding*)g_hash_table_lookup(pg->shader_cache, &state);

    if (cached_shader) {
        pg->shader_binding = cached_shader;
    } else {
#endif
        pg->shader_binding = generate_shaders(state);

#ifdef USE_SHADER_CACHE
        /* cache it */
        ShaderState *cache_state = (ShaderState *)g_malloc(sizeof(*cache_state));
        memcpy(cache_state, &state, sizeof(*cache_state));
        g_hash_table_insert(pg->shader_cache, cache_state,
                            (gpointer)pg->shader_binding);
    }
#endif

    bool binding_changed = (pg->shader_binding != old_binding);

    glUseProgram(pg->shader_binding->gl_program);

    /* Clipping regions */
    for (i = 0; i < state.psh.window_clip_count; i++) {
        if (pg->shader_binding->clip_region_loc[i] == -1) {
            continue;
        }

        uint32_t x   = pg->KelvinPrimitive.SetWindowClipHorizontal[i];
        GLuint x_min = GET_MASK(x, NV_PGRAPH_WINDOWCLIPX0_XMIN);
        GLuint x_max = GET_MASK(x, NV_PGRAPH_WINDOWCLIPX0_XMAX);

        /* Adjust y-coordinates for the OpenGL viewport: translate coordinates
         * to have the origin at the bottom-left of the surface (as opposed to
         * top-left), and flip y-min and y-max accordingly.
         */
        uint32_t y   = pg->KelvinPrimitive.SetWindowClipVertical[i];
        GLuint y_min = (pg->surface_shape.clip_height - 1) -
                       GET_MASK(y, NV_PGRAPH_WINDOWCLIPY0_YMAX);
        GLuint y_max = (pg->surface_shape.clip_height - 1) -
                       GET_MASK(y, NV_PGRAPH_WINDOWCLIPY0_YMIN);

        pgraph_apply_anti_aliasing_factor(pg, &x_min, &y_min);
        pgraph_apply_anti_aliasing_factor(pg, &x_max, &y_max);

        glUniform4i(pg->shader_binding->clip_region_loc[i],
                    x_min, y_min, x_max + 1, y_max + 1);
    }

    pgraph_update_shader_constants(pg, pg->shader_binding, binding_changed,
                                   vertex_program, fixed_function);

    NV2A_GL_DGROUP_END();
}

static bool pgraph_get_framebuffer_dirty(PGRAPHState *pg)
{
    bool shape_changed = memcmp(&pg->surface_shape, &pg->last_surface_shape,
                                sizeof(SurfaceShape)) != 0;
    if (!shape_changed || (!pg->surface_shape.color_format
            && !pg->surface_shape.zeta_format)) {
        return false;
    }
    return true;
}

static bool pgraph_get_color_write_enabled(PGRAPHState *pg)
{
//	return pg->pgraph_regs[NV_PGRAPH_CONTROL_0] & (
//		NV_PGRAPH_CONTROL_0_ALPHA_WRITE_ENABLE
//		| NV_PGRAPH_CONTROL_0_RED_WRITE_ENABLE
//		| NV_PGRAPH_CONTROL_0_GREEN_WRITE_ENABLE
//		| NV_PGRAPH_CONTROL_0_BLUE_WRITE_ENABLE);

	return (pg->KelvinPrimitive.SetColorMask & (
        NV097_SET_COLOR_MASK_ALPHA_WRITE_ENABLE
        | NV097_SET_COLOR_MASK_RED_WRITE_ENABLE
        | NV097_SET_COLOR_MASK_GREEN_WRITE_ENABLE
        | NV097_SET_COLOR_MASK_BLUE_WRITE_ENABLE))!=0;
}

static bool pgraph_get_zeta_write_enabled(PGRAPHState *pg)
{
	//return pg->pgraph_regs[NV_PGRAPH_CONTROL_0] & (		NV_PGRAPH_CONTROL_0_ZWRITEENABLE	|  NV_PGRAPH_CONTROL_0_STENCIL_WRITE_ENABLE);

	return ((pg->KelvinPrimitive.SetDepthMask&0x1!=0)|| (pg->KelvinPrimitive.SetControl0 & NV097_SET_CONTROL0_STENCIL_WRITE_ENABLE)!=0);
}

static void pgraph_set_surface_dirty(PGRAPHState *pg, bool color, bool zeta)
{
    NV2A_DPRINTF("pgraph_set_surface_dirty(%d, %d) -- %d %d\n",
                 color, zeta,
                 pgraph_get_color_write_enabled(pg), pgraph_get_zeta_write_enabled(pg));
    /* FIXME: Does this apply to CLEARs too? */
    color = color && pgraph_get_color_write_enabled(pg);
    zeta = zeta && pgraph_get_zeta_write_enabled(pg);
    pg->surface_color.draw_dirty |= color;
    pg->surface_zeta.draw_dirty |= zeta;
}

static void pgraph_update_surface_part(NV2AState *d, bool upload, bool color) {
    PGRAPHState *pg = &d->pgraph;

    unsigned int width, height;
    pgraph_get_surface_dimensions(pg, &width, &height);
    pgraph_apply_anti_aliasing_factor(pg, &width, &height);

    Surface *surface;
    hwaddr dma_address;
    GLuint *gl_buffer;
    unsigned int bytes_per_pixel;
    GLint gl_internal_format;
    GLenum gl_format, gl_type, gl_attachment;

    if (color) {
        surface = &pg->surface_color;
        dma_address = pg->KelvinPrimitive.SetContextDmaColor;
        gl_buffer = &pg->gl_color_buffer;

        assert(pg->surface_shape.color_format != 0);
        assert(pg->surface_shape.color_format
                < ARRAY_SIZE(kelvin_surface_color_format_map));
        SurfaceColorFormatInfo f =
            kelvin_surface_color_format_map[pg->surface_shape.color_format];
        if (f.bytes_per_pixel == 0) {
            fprintf(stderr, "nv2a: unimplemented color surface format 0x%x\n",
                    pg->surface_shape.color_format);
            abort();
        }

        bytes_per_pixel = f.bytes_per_pixel;
        gl_internal_format = f.gl_internal_format;
        gl_format = f.gl_format;
        gl_type = f.gl_type;
        gl_attachment = GL_COLOR_ATTACHMENT0;

    } else {
        surface = &pg->surface_zeta;
        dma_address = pg->KelvinPrimitive.SetContextDmaZeta;
        gl_buffer = &pg->gl_zeta_buffer;

        assert(pg->surface_shape.zeta_format != 0);
        switch (pg->surface_shape.zeta_format) {
        case NV097_SET_SURFACE_FORMAT_ZETA_Z16:
            bytes_per_pixel = 2;
            gl_format = GL_DEPTH_COMPONENT;
            gl_attachment = GL_DEPTH_ATTACHMENT;
            if (pg->surface_shape.z_format) {
                gl_type = GL_HALF_FLOAT;
                gl_internal_format = GL_DEPTH_COMPONENT32F;
            } else {
                gl_type = GL_UNSIGNED_SHORT;
                gl_internal_format = GL_DEPTH_COMPONENT16;
            }
            break;
        case NV097_SET_SURFACE_FORMAT_ZETA_Z24S8:
            bytes_per_pixel = 4;
            gl_format = GL_DEPTH_STENCIL;
            gl_attachment = GL_DEPTH_STENCIL_ATTACHMENT;
            if (pg->surface_shape.z_format) {
                assert(false);
                gl_type = GL_FLOAT_32_UNSIGNED_INT_24_8_REV;
                gl_internal_format = GL_DEPTH32F_STENCIL8;
            } else {
                gl_type = GL_UNSIGNED_INT_24_8;
                gl_internal_format = GL_DEPTH24_STENCIL8;
            }
            break;
        default:
            assert(false);
            break;
        }
    }


    DMAObject dma = nv_dma_load(d, dma_address);
    /* There's a bunch of bugs that could cause us to hit this function
     * at the wrong time and get a invalid dma object.
     * Check that it's sane. */
    assert(dma.dma_class == NV_DMA_IN_MEMORY_CLASS);

    assert(dma.address + surface->offset != 0);
    assert(surface->offset <= dma.limit);
    assert(surface->offset + surface->pitch * height <= dma.limit + 1);

    hwaddr data_len;
    uint8_t *data = (uint8_t*)nv_dma_map(d, dma_address, &data_len);

    /* TODO */
    // assert(pg->surface_clip_x == 0 && pg->surface_clip_y == 0);

    bool swizzle = (pg->surface_type == NV097_SET_SURFACE_FORMAT_TYPE_SWIZZLE);

    uint8_t *buf = data + surface->offset;
    if (swizzle) {
        buf = (uint8_t*)g_malloc(height * surface->pitch);
    }

    bool dirty = surface->buffer_dirty;
    if (color) {
#if 1
        // HACK: Always mark as dirty
        dirty |= 1;
#else
        dirty |= memory_region_test_and_clear_dirty(d->vram,
                                               dma.address + surface->offset,
                                               surface->pitch * height,
                                               DIRTY_MEMORY_NV2A);
#endif
    }
    if (upload && dirty) {
        /* surface modified (or moved) by the cpu.
         * copy it into the opengl renderbuffer */
        // TODO: Why does this assert?
        //assert(!surface->draw_dirty);
        assert(surface->pitch % bytes_per_pixel == 0);

        if (swizzle) {
            unswizzle_rect(data + surface->offset,
                           width, height,
                           buf,
                           surface->pitch,
                           bytes_per_pixel);
        }

        if (pg->opengl_enabled) {
            if (!color) {
                /* need to clear the depth_stencil and depth attachment for zeta */
                glFramebufferTexture2D(GL_FRAMEBUFFER,
                                       GL_DEPTH_ATTACHMENT,
                                       GL_TEXTURE_2D,
                                       0, 0);
                glFramebufferTexture2D(GL_FRAMEBUFFER,
                                       GL_DEPTH_STENCIL_ATTACHMENT,
                                       GL_TEXTURE_2D,
                                       0, 0);
            }

            glFramebufferTexture2D(GL_FRAMEBUFFER,
                                   gl_attachment,
                                   GL_TEXTURE_2D,
                                   0, 0);

            if (*gl_buffer) {
                glDeleteTextures(1, gl_buffer);
                *gl_buffer = 0;
            }

            glGenTextures(1, gl_buffer);
            glBindTexture(GL_TEXTURE_2D, *gl_buffer);

            /* This is VRAM so we can't do this inplace! */
            uint8_t *flipped_buf = (uint8_t*)g_malloc(width * height * bytes_per_pixel);
            unsigned int irow;
            for (irow = 0; irow < height; irow++) {
                memcpy(&flipped_buf[width * (height - irow - 1)
                                         * bytes_per_pixel],
                       &buf[surface->pitch * irow],
                       width * bytes_per_pixel);
            }

            glTexImage2D(GL_TEXTURE_2D, 0, gl_internal_format,
                         width, height, 0,
                         gl_format, gl_type,
                         flipped_buf);

            g_free(flipped_buf);

            glFramebufferTexture2D(GL_FRAMEBUFFER,
                                   gl_attachment,
                                   GL_TEXTURE_2D,
                                   *gl_buffer, 0);

            assert(glCheckFramebufferStatus(GL_FRAMEBUFFER)
                == GL_FRAMEBUFFER_COMPLETE);
        }

        if (color) {
            pgraph_update_memory_buffer(d, dma.address + surface->offset,
                                        surface->pitch * height, true);
        }
        surface->buffer_dirty = false;

#ifdef DEBUG_NV2A
        uint8_t *out = data + surface->offset + 64;
        NV2A_DPRINTF("upload_surface %s 0x%" HWADDR_PRIx " - 0x%" HWADDR_PRIx ", "
                      "(0x%" HWADDR_PRIx " - 0x%" HWADDR_PRIx ", "
                        "%d %d, %d %d, %d) - %x %x %x %x\n",
            color ? "color" : "zeta",
            dma.address, dma.address + dma.limit,
            dma.address + surface->offset,
            dma.address + surface->pitch * height,
            pg->surface_shape.clip_x, pg->surface_shape.clip_y,
            pg->surface_shape.clip_width,
            pg->surface_shape.clip_height,
            surface->pitch,
            out[0], out[1], out[2], out[3]);
#endif
    }

    if (!upload && surface->draw_dirty) {
        if (pg->opengl_enabled) {
            /* read the opengl framebuffer into the surface */

            glo_readpixels(gl_format, gl_type,
                           bytes_per_pixel, surface->pitch,
                           width, height,
                           buf);

//			assert(glGetError() == GL_NO_ERROR);
        }

        if (swizzle) {
            swizzle_rect(buf,
                         width, height,
                         data + surface->offset,
                         surface->pitch,
                         bytes_per_pixel);
        }

        // memory_region_set_client_dirty(d->vram,
        //                                dma.address + surface->offset,
        //                                surface->pitch * height,
        //                                DIRTY_MEMORY_VGA);

        if (color) {
            pgraph_update_memory_buffer(d, dma.address + surface->offset,
                                        surface->pitch * height, true);
        }

        surface->draw_dirty = false;
        surface->write_enabled_cache = false;

#ifdef DEBUG_NV2A
        uint8_t *out = data + surface->offset + 64;
        NV2A_DPRINTF("read_surface %s 0x%" HWADDR_PRIx " - 0x%" HWADDR_PRIx ", "
                      "(0x%" HWADDR_PRIx " - 0x%" HWADDR_PRIx ", "
                        "%d %d, %d %d, %d) - %x %x %x %x\n",
            color ? "color" : "zeta",
            dma.address, dma.address + dma.limit,
            dma.address + surface->offset,
            dma.address + surface->pitch * pg->surface_shape.clip_height,
            pg->surface_shape.clip_x, pg->surface_shape.clip_y,
            pg->surface_shape.clip_width, pg->surface_shape.clip_height,
            surface->pitch,
            out[0], out[1], out[2], out[3]);
#endif
    }

    if (swizzle) {
        g_free(buf);
    }
}

static void pgraph_update_surface(NV2AState *d, bool upload,
    bool color_write, bool zeta_write)
{
    PGRAPHState *pg = &d->pgraph;

    if (!pg->opengl_enabled) {
        return;
    }

    pg->surface_shape.z_format = GET_MASK(pg->KelvinPrimitive.SetControl0, NV097_SET_CONTROL0_Z_FORMAT);

    /* FIXME: Does this apply to CLEARs too? */
    color_write = color_write && pgraph_get_color_write_enabled(pg);
    zeta_write = zeta_write && pgraph_get_zeta_write_enabled(pg);

    if (upload && pgraph_get_framebuffer_dirty(pg)) {
        assert(!pg->surface_color.draw_dirty);
        assert(!pg->surface_zeta.draw_dirty);

        pg->surface_color.buffer_dirty = true;
        pg->surface_zeta.buffer_dirty = true;

        glFramebufferTexture2D(GL_FRAMEBUFFER,
                                GL_COLOR_ATTACHMENT0,
                                GL_TEXTURE_2D,
                                0, 0);

        if (pg->gl_color_buffer) {
            glDeleteTextures(1, &pg->gl_color_buffer);
            pg->gl_color_buffer = 0;
        }

        glFramebufferTexture2D(GL_FRAMEBUFFER,
                                GL_DEPTH_ATTACHMENT,
                                GL_TEXTURE_2D,
                                0, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER,
                                GL_DEPTH_STENCIL_ATTACHMENT,
                                GL_TEXTURE_2D,
                                0, 0);

        if (pg->gl_zeta_buffer) {
            glDeleteTextures(1, &pg->gl_zeta_buffer);
            pg->gl_zeta_buffer = 0;
        }

        memcpy(&pg->last_surface_shape, &pg->surface_shape,
            sizeof(SurfaceShape));
    }
	/* //disable for now ,since it's using LLE dma object. we're wait for our HLE alternative.
    if ((color_write || (!upload && pg->surface_color.write_enabled_cache))
        && (upload || pg->surface_color.draw_dirty)) {
        pgraph_update_surface_part(d, upload, true);
    }


    if ((zeta_write || (!upload && pg->surface_zeta.write_enabled_cache))
        && (upload || pg->surface_zeta.draw_dirty)) {
        pgraph_update_surface_part(d, upload, false);
    }
	*/
}

static void pgraph_bind_textures(NV2AState *d)
{
    int i;
    PGRAPHState *pg = &d->pgraph;

    if (!(pg->opengl_enabled))
        return;

    NV2A_GL_DGROUP_BEGIN("%s", __func__);

    for (i=0; i<NV2A_MAX_TEXTURES; i++) {

        uint32_t ctl_0 = pg->KelvinPrimitive.SetTexture[i].Control0;
        uint32_t ctl_1 = pg->KelvinPrimitive.SetTexture[i].Control1;
        uint32_t fmt = pg->KelvinPrimitive.SetTexture[i].Format;
        uint32_t filter = pg->KelvinPrimitive.SetTexture[i].Filter;
        uint32_t address = pg->KelvinPrimitive.SetTexture[i].Address;
        uint32_t palette =  pg->KelvinPrimitive.SetTexture[i].Palette;

        bool enabled = ctl_0 & NV_PGRAPH_TEXCTL0_0_ENABLE;
        unsigned int min_mipmap_level =
            GET_MASK(ctl_0, NV_PGRAPH_TEXCTL0_0_MIN_LOD_CLAMP);
        unsigned int max_mipmap_level =
            GET_MASK(ctl_0, NV_PGRAPH_TEXCTL0_0_MAX_LOD_CLAMP);

        unsigned int pitch =
            GET_MASK(ctl_1, NV_PGRAPH_TEXCTL1_0_IMAGE_PITCH);

        bool dma_select =
            fmt & NV_PGRAPH_TEXFMT0_CONTEXT_DMA;
        bool cubemap =
            fmt & NV_PGRAPH_TEXFMT0_CUBEMAPENABLE;
        unsigned int dimensionality =
            GET_MASK(fmt, NV_PGRAPH_TEXFMT0_DIMENSIONALITY);
        unsigned int color_format = GET_MASK(fmt, NV_PGRAPH_TEXFMT0_COLOR);
        unsigned int levels = GET_MASK(fmt, NV_PGRAPH_TEXFMT0_MIPMAP_LEVELS);
        unsigned int log_width = GET_MASK(fmt, NV_PGRAPH_TEXFMT0_BASE_SIZE_U);
        unsigned int log_height = GET_MASK(fmt, NV_PGRAPH_TEXFMT0_BASE_SIZE_V);
        unsigned int log_depth = GET_MASK(fmt, NV_PGRAPH_TEXFMT0_BASE_SIZE_P);

        unsigned int rect_width =
            GET_MASK(pg->KelvinPrimitive.SetTexture[i].ImageRect,
                     NV_PGRAPH_TEXIMAGERECT0_WIDTH);
        unsigned int rect_height =
            GET_MASK(pg->KelvinPrimitive.SetTexture[i].ImageRect,
                     NV_PGRAPH_TEXIMAGERECT0_HEIGHT);
#ifdef DEBUG_NV2A
        unsigned int lod_bias =
            GET_MASK(filter, NV_PGRAPH_TEXFILTER0_MIPMAP_LOD_BIAS);
#endif
        unsigned int min_filter = GET_MASK(filter, NV_PGRAPH_TEXFILTER0_MIN);
        unsigned int mag_filter = GET_MASK(filter, NV_PGRAPH_TEXFILTER0_MAG);

        unsigned int addru = GET_MASK(address, NV_PGRAPH_TEXADDRESS0_ADDRU);
        unsigned int addrv = GET_MASK(address, NV_PGRAPH_TEXADDRESS0_ADDRV);
        unsigned int addrp = GET_MASK(address, NV_PGRAPH_TEXADDRESS0_ADDRP);

        bool border_source_color = (fmt & NV_PGRAPH_TEXFMT0_BORDER_SOURCE); // != NV_PGRAPH_TEXFMT0_BORDER_SOURCE_TEXTURE;
        uint32_t border_color = pg->KelvinPrimitive.SetTexture[i].BorderColor;

        unsigned int offset = pg->KelvinPrimitive.SetTexture[i].Offset;

        bool palette_dma_select =
            palette & NV_PGRAPH_TEXPALETTE0_CONTEXT_DMA;
        unsigned int palette_length_index =
            GET_MASK(palette, NV_PGRAPH_TEXPALETTE0_LENGTH);
        unsigned int palette_offset =
            palette & NV_PGRAPH_TEXPALETTE0_OFFSET;

        unsigned int palette_length = 0;
        switch (palette_length_index) {
        case NV_PGRAPH_TEXPALETTE0_LENGTH_256: palette_length = 256; break;
        case NV_PGRAPH_TEXPALETTE0_LENGTH_128: palette_length = 128; break;
        case NV_PGRAPH_TEXPALETTE0_LENGTH_64: palette_length = 64; break;
        case NV_PGRAPH_TEXPALETTE0_LENGTH_32: palette_length = 32; break;
        default: assert(false); break;
        }

        /* Check for unsupported features */
        assert(!(filter & NV_PGRAPH_TEXFILTER0_ASIGNED));
        assert(!(filter & NV_PGRAPH_TEXFILTER0_RSIGNED));
        assert(!(filter & NV_PGRAPH_TEXFILTER0_GSIGNED));
        assert(!(filter & NV_PGRAPH_TEXFILTER0_BSIGNED));

        glActiveTexture(GL_TEXTURE0 + i);
        if (!enabled) {
            glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
            glBindTexture(GL_TEXTURE_RECTANGLE, 0);
            glBindTexture(GL_TEXTURE_1D, 0);
            glBindTexture(GL_TEXTURE_2D, 0);
            glBindTexture(GL_TEXTURE_3D, 0);
            continue;
        }

        if (!pg->texture_dirty[i] && pg->texture_binding[i]) {
            glBindTexture(pg->texture_binding[i]->gl_target,
                          pg->texture_binding[i]->gl_texture);
            continue;
        }

        NV2A_DPRINTF(" texture %d is format 0x%x, off 0x%x (r %d, %d or %d, %d, %d; %d%s),"
                        " filter %x %x, levels %d-%d %d bias %d\n",
                     i, color_format, offset,
                     rect_width, rect_height,
                     1 << log_width, 1 << log_height, 1 << log_depth,
                     pitch,
                     cubemap ? "; cubemap" : "",
                     min_filter, mag_filter,
                     min_mipmap_level, max_mipmap_level, levels,
                     lod_bias);

        assert(color_format < ARRAY_SIZE(kelvin_color_format_map));
        ColorFormatInfo f = kelvin_color_format_map[color_format];
        if (f.bytes_per_pixel == 0) {
            fprintf(stderr, "nv2a: unimplemented texture color format 0x%x\n",
                    color_format);
            abort();
        }

        unsigned int width, height, depth;
        if (f.encoding == linear) {
            assert(dimensionality == 2);
            width = rect_width;
            height = rect_height;
            depth = 1;
        } else {
            width = 1 << log_width;
            height = 1 << log_height;
            depth = 1 << log_depth;

            /* FIXME: What about 3D mipmaps? */
            levels = MIN(levels, max_mipmap_level + 1);
            if (f.encoding == swizzled) {
                /* Discard mipmap levels that would be smaller than 1x1.
                 * FIXME: Is this actually needed?
                 *
                 * >> Level 0: 32 x 4
                 *    Level 1: 16 x 2
                 *    Level 2: 8 x 1
                 *    Level 3: 4 x 1
                 *    Level 4: 2 x 1
                 *    Level 5: 1 x 1
                 */
                levels = MIN(levels, MAX(log_width, log_height) + 1);
            } else {
                /* OpenGL requires DXT textures to always have a width and
                 * height a multiple of 4. The Xbox and DirectX handles DXT
                 * textures smaller than 4 by padding the reset of the block.
                 *
                 * See:
                 * https://msdn.microsoft.com/en-us/library/windows/desktop/bb204843(v=vs.85).aspx
                 * https://msdn.microsoft.com/en-us/library/windows/desktop/bb694531%28v=vs.85%29.aspx#Virtual_Size
                 *
                 * Work around this for now by discarding mipmap levels that
                 * would result in too-small textures. A correct solution
                 * will be to decompress these levels manually, or add texture
                 * sampling logic.
                 *
                 * >> Level 0: 64 x 8
                 *    Level 1: 32 x 4
                 *    Level 2: 16 x 2 << Ignored
                 * >> Level 0: 16 x 16
                 *    Level 1: 8 x 8
                 *    Level 2: 4 x 4 << OK!
                 */
                if (log_width < 2 || log_height < 2) {
                    /* Base level is smaller than 4x4... */
                    levels = 1;
                } else {
                    levels = MIN(levels, MIN(log_width, log_height) - 1);
                }
            }
            assert(levels > 0);
        }

        hwaddr dma_len;
        uint8_t *texture_data;
        if (dma_select) {
            texture_data = (uint8_t*)nv_dma_map(d, pg->KelvinPrimitive.SetContextDmaB, &dma_len);
        } else {
            texture_data = (uint8_t*)nv_dma_map(d, pg->KelvinPrimitive.SetContextDmaA, &dma_len);
        }
        assert(offset < dma_len);
        texture_data += offset;

        hwaddr palette_dma_len;
        uint8_t *palette_data;
        if (palette_dma_select) {
            palette_data = (uint8_t*)nv_dma_map(d, pg->KelvinPrimitive.SetContextDmaB, &palette_dma_len);
        } else {
            palette_data = (uint8_t*)nv_dma_map(d, pg->KelvinPrimitive.SetContextDmaA, &palette_dma_len);
        }
        assert(palette_offset < palette_dma_len);
        palette_data += palette_offset;

        NV2A_DPRINTF(" - 0x%tx\n", texture_data - d->vram_ptr);

        size_t length = 0;
        if (f.encoding == linear) {
            assert(cubemap == false);
            assert(dimensionality == 2);
            length = height * pitch;
        } else {
            if (dimensionality >= 2) {
                unsigned int w = width, h = height;
                unsigned int level;
                if (f.encoding == swizzled) {
                    for (level = 0; level < levels; level++) {
                        w = MAX(w, 1); h = MAX(h, 1);
                        length += w * h * f.bytes_per_pixel;
                        w /= 2;
                        h /= 2;
                    }
                } else {
                    /* Compressed textures are a bit different */
                    unsigned int block_size;
                    if (f.gl_internal_format ==
                            GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) {
                        block_size = 8;
                    } else {
                        block_size = 16;
                    }

                    for (level = 0; level < levels; level++) {
                        w = MAX(w, 4); h = MAX(h, 4);
                        length += w/4 * h/4 * block_size;
                        w /= 2; h /= 2;
                    }
                }
                if (cubemap) {
                    assert(dimensionality == 2);
                    length *= 6;
                }
                if (dimensionality >= 3) {
                    length *= depth;
                }
            }
        }

        TextureShape state;
        state.cubemap = cubemap;
        state.dimensionality = dimensionality;
        state.color_format = color_format;
        state.levels = levels;
        state.width = width;
        state.height = height;
        state.depth = depth;
        state.min_mipmap_level = min_mipmap_level;
        state.max_mipmap_level = max_mipmap_level;
        state.pitch = pitch;

#ifdef USE_TEXTURE_CACHE
        TextureKey key;
        key.state = state;
        key.data_hash = fast_hash(texture_data, length, 5003)
            ^ fnv_hash(palette_data, palette_length);
        key.texture_data = texture_data;
        key.palette_data = palette_data;

        gpointer cache_key = g_malloc(sizeof(TextureKey));
        memcpy(cache_key, &key, sizeof(TextureKey));

        GError *err;
        TextureBinding *binding = (TextureBinding *)g_lru_cache_get(pg->texture_cache, cache_key, &err);
        assert(binding);
        binding->refcnt++;
#else
        TextureBinding *binding = generate_texture(state,
                                                   texture_data, palette_data);
#endif

        glBindTexture(binding->gl_target, binding->gl_texture);


        if (f.encoding == linear) {
            /* sometimes games try to set mipmap min filters on linear textures.
             * this could indicate a bug... */
            switch (min_filter) {
            case NV_PGRAPH_TEXFILTER0_MIN_BOX_NEARESTLOD:
            case NV_PGRAPH_TEXFILTER0_MIN_BOX_TENT_LOD:
                min_filter = NV_PGRAPH_TEXFILTER0_MIN_BOX_LOD0;
                break;
            case NV_PGRAPH_TEXFILTER0_MIN_TENT_NEARESTLOD:
            case NV_PGRAPH_TEXFILTER0_MIN_TENT_TENT_LOD:
                min_filter = NV_PGRAPH_TEXFILTER0_MIN_TENT_LOD0;
                break;
            }
        }

        glTexParameteri(binding->gl_target, GL_TEXTURE_MIN_FILTER,
            pgraph_texture_min_filter_map[min_filter]);
        glTexParameteri(binding->gl_target, GL_TEXTURE_MAG_FILTER,
            pgraph_texture_mag_filter_map[mag_filter]);

        /* Texture wrapping */
        assert(addru < ARRAY_SIZE(pgraph_texture_addr_map));
        glTexParameteri(binding->gl_target, GL_TEXTURE_WRAP_S,
            pgraph_texture_addr_map[addru]);
        if (dimensionality > 1) {
            assert(addrv < ARRAY_SIZE(pgraph_texture_addr_map));
            glTexParameteri(binding->gl_target, GL_TEXTURE_WRAP_T,
                pgraph_texture_addr_map[addrv]);
        }
        if (dimensionality > 2) {
            assert(addrp < ARRAY_SIZE(pgraph_texture_addr_map));
            glTexParameteri(binding->gl_target, GL_TEXTURE_WRAP_R,
                pgraph_texture_addr_map[addrp]);
        }

        /* FIXME: Only upload if necessary? [s, t or r = GL_CLAMP_TO_BORDER] */
        if (border_source_color) {
            GLfloat gl_border_color[] = {
                /* FIXME: Color channels might be wrong order */
                ((border_color >> 16) & 0xFF) / 255.0f, /* red */
                ((border_color >> 8) & 0xFF) / 255.0f,  /* green */
                (border_color & 0xFF) / 255.0f,         /* blue */
                ((border_color >> 24) & 0xFF) / 255.0f  /* alpha */
            };
            glTexParameterfv(binding->gl_target, GL_TEXTURE_BORDER_COLOR,
                gl_border_color);
        }

        if (pg->texture_binding[i]) {
            texture_binding_destroy(pg->texture_binding[i]);
        }
        pg->texture_binding[i] = binding;
        pg->texture_dirty[i] = false;
    }
    NV2A_GL_DGROUP_END();
}

static void pgraph_apply_anti_aliasing_factor(PGRAPHState *pg,
                                              unsigned int *width,
                                              unsigned int *height)
{
    switch (pg->surface_shape.anti_aliasing) {
    case NV097_SET_SURFACE_FORMAT_ANTI_ALIASING_CENTER_1:
        break;
    case NV097_SET_SURFACE_FORMAT_ANTI_ALIASING_CENTER_CORNER_2:
        if (width) { *width *= 2; }
        break;
    case NV097_SET_SURFACE_FORMAT_ANTI_ALIASING_SQUARE_OFFSET_4:
        if (width) { *width *= 2; }
        if (height) { *height *= 2; }
        break;
    default:
        assert(false);
        break;
    }
}

static void pgraph_get_surface_dimensions(PGRAPHState *pg,
                                          unsigned int *width,
                                          unsigned int *height)
{
    bool swizzle = (pg->surface_type == NV097_SET_SURFACE_FORMAT_TYPE_SWIZZLE);
    if (swizzle) {
        *width = 1 << pg->surface_shape.log_width;
        *height = 1 << pg->surface_shape.log_height;
    } else {
        *width = pg->surface_shape.clip_width;
        *height = pg->surface_shape.clip_height;
    }
}

static void pgraph_update_memory_buffer(NV2AState *d, hwaddr addr, hwaddr size,
                                        bool f)
{
    glBindBuffer(GL_ARRAY_BUFFER, d->pgraph.gl_memory_buffer);

    hwaddr end = TARGET_PAGE_ALIGN(addr + size);
    addr &= TARGET_PAGE_MASK;

    assert(end < d->vram_size);

    // if (f || memory_region_test_and_clear_dirty(d->vram,
    //                                             addr,
    //                                             end - addr,
    //                                             DIRTY_MEMORY_NV2A)) {
        glBufferSubData(GL_ARRAY_BUFFER, addr, end - addr, d->vram_ptr + addr);
    // }

//		auto error = glGetError();
//		assert(error == GL_NO_ERROR);
}

static void pgraph_bind_vertex_attributes(NV2AState *d,
                                          unsigned int num_elements,
                                          bool inline_data,
                                          unsigned int inline_stride)
{
    unsigned int i, j;
    PGRAPHState *pg = &d->pgraph;

    assert(pg->opengl_enabled);

    if (inline_data) {
        NV2A_GL_DGROUP_BEGIN("%s (num_elements: %d inline stride: %d)",
                             __func__, num_elements, inline_stride);
    } else {
        NV2A_GL_DGROUP_BEGIN("%s (num_elements: %d)", __func__, num_elements);
    }

    for (i=0; i<NV2A_VERTEXSHADER_ATTRIBUTES; i++) {
        VertexAttribute *vertex_attribute = &pg->vertex_attributes[i];
        if (vertex_attribute->count) {
            uint8_t *data;
            unsigned int in_stride;
            if (inline_data && vertex_attribute->needs_conversion) {
                data = (uint8_t*)pg->inline_array
                        + vertex_attribute->inline_array_offset;
                in_stride = inline_stride;
            } else {
                hwaddr dma_len;
                if (vertex_attribute->dma_select) {
                    data = (uint8_t*)nv_dma_map(d, pg->KelvinPrimitive.SetContextDmaVertexB, &dma_len);
                } else {
                    data = (uint8_t*)nv_dma_map(d, pg->KelvinPrimitive.SetContextDmaVertexA, &dma_len);
                }

                assert(vertex_attribute->offset < dma_len);
                data += vertex_attribute->offset;

                in_stride = vertex_attribute->stride;
            }

            if (vertex_attribute->needs_conversion) {
                NV2A_DPRINTF("converted %d\n", i);

                unsigned int out_stride = vertex_attribute->converted_size
                                        * vertex_attribute->converted_count;

                if (num_elements > vertex_attribute->converted_elements) {
                    vertex_attribute->converted_buffer = (uint8_t*)g_realloc(
                        vertex_attribute->converted_buffer,
                        num_elements * out_stride);
                }

                for (j=vertex_attribute->converted_elements; j<num_elements; j++) {
                    uint8_t *in = data + j * in_stride;
                    uint8_t *out = vertex_attribute->converted_buffer + j * out_stride;

                    switch (vertex_attribute->format) {
                    case NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_CMP: {
                        uint32_t p = ldl_le_p((uint32_t*)in);
                        float *xyz = (float*)out;
                        xyz[0] = ((int32_t)(((p >>  0) & 0x7FF) << 21) >> 21)
                                                                      / 1023.0f;
                        xyz[1] = ((int32_t)(((p >> 11) & 0x7FF) << 21) >> 21)
                                                                      / 1023.0f;
                        xyz[2] = ((int32_t)(((p >> 22) & 0x3FF) << 22) >> 22)
                                                                       / 511.0f;
                        break;
                    }
                    default:
                        assert(false);
                        break;
                    }
                }


                glBindBuffer(GL_ARRAY_BUFFER, vertex_attribute->gl_converted_buffer);
                if (num_elements != vertex_attribute->converted_elements) {
                    glBufferData(GL_ARRAY_BUFFER,
                                 num_elements * out_stride,
                                 vertex_attribute->converted_buffer,
                                 GL_DYNAMIC_DRAW);
                    vertex_attribute->converted_elements = num_elements;
                }


                glVertexAttribPointer(i,
                    vertex_attribute->converted_count,
                    vertex_attribute->gl_type,
                    vertex_attribute->gl_normalize,
                    out_stride,
                    0);
            } else if (inline_data) {
                glBindBuffer(GL_ARRAY_BUFFER, pg->gl_inline_array_buffer);
                glVertexAttribPointer(i,
                                      vertex_attribute->gl_count,
                                      vertex_attribute->gl_type,
                                      vertex_attribute->gl_normalize,
                                      inline_stride,
                                      (void*)(uintptr_t)vertex_attribute->inline_array_offset);
            } else {
                hwaddr addr = data - d->vram_ptr;
                pgraph_update_memory_buffer(d, addr,
                                            num_elements * vertex_attribute->stride,
                                            false);
                glVertexAttribPointer(i,
                    vertex_attribute->gl_count,
                    vertex_attribute->gl_type,
                    vertex_attribute->gl_normalize,
                    vertex_attribute->stride,
                    (void*)(uint64_t)(addr));
            }
            glEnableVertexAttribArray(i);
        } else {
            glDisableVertexAttribArray(i);

            glVertexAttrib4fv(i, vertex_attribute->inline_value);
        }
    }
    NV2A_GL_DGROUP_END();
}

static unsigned int pgraph_bind_inline_array(NV2AState *d)
{
    int i;

    PGRAPHState *pg = &d->pgraph;

    assert(pg->opengl_enabled);

    unsigned int offset = 0;
    for (i=0; i<NV2A_VERTEXSHADER_ATTRIBUTES; i++) {
        VertexAttribute *vertex_attribute = &pg->vertex_attributes[i];
        if (vertex_attribute->count) {
            vertex_attribute->inline_array_offset = offset;

            NV2A_DPRINTF("bind inline vertex_attribute %d size=%d, count=%d\n",
                i, vertex_attribute->size, vertex_attribute->count);
            offset += vertex_attribute->size * vertex_attribute->count;
            assert(offset % 4 == 0);
        }
    }

    unsigned int vertex_size = offset;


    unsigned int index_count = pg->inline_array_length*4 / vertex_size;

    NV2A_DPRINTF("draw inline array %d, %d\n", vertex_size, index_count);

    glBindBuffer(GL_ARRAY_BUFFER, pg->gl_inline_array_buffer);
    glBufferData(GL_ARRAY_BUFFER, pg->inline_array_length*4, pg->inline_array,
                 GL_DYNAMIC_DRAW);

    pgraph_bind_vertex_attributes(d, index_count, true, vertex_size);

    return index_count;
}

/* 16 bit to [0.0, F16_MAX = 511.9375] */
static float convert_f16_to_float(uint16_t f16) {
    if (f16 == 0x0000) { return 0.0f; }
    uint32_t i = (f16 << 11) + 0x3C000000;
    return *(float*)&i;
}

/* 24 bit to [0.0, F24_MAX] */
static float convert_f24_to_float(uint32_t f24) {
    assert(!(f24 >> 24));
    f24 &= 0xFFFFFF;
    if (f24 == 0x000000) { return 0.0f; }
    uint32_t i = f24 << 7;
    return *(float*)&i;
}

extern void __R6G5B5ToARGBRow_C(const uint8_t* src_r6g5b5, uint8_t* dst_argb, int width);
extern void ____YUY2ToARGBRow_C(const uint8_t* src_yuy2, uint8_t* rgb_buf, int width);
extern void ____UYVYToARGBRow_C(const uint8_t* src_uyvy, uint8_t* rgb_buf, int width);

/* 'converted_format' indicates the format that results when convert_texture_data() returns non-NULL converted_data. */
static const int converted_format = NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A8R8G8B8;

static uint8_t* convert_texture_data(const unsigned int color_format,
                                     const uint8_t *data,
                                     const uint8_t *palette_data,
                                     const unsigned int width,
                                     const unsigned int height,
                                     const unsigned int depth,
                                     const unsigned int row_pitch,
                                     const unsigned int slice_pitch)
{
    // Note : Unswizzle is already done when entering here
    switch (color_format) {
    case NV097_SET_TEXTURE_FORMAT_COLOR_SZ_I8_A8R8G8B8: {
        // Test-case : WWE RAW2
        assert(depth == 1); /* FIXME */
        uint8_t* converted_data = (uint8_t*)g_malloc(width * height * 4);
        unsigned int x, y;
        for (y = 0; y < height; y++) {
            for (x = 0; x < width; x++) {
                uint8_t index = data[y * row_pitch + x];
                uint32_t color = *(uint32_t*)(palette_data + index * 4);
                *(uint32_t*)(converted_data + y * width * 4 + x * 4) = color;
            }
        }
        return converted_data;
    }
    case NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_X7SY9: {
        assert(false); /* FIXME */
        return NULL;
    }
    case NV097_SET_TEXTURE_FORMAT_COLOR_LC_IMAGE_CR8YB8CB8YA8: {
        // Test-case : WWE RAW2
        assert(depth == 1); /* FIXME */
        uint8_t* converted_data = (uint8_t*)g_malloc(width * height * 4);
        unsigned int y;
        for (y = 0; y < height; y++) {
            const uint8_t* line = &data[y * width * 2];
            uint8_t* pixel = &converted_data[(y * width) * 4];
            ____YUY2ToARGBRow_C(line, pixel, width);
            // Note : LC_IMAGE_CR8YB8CB8YA8 suggests UYVY format,
            // but for an unknown reason, the actual encoding is YUY2
        }
        return converted_data;
    }	
    case NV097_SET_TEXTURE_FORMAT_COLOR_LC_IMAGE_YB8CR8YA8CB8: {
        assert(depth == 1); /* FIXME */
        uint8_t* converted_data = (uint8_t*)g_malloc(width * height * 4);
        unsigned int y;
        for (y = 0; y < height; y++) {
            const uint8_t* line = &data[y * width * 2];
            uint8_t* pixel = &converted_data[(y * width) * 4];
            ____UYVYToARGBRow_C(line, pixel, width); // TODO : Validate LC_IMAGE_YB8CR8YA8CB8 indeed requires ____UYVYToARGBRow_C()
        }
        return converted_data;
    }
    case NV097_SET_TEXTURE_FORMAT_COLOR_LC_IMAGE_A4V6YB6A4U6YA6: {
        assert(false); /* FIXME */
        return NULL;
    }
    case NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A8CR8CB8Y8: {
        assert(false); /* FIXME */
        return NULL;
    }
    case NV097_SET_TEXTURE_FORMAT_COLOR_SZ_R6G5B5:
    case NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_R6G5B5: {
        assert(depth == 1); /* FIXME */
        uint8_t *converted_data = (uint8_t*)g_malloc(width * height * 4);
        unsigned int y;
        for (y = 0; y < height; y++) {
            uint16_t rgb655 = *(uint16_t*)(data + y * row_pitch);
            int8_t *pixel = (int8_t*)&converted_data[(y * width) * 4];
            __R6G5B5ToARGBRow_C((const uint8_t*)rgb655, (uint8_t*)pixel, width);
        }
        return converted_data;
    }
    default:
        return NULL;
    }
}

/* returns the format of the output, either identical to the input format, or the converted format - see converted_format */
static int upload_gl_texture(GLenum gl_target,
                              const TextureShape s,
                              const uint8_t *texture_data,
                              const uint8_t *palette_data)
{
    //assert(pg->opengl_enabled);
    int resulting_format = s.color_format;
    ColorFormatInfo f = kelvin_color_format_map[s.color_format];

    switch(gl_target) {
    case GL_TEXTURE_1D:
        assert(false);
        break;
    case GL_TEXTURE_RECTANGLE: {
        /* Can't handle strides unaligned to pixels */
        assert(s.pitch % f.bytes_per_pixel == 0);
        glPixelStorei(GL_UNPACK_ROW_LENGTH,
                      s.pitch / f.bytes_per_pixel);

        uint8_t *unswizzled = NULL;
        if (f.encoding == swizzled) { // TODO : Verify this works correctly
            unswizzled = (uint8_t*)g_malloc(s.height * s.pitch);
            unswizzle_rect(texture_data, s.width, s.height,
                            unswizzled, s.pitch, f.bytes_per_pixel);
        }
        uint8_t *converted = convert_texture_data(s.color_format, unswizzled ? unswizzled : texture_data,
                                                  palette_data,
                                                  s.width, s.height, 1,
                                                  s.pitch, 0);

        resulting_format = converted ? converted_format : s.color_format;
        ColorFormatInfo cf = kelvin_color_format_map[resulting_format];
        glTexImage2D(gl_target, 0, cf.gl_internal_format,
                     s.width, s.height, 0,
                     cf.gl_format, cf.gl_type,
                     converted ? converted : unswizzled ? unswizzled : texture_data);

        if (converted) {
          g_free(converted);
        }
        if (unswizzled) {
            g_free(unswizzled);
        }

        glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
        break;
    }
    case GL_TEXTURE_2D:
    case GL_TEXTURE_CUBE_MAP_POSITIVE_X:
    case GL_TEXTURE_CUBE_MAP_NEGATIVE_X:
    case GL_TEXTURE_CUBE_MAP_POSITIVE_Y:
    case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y:
    case GL_TEXTURE_CUBE_MAP_POSITIVE_Z:
    case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z: {

        unsigned int width = s.width, height = s.height;

        unsigned int level;
        for (level = 0; level < s.levels; level++) {
            if (f.encoding == compressed) {

                width = MAX(width, 4); height = MAX(height, 4);

                unsigned int block_size;
                if (f.gl_internal_format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) {
                    block_size = 8;
                } else {
                    block_size = 16;
                }

                glCompressedTexImage2D(gl_target, level, f.gl_internal_format,
                                       width, height, 0,
                                       width/4 * height/4 * block_size,
                                       texture_data);

                texture_data += width/4 * height/4 * block_size;
            } else {

                width = MAX(width, 1); height = MAX(height, 1);

                unsigned int pitch = width * f.bytes_per_pixel;
                uint8_t *unswizzled = NULL;
                if (f.encoding == swizzled) {
                    unswizzled = (uint8_t*)g_malloc(height * pitch);
                    unswizzle_rect(texture_data, width, height,
                                   unswizzled, pitch, f.bytes_per_pixel);
                }

                uint8_t *converted = convert_texture_data(s.color_format, unswizzled ? unswizzled : texture_data,
                                                          palette_data,
                                                          width, height, 1,
                                                          pitch, 0);

                resulting_format = converted ? converted_format : s.color_format;
                ColorFormatInfo cf = kelvin_color_format_map[resulting_format];
                glTexImage2D(gl_target, level, cf.gl_internal_format,
                             width, height, 0,
                             cf.gl_format, cf.gl_type,
                             converted ? converted : unswizzled ? unswizzled : texture_data);

                if (converted) {
                    g_free(converted);
                }
                if (unswizzled) {
                    g_free(unswizzled);
                }

                texture_data += pitch * height;
            }

            width /= 2;
            height /= 2;
        }

        break;
    }
    case GL_TEXTURE_3D: {

        unsigned int width = s.width, height = s.height, depth = s.depth;

        unsigned int level;
        for (level = 0; level < s.levels; level++) {

            unsigned int row_pitch = width * f.bytes_per_pixel;
            unsigned int slice_pitch = row_pitch * height;
            uint8_t *unswizzled = NULL;
            if (f.encoding == swizzled) {
                unswizzled = (uint8_t*)g_malloc(slice_pitch * depth);
                unswizzle_box(texture_data, width, height, depth, unswizzled,
                               row_pitch, slice_pitch, f.bytes_per_pixel);
            }
            uint8_t *converted = convert_texture_data(s.color_format, unswizzled ? unswizzled : texture_data,
                                                      palette_data,
                                                      width, height, depth,
                                                      row_pitch, slice_pitch);

            resulting_format = converted ? converted_format : s.color_format;
            ColorFormatInfo cf = kelvin_color_format_map[resulting_format];
            glTexImage3D(gl_target, level, cf.gl_internal_format,
                         width, height, depth, 0,
                         cf.gl_format, cf.gl_type,
                         converted ? converted : unswizzled ? unswizzled : texture_data);

            if (converted) {
                g_free(converted);
            }
            if (unswizzled) {
                g_free(unswizzled);
            }

            texture_data += width * height * depth * f.bytes_per_pixel;

            width /= 2;
            height /= 2;
            depth /= 2;
        }
        break;
    }
    default:
        assert(false);
        break;
    }
    return resulting_format;
}

static TextureBinding* generate_texture(const TextureShape s,
                                        const uint8_t *texture_data,
                                        const uint8_t *palette_data)
{
    // assert(pg->opengl_enabled);

    ColorFormatInfo f = kelvin_color_format_map[s.color_format];

    /* Create a new opengl texture */
    GLuint gl_texture;
    glGenTextures(1, &gl_texture);

    GLenum gl_target;
    if (s.cubemap) {
        assert(f.encoding != linear);
        assert(s.dimensionality == 2);
        gl_target = GL_TEXTURE_CUBE_MAP;
    } else {
        if (f.encoding == linear) { /* FIXME : Include compressed too? (!= swizzled) */
            /* linear textures use unnormalised texcoords.
             * GL_TEXTURE_RECTANGLE_ARB conveniently also does, but
             * does not allow repeat and mirror wrap modes.
             *  (or mipmapping, but xbox d3d says 'Non swizzled and non
             *   compressed textures cannot be mip mapped.')
             * Not sure if that'll be an issue. */

            /* FIXME: GLSL 330 provides us with textureSize()! Use that? */
            gl_target = GL_TEXTURE_RECTANGLE;
            assert(s.dimensionality == 2);
        } else {
            switch(s.dimensionality) {
            case 1: gl_target = GL_TEXTURE_1D; break;
            case 2: gl_target = GL_TEXTURE_2D; break;
            case 3: gl_target = GL_TEXTURE_3D; break;
            default:
                assert(false);
                break;
            }
        }
    }

    glBindTexture(gl_target, gl_texture);

    NV2A_GL_DLABEL(GL_TEXTURE, gl_texture,
                   "format: 0x%02X%s, %d dimensions%s, width: %d, height: %d, depth: %d",
                   s.color_format, (f.encoding == linear) ? "" : (f.encoding == swizzled) ? " (SZ)" : " (DXT)", // compressed
                   s.dimensionality, s.cubemap ? " (Cubemap)" : "",
                   s.width, s.height, s.depth);

    /* Linear textures don't support mipmapping */
    if (f.encoding != linear) {
        glTexParameteri(gl_target, GL_TEXTURE_BASE_LEVEL,
            s.min_mipmap_level);
        glTexParameteri(gl_target, GL_TEXTURE_MAX_LEVEL,
            s.levels - 1);
    }

    /* Set this before calling upload_gl_texture() to prevent potential conversions */
    if (f.gl_swizzle_mask) {
        glTexParameteriv(gl_target, GL_TEXTURE_SWIZZLE_RGBA,
                         f.gl_swizzle_mask);
    }

    if (gl_target == GL_TEXTURE_CUBE_MAP) {

        size_t length = 0;
        unsigned int w = s.width, h = s.height;
        unsigned int level;
        for (level = 0; level < s.levels; level++) {
            /* FIXME: This is wrong for compressed textures and textures with 1x? non-square mipmaps */
            length += w * h * f.bytes_per_pixel;
            w /= 2;
            h /= 2;
        }

        upload_gl_texture(GL_TEXTURE_CUBE_MAP_POSITIVE_X,
                          s, texture_data + 0 * length, palette_data);
        upload_gl_texture(GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
                          s, texture_data + 1 * length, palette_data);
        upload_gl_texture(GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
                          s, texture_data + 2 * length, palette_data);
        upload_gl_texture(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
                          s, texture_data + 3 * length, palette_data);
        upload_gl_texture(GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
                          s, texture_data + 4 * length, palette_data);
        upload_gl_texture(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
                          s, texture_data + 5 * length, palette_data);
    } else {
        upload_gl_texture(gl_target, s, texture_data, palette_data);
    }

    TextureBinding* ret = (TextureBinding *)g_malloc(sizeof(TextureBinding));
    ret->gl_target = gl_target;
    ret->gl_texture = gl_texture;
    ret->refcnt = 1;
    return ret;
}

// NOTE: Might want to change guint to guint64 for return.
/* functions for texture LRU cache */
static guint texture_key_hash(gconstpointer key)
{
    const TextureKey *k = (const TextureKey *)key;
    uint64_t state_hash = fnv_hash(
        (const uint8_t*)&k->state, sizeof(TextureShape));
    return guint(state_hash ^ k->data_hash);
}
static gboolean texture_key_equal(gconstpointer a, gconstpointer b)
{
    const TextureKey *ak = (const TextureKey *)a, *bk = (const TextureKey *)b;
    return memcmp(&ak->state, &bk->state, sizeof(TextureShape)) == 0
            && ak->data_hash == bk->data_hash;
}
static gpointer texture_key_retrieve(gpointer key, gpointer user_data, GError **error)
{
    const TextureKey *k = (const TextureKey *)key;
    TextureBinding *v = generate_texture(k->state,
                                         k->texture_data,
                                         k->palette_data);
    if (error != NULL) {
        *error = NULL;
    }
    return v;
}
static void texture_key_destroy(gpointer data)
{
    g_free(data);
}
static void texture_binding_destroy(gpointer data)
{
    TextureBinding *binding = (TextureBinding *)data;

    // assert(pg->opengl_enabled);

    assert(binding->refcnt > 0);
    binding->refcnt--;
    if (binding->refcnt == 0) {
        glDeleteTextures(1, &binding->gl_texture);
        g_free(binding);
    }
}

// NOTE: Might want to change guint to guint64 for return.
/* hash and equality for shader cache hash table */
static guint shader_hash(gconstpointer key)
{
    return (guint)fnv_hash((const uint8_t *)key, sizeof(ShaderState));
}
static gboolean shader_equal(gconstpointer a, gconstpointer b)
{
    const ShaderState *as = (const ShaderState *)a, *bs = (const ShaderState *)b;
    return memcmp(as, bs, sizeof(ShaderState)) == 0;
}

static unsigned int kelvin_map_stencil_op(uint32_t parameter)
{
    unsigned int op;
    switch (parameter) {
    case NV097_SET_STENCIL_OP_V_KEEP:
        op = NV_PGRAPH_CONTROL_2_STENCIL_OP_V_KEEP; break;
    case NV097_SET_STENCIL_OP_V_ZERO:
        op = NV_PGRAPH_CONTROL_2_STENCIL_OP_V_ZERO; break;
    case NV097_SET_STENCIL_OP_V_REPLACE:
        op = NV_PGRAPH_CONTROL_2_STENCIL_OP_V_REPLACE; break;
    case NV097_SET_STENCIL_OP_V_INCRSAT:
        op = NV_PGRAPH_CONTROL_2_STENCIL_OP_V_INCRSAT; break;
    case NV097_SET_STENCIL_OP_V_DECRSAT:
        op = NV_PGRAPH_CONTROL_2_STENCIL_OP_V_DECRSAT; break;
    case NV097_SET_STENCIL_OP_V_INVERT:
        op = NV_PGRAPH_CONTROL_2_STENCIL_OP_V_INVERT; break;
    case NV097_SET_STENCIL_OP_V_INCR:
        op = NV_PGRAPH_CONTROL_2_STENCIL_OP_V_INCR; break;
    case NV097_SET_STENCIL_OP_V_DECR:
        op = NV_PGRAPH_CONTROL_2_STENCIL_OP_V_DECR; break;
    default:
        assert(false);
        break;
    }
    return op;
}

static unsigned int kelvin_map_polygon_mode(uint32_t parameter)
{
    unsigned int mode;
    switch (parameter) {
    case NV097_SET_FRONT_POLYGON_MODE_V_POINT:
        mode = NV_PGRAPH_SETUPRASTER_FRONTFACEMODE_POINT; break;
    case NV097_SET_FRONT_POLYGON_MODE_V_LINE:
        mode = NV_PGRAPH_SETUPRASTER_FRONTFACEMODE_LINE; break;
    case NV097_SET_FRONT_POLYGON_MODE_V_FILL:
        mode = NV_PGRAPH_SETUPRASTER_FRONTFACEMODE_FILL; break;
    default:
        assert(false);
        break;
    }
    return mode;
}

static unsigned int kelvin_map_texgen(uint32_t parameter, unsigned int channel)
{
    assert(channel < 4);
    unsigned int texgen;
    switch (parameter) {
    case NV097_SET_TEXGEN_S_DISABLE:
        texgen = NV_PGRAPH_CSV1_A_T0_S_DISABLE; break;
    case NV097_SET_TEXGEN_S_EYE_LINEAR:
        texgen = NV_PGRAPH_CSV1_A_T0_S_EYE_LINEAR; break;
    case NV097_SET_TEXGEN_S_OBJECT_LINEAR:
        texgen = NV_PGRAPH_CSV1_A_T0_S_OBJECT_LINEAR; break;
    case NV097_SET_TEXGEN_S_SPHERE_MAP:
        assert(channel < 2);
        texgen = NV_PGRAPH_CSV1_A_T0_S_SPHERE_MAP; break;
    case NV097_SET_TEXGEN_S_REFLECTION_MAP:
        assert(channel < 3);
        texgen = NV_PGRAPH_CSV1_A_T0_S_REFLECTION_MAP; break;
    case NV097_SET_TEXGEN_S_NORMAL_MAP:
        assert(channel < 3);
        texgen = NV_PGRAPH_CSV1_A_T0_S_NORMAL_MAP; break;
    default:
        assert(false);
        break;
    }
    return texgen;
}

static uint64_t fnv_hash(const uint8_t *data, size_t len)
{
    /* 64 bit Fowler/Noll/Vo FNV-1a hash code */
    uint64_t hval = 0xcbf29ce484222325ULL;
    const uint8_t *dp = data;
    const uint8_t *de = data + len;
    while (dp < de) {
        hval ^= (uint64_t) *dp++;
        hval += (hval << 1) + (hval << 4) + (hval << 5) +
            (hval << 7) + (hval << 8) + (hval << 40);
    }

    return hval;
}

static uint64_t fast_hash(const uint8_t *data, size_t len, unsigned int samples)
{
#ifdef __SSE4_2__
    uint64_t h[4] = {len, 0, 0, 0};
    assert(samples > 0);

    if (len < 8 || len % 8) {
        return fnv_hash(data, len);
    }

    assert(len >= 8 && len % 8 == 0);
    const uint64_t *dp = (const uint64_t*)data;
    const uint64_t *de = dp + (len / 8);
    size_t step = len / 8 / samples;
    if (step == 0) step = 1;

    while (dp < de - step * 3) {
        h[0] = __builtin_ia32_crc32di(h[0], dp[step * 0]);
        h[1] = __builtin_ia32_crc32di(h[1], dp[step * 1]);
        h[2] = __builtin_ia32_crc32di(h[2], dp[step * 2]);
        h[3] = __builtin_ia32_crc32di(h[3], dp[step * 3]);
        dp += step * 4;
    }
    if (dp < de - step * 0)
        h[0] = __builtin_ia32_crc32di(h[0], dp[step * 0]);
    if (dp < de - step * 1)
        h[1] = __builtin_ia32_crc32di(h[1], dp[step * 1]);
    if (dp < de - step * 2)
        h[2] = __builtin_ia32_crc32di(h[2], dp[step * 2]);

    return h[0] + (h[1] << 10) + (h[2] << 21) + (h[3] << 32);
#else
    return fnv_hash(data, len);
#endif
}
