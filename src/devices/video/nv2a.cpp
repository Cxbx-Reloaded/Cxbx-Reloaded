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
// *  (c) 2002-2003 Aaron Robinson <caustik@caustik.com>
// * 
// *  This file is heavily based on code from XQEMU
// *  https://github.com/xqemu/xqemu/blob/master/hw/xbox/nv2a/nv2a.c
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

#define LOG_PREFIX CXBXR_MODULE::NV2A

#include <core/kernel/exports/xboxkrnl.h> // For PKINTERRUPT, etc.

#ifdef _MSC_VER                         // Check if MS Visual C compiler
#  pragma comment(lib, "opengl32.lib")  // Compiler-specific directive to avoid manually configuration
//#  pragma comment(lib, "glu32.lib")     // Link libraries
#  pragma comment(lib, "glew32.lib")
#endif

#include <string> // For std::string
#include <distorm.h> // For uint32_t
#include <process.h> // For __beginthreadex(), etc.

#include "core\kernel\init\CxbxKrnl.h" // For XBOX_MEMORY_SIZE, DWORD, etc
#include "core\kernel\support\Emu.h"
#include "core\kernel\exports\EmuKrnl.h"
#include <backends/imgui_impl_win32.h>
#include <backends/imgui_impl_opengl3.h>
#include "core/common/video/RenderBase.hpp"
#include "core\hle\Intercept.hpp"
#include "common/win32/Threads.h"
#include "Logging.h"


#include "vga.h"
#include "nv2a.h" // For NV2AState
#include "nv2a_int.h" // from https://github.com/espes/xqemu/tree/xbox/hw/xbox
//#include <gl\glew.h>
#include <gl\GL.h>
#include <gl\GLU.h>
#include <cassert>
//#include <gl\glut.h>

// glib types
typedef char gchar;
typedef int gint;
typedef unsigned int guint;
typedef unsigned int guint32;
typedef const void *gconstpointer;
typedef gint   gboolean;
typedef void* gpointer;

typedef guint32 GQuark;

typedef struct _GError GError;

struct _GError
{
	GQuark       domain;
	gint         code;
	gchar       *message;
};

#include "common\util\gloffscreen\glextensions.h" // for glextensions_init

GLuint create_gl_shader(GLenum gl_shader_type,
	const char *code,
	const char *name); // forward to nv2a_shaders.cpp

static void update_irq(NV2AState *d)
{
	/* PFIFO */
	if (d->pfifo.pending_interrupts & d->pfifo.enabled_interrupts) {
		d->pmc.pending_interrupts |= NV_PMC_INTR_0_PFIFO;
	}
	else {
		d->pmc.pending_interrupts &= ~NV_PMC_INTR_0_PFIFO;
	}

	/* PCRTC */
	if (d->pcrtc.pending_interrupts & d->pcrtc.enabled_interrupts) {
		d->pmc.pending_interrupts |= NV_PMC_INTR_0_PCRTC;
	}
	else {
		d->pmc.pending_interrupts &= ~NV_PMC_INTR_0_PCRTC;
	}

	/* PGRAPH */
	if (d->pgraph.pending_interrupts & d->pgraph.enabled_interrupts) {
		d->pmc.pending_interrupts |= NV_PMC_INTR_0_PGRAPH;
	}
	else {
		d->pmc.pending_interrupts &= ~NV_PMC_INTR_0_PGRAPH;
	}

	/* PVIDEO */
	if (d->pvideo.pending_interrupts & d->pvideo.enabled_interrupts) {
		d->pmc.pending_interrupts |= NV_PMC_INTR_0_PVIDEO;
	}
	else {
		d->pmc.pending_interrupts &= ~NV_PMC_INTR_0_PVIDEO;
	}

	/* TODO : PBUS * /
	if (d->pbus.pending_interrupts & d->pbus.enabled_interrupts) {
		d->pmc.pending_interrupts |= NV_PMC_INTR_0_PBUS;
	}
	else {
		d->pmc.pending_interrupts &= ~NV_PMC_INTR_0_PBUS;
	} */

	/* TODO : SOFTWARE * /
	if (d->user.pending_interrupts & d->.enabled_interrupts) {
		d->pmc.pending_interrupts |= NV_PMC_INTR_0_SOFTWARE;
	}
	else {
		d->pmc.pending_interrupts &= ~NV_PMC_INTR_0_SOFTWARE;
	} */

	if (d->pmc.pending_interrupts && d->pmc.enabled_interrupts) {
		HalSystemInterrupts[3].Assert(true);
	}
	else {
		HalSystemInterrupts[3].Assert(false);
	}
}


#include "EmuNV2A_DEBUG.cpp"


#define DEBUG_READ32(DEV)              EmuLog(LOG_LEVEL::DEBUG, "Rd32 NV2A " #DEV "(0x%08X) = 0x%08X [Handled %s]", addr, result, DebugNV_##DEV(addr))
#define DEBUG_READ32_UNHANDLED(DEV)  { EmuLog(LOG_LEVEL::DEBUG, "Rd32 NV2A " #DEV "(0x%08X) = 0x%08X [Unhandled %s]", addr, result, DebugNV_##DEV(addr)); return result; }

#define DEBUG_WRITE32(DEV)             EmuLog(LOG_LEVEL::DEBUG, "Wr32 NV2A " #DEV "(0x%08X, 0x%08X) [Handled %s]", addr, value, DebugNV_##DEV(addr))
#define DEBUG_WRITE32_UNHANDLED(DEV) { EmuLog(LOG_LEVEL::DEBUG, "Wr32 NV2A " #DEV "(0x%08X, 0x%08X) [Unhandled %s]", addr, value, DebugNV_##DEV(addr)); return; }

#define DEVICE_READ32(DEV) uint32_t EmuNV2A_##DEV##_Read32(NV2AState *d, xbox::addr_xt addr)
#define DEVICE_READ32_SWITCH() uint32_t result = 0; switch (addr) 
#define DEVICE_READ32_REG(dev) result = d->dev.regs[addr/4]
#define DEVICE_READ32_END(DEV) DEBUG_READ32(DEV); return result

#define DEVICE_WRITE32(DEV) void EmuNV2A_##DEV##_Write32(NV2AState *d, xbox::addr_xt addr, uint32_t value)
#define DEVICE_WRITE32_REG(dev) d->dev.regs[addr/4] = value
#define DEVICE_WRITE32_END(DEV) DEBUG_WRITE32(DEV)

static inline uint32_t ldl_le_p(const void *p)
{
	return *(uint32_t*)p;
}

static inline void stq_le_p(uint64_t *p, uint64_t v)
{
	*p = v;
}

static inline void stl_le_p(uint32_t *p, uint32_t v)
{
	*p = v;
}

static DMAObject nv_dma_load(NV2AState *d, xbox::addr_xt dma_obj_address)
{
	assert(dma_obj_address < d->pramin.ramin_size);

	uint32_t *dma_obj = (uint32_t*)(d->pramin.ramin_ptr + dma_obj_address);
	uint32_t flags = ldl_le_p(dma_obj);
	uint32_t limit = ldl_le_p(dma_obj + 1);
	uint32_t frame = ldl_le_p(dma_obj + 2);

	DMAObject object;
	object.dma_class = GET_MASK(flags, NV_DMA_CLASS);
	object.dma_target = GET_MASK(flags, NV_DMA_TARGET);
	object.address = (frame & NV_DMA_ADDRESS) | GET_MASK(flags, NV_DMA_ADJUST);
	object.limit = limit;

	return object;
}

static void *nv_dma_map(NV2AState *d, xbox::addr_xt dma_obj_address, xbox::addr_xt *len)
{
//	assert(dma_obj_address < d->pramin.ramin_size);

	DMAObject dma = nv_dma_load(d, dma_obj_address);

	/* TODO: Handle targets and classes properly */
	NV2A_DPRINTF("dma_map %x, %x, %x %x\n",
		dma.dma_class, dma.dma_target, dma.address, dma.limit);

	dma.address &= 0x07FFFFFF;

	// assert(dma.address + dma.limit < memory_region_size(d->vram));
	*len = dma.limit;
	return d->vram_ptr + dma.address;
//	return (void*)(PHYSICAL_MAP_BASE  + dma.address);
}
void *ext_nv_dma_map(NV2AState *d, xbox::addr_xt dma_obj_address, xbox::addr_xt *len)
{
	return nv_dma_map(d, dma_obj_address, len);
}
#include "EmuNV2A_PBUS.cpp"
#include "EmuNV2A_PCRTC.cpp"
#include "EmuNV2A_PFB.cpp"
#include "EmuNV2A_PGRAPH.cpp"
#include "EmuNV2A_PFIFO.cpp"
#include "EmuNV2A_PMC.cpp"
#include "EmuNV2A_PRAMDAC.cpp"
#include "EmuNV2A_PRMCIO.cpp"
#include "EmuNV2A_PRMVIO.cpp"
#include "EmuNV2A_PTIMER.cpp"
#include "EmuNV2A_PVIDEO.cpp"
#include "EmuNV2A_USER.cpp"

#include "EmuNV2A_PRMA.cpp"
#include "EmuNV2A_PCOUNTER.cpp"
#include "EmuNV2A_PVPE.cpp"
#include "EmuNV2A_PTV.cpp"
#include "EmuNV2A_PRMFB.cpp"
#include "EmuNV2A_PSTRAPS.cpp"
#include "EmuNV2A_PRMDIO.cpp"
#include "EmuNV2A_PRAMIN.cpp"

const NV2ABlockInfo regions[] = { // blocktable

// Note : Avoid designated initializers to facilitate C++ builds
#define ENTRY(OFFSET, SIZE, NAME, RDFUNC, WRFUNC) \
	{ \
        #NAME, OFFSET, SIZE, \
        { RDFUNC, WRFUNC }, \
    }, \

	/* card master control */
	ENTRY(0x000000, 0x001000, PMC, EmuNV2A_PMC_Read32, EmuNV2A_PMC_Write32)
	/* bus control */
	ENTRY(0x001000, 0x001000, PBUS, EmuNV2A_PBUS_Read32, EmuNV2A_PBUS_Write32)
	/* MMIO and DMA FIFO submission to PGRAPH and VPE */
	ENTRY(0x002000, 0x002000, PFIFO, EmuNV2A_PFIFO_Read32, EmuNV2A_PFIFO_Write32)
	/* access to BAR0/BAR1 from real mode */
	ENTRY(0x007000, 0x001000, PRMA, EmuNV2A_PRMA_Read32, EmuNV2A_PRMA_Write32)
	/* video overlay */
	ENTRY(0x008000, 0x001000, PVIDEO, EmuNV2A_PVIDEO_Read32, EmuNV2A_PVIDEO_Write32)
	/* time measurement and time-based alarms */
	ENTRY(0x009000, 0x001000, PTIMER, EmuNV2A_PTIMER_Read32, EmuNV2A_PTIMER_Write32)
	/* performance monitoring counters */
	ENTRY(0x00a000, 0x001000, PCOUNTER, EmuNV2A_PCOUNTER_Read32, EmuNV2A_PCOUNTER_Write32)
	/* MPEG2 decoding engine */
	ENTRY(0x00b000, 0x001000, PVPE, EmuNV2A_PVPE_Read32, EmuNV2A_PVPE_Write32)
	/* TV encoder */
	ENTRY(0x00d000, 0x001000, PTV, EmuNV2A_PTV_Read32, EmuNV2A_PTV_Write32)
	/* aliases VGA memory window */
	ENTRY(0x0a0000, 0x020000, PRMFB, EmuNV2A_PRMFB_Read32, EmuNV2A_PRMFB_Write32)
	/* aliases VGA sequencer and graphics controller registers */
	ENTRY(0x0c0000, 0x008000, PRMVIO, EmuNV2A_PRMVIO_Read32, EmuNV2A_PRMVIO_Write32) // Size was 0x001000
	/* memory interface */
	ENTRY(0x100000, 0x001000, PFB, EmuNV2A_PFB_Read32, EmuNV2A_PFB_Write32)
	/* straps readout / override */
	ENTRY(0x101000, 0x001000, PSTRAPS, EmuNV2A_PSTRAPS_Read32, EmuNV2A_PSTRAPS_Write32)
	/* accelerated 2d/3d drawing engine */
	ENTRY(0x400000, 0x002000, PGRAPH, EmuNV2A_PGRAPH_Read32, EmuNV2A_PGRAPH_Write32)
	/* more CRTC controls */
	ENTRY(0x600000, 0x001000, PCRTC, EmuNV2A_PCRTC_Read32, EmuNV2A_PCRTC_Write32)
	/* aliases VGA CRTC and attribute controller registers */
	ENTRY(0x601000, 0x001000, PRMCIO, EmuNV2A_PRMCIO_Read32, EmuNV2A_PRMCIO_Write32)
	/* RAMDAC, cursor, and PLL control */
	ENTRY(0x680000, 0x001000, PRAMDAC, EmuNV2A_PRAMDAC_Read32, EmuNV2A_PRAMDAC_Write32)
	/* aliases VGA palette registers */
	ENTRY(0x681000, 0x001000, PRMDIO, EmuNV2A_PRMDIO_Read32, EmuNV2A_PRMDIO_Write32)
	/* RAMIN access */
	ENTRY(0x700000, 0x100000, PRAMIN, EmuNV2A_PRAMIN_Read32, EmuNV2A_PRAMIN_Write32)
	/* PFIFO MMIO and DMA submission area */
	ENTRY(0x800000, 0x400000, USER, EmuNV2A_USER_Read32, EmuNV2A_USER_Write32) // Size was 0x800000
	/* UREMAP User area mirror - TODO : Confirm */
	ENTRY(0xC00000, 0x400000, UREMAP, EmuNV2A_USER_Read32, EmuNV2A_USER_Write32) // NOTE : Mirror of USER
	/* Terminating entry */
	ENTRY(0xFFFFFF, 0x000000, END, nullptr, nullptr)
#undef ENTRY
};

const NV2ABlockInfo* EmuNV2A_Block(xbox::addr_xt addr)
{
	// Find the block in the block table
	const NV2ABlockInfo* block = &regions[0];
	int i = 0;

	while (block->size > 0) {
		if (addr >= block->offset && addr < block->offset + block->size) {
			return block;
		}

		block = &regions[++i];
	}

	return nullptr;
}

// HACK: Until we implement VGA/proper interrupt generation
// we simulate VBLANK by calling the interrupt at 60Hz
std::thread vblank_thread;
extern std::chrono::steady_clock::time_point GetNextVBlankTime();

void _check_gl_reset()
{
	while (true) {
		GLenum err = glGetError();
		if (err == GL_NO_ERROR) 
			return;
		if (err == 0)
			return;
	}
}

void _check_gl_error(const char *file, int line)
{
	while (true) {
		GLenum err = glGetError();
		const char *error;
		switch (err) {
		case GL_NO_ERROR: return;
		case GL_INVALID_ENUM: error = "GL_INVALID_ENUM"; break;
		case GL_INVALID_VALUE: error = "GL_INVALID_VALUE"; break;
		case GL_INVALID_OPERATION: error = "GL_INVALID_OPERATION"; break;
		case GL_STACK_OVERFLOW: error = "GL_STACK_OVERFLOW"; break;
		case GL_STACK_UNDERFLOW: error = "GL_STACK_UNDERFLOW"; break;
		case GL_OUT_OF_MEMORY: error = "GL_OUT_OF_MEMORY"; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: error = "GL_INVALID_FRAMEBUFFER_OPERATION"; break;
		//case GL_INVALID_FRAMEBUFFER_OPERATION_EXT: error = "GL_INVALID_FRAMEBUFFER_OPERATION_EXT"; break;
		case GL_CONTEXT_LOST: error = "GL_CONTEXT_LOST"; break;
		default: error = "(unknown)"; break;
		}

		printf("OpenGL error 0x%.8X %s\n", err, error);
		assert(false);
	}
}

#ifdef DEBUG_NV2A_GL
#define GL_RESET() _check_gl_reset()
#define GL_CHECK() _check_gl_error(__FILE__,__LINE__)
#else
#define GL_RESET()
#define GL_CHECK()
#endif

enum {
	SAMP_TEXCOORD = 0,
};

enum {
	FRAG_COLOR = 0,
};

enum {
	ATTR_POSITION = 0,
	ATTR_TEXCOORD = 4,
};

static GLint m_overlay_gl_uniform_location_texture = -1;

GLuint Get_YUV_to_RGB_shader_program()
{
	// Use a shader to convert YUV to RGB using information from :
	// From https://stackoverflow.com/questions/44291939/portable-yuv-drawing-context
	// to https://hg.libsdl.org/SDL/file/1f2cb42aa5d3/src/render/opengl/SDL_shaders_gl.c#l128
	// From https://stackoverflow.com/questions/12428108/ios-how-to-draw-a-yuv-image-using-opengl
	// to https://github.com/kolyvan/kxmovie/blob/master/kxmovie/KxMovieGLView.m
	// and https://www.opengl.org/discussion_boards/archive/index.php/t-169186.html
	// and https://gist.github.com/roxlu/9329339
	// https://github.com/g-truc/ogl-samples/blob/master/data/gl-330/texture-2d.vert
	static const char *OPENGL_SHADER_YUV[2] = {
		/* vertex shader */
		"#version 330 core                                                       \n"
		"#define ATTR_POSITION 0                                                 \n"
		"#define ATTR_TEXCOORD 4                                                 \n"
		"                                                                        \n"
		"precision highp float;                                                  \n"
		"precision highp int;                                                    \n"
		"layout(std140, column_major) uniform;                                   \n"
		"                                                                        \n"
		"layout(location = ATTR_POSITION) in vec2 Position;                      \n"
		"layout(location = ATTR_TEXCOORD) in vec2 Texcoord;                      \n"
		"                                                                        \n"
		"out block                                                               \n"
		"{                                                                       \n"
		"  vec2 Texcoord;                                                        \n"
		"} Out;                                                                  \n"
		"                                                                        \n"
		"void main()                                                             \n"
		"{                                                                       \n"
		"  Out.Texcoord = Texcoord;                                              \n"
		"  gl_Position = vec4(Position, 0.0, 1.0);                               \n"
		"}                                                                       \n"
		, /* fragment shader */
		// https://github.com/g-truc/ogl-samples/blob/master/data/gl-330/texture-2d.frag
		"#version 330 core                                                       \n"
		"#define FRAG_COLOR 0                                                    \n"
		"                                                                        \n"
		"precision highp float;                                                  \n"
		"precision highp int;                                                    \n"
		"layout(std140, column_major) uniform;                                   \n"
		"                                                                        \n"
		"uniform sampler2D tex_yuyv;                                             \n"
		"                                                                        \n"
		"in block                                                                \n"
		"{                                                                       \n"
		"  vec2 Texcoord;                                                        \n"
		"} In;                                                                   \n"
		"                                                                        \n"
		"layout(location = FRAG_COLOR, index = 0) out vec4 Color;                \n"
		"                                                                        \n"
		"// YUV offset                                                           \n"
		"const vec3 offset = vec3(-0.0627451017, -0.501960814, -0.501960814);    \n"
		"// RGB coefficients                                                     \n"
		"const vec3 Rcoeff = vec3(1.164,  0.000,  1.596);                        \n"
		"const vec3 Gcoeff = vec3(1.164, -0.391, -0.813);                        \n"
		"const vec3 Bcoeff = vec3(1.164,  2.018,  0.000);                        \n"
		"void main(void)                                                         \n"
		"{                                                                       \n"
		"  // Fetch 4:2:2 YUYV macropixel                                        \n"
		"  vec4 yuyv = texture2D(tex_yuyv, In.Texcoord);                         \n"
		"  // Now r-g-b-a is actually y1-u-y2-v                                  \n"
		"  float u = yuyv.g;                                                     \n"
		"  float v = yuyv.a;                                                     \n"
		"  vec3 yuv;                                                             \n"
		"  // Convert texture coordinate into texture x position                 \n"
		"  ivec2 texture_size = textureSize(tex_yuyv, 0);                        \n"
		"  float texture_x = In.Texcoord.x * texture_size.x;                     \n"
		"  // Depending on fragment x position choose y1-u-v or y2-u-v           \n"
		"  if (mod(texture_x, 1.0) < 0.5) { // left half                         \n"
		"    float y1 = yuyv.r;                                                  \n"
		"    yuv = vec3(y1, u, v);                                               \n"
		"  } else { // right half                                                \n"
		"    float y2 = yuyv.b;                                                  \n"
		"    yuv = vec3(y2, u, v);                                               \n"
		"  }                                                                     \n"
		"  // Do the color transform                                             \n"
		"  yuv += offset;                                                        \n"
		"  Color.r = dot(yuv, Rcoeff);                                           \n"
		"  Color.g = dot(yuv, Gcoeff);                                           \n"
		"  Color.b = dot(yuv, Bcoeff);                                           \n"
		"  Color.a = 1.0;                                                        \n"
		"}                                                                       \n"
	};

	// Bind shader
	static GLuint shader_program_name_yuv_to_rgb = -1;
	if (shader_program_name_yuv_to_rgb == -1) {
		// Compile vertex shader
		GLuint vertex_shader_name = create_gl_shader(GL_VERTEX_SHADER, OPENGL_SHADER_YUV[0], "YUV>RGB Vertex shader");
		GL_CHECK();
		// Compile fragment shader
		GLuint fragment_shader_name = create_gl_shader(GL_FRAGMENT_SHADER, OPENGL_SHADER_YUV[1], "YUV>RGB Fragment shader");
		GL_CHECK();

		shader_program_name_yuv_to_rgb = glCreateProgram();
		GL_CHECK();
		// Link vertex and fragment shaders
		glAttachShader(shader_program_name_yuv_to_rgb, vertex_shader_name);
		GL_CHECK();
		glAttachShader(shader_program_name_yuv_to_rgb, fragment_shader_name);
		GL_CHECK();
		glBindAttribLocation(shader_program_name_yuv_to_rgb, ATTR_POSITION, "Position");
		GL_CHECK();
		glBindAttribLocation(shader_program_name_yuv_to_rgb, ATTR_TEXCOORD, "Texcoord");
		GL_CHECK();
		glBindFragDataLocation(shader_program_name_yuv_to_rgb, FRAG_COLOR, "Color");
		GL_CHECK();
		glLinkProgram(shader_program_name_yuv_to_rgb);
		GL_CHECK();

		/* Check it linked */
		GLint linked = 0;
		glGetProgramiv(shader_program_name_yuv_to_rgb, GL_LINK_STATUS, &linked);
		GL_CHECK();
		if (!linked) {
			GLchar log[2048];
			glGetProgramInfoLog(shader_program_name_yuv_to_rgb, 2048, NULL, log);
			fprintf(stderr, "nv2a: shader linking failed: %s\n", log);
			abort();
		}

		m_overlay_gl_uniform_location_texture = glGetUniformLocation(shader_program_name_yuv_to_rgb, "tex_yuyv");
		GL_CHECK();
		assert(m_overlay_gl_uniform_location_texture >= 0);
	}

	return shader_program_name_yuv_to_rgb;
}

GLuint m_framebuffer_gl_uniform_location_texture = -1;

GLuint GetFramebufferShaderProgram()
{
	static const char *gl_framebuffer_shader_src[2] = {
		/* vertex shader */
		"#version 330 core                                                       \n"
		"#define ATTR_POSITION 0                                                 \n"
		"#define ATTR_TEXCOORD 4                                                 \n"
		"                                                                        \n"
		"precision highp float;                                                  \n"
		"precision highp int;                                                    \n"
		"layout(std140, column_major) uniform;                                   \n"
		"                                                                        \n"
		"layout(location = ATTR_POSITION) in vec2 Position;                      \n"
		"layout(location = ATTR_TEXCOORD) in vec2 Texcoord;                      \n"
		"                                                                        \n"
		"out block                                                               \n"
		"{                                                                       \n"
		"  vec2 Texcoord;                                                        \n"
		"} Out;                                                                  \n"
		"                                                                        \n"
		"void main(void)                                                         \n"
		"{                                                                       \n"
		"	Out.Texcoord = Texcoord;                                             \n"
		"	gl_Position = vec4(Position, 0.0, 1.0);                              \n"
		"}                                                                       \n"
		, /* fragment shader */
		"#version 330 core                                                       \n"
		"#define FRAG_COLOR 0                                                    \n"
		"                                                                        \n"
		"precision highp float;                                                  \n"
		"precision highp int;                                                    \n"
		"layout(std140, column_major) uniform;                                   \n"
		"                                                                        \n"
		"uniform sampler2D tex;                                                  \n"
		"                                                                        \n"
		"in block                                                                \n"
		"{                                                                       \n"
		"  vec2 Texcoord;                                                        \n"
		"} In;                                                                   \n"
		"                                                                        \n"
		"layout(location = FRAG_COLOR, index = 0) out vec4 Color;                \n"
		"                                                                        \n"
		"void main()                                                             \n"
		"{                                                                       \n"
		"  Color = texture2D(tex, In.Texcoord);                                  \n"
		"}                                                                       \n"
	};

	// Bind shader
	static GLuint m_framebuffer_gl_shader_program = -1;
	if (m_framebuffer_gl_shader_program == -1) {
		// Compile vertex shader
		GLuint vertex_shader = create_gl_shader(GL_VERTEX_SHADER, gl_framebuffer_shader_src[0], "Framebuffer vertex shader");
		GL_CHECK();
		// Compile fragment shader
		GLuint fragment_shader = create_gl_shader(GL_FRAGMENT_SHADER, gl_framebuffer_shader_src[1], "Framebuffer fragment shader");
		GL_CHECK();

		m_framebuffer_gl_shader_program = glCreateProgram();
		GL_CHECK();
		// Link vertex and fragment shaders
		glAttachShader(m_framebuffer_gl_shader_program, vertex_shader);
		GL_CHECK();
		glAttachShader(m_framebuffer_gl_shader_program, fragment_shader);
		GL_CHECK();
		glBindAttribLocation(m_framebuffer_gl_shader_program, ATTR_POSITION, "Position");
		GL_CHECK();
		glBindAttribLocation(m_framebuffer_gl_shader_program, ATTR_TEXCOORD, "Texcoord");
		GL_CHECK();
		glBindFragDataLocation(m_framebuffer_gl_shader_program, FRAG_COLOR, "Color");
		GL_CHECK();
		glLinkProgram(m_framebuffer_gl_shader_program);
		GL_CHECK();

		/* Check it linked */
		GLint linked = 0;
		glGetProgramiv(m_framebuffer_gl_shader_program, GL_LINK_STATUS, &linked);
		GL_CHECK();
		if (!linked) {
			GLchar log[2048];
			glGetProgramInfoLog(m_framebuffer_gl_shader_program, 2048, NULL, log);
			fprintf(stderr, "nv2a: shader linking failed: %s\n", log);
			abort();
		}

		m_framebuffer_gl_uniform_location_texture = glGetUniformLocation(m_framebuffer_gl_shader_program, "tex");
		GL_CHECK();
		assert(m_framebuffer_gl_uniform_location_texture >= 0);
	}

	return m_framebuffer_gl_shader_program;
}

static int display_mode_format = 0;
static int frame_pixel_bytes = 1;

static GLenum frame_gl_internal_format = GL_RGBA8;
static GLenum frame_gl_format = GL_BGRA;
static GLenum frame_gl_type = GL_UNSIGNED_INT_8_8_8_8_REV;
static GLuint frame_gl_texture = 0;

static GLsizei frame_width = 640;
static GLsizei frame_height = 480;

void cxbx_gl_update_displaymode(NV2AState *d) {
	static GLenum old_frame_gl_internal_format = GL_RGBA8;
	static GLenum old_frame_gl_format = GL_BGRA;
	static GLenum old_frame_gl_type = GL_UNSIGNED_INT_8_8_8_8_REV;
	static GLsizei old_frame_width = 640;
	static GLsizei old_frame_height = 480;

	// Derive display mode and bytes per pixel from actual hardware register contents:
	// This is required for titles that use a non ARGB framebuffer, such as Beats of Rage
	bool alt_mode = d->pramdac.regs[(NV_PRAMDAC_GENERAL_CONTROL/4 & (NV_PRAMDAC_SIZE - 1))]
		& NV_PRAMDAC_GENERAL_CONTROL_ALT_MODE_SEL;
	switch (d->prmcio.cr[NV_CIO_CRE_PIXEL_INDEX] & 0x03) {
	case 1: // 8bpp
		assert(false); // TODO : Verify this
		frame_pixel_bytes = 1;
		break;
	case 2: // 15 or 16 bpp
		if (alt_mode) {
			// Test case : Arctic Thunder
			display_mode_format = NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_R5G6B5;
		}
		else {
			display_mode_format = NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_X1R5G5B5;
		}

		frame_pixel_bytes = 2;
		break;
	case 0: // VGA; Fall through
	case 3: // 24 or 32 bpp
		if (alt_mode) {
			// Test-case : WWE RAW2
			display_mode_format = NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_X8R8G8B8;
		}
		else {
			// Test-case : XDK sample DolphinClassic (after VGA fall-through)
			display_mode_format = NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A8R8G8B8;
		}

		frame_pixel_bytes = 4;
		break;
	}

	// Convert displau format to OpenGl format details
	frame_gl_internal_format = kelvin_color_format_map[display_mode_format].gl_internal_format;
	frame_gl_format = kelvin_color_format_map[display_mode_format].gl_format;
	frame_gl_type = kelvin_color_format_map[display_mode_format].gl_type;

	frame_width = NV2ADevice::GetFrameWidth(d);
	frame_height = NV2ADevice::GetFrameHeight(d);

	// Detect changes in framebuffer dimensions
	if (old_frame_gl_internal_format != frame_gl_internal_format
		|| old_frame_gl_format != frame_gl_format
		|| old_frame_gl_type != frame_gl_type
		|| old_frame_width != frame_width
		|| old_frame_height != frame_height) {
		old_frame_gl_internal_format = frame_gl_internal_format;
		old_frame_gl_format = frame_gl_format;
		old_frame_gl_type = frame_gl_type;
		old_frame_width = frame_width;
		old_frame_height = frame_height;
		if (frame_gl_texture) {
			// Destroy the texture if format changed
			// Test case : XDK sample DolphinClassic
			glDeleteTextures(1, &frame_gl_texture);
			frame_gl_texture = 0;
		}
	}
}

void cxbx_gl_render_framebuffer(NV2AState *d)
{
	// Update the frame texture
	uint8_t* frame_pixels = (uint8_t*)(/*CONTIGUOUS_MEMORY_BASE=*/0x80000000 | d->pcrtc.start); // NV_PCRTC_START
	uint8_t* palette_data = xbox::zeroptr; // Note : Framebuffer formats aren't paletized

	TextureShape s;
	s.cubemap = false; // Note : Unused in upload_gl_texture GL_TEXTURE_2D path
	s.dimensionality = 2; // Note : Unused in upload_gl_texture GL_TEXTURE_2D path
	s.color_format = display_mode_format;
	s.levels = 1;
	s.width = frame_width;
	s.height = frame_height;
	s.depth = 0; // Note : Unused in upload_gl_texture GL_TEXTURE_2D path
	s.min_mipmap_level = 0;
	s.max_mipmap_level = 0;
	s.pitch = 0; // Note : Unused in upload_gl_texture GL_TEXTURE_2D path

	// If we need to create a (new) texture, do so
	if (!frame_gl_texture) {
		glGenTextures(1, &frame_gl_texture);
		GL_CHECK();
		glBindTexture(GL_TEXTURE_2D, frame_gl_texture);
		GL_CHECK();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
		GL_CHECK();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
		GL_CHECK();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		GL_CHECK();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		GL_CHECK();
		glTexImage2D(GL_TEXTURE_2D, /*level=*/0, frame_gl_internal_format, frame_width, frame_height, /*border=*/0, frame_gl_format, frame_gl_type, NULL);
		GL_CHECK();
	}
	else {
		glBindTexture(GL_TEXTURE_2D, frame_gl_texture);
		GL_CHECK();
	}

	int rf = upload_gl_texture(GL_TEXTURE_2D,
		s,
		frame_pixels,
		palette_data);
	GL_CHECK();

	// Note : It'd be less code to use generate_texture(), except that puts linear formats
	// into GL_TEXTURE_RECTANGLE, while we need GL_TEXTURE_2D here. So instead, handle the
	// difference here by separately setting the resulting format's RGBA swizzle:
	ColorFormatInfo cfi = kelvin_color_format_map[rf];
	if (cfi.gl_swizzle_mask) {
		glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, cfi.gl_swizzle_mask);
	}

#define BLIT_FRAMEBUFFER
#ifdef BLIT_FRAMEBUFFER
	// If we need to create an OpenGL framebuffer, do so
	static GLuint framebuffer = -1;
	if (framebuffer == -1) {
		glGenFramebuffers(1, &framebuffer);
		GL_CHECK();
	}

	// Target the actual framebuffer
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
#endif

#ifdef DEBUG_NV2A_GL
	// If the screen turns purple, glDrawArrays/glBlitFramebuffer below failed
	glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
	GL_CHECK();
	glClear(GL_COLOR_BUFFER_BIT);
	GL_CHECK();
#endif

#ifdef BLIT_FRAMEBUFFER
	// Copy frame texture to an internal frame buffer
	glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer);
	glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, frame_gl_texture, /*level=*/0);
	// Blit the active internal 'read' frame buffer to the actual 'draw' framebuffer
	static const GLenum filter = GL_NEAREST;
	// Note : dstY0 and dstY1 are swapped so the screen doesn't appear upside down
	glBlitFramebuffer(0, 0, frame_width, frame_height, 0, frame_height, frame_width, 0, GL_COLOR_BUFFER_BIT, filter);
	// Detach internal framebuffer
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
#else
	// Draw frame texture to host frame buffer
	glUseProgram(GetFramebufferShaderProgram());
	GL_CHECK();
	glUniform1i(m_framebuffer_gl_uniform_location_texture, SAMP_TEXCOORD);
	GL_CHECK();

	static const GLfloat vertices[] = {
		//  x      y      s      t
		-1.0f, -1.0f,  0.0f,  1.0f, // BL
		 1.0f, -1.0f,  1.0f,  1.0f, // BR
		 1.0f,  1.0f,  1.0f,  0.0f, // TR
		-1.0f,  1.0f,  0.0f,  0.0f, // TL
	};

	// Populate vertex buffer
	static GLuint m_framebuffer_gl_vertex_buffer_object = -1;
	if (m_framebuffer_gl_vertex_buffer_object == -1) {
		glGenBuffers(1, &m_framebuffer_gl_vertex_buffer_object);
		GL_CHECK();
	}

	glBindBuffer(GL_ARRAY_BUFFER, m_framebuffer_gl_vertex_buffer_object);
	GL_CHECK();
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
	GL_CHECK();
	// Bind vertex position attribute
	glVertexAttribPointer(
		/*index=*/ATTR_POSITION,
		/*size=vec*/2,
		/*type=*/GL_FLOAT,
		/*normalized?=*/GL_FALSE,
		/*stride=*/4 * sizeof(GLfloat),
		/*array buffer offset=*/(void*)0
	);
	GL_CHECK();
	glEnableVertexAttribArray(ATTR_POSITION);
	GL_CHECK();
	// Bind vertex texture coordinate attribute
	glVertexAttribPointer(
		/*index=*/ATTR_TEXCOORD,
		/*size=vec*/2,
		/*type=*/GL_FLOAT,
		/*normalized?=*/GL_FALSE,
		/*stride=*/4 * sizeof(GLfloat),
		/*array buffer offset=*/(void*)(2 * sizeof(GLfloat))
	);
	GL_CHECK();
	glEnableVertexAttribArray(ATTR_TEXCOORD);
	GL_CHECK();
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	GL_CHECK();
#endif
}

void pvideo_init(NV2AState *d)
{
	//qemu_cond_init(&d->pvideo.interrupt_cond);
}

void pvideo_destroy(NV2AState *d)
{
	if (d->pvideo.overlays[0].gl_texture) {
		glDeleteTextures(1, &d->pvideo.overlays[0].gl_texture);
		d->pvideo.overlays[0].gl_texture = 0;
	}

	if (d->pvideo.overlays[1].gl_texture) {
		glDeleteTextures(1, &d->pvideo.overlays[1].gl_texture);
		d->pvideo.overlays[1].gl_texture = 0;
	}

	//qemu_cond_destroy(&d->pvideo.interrupt_cond);
}

void cxbx_gl_parse_overlay(NV2AState *d, int v)
{
	OverlayState &overlay = d->pvideo.overlays[v];

	uint32_t video_buffer_use = (v == 0) ? NV_PVIDEO_BUFFER_0_USE : NV_PVIDEO_BUFFER_1_USE;
	overlay.video_buffer_use = d->pvideo.regs[NV_PVIDEO_BUFFER/4] & video_buffer_use;

	// Get overlay measures (from xqemu nv2a_overlay_draw_line) :
	uint32_t overlay_offset_high_26 = d->pvideo.regs[NV_PVIDEO_OFFSET(v)/4];
	uint32_t overlay_offset_lower_6 = d->pvideo.regs[NV_PVIDEO_POINT_IN(v)/4] >> 3;
	uint32_t overlay_size_in = d->pvideo.regs[NV_PVIDEO_SIZE_IN(v)/4];
	uint32_t overlay_color_key = d->pvideo.regs[NV_PVIDEO_COLOR_KEY(v)/4];
	uint32_t overlay_format = d->pvideo.regs[NV_PVIDEO_FORMAT(v)/4];

#ifdef DEBUG
	// Check a few assumptions
	overlay.base = d->pvideo.regs[NV_PVIDEO_BASE(v)];
	overlay.limit = d->pvideo.regs[NV_PVIDEO_LIMIT(v)];
	assert(overlay.base == 0);
	assert(overlay.limit == (128 * ONE_MB) - 1); // = CHIHIRO_CONTIGUOUS_MEMORY_SIZE - 1
	assert(GET_MASK(overlay_format, NV_PVIDEO_FORMAT_COLOR) == NV_PVIDEO_FORMAT_COLOR_LE_CR8YB8CB8YA8);

#endif
	// Derive actual attributes
	overlay.pitch = overlay_format & NV_PVIDEO_FORMAT_PITCH;
	overlay.is_transparent = overlay_format & NV_PVIDEO_FORMAT_DISPLAY;
	overlay.offset = overlay_offset_high_26 | overlay_offset_lower_6;

	uint32_t overlay_size_in_height_width = overlay_size_in - (overlay_offset_lower_6 >> 1);
	overlay.in_height = overlay_size_in_height_width >> 16;
	overlay.in_width = overlay_size_in_height_width & 0xFFFF;

	overlay.out_x = GET_MASK(d->pvideo.regs[NV_PVIDEO_POINT_OUT(v)/4], NV_PVIDEO_POINT_OUT_X);
	overlay.out_y = GET_MASK(d->pvideo.regs[NV_PVIDEO_POINT_OUT(v)/4], NV_PVIDEO_POINT_OUT_Y);
	overlay.out_width = GET_MASK(d->pvideo.regs[NV_PVIDEO_SIZE_OUT(v)/4], NV_PVIDEO_SIZE_OUT_WIDTH);
	overlay.out_height = GET_MASK(d->pvideo.regs[NV_PVIDEO_SIZE_OUT(v)/4], NV_PVIDEO_SIZE_OUT_HEIGHT);

	// Detect changes in overlay dimensions
	if (overlay.old_in_width != overlay.in_width
	 || overlay.old_in_height != overlay.in_height
	 || overlay.old_pitch != overlay.pitch) {
		overlay.old_in_width = overlay.in_width;
		overlay.old_in_height = overlay.in_height;
		overlay.old_pitch = overlay.pitch;
		if (overlay.gl_texture) {
			glDeleteTextures(1, &overlay.gl_texture);
			overlay.gl_texture = 0;
		}
	}

	overlay.covers_framebuffer = overlay.video_buffer_use
		&& (!overlay.is_transparent)
		&& (overlay.out_x == 0)
		&& (overlay.out_y == 0)
		&& (overlay.out_width == frame_width)
		&& (overlay.out_height == frame_height);
}

void cxbx_gl_render_overlays(NV2AState *d)
{
	static const GLenum overlay_gl_internal_format = GL_RGBA8;
	static const GLenum overlay_gl_format = GL_BGRA;
	static const GLenum overlay_gl_type = GL_UNSIGNED_INT_8_8_8_8_REV;

	for (int v = 0; v < 2; v++) {
		OverlayState &overlay = d->pvideo.overlays[v];
		if (!overlay.video_buffer_use) {
			continue;
		}

		// TODO : Speed this up using 2 PixelBufferObjects (and use asynchronous DMA transfer)?

		// If we need to create a (new) overlay texture, do so
		if (!overlay.gl_texture) {
			glGenTextures(1, &overlay.gl_texture);
			GL_CHECK();
			glBindTexture(GL_TEXTURE_2D, overlay.gl_texture);
			GL_CHECK();
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
			GL_CHECK();
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
			GL_CHECK();
			// Don't average YUYV samples when resizing
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			GL_CHECK();
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			GL_CHECK();
			// Note : YUYV formats are sampled using BGRA in OPENGL_SHADER_YUV[1] fragment shader
			glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, gl_swizzle_mask_BGRA);
			GL_CHECK();
			glTexImage2D(GL_TEXTURE_2D, /*level=*/0, overlay_gl_internal_format, overlay.pitch / 4, overlay.in_height, /*border=*/0, overlay_gl_format, overlay_gl_type, NULL);
			GL_CHECK();
		}
		else {
			glBindTexture(GL_TEXTURE_2D, overlay.gl_texture);
			GL_CHECK();
		}

		// Update the YUV video texture
		hwaddr overlay_pixels = /*CONTIGUOUS_MEMORY_BASE=*/0x80000000 | overlay.offset;
		glTexSubImage2D(GL_TEXTURE_2D, /*level=*/0, /*xoffset=*/0, /*yoffset=*/0, overlay.pitch / 4, overlay.in_height, overlay_gl_format, overlay_gl_type, (void*)overlay_pixels);
		GL_CHECK();

		// Note : we cannot convert overlay_offset into actual top/left coordinate, so assume (0,0)
		static const int overlay_in_s = 0;
		static const int overlay_in_t = 0;

		// Determine source and destination coordinates, with that draw the overlay over the framebuffer
		GLint srcX0 = overlay_in_s;
		GLint srcY0 = overlay_in_t;
		GLint srcX1 = overlay.in_width;
		GLint srcY1 = overlay.in_height;
		GLint dstX0 = overlay.out_x;
		GLint dstY0 = overlay.out_y;
		GLint dstX1 = overlay.out_width;
		GLint dstY1 = overlay.out_height;

		// Detect some special cases, for later finetuning
		if (overlay_in_s != 0 || overlay_in_t != 0 || !overlay.covers_framebuffer) {
			LOG_TEST_CASE("Non-standard overlay dimensions");
		}

		// Convert UV coordinates to [0.0, 1.0]
		GLfloat srcX0f = (GLfloat)srcX0 / overlay.in_width;
		GLfloat srcX1f = (GLfloat)srcX1 / overlay.in_width;
		GLfloat srcY0f = (GLfloat)srcY0 / overlay.in_height;
		GLfloat srcY1f = (GLfloat)srcY1 / overlay.in_height;

		// Convert screen coordinates to [-1.0, 1.0]
		GLfloat dstX0f = (GLfloat)((dstX0 / frame_width) * 2.0f) - 1.0f;
		GLfloat dstX1f = (GLfloat)((dstX1 / frame_width) * 2.0f) - 1.0f;
		GLfloat dstY0f = (GLfloat)((dstY0 / frame_height) * 2.0f) - 1.0f;
		GLfloat dstY1f = (GLfloat)((dstY1 / frame_height) * 2.0f) - 1.0f;

		glUseProgram(Get_YUV_to_RGB_shader_program());
		GL_CHECK();

		// Attach texture #0 to the shader sampler location 
		glUniform1i(m_overlay_gl_uniform_location_texture, SAMP_TEXCOORD);
		GL_CHECK();

		// Flip Y to prevent upside down rendering
		std::swap(srcY0f, srcY1f);

		// Feed screen and texture coordinates through a vertex buffer object
		const GLfloat overlay_vertex_buffer_data[] = {
			dstX0f, dstY0f, srcX0f, srcY0f,
			dstX1f, dstY0f, srcX1f, srcY0f,
			dstX1f, dstY1f, srcX1f, srcY1f,
			dstX0f, dstY1f, srcX0f, srcY1f,
		};

		static GLuint overlay_gl_vertex_buffer_object = -1;
		if (overlay_gl_vertex_buffer_object == -1) {
			glGenBuffers(1, &overlay_gl_vertex_buffer_object);
			GL_CHECK();
		}

		glBindBuffer(GL_ARRAY_BUFFER, overlay_gl_vertex_buffer_object);
		GL_CHECK();
		glBufferData(GL_ARRAY_BUFFER, sizeof(overlay_vertex_buffer_data), overlay_vertex_buffer_data, GL_DYNAMIC_DRAW);
		GL_CHECK();
		// Bind vertex position attribute
		glVertexAttribPointer(
			/*index=*/ATTR_POSITION,
			/*size=vec*/2,
			/*type=*/GL_FLOAT,
			/*normalized?=*/GL_FALSE,
			/*stride=*/4 * sizeof(GLfloat),
			/*array buffer offset=*/(void*)0
		);
		GL_CHECK();
		glEnableVertexAttribArray(ATTR_POSITION);
		GL_CHECK();
		// Bind vertex texture coordinate attribute
		glVertexAttribPointer(
			/*index=*/ATTR_TEXCOORD,
			/*size=vec*/2,
			/*type=*/GL_FLOAT,
			/*normalized?=*/GL_FALSE,
			/*stride=*/4 * sizeof(GLfloat),
			/*array buffer offset=*/(void*)(2 * sizeof(GLfloat))
		);
		GL_CHECK();
		glEnableVertexAttribArray(ATTR_TEXCOORD);
		GL_CHECK();
		// Finally! Draw the dang overlay...
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		GL_CHECK();
	}
}

void NV2ADevice::UpdateHostDisplay(NV2AState *d)
{
	PGRAPHState *pg = &d->pgraph;
	if (!pg->opengl_enabled) {
		return;
	}

	NV2A_GL_DGROUP_BEGIN("VGA Frame");

	glo_set_current(pg->gl_context);

	cxbx_gl_update_displaymode(d);

	for (int v = 0; v < 2; v++) {
		cxbx_gl_parse_overlay(d, v);
	}

	GL_RESET();

	// Target the host framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0); // NOTE : If disabled, overlays don't show?!
	GL_CHECK();
	glDisable(GL_CULL_FACE);
	GL_CHECK();
	// Render using texture #0
	glActiveTexture(GL_TEXTURE0);
	GL_CHECK();

	// Is either overlay fullscreen ?
	if (d->pvideo.overlays[0].covers_framebuffer 
	 || d->pvideo.overlays[1].covers_framebuffer) {
		// Then the framebuffer won't be visible anyway, so doesn't have to be rendered
	} else {
		cxbx_gl_render_framebuffer(d);
	}

	cxbx_gl_render_overlays(d);

	// Unbind everything we've used
	glUseProgram(0);
	GL_CHECK();
	glBindBuffer(GL_ARRAY_BUFFER, 0);//pg->gl_memory_buffer);
	GL_CHECK();
	glBindTexture(GL_TEXTURE_2D, 0);//pg->gl_color_buffer);
	GL_CHECK();
	// Restore xbox framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, pg->gl_framebuffer);
	GL_CHECK();

	glo_swap(pg->gl_context);

	NV2A_GL_DGROUP_END();

//	glo_set_current(NULL);

	g_renderbase->UpdateFPSCounter();
}

// TODO: Fix this properly
static void nv2a_vblank_thread(NV2AState *d)
{
	g_AffinityPolicy->SetAffinityOther();
	CxbxSetThreadName("Cxbx NV2A VBLANK");
	auto nextVBlankTime = GetNextVBlankTime();

	while (!d->exiting) {
		// Handle VBlank
		if (std::chrono::steady_clock::now() > nextVBlankTime) {
			d->pcrtc.pending_interrupts |= NV_PCRTC_INTR_0_VBLANK;
			update_irq(d);
			nextVBlankTime = GetNextVBlankTime();

			// TODO: We should swap here for the purposes of supporting overlays + direct framebuffer access
			// But it causes crashes on AMD hardware for reasons currently unknown...
			//NV2ADevice::UpdateHostDisplay(d);
		}

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}

// See NV2ABlockInfo regions[] PRAMIN
#define NV_PRAMIN_ADDR   0x00700000
#define NV_PRAMIN_SIZE              0x100000

void CxbxReserveNV2AMemory(NV2AState *d)
{
	// The NV2A memory was reserved already by the loader!

	printf("[0x%.4X] INIT: Reserved %d MiB of Xbox NV2A memory at 0x%.8X to 0x%.8X\n",
		GetCurrentThreadId(), NV2A_SIZE / ONE_MB, NV2A_ADDR, NV2A_ADDR + NV2A_SIZE - 1);

	// Allocate PRAMIN Region (the loader only reserved this region, it still needs to be committed!)
	// We are looping here because memory-reservation happens in 64 KiB increments
	d->pramin.ramin_size = NV_PRAMIN_SIZE;
	d->pramin.ramin_ptr = (uint8_t*)(NV2A_ADDR + NV_PRAMIN_ADDR);
	for (int i = 0; i < 16; i++) {
		LPVOID ret = VirtualAlloc((LPVOID)(NV2A_ADDR + NV_PRAMIN_ADDR + i * 64 * ONE_KB), 64 * ONE_KB, MEM_COMMIT, PAGE_READWRITE);
		if (ret != (LPVOID)(NV2A_ADDR + NV_PRAMIN_ADDR + i * 64 * ONE_KB)) {
			CxbxrAbort("VirtualAlloc failed to commit the memory for the nv2a pramin. The error was 0x%08X", GetLastError());
		}
	}

	printf("[0x%.4X] INIT: Allocated %d MiB of Xbox NV2A PRAMIN memory at 0x%.8x to 0x%.8x\n",
		GetCurrentThreadId(), d->pramin.ramin_size / ONE_MB, (uintptr_t)d->pramin.ramin_ptr, (uintptr_t)d->pramin.ramin_ptr + d->pramin.ramin_size - 1);
}

/* NV2ADevice */

NV2ADevice::NV2ADevice()
{
	m_nv2a_state = new NV2AState();
	m_nv2a_state->pgraph.opengl_enabled = bLLE_GPU;
}

NV2ADevice::~NV2ADevice()
{
	Reset(); // TODO : Review this

	g_renderbase->DeviceRelease();

	delete m_nv2a_state;
}

// PCI Device functions

void NV2ADevice::Init()
{
	PCIBarRegister r;

	// Register Memory bar :
	r.Raw.type = PCI_BAR_TYPE_MEMORY;
	r.Memory.address = NV2A_ADDR >> 4;
	RegisterBAR(0, NV2A_SIZE, r.value);

	// Register physical memory on bar 1
	r.Memory.address = 0xF0000000 >> 4;
	RegisterBAR(1, g_SystemMaxMemory, r.value);
	
	m_DeviceId = 0x02A5;
	m_VendorId = PCI_VENDOR_ID_NVIDIA;

	NV2AState *d = m_nv2a_state; // glue

	CxbxReserveNV2AMemory(d);

	d->pcrtc.start = 0;

	d->vram_ptr = (uint8_t*)PHYSICAL_MAP_BASE;
	d->vram_size = g_SystemMaxMemory;

	d->pramdac.core_clock_coeff = 0x00011c01; /* 189MHz...? */
	d->pramdac.core_clock_freq = 189000000;
	d->pramdac.memory_clock_coeff = 0;
	d->pramdac.video_clock_coeff = 0x0003C20D; /* 25182Khz...? */

	// Setup the conditions/mutexes
	pgraph_init(d);

	// Only init PVIDEO when LLE is enabled
	if (d->pgraph.opengl_enabled) {
		pvideo_init(d);
	}

    vblank_thread = std::thread(nv2a_vblank_thread, d);

    qemu_mutex_init(&d->pfifo.pfifo_lock);
    //qemu_cond_init(&d->pfifo.puller_cond);
    qemu_cond_init(&d->pfifo.pusher_cond);
    d->pfifo.regs[NV_PFIFO_CACHE1_STATUS/4] |= NV_PFIFO_CACHE1_STATUS_LOW_MARK;

    /* fire up puller */
	//d->pfifo.puller_thread = std::thread(pfifo_puller_thread, d);
	/* fire up pusher */
	d->pfifo.pusher_thread = std::thread(pfifo_pusher_thread, d);
}

void NV2ADevice::Reset()
{
	NV2AState *d = m_nv2a_state; // glue
	if (!d) return;

	d->exiting = true;

	//qemu_cond_broadcast(&d->pfifo.puller_cond);
	qemu_cond_broadcast(&d->pfifo.pusher_cond);
	//d->pfifo.puller_thread.join();
	d->pfifo.pusher_thread.join();
	qemu_mutex_destroy(&d->pfifo.pfifo_lock); // Cbxbx addition
	if (d->pgraph.opengl_enabled) {
		vblank_thread.join();
		pvideo_destroy(d);
	}

	pgraph_destroy(&d->pgraph);
}

uint32_t NV2ADevice::IORead(int barIndex, uint32_t port, unsigned size)
{
	return 0;
}

void NV2ADevice::IOWrite(int barIndex, uint32_t port, uint32_t value, unsigned size)
{
}

uint32_t NV2ADevice::BlockRead(const NV2ABlockInfo* block, uint32_t addr, unsigned size)
{
	switch (size) {
	case sizeof(uint8_t) :
		return block->ops.read(m_nv2a_state, addr - block->offset) & 0xFF;
	case sizeof(uint16_t) :
		assert((addr & 1) == 0); // TODO : What if this fails?	

		return block->ops.read(m_nv2a_state, addr - block->offset) & 0xFFFF;
	case sizeof(uint32_t) :
		assert((addr & 3) == 0); // TODO : What if this fails?	

		return block->ops.read(m_nv2a_state, addr - block->offset);
	default:
		assert(false);

		return 0;
	}
}

uint32_t NV2ADevice::MMIORead(int barIndex, uint32_t addr, unsigned size)
{ 
	switch (barIndex) {
	case 0: {
		// Access NV2A regardless weither HLE is disabled or not (ignoring bLLE_GPU)
		const NV2ABlockInfo* block = EmuNV2A_Block(addr);
		if (block != nullptr) {
			return BlockRead(block, addr, size);
		}
		break;
	}
	case 1: {
		// TODO : access physical memory
		break;
	}
	}

	EmuLog(LOG_LEVEL::WARNING, "NV2ADevice::MMIORead: Unhandled barIndex %d, addr %08X, size %d", barIndex, addr, size);
	return 0;
}

void NV2ADevice::BlockWrite(const NV2ABlockInfo* block, uint32_t addr, uint32_t value, unsigned size)
{
	switch (size) {
	case sizeof(uint8_t) : {
#if 0
		xbox::addr_xt aligned_addr;
		uint32_t aligned_value;
		int shift;
		uint32_t mask;

		aligned_addr = addr & ~3;
		aligned_value = block->ops.read(m_nv2a_state, aligned_addr - block->offset);
		shift = (addr & 3) * 8;
		mask = 0xFF << shift;
		block->ops.write(m_nv2a_state, aligned_addr - block->offset, (aligned_value & ~mask) | (value << shift));
#else
		block->ops.write(m_nv2a_state, addr - block->offset, value);
#endif
		return;
	}
	case sizeof(uint16_t) : {
		assert((addr & 1) == 0); // TODO : What if this fails?

		xbox::addr_xt aligned_addr;
		uint32_t aligned_value;
		int shift;
		uint32_t mask;

		aligned_addr = addr & ~3;
		aligned_value = block->ops.read(m_nv2a_state, aligned_addr - block->offset);
		shift = (addr & 2) * 16;
		mask = 0xFFFF << shift;
		block->ops.write(m_nv2a_state, aligned_addr - block->offset, (aligned_value & ~mask) | (value << shift));
		return;
	}
	case sizeof(uint32_t) :
		assert((addr & 3) == 0); // TODO : What if this fails?	

		block->ops.write(m_nv2a_state, addr - block->offset, value);
		return;
	}
}

void NV2ADevice::MMIOWrite(int barIndex, uint32_t addr, uint32_t value, unsigned size)
{
	switch (barIndex) {
	case 0: {
		// Access NV2A regardless whether HLE is disabled or not (ignoring bLLE_GPU)
		const NV2ABlockInfo* block = EmuNV2A_Block(addr);

		if (block != nullptr) {
			BlockWrite(block, addr, value, size);
			return;
		}

		break;
	}
	case 1: {
		// TODO : access physical memory
		break;
	}
	}

	EmuLog(LOG_LEVEL::WARNING, "NV2ADevice::MMIOWrite: Unhandled barIndex %d, addr %08X, value %08X, size %d", barIndex, addr, value, size);
}

int NV2ADevice::GetFrameHeight(NV2AState* d)
{
	// Derive frame_height from hardware registers
	int height = ((int)d->prmcio.cr[NV_CIO_CR_VDE_INDEX])
		| (((int)d->prmcio.cr[NV_CIO_CR_OVL_INDEX] & 0x02) >> 1 << 8)
		| (((int)d->prmcio.cr[NV_CIO_CR_OVL_INDEX] & 0x40) >> 6 << 9)
		| (((int)d->prmcio.cr[NV_CIO_CRE_LSR_INDEX] & 0x02) >> 1 << 10);

	return height++;
}

int NV2ADevice::GetFrameWidth(NV2AState* d)
{
	// Test case : Arctic Thunder, sets a 16 bit framebuffer (R5G6B5) not via
	// AvSetDisplayMode(), but via VGA control register writes, which implies
	// that it's format argument cannot be used to determine the framebuffer
	// width. Instead, read the framebuffer width from the VGA control registers :
	int width = ((int)d->prmcio.cr[NV_CIO_CR_OFFSET_INDEX])
		| (0x700 & ((int)d->prmcio.cr[NV_CIO_CRE_RPC0_INDEX] << 3))
		| (0x800 & ((int)d->prmcio.cr[NV_CIO_CRE_LSR_INDEX] << 6));
	width *= 8;
	width /= frame_pixel_bytes;

	return width;
}
