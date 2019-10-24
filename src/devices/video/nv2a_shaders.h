/*
 * QEMU Geforce NV2A shader generator
 *
 * Copyright (c) 2015 espes
 * Copyright (c) 2015 Jannik Vogel
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 or
 * (at your option) version 3 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <https://www.gnu.org/licenses/>.
 */

#ifndef HW_NV2A_SHADERS_H
#define HW_NV2A_SHADERS_H

#include "qstring.h"
#include "common\util\gloffscreen\gloffscreen.h" // For GLenum, etc

#include "nv2a_vsh.h"
#include "nv2a_psh.h"
#include "nv2a_regs.h"

enum ShaderPrimitiveMode {
    PRIM_TYPE_NONE,
    PRIM_TYPE_POINTS,
    PRIM_TYPE_LINES,
    PRIM_TYPE_LINE_LOOP,
    PRIM_TYPE_LINE_STRIP,
    PRIM_TYPE_TRIANGLES,
    PRIM_TYPE_TRIANGLE_STRIP,
    PRIM_TYPE_TRIANGLE_FAN,
    PRIM_TYPE_QUADS,
    PRIM_TYPE_QUAD_STRIP,
    PRIM_TYPE_POLYGON,
};

enum ShaderPolygonMode {
    POLY_MODE_FILL,
    POLY_MODE_POINT,
    POLY_MODE_LINE,
};

typedef struct ShaderState {
    PshState psh;

    bool texture_matrix_enable[4];
    enum VshTexgen texgen[4][4];

    bool fog_enable;
    enum VshFoggen foggen;
    enum VshFogMode fog_mode;

    enum VshSkinning skinning;

    bool normalization;

    bool lighting;
    enum VshLight light[NV2A_MAX_LIGHTS];

    bool fixed_function;

    /* vertex program */
    bool vertex_program;
    uint32_t program_data[NV2A_MAX_TRANSFORM_PROGRAM_LENGTH][VSH_TOKEN_SIZE];
    int program_length;
    bool z_perspective;

    /* primitive format for geometry shader */
    enum ShaderPolygonMode polygon_front_mode;
    enum ShaderPolygonMode polygon_back_mode;
    enum ShaderPrimitiveMode primitive_mode;
} ShaderState;

typedef struct ShaderBinding {
    GLuint gl_program;
    GLenum gl_primitive_mode;

    GLint psh_constant_loc[9][2];
    GLint alpha_ref_loc;

    GLint bump_mat_loc[NV2A_MAX_TEXTURES];
    GLint bump_scale_loc[NV2A_MAX_TEXTURES];
    GLint bump_offset_loc[NV2A_MAX_TEXTURES];

    GLint surface_size_loc;
    GLint clip_range_loc;

    GLint vsh_constant_loc[NV2A_VERTEXSHADER_CONSTANTS];

    GLint inv_viewport_loc;
    GLint ltctxa_loc[NV2A_LTCTXA_COUNT];
    GLint ltctxb_loc[NV2A_LTCTXB_COUNT];
    GLint ltc1_loc[NV2A_LTC1_COUNT];

    GLint fog_color_loc;
    GLint fog_param_loc[2];
    GLint light_infinite_half_vector_loc[NV2A_MAX_LIGHTS];
    GLint light_infinite_direction_loc[NV2A_MAX_LIGHTS];
    GLint light_local_position_loc[NV2A_MAX_LIGHTS];
    GLint light_local_attenuation_loc[NV2A_MAX_LIGHTS];

    GLint clip_region_loc[8];
} ShaderBinding;

ShaderBinding* generate_shaders(const ShaderState state);

#endif
