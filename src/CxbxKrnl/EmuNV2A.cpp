// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->EmuNV2A.cpp
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
// *  (c) 2016 Luke Usher <luke.usher@outlook.com>
// * 
// *  EmuNV2A.cpp is heavily based on code from XQEMU
// *  Copyright(c) 2012 espes
// *  Copyright(c) 2015 Jannik Vogel
// *  https://github.com/espes/xqemu/blob/xbox/hw/xbox/nv2a.c
// * 
// *  All rights reserved
// *
// ******************************************************************
#define _CXBXKRNL_INTERNAL
#define _XBOXKRNL_DEFEXTRN_

#include <process.h> // For __beginthreadex(), etc.

// prevent name collisions
namespace xboxkrnl
{
	#include <xboxkrnl/xboxkrnl.h> // For PKINTERRUPT, etc.
};


#ifdef _MSC_VER                         // Check if MS Visual C compiler
#  pragma comment(lib, "opengl32.lib")  // Compiler-specific directive to avoid manually configuration
//#  pragma comment(lib, "glu32.lib")     // Link libraries
#  pragma comment(lib, "glew32.lib")
#endif

#include <distorm.h> // For uint32_t
#include <string> // For std::string
#include <mutex>
#include <condition_variable>
#include <queue>
#include <thread>

#include "CxbxKrnl.h"
#include "device.h"
#include "Emu.h"
#include "EmuFS.h"
#include "EmuKrnl.h"
#include "EmuNV2A.h"
#include "nv2a_int.h" // from https://github.com/espes/xqemu/tree/xbox/hw/xbox

#include <gl\glew.h>
#include <gl\GL.h>
#include <gl\GLU.h>
#include <cassert>
//#include <gl\glut.h>

// Public Domain ffs Implementation
// See: http://snipplr.com/view/22147/stringsh-implementation/
int ffs(int v)
{
	unsigned int x = v;
	int c = 1;

	/*
	* adapted from from
	*      http://graphics.stanford.edu/~seander/bithacks.html#ZerosOnRightBinSearch
	*
	* a modified binary search algorithm to count 0 bits from
	*  the right (lsb).  This algorithm should work regardless
	*  of the size of ints on the platform.
	*
	*/

	/* a couple special cases */
	if (x == 0) return 0;
	if (x & 1)  return 1;   /* probably pretty common */

	c = 1;
	while ((x & 0xffff) == 0) {
		x >>= 16;
		c += 16;
	}
	if ((x & 0xff) == 0) {
		x >>= 8;
		c += 8;
	}
	if ((x & 0x0f) == 0) {
		x >>= 4;
		c += 4;
	}
	if ((x & 0x03) == 0) {
		x >>= 2;
		c += 2;
	}

	c -= (x & 1);
	c += 1;     /* ffs() requires indexing bits from 1 */
				/*   ie., the lsb is bit 1, not bit 0  */
	return c;
}

inline int GET_MASK(int v, int mask) {
	return (((v) & (mask)) >> (ffs(mask) - 1));
};

inline int SET_MASK(int v, int mask, int val) {  
    const unsigned int __val = (val);                            
    const unsigned int __mask = (mask);                          

    (v) &= ~(__mask);                                            
    return (v) |= ((__val) << (ffs(__mask)-1)) & (__mask);              
}

#define CASE_4(v, step)                                              \
    case (v):                                                        \
	case (v)+(step) :												 \
	case (v)+(step) * 2:                                             \
	case (v)+(step) * 3


enum FifoMode {
	FIFO_PIO = 0,
	FIFO_DMA = 1,
};

enum FIFOEngine {
	ENGINE_SOFTWARE = 0,
	ENGINE_GRAPHICS = 1,
	ENGINE_DVD = 2,
};

typedef struct RAMHTEntry {
	uint32_t handle;
	xbaddr instance;
	enum FIFOEngine engine;
	unsigned int channel_id : 5;
	bool valid;
} RAMHTEntry;

typedef struct DMAObject {
	unsigned int dma_class;
	unsigned int dma_target;
	xbaddr address;
	xbaddr limit;
} DMAObject;

typedef struct VertexAttribute {
	bool dma_select;
	xbaddr offset;

	/* inline arrays are packed in order?
	* Need to pass the offset to converted attributes */
	unsigned int inline_array_offset;

	float inline_value[4];

	unsigned int format;
	unsigned int size; /* size of the data type */
	unsigned int count; /* number of components */
	uint32_t stride;

	bool needs_conversion;
	uint8_t *converted_buffer;
	unsigned int converted_elements;
	unsigned int converted_size;
	unsigned int converted_count;

	float *inline_buffer;

	GLint gl_count;
	GLenum gl_type;
	GLboolean gl_normalize;

	GLuint gl_converted_buffer;
	GLuint gl_inline_buffer;
} VertexAttribute;

typedef struct Surface {
	bool draw_dirty;
	bool buffer_dirty;
	bool write_enabled_cache;
	unsigned int pitch;

	xbaddr offset;
} Surface;

typedef struct SurfaceShape {
	unsigned int z_format;
	unsigned int color_format;
	unsigned int zeta_format;
	unsigned int log_width, log_height;
	unsigned int clip_x, clip_y;
	unsigned int clip_width, clip_height;
	unsigned int anti_aliasing;
} SurfaceShape;

typedef struct TextureShape {
	bool cubemap;
	unsigned int dimensionality;
	unsigned int color_format;
	unsigned int levels;
	unsigned int width, height, depth;

	unsigned int min_mipmap_level, max_mipmap_level;
	unsigned int pitch;
} TextureShape;

typedef struct TextureKey {
	TextureShape state;
	uint64_t data_hash;
	uint8_t* texture_data;
	uint8_t* palette_data;
} TextureKey;

typedef struct TextureBinding {
	GLenum gl_target;
	GLuint gl_texture;
	unsigned int refcnt;
} TextureBinding;

typedef struct KelvinState {
	xbaddr dma_notifies;
	xbaddr dma_state;
	xbaddr dma_semaphore;
	unsigned int semaphore_offset;
} KelvinState;

typedef struct ContextSurfaces2DState {
	xbaddr dma_image_source;
	xbaddr dma_image_dest;
	unsigned int color_format;
	unsigned int source_pitch, dest_pitch;
	xbaddr source_offset, dest_offset;

} ContextSurfaces2DState;

typedef struct ImageBlitState {
	xbaddr context_surfaces;
	unsigned int operation;
	unsigned int in_x, in_y;
	unsigned int out_x, out_y;
	unsigned int width, height;

} ImageBlitState;

typedef struct GraphicsObject {
	uint8_t graphics_class;
	union {
		ContextSurfaces2DState context_surfaces_2d;

		ImageBlitState image_blit;

		KelvinState kelvin;
	} data;
} GraphicsObject;

typedef struct GraphicsSubchannel {
	xbaddr object_instance;
	GraphicsObject object;
	uint32_t object_cache[5];
} GraphicsSubchannel;

typedef struct GraphicsContext {
	bool channel_3d;
	unsigned int subchannel;
} GraphicsContext;

typedef struct CacheEntry {
	unsigned int method : 14;
	unsigned int subchannel : 3;
	bool nonincreasing;
	uint32_t parameter;
} CacheEntry;

typedef struct Cache1State {
	unsigned int channel_id;
	FifoMode mode;

	/* Pusher state */
	bool push_enabled;
	bool dma_push_enabled;
	bool dma_push_suspended;
	xbaddr dma_instance;

	bool method_nonincreasing;
	unsigned int method : 14;
	unsigned int subchannel : 3;
	unsigned int method_count : 24;
	uint32_t dcount;

	bool subroutine_active;
	xbaddr subroutine_return;
	xbaddr get_jmp_shadow;
	uint32_t rsvd_shadow;
	uint32_t data_shadow;
	uint32_t error;

	bool pull_enabled;
	enum FIFOEngine bound_engines[NV2A_NUM_SUBCHANNELS];
	enum FIFOEngine last_engine;

	/* The actual command queue */
	std::mutex mutex;
	std::condition_variable cache_cond;
	std::queue<CacheEntry*> cache;
	std::queue<CacheEntry*> working_cache;
} Cache1State;

typedef struct ChannelControl {
	xbaddr dma_put;
	xbaddr dma_get;
	uint32_t ref;
} ChannelControl;

struct {
	uint32_t pending_interrupts;
	uint32_t enabled_interrupts;
	uint32_t regs[NV_PMC_SIZE]; // TODO : union
} pmc;

struct {
	uint32_t pending_interrupts;
	uint32_t enabled_interrupts;
	std::thread puller_thread;
	Cache1State cache1;
	uint32_t regs[NV_PFIFO_SIZE]; // TODO : union
} pfifo;

struct {
	uint32_t regs[NV_PVIDEO_SIZE]; // TODO : union
} pvideo;


struct {
	uint32_t pending_interrupts;
	uint32_t enabled_interrupts;
	uint32_t numerator;
	uint32_t denominator;
	uint32_t alarm_time;
	uint32_t regs[NV_PTIMER_SIZE]; // TODO : union
} ptimer;

struct {
	uint32_t regs[NV_PFB_SIZE]; // TODO : union
} pfb;

struct {
	uint32_t pending_interrupts;
	uint32_t enabled_interrupts;
	uint32_t start;
	uint32_t regs[NV_PCRTC_SIZE]; // TODO : union
} pcrtc;

struct {
	uint32_t core_clock_coeff;
	uint64_t core_clock_freq;
	uint32_t memory_clock_coeff;
	uint32_t video_clock_coeff;
	uint32_t regs[NV_PRAMDAC_SIZE]; // TODO : union
} pramdac;

struct {
	uint32_t regs[NV_PRAMIN_SIZE]; // TODO : union
} pramin;

struct {
	ChannelControl channel_control[NV2A_NUM_CHANNELS];
} user;

struct {
	std::mutex mutex;

	uint32_t pending_interrupts;
	uint32_t enabled_interrupts;
	std::condition_variable interrupt_cond;

	xbaddr context_table;
	xbaddr context_address;


	unsigned int trapped_method;
	unsigned int trapped_subchannel;
	unsigned int trapped_channel_id;
	uint32_t trapped_data[2];
	uint32_t notify_source;

	bool fifo_access;
	std::condition_variable fifo_access_cond;
	std::condition_variable flip_3d;

	unsigned int channel_id;
	bool channel_valid;
	GraphicsContext context[NV2A_NUM_CHANNELS];

	xbaddr dma_color, dma_zeta;
	Surface surface_color, surface_zeta;
	unsigned int surface_type;
	SurfaceShape surface_shape;
	SurfaceShape last_surface_shape;

	xbaddr dma_a, dma_b;
	//GLruCache *texture_cache;
	bool texture_dirty[NV2A_MAX_TEXTURES];
	TextureBinding *texture_binding[NV2A_MAX_TEXTURES];

	//GHashTable *shader_cache;
	//ShaderBinding *shader_binding;

	bool texture_matrix_enable[NV2A_MAX_TEXTURES];

	/* FIXME: Move to NV_PGRAPH_BUMPMAT... */
	float bump_env_matrix[NV2A_MAX_TEXTURES - 1][4]; /* 3 allowed stages with 2x2 matrix each */

	// wglContext *gl_context;
	GLuint gl_framebuffer;
	GLuint gl_color_buffer, gl_zeta_buffer;
	GraphicsSubchannel subchannel_data[NV2A_NUM_SUBCHANNELS];

	xbaddr dma_report;
	xbaddr report_offset;
	bool zpass_pixel_count_enable;
	unsigned int zpass_pixel_count_result;
	unsigned int gl_zpass_pixel_count_query_count;
	GLuint* gl_zpass_pixel_count_queries;

	xbaddr dma_vertex_a, dma_vertex_b;

	unsigned int primitive_mode;

	bool enable_vertex_program_write;

	//uint32_t program_data[NV2A_MAX_TRANSFORM_PROGRAM_LENGTH][VSH_TOKEN_SIZE];

	uint32_t vsh_constants[NV2A_VERTEXSHADER_CONSTANTS][4];
	bool vsh_constants_dirty[NV2A_VERTEXSHADER_CONSTANTS];

	/* lighting constant arrays */
	uint32_t ltctxa[NV2A_LTCTXA_COUNT][4];
	bool ltctxa_dirty[NV2A_LTCTXA_COUNT];
	uint32_t ltctxb[NV2A_LTCTXB_COUNT][4];
	bool ltctxb_dirty[NV2A_LTCTXB_COUNT];
	uint32_t ltc1[NV2A_LTC1_COUNT][4];
	bool ltc1_dirty[NV2A_LTC1_COUNT];

	// should figure out where these are in lighting context
	float light_infinite_half_vector[NV2A_MAX_LIGHTS][3];
	float light_infinite_direction[NV2A_MAX_LIGHTS][3];
	float light_local_position[NV2A_MAX_LIGHTS][3];
	float light_local_attenuation[NV2A_MAX_LIGHTS][3];

	//VertexAttribute vertex_attributes[NV2A_VERTEXSHADER_ATTRIBUTES];

	unsigned int inline_array_length;
	uint32_t inline_array[NV2A_MAX_BATCH_LENGTH];
	GLuint gl_inline_array_buffer;

	unsigned int inline_elements_length;
	uint32_t inline_elements[NV2A_MAX_BATCH_LENGTH];

	unsigned int inline_buffer_length;

	unsigned int draw_arrays_length;
	unsigned int draw_arrays_max_count;

	/* FIXME: Unknown size, possibly endless, 1000 will do for now */
	GLint gl_draw_arrays_start[1000];
	GLsizei gl_draw_arrays_count[1000];

	GLuint gl_element_buffer;
	GLuint gl_memory_buffer;
	GLuint gl_vertex_array;

	uint32_t regs[NV_PGRAPH_SIZE]; // TODO : union
} pgraph;

static void update_irq()
{
	/* PFIFO */
	if (pfifo.pending_interrupts & pfifo.enabled_interrupts) {
		pmc.pending_interrupts |= NV_PMC_INTR_0_PFIFO;
	} else {
		pmc.pending_interrupts &= ~NV_PMC_INTR_0_PFIFO;
	}

	/* PCRTC */
	if (pcrtc.pending_interrupts & pcrtc.enabled_interrupts) {
		pmc.pending_interrupts |= NV_PMC_INTR_0_PCRTC;
	} else {
		pmc.pending_interrupts &= ~NV_PMC_INTR_0_PCRTC;
	}

	/* PGRAPH */
	if (pgraph.pending_interrupts & pgraph.enabled_interrupts) {
		pmc.pending_interrupts |= NV_PMC_INTR_0_PGRAPH;
	} else {
		pmc.pending_interrupts &= ~NV_PMC_INTR_0_PGRAPH;
	}

	/* TODO : PBUS * /
	if (pbus.pending_interrupts & pbus.enabled_interrupts) {
		pmc.pending_interrupts |= NV_PMC_INTR_0_PBUS;
	}
	else {
		pmc.pending_interrupts &= ~NV_PMC_INTR_0_PBUS;
	} */

	/* TODO : SOFTWARE * /
	if (user.pending_interrupts & user.enabled_interrupts) {
		pmc.pending_interrupts |= NV_PMC_INTR_0_SOFTWARE;
	}
	else {
		pmc.pending_interrupts &= ~NV_PMC_INTR_0_SOFTWARE;
	} */

	if (pmc.pending_interrupts && pmc.enabled_interrupts) {
		HalSystemInterrupts[3].Assert(true);
	} else {
		HalSystemInterrupts[3].Assert(false);
	}

	SwitchToThread();
}


#define DEBUG_START(DEV) \
const char *DebugNV_##DEV##(xbaddr addr) \
{ \
	switch (addr) {
#define DEBUG_CASE(a) \
		case a: return #a;
#define DEBUG_CASE_EX(a, c) \
		case a: return #a##c;
#define DEBUG_END(DEV) \
		default: \
			return "Unknown " #DEV " Address"; \
	} \
}

DEBUG_START(PMC)
	DEBUG_CASE(NV_PMC_BOOT_0);
	DEBUG_CASE(NV_PMC_INTR_0);
	DEBUG_CASE(NV_PMC_INTR_EN_0);
	DEBUG_CASE(NV_PMC_ENABLE);
DEBUG_END(PMC)

DEBUG_START(PBUS)
	DEBUG_CASE_EX(NV_PBUS_PCI_NV_0, ":VENDOR_ID");
	DEBUG_CASE(NV_PBUS_PCI_NV_1);
	DEBUG_CASE_EX(NV_PBUS_PCI_NV_2, ":REVISION_ID");
	DEBUG_CASE_EX(NV_PBUS_PCI_NV_3, ":LATENCY_TIMER");
	DEBUG_CASE(NV_PBUS_PCI_NV_4);
	DEBUG_CASE(NV_PBUS_PCI_NV_5);
	DEBUG_CASE(NV_PBUS_PCI_NV_6);
	DEBUG_CASE(NV_PBUS_PCI_NV_7);
	DEBUG_CASE_EX(NV_PBUS_PCI_NV_11, ":SUBSYSTEM");
	DEBUG_CASE_EX(NV_PBUS_PCI_NV_12, ":ROM_BASE");
	DEBUG_CASE_EX(NV_PBUS_PCI_NV_13, ":CAP_PTR");
	DEBUG_CASE_EX(NV_PBUS_PCI_NV_14, ":RESERVED");
	DEBUG_CASE(NV_PBUS_PCI_NV_15);
	DEBUG_CASE_EX(NV_PBUS_PCI_NV_16, ":SUBSYSTEM");
	DEBUG_CASE(NV_PBUS_PCI_NV_17);
	DEBUG_CASE_EX(NV_PBUS_PCI_NV_18, ":AGP_STATUS");
	DEBUG_CASE_EX(NV_PBUS_PCI_NV_19, ":AGP_COMMAND");
	DEBUG_CASE_EX(NV_PBUS_PCI_NV_20, ":ROM_SHADOW");
	DEBUG_CASE_EX(NV_PBUS_PCI_NV_21, ":VGA");
	DEBUG_CASE_EX(NV_PBUS_PCI_NV_22, ":SCRATCH");
	DEBUG_CASE_EX(NV_PBUS_PCI_NV_23, ":DT_TIMEOUT");
	DEBUG_CASE_EX(NV_PBUS_PCI_NV_24, ":PME");
	DEBUG_CASE_EX(NV_PBUS_PCI_NV_25, ":POWER_STATE");
	DEBUG_CASE_EX(NV_PBUS_PCI_NV_26, ":RESERVED");
DEBUG_END(PBUS)

DEBUG_START(PFIFO)
	DEBUG_CASE(NV_PFIFO_INTR_0);
	DEBUG_CASE(NV_PFIFO_INTR_EN_0);
	DEBUG_CASE(NV_PFIFO_RAMHT);
	DEBUG_CASE(NV_PFIFO_RAMFC);
	DEBUG_CASE(NV_PFIFO_RAMRO);
	DEBUG_CASE(NV_PFIFO_RUNOUT_STATUS);
	DEBUG_CASE(NV_PFIFO_MODE);
	DEBUG_CASE(NV_PFIFO_DMA);
	DEBUG_CASE(NV_PFIFO_CACHE1_PUSH0);
	DEBUG_CASE(NV_PFIFO_CACHE1_PUSH1);
	DEBUG_CASE(NV_PFIFO_CACHE1_STATUS);
	DEBUG_CASE(NV_PFIFO_CACHE1_DMA_PUSH);
	DEBUG_CASE(NV_PFIFO_CACHE1_DMA_FETCH);
	DEBUG_CASE(NV_PFIFO_CACHE1_DMA_STATE);
	DEBUG_CASE(NV_PFIFO_CACHE1_DMA_INSTANCE);
	DEBUG_CASE(NV_PFIFO_CACHE1_DMA_PUT);
	DEBUG_CASE(NV_PFIFO_CACHE1_DMA_GET);
	DEBUG_CASE(NV_PFIFO_CACHE1_DMA_SUBROUTINE);
	DEBUG_CASE(NV_PFIFO_CACHE1_PULL0);
	DEBUG_CASE(NV_PFIFO_CACHE1_ENGINE);
	DEBUG_CASE(NV_PFIFO_CACHE1_DMA_DCOUNT);
	DEBUG_CASE(NV_PFIFO_CACHE1_DMA_GET_JMP_SHADOW);
	DEBUG_CASE(NV_PFIFO_CACHE1_DMA_RSVD_SHADOW);
	DEBUG_CASE(NV_PFIFO_CACHE1_DMA_DATA_SHADOW);
DEBUG_END(PFIFO)

DEBUG_START(PRMA)
DEBUG_END(PRMA)

DEBUG_START(PVIDEO)
	DEBUG_CASE(NV_PVIDEO_INTR);
	DEBUG_CASE(NV_PVIDEO_INTR_EN);
	DEBUG_CASE(NV_PVIDEO_BUFFER);
	DEBUG_CASE(NV_PVIDEO_STOP);
	DEBUG_CASE(NV_PVIDEO_BASE);
	DEBUG_CASE(NV_PVIDEO_LIMIT);
	DEBUG_CASE(NV_PVIDEO_LUMINANCE);
	DEBUG_CASE(NV_PVIDEO_CHROMINANCE);
	DEBUG_CASE(NV_PVIDEO_OFFSET);
	DEBUG_CASE(NV_PVIDEO_SIZE_IN);
	DEBUG_CASE(NV_PVIDEO_POINT_IN);
	DEBUG_CASE(NV_PVIDEO_DS_DX);
	DEBUG_CASE(NV_PVIDEO_DT_DY);
	DEBUG_CASE(NV_PVIDEO_POINT_OUT);
	DEBUG_CASE(NV_PVIDEO_SIZE_OUT);
	DEBUG_CASE(NV_PVIDEO_FORMAT);
DEBUG_END(PVIDEO)

DEBUG_START(PTIMER)
	DEBUG_CASE(NV_PTIMER_INTR_0);
	DEBUG_CASE(NV_PTIMER_INTR_EN_0);
	DEBUG_CASE(NV_PTIMER_NUMERATOR);
	DEBUG_CASE(NV_PTIMER_DENOMINATOR);
	DEBUG_CASE(NV_PTIMER_TIME_0);
	DEBUG_CASE(NV_PTIMER_TIME_1);
	DEBUG_CASE(NV_PTIMER_ALARM_0);

DEBUG_END(PTIMER)

DEBUG_START(PCOUNTER)
DEBUG_END(PCOUNTER)

DEBUG_START(PVPE)
DEBUG_END(PVPE)

DEBUG_START(PTV)
DEBUG_END(PTV)

DEBUG_START(PRMFB)
DEBUG_END(PRMFB)

DEBUG_START(PRMVIO)
DEBUG_END(PRMVIO)

DEBUG_START(PFB)
	DEBUG_CASE(NV_PFB_CFG0)
	DEBUG_CASE(NV_PFB_CSTATUS)
	DEBUG_CASE(NV_PFB_REFCTRL)
	DEBUG_CASE(NV_PFB_NVM) // 	NV_PFB_NVM_MODE_DISABLE 
	DEBUG_CASE(NV_PFB_PIN)
	DEBUG_CASE(NV_PFB_PAD)
	DEBUG_CASE(NV_PFB_TIMING0)
	DEBUG_CASE(NV_PFB_TIMING1)
	DEBUG_CASE(NV_PFB_TIMING2)
	DEBUG_CASE(NV_PFB_TILE)
	DEBUG_CASE(NV_PFB_TLIMIT)
	DEBUG_CASE(NV_PFB_TSIZE)
	DEBUG_CASE(NV_PFB_TSTATUS)
	DEBUG_CASE(NV_PFB_MRS)
	DEBUG_CASE(NV_PFB_EMRS)
	DEBUG_CASE(NV_PFB_MRS_EXT)
	DEBUG_CASE(NV_PFB_EMRS_EXT)
	DEBUG_CASE(NV_PFB_REF)
	DEBUG_CASE(NV_PFB_PRE)
	DEBUG_CASE(NV_PFB_ZCOMP)
	DEBUG_CASE(NV_PFB_ARB_PREDIVIDER)
	DEBUG_CASE(NV_PFB_ARB_TIMEOUT)
	DEBUG_CASE(NV_PFB_ARB_XFER_REM)
	DEBUG_CASE(NV_PFB_ARB_DIFF_BANK)
	DEBUG_CASE(NV_PFB_CLOSE_PAGE0)
	DEBUG_CASE(NV_PFB_CLOSE_PAGE1)
	DEBUG_CASE(NV_PFB_CLOSE_PAGE2)
	DEBUG_CASE(NV_PFB_BPARB)
	DEBUG_CASE(NV_PFB_CMDQ0)
	DEBUG_CASE(NV_PFB_CMDQ1)
	DEBUG_CASE(NV_PFB_ILL_INSTR)
	DEBUG_CASE(NV_PFB_RT)
	DEBUG_CASE(NV_PFB_AUTOCLOSE)
	DEBUG_CASE(NV_PFB_WBC)
	DEBUG_CASE(NV_PFB_CMDQ_PRT)
	DEBUG_CASE(NV_PFB_CPU_RRQ)
	DEBUG_CASE(NV_PFB_BYPASS);
DEBUG_END(PFB)

DEBUG_START(PSTRAPS)
DEBUG_END(PSTRAPS)

DEBUG_START(PGRAPH)
	DEBUG_CASE(NV_PGRAPH_INTR);
	DEBUG_CASE(NV_PGRAPH_NSOURCE);
	DEBUG_CASE(NV_PGRAPH_INTR_EN);
	DEBUG_CASE(NV_PGRAPH_CTX_CONTROL);
	DEBUG_CASE(NV_PGRAPH_CTX_USER);
	DEBUG_CASE(NV_PGRAPH_CTX_SWITCH1);
	DEBUG_CASE(NV_PGRAPH_TRAPPED_ADDR);
	DEBUG_CASE(NV_PGRAPH_TRAPPED_DATA_LOW);
	DEBUG_CASE(NV_PGRAPH_SURFACE);
	DEBUG_CASE(NV_PGRAPH_INCREMENT);
	DEBUG_CASE(NV_PGRAPH_FIFO);
	DEBUG_CASE(NV_PGRAPH_CHANNEL_CTX_TABLE);
	DEBUG_CASE(NV_PGRAPH_CHANNEL_CTX_POINTER);
	DEBUG_CASE(NV_PGRAPH_CHANNEL_CTX_TRIGGER);
	DEBUG_CASE(NV_PGRAPH_CSV0_D);
	DEBUG_CASE(NV_PGRAPH_CSV0_C);
	DEBUG_CASE(NV_PGRAPH_CSV1_B);
	DEBUG_CASE(NV_PGRAPH_CSV1_A);
	DEBUG_CASE(NV_PGRAPH_CHEOPS_OFFSET);
	DEBUG_CASE(NV_PGRAPH_BLEND);
	DEBUG_CASE(NV_PGRAPH_BLENDCOLOR);
	DEBUG_CASE(NV_PGRAPH_BORDERCOLOR0);
	DEBUG_CASE(NV_PGRAPH_BORDERCOLOR1);
	DEBUG_CASE(NV_PGRAPH_BORDERCOLOR2);
	DEBUG_CASE(NV_PGRAPH_BORDERCOLOR3);
	DEBUG_CASE(NV_PGRAPH_BUMPOFFSET1);
	DEBUG_CASE(NV_PGRAPH_BUMPSCALE1);
	DEBUG_CASE(NV_PGRAPH_CLEARRECTX);
	DEBUG_CASE(NV_PGRAPH_CLEARRECTY);
	DEBUG_CASE(NV_PGRAPH_COLORCLEARVALUE);
	DEBUG_CASE(NV_PGRAPH_COMBINEFACTOR0);
	DEBUG_CASE(NV_PGRAPH_COMBINEFACTOR1);
	DEBUG_CASE(NV_PGRAPH_COMBINEALPHAI0);
	DEBUG_CASE(NV_PGRAPH_COMBINEALPHAO0);
	DEBUG_CASE(NV_PGRAPH_COMBINECOLORI0);
	DEBUG_CASE(NV_PGRAPH_COMBINECOLORO0);
	DEBUG_CASE(NV_PGRAPH_COMBINECTL);
	DEBUG_CASE(NV_PGRAPH_COMBINESPECFOG0);
	DEBUG_CASE(NV_PGRAPH_COMBINESPECFOG1);
	DEBUG_CASE(NV_PGRAPH_CONTROL_0);
	DEBUG_CASE(NV_PGRAPH_CONTROL_2);
	DEBUG_CASE(NV_PGRAPH_CONTROL_3);
	DEBUG_CASE(NV_PGRAPH_FOGCOLOR);
	DEBUG_CASE(NV_PGRAPH_FOGPARAM0);
	DEBUG_CASE(NV_PGRAPH_FOGPARAM1);
	DEBUG_CASE(NV_PGRAPH_SETUPRASTER);
	DEBUG_CASE(NV_PGRAPH_SHADERCLIPMODE);
	DEBUG_CASE(NV_PGRAPH_SHADERCTL);
	DEBUG_CASE(NV_PGRAPH_SHADERPROG);
	DEBUG_CASE(NV_PGRAPH_SHADOWZSLOPETHRESHOLD);
	DEBUG_CASE(NV_PGRAPH_SPECFOGFACTOR0);
	DEBUG_CASE(NV_PGRAPH_SPECFOGFACTOR1);
	DEBUG_CASE(NV_PGRAPH_TEXADDRESS0);
	DEBUG_CASE(NV_PGRAPH_TEXADDRESS0_ADDRV);
	DEBUG_CASE(NV_PGRAPH_TEXADDRESS1);
	DEBUG_CASE(NV_PGRAPH_TEXADDRESS2);
	DEBUG_CASE(NV_PGRAPH_TEXADDRESS3);
	DEBUG_CASE(NV_PGRAPH_TEXCTL0_0);
	DEBUG_CASE(NV_PGRAPH_TEXCTL0_1);
	DEBUG_CASE(NV_PGRAPH_TEXCTL0_2);
	DEBUG_CASE(NV_PGRAPH_TEXCTL0_3);
	DEBUG_CASE(NV_PGRAPH_TEXCTL1_0);
	DEBUG_CASE(NV_PGRAPH_TEXCTL1_1);
	DEBUG_CASE(NV_PGRAPH_TEXCTL1_2);
	DEBUG_CASE(NV_PGRAPH_TEXCTL1_3);
	DEBUG_CASE(NV_PGRAPH_TEXCTL2_0);
	DEBUG_CASE(NV_PGRAPH_TEXCTL2_1);
	DEBUG_CASE(NV_PGRAPH_TEXFILTER0);
	DEBUG_CASE(NV_PGRAPH_TEXFILTER1);
	DEBUG_CASE(NV_PGRAPH_TEXFILTER2);
	DEBUG_CASE(NV_PGRAPH_TEXFILTER3);
	DEBUG_CASE(NV_PGRAPH_TEXFMT0);
	DEBUG_CASE(NV_PGRAPH_TEXFMT1);
	DEBUG_CASE(NV_PGRAPH_TEXFMT2);
	DEBUG_CASE(NV_PGRAPH_TEXFMT3);
	DEBUG_CASE(NV_PGRAPH_TEXIMAGERECT0);
	DEBUG_CASE(NV_PGRAPH_TEXIMAGERECT1);
	DEBUG_CASE(NV_PGRAPH_TEXIMAGERECT2);
	DEBUG_CASE(NV_PGRAPH_TEXIMAGERECT3);
	DEBUG_CASE(NV_PGRAPH_TEXOFFSET0);
	DEBUG_CASE(NV_PGRAPH_TEXOFFSET1);
	DEBUG_CASE(NV_PGRAPH_TEXOFFSET2);
	DEBUG_CASE(NV_PGRAPH_TEXOFFSET3);
	DEBUG_CASE(NV_PGRAPH_TEXPALETTE0);
	DEBUG_CASE(NV_PGRAPH_TEXPALETTE1);
	DEBUG_CASE(NV_PGRAPH_TEXPALETTE2);
	DEBUG_CASE(NV_PGRAPH_TEXPALETTE3);
	DEBUG_CASE(NV_PGRAPH_ZSTENCILCLEARVALUE);
	DEBUG_CASE(NV_PGRAPH_ZCLIPMIN);
	DEBUG_CASE(NV_PGRAPH_ZOFFSETBIAS);
	DEBUG_CASE(NV_PGRAPH_ZOFFSETFACTOR);
	DEBUG_CASE(NV_PGRAPH_EYEVEC0);
	DEBUG_CASE(NV_PGRAPH_EYEVEC1);
	DEBUG_CASE(NV_PGRAPH_EYEVEC2);
	DEBUG_CASE(NV_PGRAPH_ZCLIPMAX);
DEBUG_END(PGRAPH)

DEBUG_START(PCRTC)
	DEBUG_CASE(NV_PCRTC_INTR_0);
	DEBUG_CASE(NV_PCRTC_INTR_EN_0);
	DEBUG_CASE(NV_PCRTC_START);
	DEBUG_CASE(NV_PCRTC_CONFIG);

DEBUG_END(PCRTC)

DEBUG_START(PRMCIO)
DEBUG_END(PRMCIO)

DEBUG_START(PRAMDAC)
	DEBUG_CASE(NV_PRAMDAC_NVPLL_COEFF);
	DEBUG_CASE(NV_PRAMDAC_MPLL_COEFF);
	DEBUG_CASE(NV_PRAMDAC_VPLL_COEFF);
	DEBUG_CASE(NV_PRAMDAC_PLL_TEST_COUNTER);

DEBUG_END(PRAMDAC)

DEBUG_START(PRMDIO)
DEBUG_END(PRMDIO)

DEBUG_START(PRAMIN)
DEBUG_END(PRAMIN)

DEBUG_START(USER)

	DEBUG_CASE(NV_USER_DMA_PUT);
	DEBUG_CASE(NV_USER_DMA_GET);
	DEBUG_CASE(NV_USER_REF);

	DEBUG_END(USER)




#define DEBUG_READ32(DEV)            DbgPrintf("EmuX86 Read32 NV2A " #DEV "(0x%08X) = 0x%08X [Handle%s]\n", addr, result, DebugNV_##DEV##(addr))
#define DEBUG_READ32_UNHANDLED(DEV)  { DbgPrintf("EmuX86 Read32 NV2A " #DEV "(0x%08X) = 0x%08X [Unhandle%s]\n", addr, result, DebugNV_##DEV##(addr)); return result; }

#define DEBUG_WRITE32(DEV)           DbgPrintf("EmuX86 Write32 NV2A " #DEV "(0x%08X, 0x%08X) [Handle%s]\n", addr, value, DebugNV_##DEV##(addr))
#define DEBUG_WRITE32_UNHANDLED(DEV) { DbgPrintf("EmuX86 Write32 NV2A " #DEV "(0x%08X, 0x%08X) [Unhandle%s]\n", addr, value, DebugNV_##DEV##(addr)); return; }

#define DEVICE_READ32(DEV) uint32_t EmuNV2A_##DEV##_Read32(xbaddr addr)
#define DEVICE_READ32_SWITCH() uint32_t result = 0; switch (addr) 
#define DEVICE_READ32_REG(dev) result = dev.regs[addr]
#define DEVICE_READ32_END(DEV) DEBUG_READ32(DEV); return result

#define DEVICE_WRITE32(DEV) void EmuNV2A_##DEV##_Write32(xbaddr addr, uint32_t value)
#define DEVICE_WRITE32_REG(dev) dev.regs[addr] = value
#define DEVICE_WRITE32_END(DEV) DEBUG_WRITE32(DEV)

static inline uint32_t ldl_le_p(const void *p)
{
	return *(uint32_t*)p;
}

static DMAObject nv_dma_load(xbaddr dma_obj_address)
{
	assert(dma_obj_address < NV_PRAMIN_SIZE);

	uint32_t *dma_obj = (uint32_t*)(NV2A_ADDR + NV_PRAMIN_ADDR + dma_obj_address);
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

static void *nv_dma_map(xbaddr dma_obj_address, xbaddr *len)
{
	assert(dma_obj_address < NV_PRAMIN_SIZE);

	DMAObject dma = nv_dma_load(dma_obj_address);

	/* TODO: Handle targets and classes properly */
	printf("dma_map %x, %x, %x %x"  "\n",
		dma.dma_class, dma.dma_target, dma.address, dma.limit);

	dma.address &= 0x07FFFFFF;

	// assert(dma.address + dma.limit < memory_region_size(d->vram));
	*len = dma.limit;
	return (void*)(MM_SYSTEM_PHYSICAL_MAP + dma.address);
}

/* pusher should be fine to run from a mimo handler
* whenever's it's convenient */
static void pfifo_run_pusher() {
	uint8_t channel_id;
	ChannelControl *control;
	Cache1State *state;
	CacheEntry *command;
	uint8_t *dma;
	xbaddr dma_len;
	uint32_t word;

	/* TODO: How is cache1 selected? */
	state = &pfifo.cache1;
	channel_id = state->channel_id;
	control = &user.channel_control[channel_id];

	if (!state->push_enabled) return;

	/* only handling DMA for now... */

	/* Channel running DMA */
	uint32_t channel_modes = pfifo.regs[NV_PFIFO_MODE];
	assert(channel_modes & (1 << channel_id));
	assert(state->mode == FIFO_DMA);

	if (!state->dma_push_enabled) return;
	if (state->dma_push_suspended) return;

	/* We're running so there should be no pending errors... */
	assert(state->error == NV_PFIFO_CACHE1_DMA_STATE_ERROR_NONE);

	dma = (uint8_t*)nv_dma_map(state->dma_instance, &dma_len);

	printf("DMA pusher: max 0x%08X, 0x%08X - 0x%08X\n",
		dma_len, control->dma_get, control->dma_put);

	/* based on the convenient pseudocode in envytools */
	while (control->dma_get != control->dma_put) {
		if (control->dma_get >= dma_len) {
			state->error = NV_PFIFO_CACHE1_DMA_STATE_ERROR_PROTECTION;
			break;
		}

		word = ldl_le_p((uint32_t*)(dma + control->dma_get));
		control->dma_get += 4;

		if (state->method_count) {
			/* data word of methods command */
			state->data_shadow = word;

			CacheEntry* command = (CacheEntry*)calloc(1, sizeof(CacheEntry));
			command->method = state->method;
			command->subchannel = state->subchannel;
			command->nonincreasing = state->method_nonincreasing;
			command->parameter = word;

			std::lock_guard<std::mutex> lk(state->mutex);
			state->cache.push(command);
			state->cache_cond.notify_all();

			if (!state->method_nonincreasing) {
				state->method += 4;
			}

			state->method_count--;
			state->dcount++;
		} else {
			/* no command active - this is the first word of a new one */
			state->rsvd_shadow = word;
			/* match all forms */
			if ((word & 0xe0000003) == 0x20000000) {
				/* old jump */
				state->get_jmp_shadow = control->dma_get;
				control->dma_get = word & 0x1fffffff;
				printf("pb OLD_JMP 0x%08X\n", control->dma_get);
			}
			else if ((word & 3) == 1) {
				/* jump */
				state->get_jmp_shadow = control->dma_get;
				control->dma_get = word & 0xfffffffc;
				printf("pb JMP 0x%08X\n", control->dma_get);
			}
			else if ((word & 3) == 2) {
				/* call */
				if (state->subroutine_active) {
					state->error = NV_PFIFO_CACHE1_DMA_STATE_ERROR_CALL;
					break;
				}
				state->subroutine_return = control->dma_get;
				state->subroutine_active = true;
				control->dma_get = word & 0xfffffffc;
				printf("pb CALL 0x%08X\n", control->dma_get);
			}
			else if (word == 0x00020000) {
				/* return */
				if (!state->subroutine_active) {
					state->error = NV_PFIFO_CACHE1_DMA_STATE_ERROR_RETURN;
					break;
				}
				control->dma_get = state->subroutine_return;
				state->subroutine_active = false;
				printf("pb RET 0x%08X\n", control->dma_get);
			}
			else if ((word & 0xe0030003) == 0) {
				/* increasing methods */
				state->method = word & 0x1fff;
				state->subchannel = (word >> 13) & 7;
				state->method_count = (word >> 18) & 0x7ff;
				state->method_nonincreasing = false;
				state->dcount = 0;
			}
			else if ((word & 0xe0030003) == 0x40000000) {
				/* non-increasing methods */
				state->method = word & 0x1fff;
				state->subchannel = (word >> 13) & 7;
				state->method_count = (word >> 18) & 0x7ff;
				state->method_nonincreasing = true;
				state->dcount = 0;
			}
			else {
				printf("pb reserved cmd 0x%08X - 0x%08X\n",
					control->dma_get, word);
				state->error = NV_PFIFO_CACHE1_DMA_STATE_ERROR_RESERVED_CMD;
				break;
			}
		}
	}

	printf("DMA pusher done: max 0x%08X, 0x%08X - 0x%08X\n",
		dma_len, control->dma_get, control->dma_put);

	if (state->error) {
		printf("pb error: %d\n", state->error);
		assert(false);

		state->dma_push_suspended = true;

		pfifo.pending_interrupts |= NV_PFIFO_INTR_0_DMA_PUSHER;
		update_irq();
	}
}

static uint32_t ramht_hash(uint32_t handle)
{
	unsigned int ramht_size = 1 << (GET_MASK(pfifo.regs[NV_PFIFO_RAMHT], NV_PFIFO_RAMHT_SIZE) + 12);

	/* XXX: Think this is different to what nouveau calculates... */
	unsigned int bits = ffs(ramht_size) - 2;

	uint32_t hash = 0;
	while (handle) {
		hash ^= (handle & ((1 << bits) - 1));
		handle >>= bits;
	}
	hash ^= pfifo.cache1.channel_id << (bits - 4);

	return hash;
}


static RAMHTEntry ramht_lookup(uint32_t handle)
{
	unsigned int ramht_size = 1 << (GET_MASK(pfifo.regs[NV_PFIFO_RAMHT], NV_PFIFO_RAMHT_SIZE) + 12);

	uint32_t hash = ramht_hash(handle);
	assert(hash * 8 < ramht_size);

	uint32_t ramht_address =
		GET_MASK(pfifo.regs[NV_PFIFO_RAMHT],
			NV_PFIFO_RAMHT_BASE_ADDRESS) << 12;

	uint8_t *entry_ptr = (uint8_t*)(NV2A_ADDR + NV_PRAMIN_ADDR + ramht_address + hash * 8);

	uint32_t entry_handle = ldl_le_p((uint32_t*)entry_ptr);
	uint32_t entry_context = ldl_le_p((uint32_t*)(entry_ptr + 4));

	RAMHTEntry entry;
	entry.handle = entry_handle;
	entry.instance = (entry_context & NV_RAMHT_INSTANCE) << 4;
	entry.engine = (FIFOEngine)((entry_context & NV_RAMHT_ENGINE) >> 16);
	entry.channel_id = (entry_context & NV_RAMHT_CHID) >> 24;
	entry.valid = entry_context & NV_RAMHT_STATUS;

	return entry;
}

static void pgraph_context_switch(unsigned int channel_id)
{
	bool valid = false;

	// Scope the lock so that it gets unlocked at end of this block
	{
		std::lock_guard<std::mutex> lk(pgraph.mutex);

		valid = pgraph.channel_valid && pgraph.channel_id == channel_id;
		if (!valid) {
			pgraph.trapped_channel_id = channel_id;
		}
	}

	if (!valid) {
		printf("puller needs to switch to ch %d\n", channel_id);

		//qemu_mutex_lock_iothread();
		pgraph.pending_interrupts |= NV_PGRAPH_INTR_CONTEXT_SWITCH;
		update_irq();

		std::unique_lock<std::mutex> lk(pgraph.mutex);
		//qemu_mutex_unlock_iothread();

		while (pgraph.pending_interrupts & NV_PGRAPH_INTR_CONTEXT_SWITCH) {
			pgraph.interrupt_cond.wait(lk);
		}
	}
}

static void pgraph_wait_fifo_access() {
	std::unique_lock<std::mutex> lk(pgraph.mutex);

	while (!pgraph.fifo_access) {	
		pgraph.fifo_access_cond.wait(lk);
	}
}

static void pgraph_method_log(unsigned int subchannel,	unsigned int graphics_class, unsigned int method, uint32_t parameter) {
	static unsigned int last = 0;
	static unsigned int count = 0;

	if (last == 0x1800 && method != last) {
		printf("pgraph method (%d) 0x%08X * %d", subchannel, last, count);
	}
	if (method != 0x1800) {
		const char* method_name = NULL;
		unsigned int nmethod = 0;
		switch (graphics_class) {
		case NV_KELVIN_PRIMITIVE:
			nmethod = method | (0x5c << 16);
			break;
		case NV_CONTEXT_SURFACES_2D:
			nmethod = method | (0x6d << 16);
			break;
		default:
			break;
		}
		/*
		if (nmethod != 0 && nmethod < ARRAY_SIZE(nv2a_method_names)) {
			method_name = nv2a_method_names[nmethod];
		}
		if (method_name) {
			printf("pgraph method (%d): %s (0x%x)\n",
				subchannel, method_name, parameter);
		}
		else {
		*/
			printf("pgraph method (%d): 0x%x -> 0x%04x (0x%x)\n",
				subchannel, graphics_class, method, parameter);
		//}

	}
	if (method == last) { count++; }
	else { count = 0; }
	last = method;
}

static void load_graphics_object(xbaddr instance_address, GraphicsObject *obj)
{
	uint8_t *obj_ptr;
	uint32_t switch1, switch2, switch3;
	
	assert(instance_address < NV_PRAMIN_SIZE);
	obj_ptr = (uint8_t*)(NV2A_ADDR + NV_PRAMIN_ADDR  + instance_address);

	switch1 = ldl_le_p((uint32_t*)obj_ptr);
	switch2 = ldl_le_p((uint32_t*)(obj_ptr + 4));
	switch3 = ldl_le_p((uint32_t*)(obj_ptr + 8));

	obj->graphics_class = switch1 & NV_PGRAPH_CTX_SWITCH1_GRCLASS;

	/* init graphics object */
	switch (obj->graphics_class) {
	case NV_KELVIN_PRIMITIVE:
		// kelvin->vertex_attributes[NV2A_VERTEX_ATTR_DIFFUSE].inline_value = 0xFFFFFFF;
		break;
	default:
		break;
	}
}

static GraphicsObject* lookup_graphics_object(xbaddr instance_address)
{
	int i;
	for (i = 0; i<NV2A_NUM_SUBCHANNELS; i++) {
		if (pgraph.subchannel_data[i].object_instance == instance_address) {
			return &pgraph.subchannel_data[i].object;
		}
	}
	return NULL;
}

static bool pgraph_color_write_enabled()
{
	return pgraph.regs[NV_PGRAPH_CONTROL_0] & (
		NV_PGRAPH_CONTROL_0_ALPHA_WRITE_ENABLE
		| NV_PGRAPH_CONTROL_0_RED_WRITE_ENABLE
		| NV_PGRAPH_CONTROL_0_GREEN_WRITE_ENABLE
		| NV_PGRAPH_CONTROL_0_BLUE_WRITE_ENABLE);
}

static bool pgraph_zeta_write_enabled()
{
	return pgraph.regs[NV_PGRAPH_CONTROL_0] & (
		NV_PGRAPH_CONTROL_0_ZWRITEENABLE
		| NV_PGRAPH_CONTROL_0_STENCIL_WRITE_ENABLE);
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

static void pgraph_method(unsigned int subchannel, unsigned int method, uint32_t parameter)
{
	std::lock_guard<std::mutex> lk(pgraph.mutex);

	int i;
	GraphicsSubchannel *subchannel_data;
	GraphicsObject *object;

	unsigned int slot;

	assert(pgraph.channel_valid);
	subchannel_data = &pgraph.subchannel_data[subchannel];
	object = &subchannel_data->object;

	ContextSurfaces2DState *context_surfaces_2d = &object->data.context_surfaces_2d;
	ImageBlitState *image_blit = &object->data.image_blit;
	KelvinState *kelvin = &object->data.kelvin;

	pgraph_method_log(subchannel, object->graphics_class, method, parameter);

	if (method == NV_SET_OBJECT) {
		subchannel_data->object_instance = parameter;

		//qemu_mutex_lock_iothread();
		load_graphics_object(parameter, object);
		//qemu_mutex_unlock_iothread();
		return;
	}

	switch (object->graphics_class) {
	case NV_CONTEXT_SURFACES_2D: {
		switch (method) {
		case NV062_SET_CONTEXT_DMA_IMAGE_SOURCE:
			context_surfaces_2d->dma_image_source = parameter;
			break;
		case NV062_SET_CONTEXT_DMA_IMAGE_DESTIN:
			context_surfaces_2d->dma_image_dest = parameter;
			break;
		case NV062_SET_COLOR_FORMAT:
			context_surfaces_2d->color_format = parameter;
			break;
		case NV062_SET_PITCH:
			context_surfaces_2d->source_pitch = parameter & 0xFFFF;
			context_surfaces_2d->dest_pitch = parameter >> 16;
			break;
		case NV062_SET_OFFSET_SOURCE:
			context_surfaces_2d->source_offset = parameter & 0x07FFFFFF;
			break;
		case NV062_SET_OFFSET_DESTIN:
			context_surfaces_2d->dest_offset = parameter & 0x07FFFFFF;
			break;
		default:
			EmuWarning("EmuNV2A: Unknown NV_CONTEXT_SURFACES_2D Method: 0x%08X\n", method);
		}
	
		break; 
	}
	
	case NV_IMAGE_BLIT: {
		switch (method) {
		case NV09F_SET_CONTEXT_SURFACES:
			image_blit->context_surfaces = parameter;
			break;
		case NV09F_SET_OPERATION:
			image_blit->operation = parameter;
			break;
		case NV09F_CONTROL_POINT_IN:
			image_blit->in_x = parameter & 0xFFFF;
			image_blit->in_y = parameter >> 16;
			break;
		case NV09F_CONTROL_POINT_OUT:
			image_blit->out_x = parameter & 0xFFFF;
			image_blit->out_y = parameter >> 16;
			break;
		case NV09F_SIZE:
			image_blit->width = parameter & 0xFFFF;
			image_blit->height = parameter >> 16;

			/* I guess this kicks it off? */
			if (image_blit->operation == NV09F_SET_OPERATION_SRCCOPY) {

				printf("NV09F_SET_OPERATION_SRCCOPY");

				GraphicsObject *context_surfaces_obj = lookup_graphics_object(image_blit->context_surfaces);
				assert(context_surfaces_obj);
				assert(context_surfaces_obj->graphics_class == NV_CONTEXT_SURFACES_2D);

				ContextSurfaces2DState *context_surfaces =	&context_surfaces_obj->data.context_surfaces_2d;

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

				xbaddr source_dma_len, dest_dma_len;
				uint8_t *source, *dest;

				source = (uint8_t*)nv_dma_map(context_surfaces->dma_image_source,	&source_dma_len);
				assert(context_surfaces->source_offset < source_dma_len);
				source += context_surfaces->source_offset;

				dest = (uint8_t*)nv_dma_map(context_surfaces->dma_image_dest,	&dest_dma_len);
				assert(context_surfaces->dest_offset < dest_dma_len);
				dest += context_surfaces->dest_offset;

				printf("  - 0x%tx -> 0x%tx\n", source - MM_SYSTEM_PHYSICAL_MAP,dest - MM_SYSTEM_PHYSICAL_MAP);

				int y;
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
			}
			else {
				assert(false);
			}

			break;
		default:
			EmuWarning("EmuNV2A: Unknown NV_IMAGE_BLIT Method: 0x%08X\n", method);
		}
		break;
	}

	case NV_KELVIN_PRIMITIVE: {
		switch (method) {
		case NV097_SET_CONTEXT_DMA_NOTIFIES:
			kelvin->dma_notifies = parameter;
			break;
		case NV097_SET_CONTEXT_DMA_A:
			pgraph.dma_a = parameter;
			break;
		case NV097_SET_CONTEXT_DMA_B:
			pgraph.dma_b = parameter;
			break;
		case NV097_SET_CONTEXT_DMA_STATE:
			kelvin->dma_state = parameter;
			break;
		case NV097_SET_CONTEXT_DMA_COLOR:
			printf("TODO: pgraph_update_surface\n");
			/* try to get any straggling draws in before the surface's changed :/ */
			//pgraph_update_surface(d, false, true, true);

			pgraph.dma_color = parameter;
			break;
		case NV097_SET_CONTEXT_DMA_ZETA:
			pgraph.dma_zeta = parameter;
			break;
		case NV097_SET_CONTEXT_DMA_VERTEX_A:
			pgraph.dma_vertex_a = parameter;
			break;
		case NV097_SET_CONTEXT_DMA_VERTEX_B:
			pgraph.dma_vertex_b = parameter;
			break;
		case NV097_SET_CONTEXT_DMA_SEMAPHORE:
			kelvin->dma_semaphore = parameter;
			break;
		case NV097_SET_CONTEXT_DMA_REPORT:
			pgraph.dma_report = parameter;
			break;
		case NV097_SET_SURFACE_CLIP_HORIZONTAL:
			printf("TODO: pgraph_update_surface\n");
			//pgraph_update_surface(d, false, true, true);

			pgraph.surface_shape.clip_x =
				GET_MASK(parameter, NV097_SET_SURFACE_CLIP_HORIZONTAL_X);
			pgraph.surface_shape.clip_width =
				GET_MASK(parameter, NV097_SET_SURFACE_CLIP_HORIZONTAL_WIDTH);
			break;
		case NV097_SET_SURFACE_CLIP_VERTICAL:
			printf("TODO: pgraph_update_surface\n");
			//pgraph_update_surface(d, false, true, true);

			pgraph.surface_shape.clip_y =
				GET_MASK(parameter, NV097_SET_SURFACE_CLIP_VERTICAL_Y);
			pgraph.surface_shape.clip_height =
				GET_MASK(parameter, NV097_SET_SURFACE_CLIP_VERTICAL_HEIGHT);
			break;
		case NV097_SET_SURFACE_FORMAT:
			printf("TODO: pgraph_update_surface\n");
			//pgraph_update_surface(d, false, true, true);

			pgraph.surface_shape.color_format =
				GET_MASK(parameter, NV097_SET_SURFACE_FORMAT_COLOR);
			pgraph.surface_shape.zeta_format =
				GET_MASK(parameter, NV097_SET_SURFACE_FORMAT_ZETA);
			pgraph.surface_type =
				GET_MASK(parameter, NV097_SET_SURFACE_FORMAT_TYPE);
			pgraph.surface_shape.anti_aliasing =
				GET_MASK(parameter, NV097_SET_SURFACE_FORMAT_ANTI_ALIASING);
			pgraph.surface_shape.log_width =
				GET_MASK(parameter, NV097_SET_SURFACE_FORMAT_WIDTH);
			pgraph.surface_shape.log_height =
				GET_MASK(parameter, NV097_SET_SURFACE_FORMAT_HEIGHT);
			break;
		case NV097_SET_SURFACE_PITCH:
			printf("TODO: pgraph_update_surface\n");
			//pgraph_update_surface(d, false, true, true);

			pgraph.surface_color.pitch =
				GET_MASK(parameter, NV097_SET_SURFACE_PITCH_COLOR);
			pgraph.surface_zeta.pitch =
				GET_MASK(parameter, NV097_SET_SURFACE_PITCH_ZETA);
			break;
		case NV097_SET_SURFACE_COLOR_OFFSET:
			printf("TODO: pgraph_update_surface\n");
			//pgraph_update_surface(d, false, true, true);

			pgraph.surface_color.offset = parameter;
			break;
		case NV097_SET_SURFACE_ZETA_OFFSET:
			printf("TODO: pgraph_update_surface\n");
			//pgraph_update_surface(d, false, true, true);

			pgraph.surface_zeta.offset = parameter;
			break;
		// TODO: Is there a better way to do this?
		// MSVC doesn't support GCC's case_range extention
		// this prevents use of NV097_SET_COMBINER_ALPHA_ICW ... NV097_SET_COMBINER_ALPHA_ICW + 28
		// Investigated making a macro, but doesn't seem possible to do a variable number of results
		case NV097_SET_COMBINER_ALPHA_ICW: case NV097_SET_COMBINER_ALPHA_ICW + 1: case NV097_SET_COMBINER_ALPHA_ICW + 2:
		case NV097_SET_COMBINER_ALPHA_ICW + 3:	case NV097_SET_COMBINER_ALPHA_ICW + 4:	case NV097_SET_COMBINER_ALPHA_ICW + 5:
		case NV097_SET_COMBINER_ALPHA_ICW + 6:	case NV097_SET_COMBINER_ALPHA_ICW + 7:	case NV097_SET_COMBINER_ALPHA_ICW + 8:
		case NV097_SET_COMBINER_ALPHA_ICW + 9:	case NV097_SET_COMBINER_ALPHA_ICW + 10:	case NV097_SET_COMBINER_ALPHA_ICW + 11:
		case NV097_SET_COMBINER_ALPHA_ICW + 12:	case NV097_SET_COMBINER_ALPHA_ICW + 13:	case NV097_SET_COMBINER_ALPHA_ICW + 14:
		case NV097_SET_COMBINER_ALPHA_ICW + 15:	case NV097_SET_COMBINER_ALPHA_ICW + 16:	case NV097_SET_COMBINER_ALPHA_ICW + 17:
		case NV097_SET_COMBINER_ALPHA_ICW + 18:	case NV097_SET_COMBINER_ALPHA_ICW + 19:	case NV097_SET_COMBINER_ALPHA_ICW + 20:
		case NV097_SET_COMBINER_ALPHA_ICW + 21:	case NV097_SET_COMBINER_ALPHA_ICW + 22:	case NV097_SET_COMBINER_ALPHA_ICW + 23:
		case NV097_SET_COMBINER_ALPHA_ICW + 24:	case NV097_SET_COMBINER_ALPHA_ICW + 25:	case NV097_SET_COMBINER_ALPHA_ICW + 26:
		case NV097_SET_COMBINER_ALPHA_ICW + 27:	case NV097_SET_COMBINER_ALPHA_ICW + 28:
				slot = (method - NV097_SET_COMBINER_ALPHA_ICW) / 4;
				pgraph.regs[NV_PGRAPH_COMBINEALPHAI0 + slot * 4] = parameter;
				break;
		case NV097_SET_COMBINER_SPECULAR_FOG_CW0:
			pgraph.regs[NV_PGRAPH_COMBINESPECFOG0] = parameter;
			break;
		case NV097_SET_COMBINER_SPECULAR_FOG_CW1:
			pgraph.regs[NV_PGRAPH_COMBINESPECFOG1] = parameter;
			break;
		CASE_4(NV097_SET_TEXTURE_ADDRESS, 64):
			slot = (method - NV097_SET_TEXTURE_ADDRESS) / 64;
			pgraph.regs[NV_PGRAPH_TEXADDRESS0 + slot * 4] = parameter;
			break;
		case NV097_SET_CONTROL0: {
			printf("TODO: pgraph_update_surface\n");
			//pgraph_update_surface(d, false, true, true);

			bool stencil_write_enable =
				parameter & NV097_SET_CONTROL0_STENCIL_WRITE_ENABLE;
			SET_MASK(pgraph.regs[NV_PGRAPH_CONTROL_0],
				NV_PGRAPH_CONTROL_0_STENCIL_WRITE_ENABLE,
				stencil_write_enable);

			uint32_t z_format = GET_MASK(parameter, NV097_SET_CONTROL0_Z_FORMAT);
			SET_MASK(pgraph.regs[NV_PGRAPH_SETUPRASTER],
				NV_PGRAPH_SETUPRASTER_Z_FORMAT, z_format);

			bool z_perspective =
				parameter & NV097_SET_CONTROL0_Z_PERSPECTIVE_ENABLE;
			SET_MASK(pgraph.regs[NV_PGRAPH_CONTROL_0],
				NV_PGRAPH_CONTROL_0_Z_PERSPECTIVE_ENABLE,
				z_perspective);
			break;
		}

		case NV097_SET_FOG_MODE: {
			/* FIXME: There is also NV_PGRAPH_CSV0_D_FOG_MODE */
			unsigned int mode;
			switch (parameter) {
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
				break;
			}
			SET_MASK(pgraph.regs[NV_PGRAPH_CONTROL_3], NV_PGRAPH_CONTROL_3_FOG_MODE,
				mode);
			break;
		}
		case NV097_SET_FOG_GEN_MODE: {
			unsigned int mode;
			switch (parameter) {
			case NV097_SET_FOG_GEN_MODE_V_SPEC_ALPHA:
				mode = NV_PGRAPH_CSV0_D_FOGGENMODE_SPEC_ALPHA; break;
			case NV097_SET_FOG_GEN_MODE_V_RADIAL:
				mode = NV_PGRAPH_CSV0_D_FOGGENMODE_RADIAL; break;
			case NV097_SET_FOG_GEN_MODE_V_PLANAR:
				mode = NV_PGRAPH_CSV0_D_FOGGENMODE_PLANAR; break;
			case NV097_SET_FOG_GEN_MODE_V_ABS_PLANAR:
				mode = NV_PGRAPH_CSV0_D_FOGGENMODE_ABS_PLANAR; break;
			case NV097_SET_FOG_GEN_MODE_V_FOG_X:
				mode = NV_PGRAPH_CSV0_D_FOGGENMODE_FOG_X; break;
			default:
				assert(false);
				break;
			}
			SET_MASK(pgraph.regs[NV_PGRAPH_CSV0_D], NV_PGRAPH_CSV0_D_FOGGENMODE, mode);
			break;
		}
		case NV097_SET_FOG_ENABLE:
			/*
			FIXME: There is also:
			SET_MASK(pgraph.regs[NV_PGRAPH_CSV0_D], NV_PGRAPH_CSV0_D_FOGENABLE,
			parameter);
			*/
			SET_MASK(pgraph.regs[NV_PGRAPH_CONTROL_3], NV_PGRAPH_CONTROL_3_FOGENABLE,
				parameter);
			break;
		case NV097_SET_FOG_COLOR: {
			/* PGRAPH channels are ARGB, parameter channels are ABGR */
			uint8_t red = GET_MASK(parameter, NV097_SET_FOG_COLOR_RED);
			uint8_t green = GET_MASK(parameter, NV097_SET_FOG_COLOR_GREEN);
			uint8_t blue = GET_MASK(parameter, NV097_SET_FOG_COLOR_BLUE);
			uint8_t alpha = GET_MASK(parameter, NV097_SET_FOG_COLOR_ALPHA);
			SET_MASK(pgraph.regs[NV_PGRAPH_FOGCOLOR], NV_PGRAPH_FOGCOLOR_RED, red);
			SET_MASK(pgraph.regs[NV_PGRAPH_FOGCOLOR], NV_PGRAPH_FOGCOLOR_GREEN, green);
			SET_MASK(pgraph.regs[NV_PGRAPH_FOGCOLOR], NV_PGRAPH_FOGCOLOR_BLUE, blue);
			SET_MASK(pgraph.regs[NV_PGRAPH_FOGCOLOR], NV_PGRAPH_FOGCOLOR_ALPHA, alpha);
			break;
		}
		case NV097_SET_ALPHA_TEST_ENABLE:
			SET_MASK(pgraph.regs[NV_PGRAPH_CONTROL_0],
				NV_PGRAPH_CONTROL_0_ALPHATESTENABLE, parameter);
			break;
		case NV097_SET_BLEND_ENABLE:
			SET_MASK(pgraph.regs[NV_PGRAPH_BLEND], NV_PGRAPH_BLEND_EN, parameter);
			break;
		case NV097_SET_CULL_FACE_ENABLE:
			SET_MASK(pgraph.regs[NV_PGRAPH_SETUPRASTER],
				NV_PGRAPH_SETUPRASTER_CULLENABLE,
				parameter);
			break;
		case NV097_SET_DEPTH_TEST_ENABLE:
			SET_MASK(pgraph.regs[NV_PGRAPH_CONTROL_0], NV_PGRAPH_CONTROL_0_ZENABLE,
				parameter);
			break;
		case NV097_SET_DITHER_ENABLE:
			SET_MASK(pgraph.regs[NV_PGRAPH_CONTROL_0],
				NV_PGRAPH_CONTROL_0_DITHERENABLE, parameter);
			break;
		case NV097_SET_LIGHTING_ENABLE:
			SET_MASK(pgraph.regs[NV_PGRAPH_CSV0_C], NV_PGRAPH_CSV0_C_LIGHTING,
				parameter);
			break;
		case NV097_SET_SKIN_MODE:
			SET_MASK(pgraph.regs[NV_PGRAPH_CSV0_D], NV_PGRAPH_CSV0_D_SKIN,
				parameter);
			break;
		case NV097_SET_STENCIL_TEST_ENABLE:
			SET_MASK(pgraph.regs[NV_PGRAPH_CONTROL_1],
				NV_PGRAPH_CONTROL_1_STENCIL_TEST_ENABLE, parameter);
			break;
		case NV097_SET_POLY_OFFSET_POINT_ENABLE:
			SET_MASK(pgraph.regs[NV_PGRAPH_SETUPRASTER],
				NV_PGRAPH_SETUPRASTER_POFFSETPOINTENABLE, parameter);
			break;
		case NV097_SET_POLY_OFFSET_LINE_ENABLE:
			SET_MASK(pgraph.regs[NV_PGRAPH_SETUPRASTER],
				NV_PGRAPH_SETUPRASTER_POFFSETLINEENABLE, parameter);
			break;
		case NV097_SET_POLY_OFFSET_FILL_ENABLE:
			SET_MASK(pgraph.regs[NV_PGRAPH_SETUPRASTER],
				NV_PGRAPH_SETUPRASTER_POFFSETFILLENABLE, parameter);
			break;
		case NV097_SET_ALPHA_FUNC:
			SET_MASK(pgraph.regs[NV_PGRAPH_CONTROL_0],
				NV_PGRAPH_CONTROL_0_ALPHAFUNC, parameter & 0xF);
			break;
		case NV097_SET_ALPHA_REF:
			SET_MASK(pgraph.regs[NV_PGRAPH_CONTROL_0],
				NV_PGRAPH_CONTROL_0_ALPHAREF, parameter);
			break;
		case NV097_SET_BLEND_FUNC_SFACTOR: {
			unsigned int factor;
			switch (parameter) {
			case NV097_SET_BLEND_FUNC_SFACTOR_V_ZERO:
				factor = NV_PGRAPH_BLEND_SFACTOR_ZERO; break;
			case NV097_SET_BLEND_FUNC_SFACTOR_V_ONE:
				factor = NV_PGRAPH_BLEND_SFACTOR_ONE; break;
			case NV097_SET_BLEND_FUNC_SFACTOR_V_SRC_COLOR:
				factor = NV_PGRAPH_BLEND_SFACTOR_SRC_COLOR; break;
			case NV097_SET_BLEND_FUNC_SFACTOR_V_ONE_MINUS_SRC_COLOR:
				factor = NV_PGRAPH_BLEND_SFACTOR_ONE_MINUS_SRC_COLOR; break;
			case NV097_SET_BLEND_FUNC_SFACTOR_V_SRC_ALPHA:
				factor = NV_PGRAPH_BLEND_SFACTOR_SRC_ALPHA; break;
			case NV097_SET_BLEND_FUNC_SFACTOR_V_ONE_MINUS_SRC_ALPHA:
				factor = NV_PGRAPH_BLEND_SFACTOR_ONE_MINUS_SRC_ALPHA; break;
			case NV097_SET_BLEND_FUNC_SFACTOR_V_DST_ALPHA:
				factor = NV_PGRAPH_BLEND_SFACTOR_DST_ALPHA; break;
			case NV097_SET_BLEND_FUNC_SFACTOR_V_ONE_MINUS_DST_ALPHA:
				factor = NV_PGRAPH_BLEND_SFACTOR_ONE_MINUS_DST_ALPHA; break;
			case NV097_SET_BLEND_FUNC_SFACTOR_V_DST_COLOR:
				factor = NV_PGRAPH_BLEND_SFACTOR_DST_COLOR; break;
			case NV097_SET_BLEND_FUNC_SFACTOR_V_ONE_MINUS_DST_COLOR:
				factor = NV_PGRAPH_BLEND_SFACTOR_ONE_MINUS_DST_COLOR; break;
			case NV097_SET_BLEND_FUNC_SFACTOR_V_SRC_ALPHA_SATURATE:
				factor = NV_PGRAPH_BLEND_SFACTOR_SRC_ALPHA_SATURATE; break;
			case NV097_SET_BLEND_FUNC_SFACTOR_V_CONSTANT_COLOR:
				factor = NV_PGRAPH_BLEND_SFACTOR_CONSTANT_COLOR; break;
			case NV097_SET_BLEND_FUNC_SFACTOR_V_ONE_MINUS_CONSTANT_COLOR:
				factor = NV_PGRAPH_BLEND_SFACTOR_ONE_MINUS_CONSTANT_COLOR; break;
			case NV097_SET_BLEND_FUNC_SFACTOR_V_CONSTANT_ALPHA:
				factor = NV_PGRAPH_BLEND_SFACTOR_CONSTANT_ALPHA; break;
			case NV097_SET_BLEND_FUNC_SFACTOR_V_ONE_MINUS_CONSTANT_ALPHA:
				factor = NV_PGRAPH_BLEND_SFACTOR_ONE_MINUS_CONSTANT_ALPHA; break;
			default:
				fprintf(stderr, "Unknown blend source factor: 0x%x\n", parameter);
				assert(false);
				break;
			}
			SET_MASK(pgraph.regs[NV_PGRAPH_BLEND], NV_PGRAPH_BLEND_SFACTOR, factor);

			break;
		}

		case NV097_SET_BLEND_FUNC_DFACTOR: {
			unsigned int factor;
			switch (parameter) {
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
				fprintf(stderr, "Unknown blend destination factor: 0x%x\n", parameter);
				assert(false);
				break;
			}
			SET_MASK(pgraph.regs[NV_PGRAPH_BLEND], NV_PGRAPH_BLEND_DFACTOR, factor);

			break;
		}

		case NV097_SET_BLEND_COLOR:
			pgraph.regs[NV_PGRAPH_BLENDCOLOR] = parameter;
			break;

		case NV097_SET_BLEND_EQUATION: {
			unsigned int equation;
			switch (parameter) {
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
				break;
			}
			SET_MASK(pgraph.regs[NV_PGRAPH_BLEND], NV_PGRAPH_BLEND_EQN, equation);

			break;
		}

		case NV097_SET_DEPTH_FUNC:
			SET_MASK(pgraph.regs[NV_PGRAPH_CONTROL_0], NV_PGRAPH_CONTROL_0_ZFUNC,
				parameter & 0xF);
			break;

		case NV097_SET_COLOR_MASK: {
			pgraph.surface_color.write_enabled_cache |= pgraph_color_write_enabled();

			bool alpha = parameter & NV097_SET_COLOR_MASK_ALPHA_WRITE_ENABLE;
			bool red = parameter & NV097_SET_COLOR_MASK_RED_WRITE_ENABLE;
			bool green = parameter & NV097_SET_COLOR_MASK_GREEN_WRITE_ENABLE;
			bool blue = parameter & NV097_SET_COLOR_MASK_BLUE_WRITE_ENABLE;
			SET_MASK(pgraph.regs[NV_PGRAPH_CONTROL_0],
				NV_PGRAPH_CONTROL_0_ALPHA_WRITE_ENABLE, alpha);
			SET_MASK(pgraph.regs[NV_PGRAPH_CONTROL_0],
				NV_PGRAPH_CONTROL_0_RED_WRITE_ENABLE, red);
			SET_MASK(pgraph.regs[NV_PGRAPH_CONTROL_0],
				NV_PGRAPH_CONTROL_0_GREEN_WRITE_ENABLE, green);
			SET_MASK(pgraph.regs[NV_PGRAPH_CONTROL_0],
				NV_PGRAPH_CONTROL_0_BLUE_WRITE_ENABLE, blue);
			break;
		}
		case NV097_SET_DEPTH_MASK:
			pgraph.surface_zeta.write_enabled_cache |= pgraph_zeta_write_enabled();

			SET_MASK(pgraph.regs[NV_PGRAPH_CONTROL_0],
				NV_PGRAPH_CONTROL_0_ZWRITEENABLE, parameter);
			break;
		case NV097_SET_STENCIL_MASK:
			SET_MASK(pgraph.regs[NV_PGRAPH_CONTROL_1],
				NV_PGRAPH_CONTROL_1_STENCIL_MASK_WRITE, parameter);
			break;
		case NV097_SET_STENCIL_FUNC:
			SET_MASK(pgraph.regs[NV_PGRAPH_CONTROL_1],
				NV_PGRAPH_CONTROL_1_STENCIL_FUNC, parameter & 0xF);
			break;
		case NV097_SET_STENCIL_FUNC_REF:
			SET_MASK(pgraph.regs[NV_PGRAPH_CONTROL_1],
				NV_PGRAPH_CONTROL_1_STENCIL_REF, parameter);
			break;
		case NV097_SET_STENCIL_FUNC_MASK:
			SET_MASK(pgraph.regs[NV_PGRAPH_CONTROL_1],
				NV_PGRAPH_CONTROL_1_STENCIL_MASK_READ, parameter);
			break;
		case NV097_SET_STENCIL_OP_FAIL:
			SET_MASK(pgraph.regs[NV_PGRAPH_CONTROL_2],
				NV_PGRAPH_CONTROL_2_STENCIL_OP_FAIL,
				kelvin_map_stencil_op(parameter));
			break;
		case NV097_SET_STENCIL_OP_ZFAIL:
			SET_MASK(pgraph.regs[NV_PGRAPH_CONTROL_2],
				NV_PGRAPH_CONTROL_2_STENCIL_OP_ZFAIL,
				kelvin_map_stencil_op(parameter));
			break;
		case NV097_SET_STENCIL_OP_ZPASS:
			SET_MASK(pgraph.regs[NV_PGRAPH_CONTROL_2],
				NV_PGRAPH_CONTROL_2_STENCIL_OP_ZPASS,
				kelvin_map_stencil_op(parameter));
			break;

		case NV097_SET_POLYGON_OFFSET_SCALE_FACTOR:
			pgraph.regs[NV_PGRAPH_ZOFFSETFACTOR] = parameter;
			break;
		case NV097_SET_POLYGON_OFFSET_BIAS:
			pgraph.regs[NV_PGRAPH_ZOFFSETBIAS] = parameter;
			break;
		case NV097_SET_FRONT_POLYGON_MODE:
			SET_MASK(pgraph.regs[NV_PGRAPH_SETUPRASTER],
				NV_PGRAPH_SETUPRASTER_FRONTFACEMODE,
				kelvin_map_polygon_mode(parameter));
			break;
		case NV097_SET_BACK_POLYGON_MODE:
			SET_MASK(pgraph.regs[NV_PGRAPH_SETUPRASTER],
				NV_PGRAPH_SETUPRASTER_BACKFACEMODE,
				kelvin_map_polygon_mode(parameter));
			break;
		case NV097_SET_CLIP_MIN:
			pgraph.regs[NV_PGRAPH_ZCLIPMIN] = parameter;
			break;
		case NV097_SET_CLIP_MAX:
			pgraph.regs[NV_PGRAPH_ZCLIPMAX] = parameter;
			break;
		case NV097_SET_CULL_FACE: {
			unsigned int face;
			switch (parameter) {
			case NV097_SET_CULL_FACE_V_FRONT:
				face = NV_PGRAPH_SETUPRASTER_CULLCTRL_FRONT; break;
			case NV097_SET_CULL_FACE_V_BACK:
				face = NV_PGRAPH_SETUPRASTER_CULLCTRL_BACK; break;
			case NV097_SET_CULL_FACE_V_FRONT_AND_BACK:
				face = NV_PGRAPH_SETUPRASTER_CULLCTRL_FRONT_AND_BACK; break;
			default:
				assert(false);
				break;
			}
			SET_MASK(pgraph.regs[NV_PGRAPH_SETUPRASTER],
				NV_PGRAPH_SETUPRASTER_CULLCTRL,
				face);
			break;
		}
		case NV097_SET_FRONT_FACE: {
			bool ccw;
			switch (parameter) {
			case NV097_SET_FRONT_FACE_V_CW:
				ccw = false; break;
			case NV097_SET_FRONT_FACE_V_CCW:
				ccw = true; break;
			default:
				fprintf(stderr, "Unknown front face: 0x%x\n", parameter);
				assert(false);
				break;
			}
			SET_MASK(pgraph.regs[NV_PGRAPH_SETUPRASTER],
				NV_PGRAPH_SETUPRASTER_FRONTFACE,
				ccw ? 1 : 0);
			break;
		}
		case NV097_SET_NORMALIZATION_ENABLE:
			SET_MASK(pgraph.regs[NV_PGRAPH_CSV0_C],
				NV_PGRAPH_CSV0_C_NORMALIZATION_ENABLE,
				parameter);
			break;

		case NV097_SET_LIGHT_ENABLE_MASK:
			SET_MASK(pgraph.regs[NV_PGRAPH_CSV0_D],
				NV_PGRAPH_CSV0_D_LIGHTS,
				parameter);
			break;

			CASE_4(NV097_SET_TEXGEN_S, 16) : {
				slot = (method - NV097_SET_TEXGEN_S) / 16;
				unsigned int reg = (slot < 2) ? NV_PGRAPH_CSV1_A
					: NV_PGRAPH_CSV1_B;
				unsigned int mask = (slot % 2) ? NV_PGRAPH_CSV1_A_T1_S
					: NV_PGRAPH_CSV1_A_T0_S;
				SET_MASK(pgraph.regs[reg], mask, kelvin_map_texgen(parameter, 0));
				break;
			}
			CASE_4(NV097_SET_TEXGEN_T, 16) : {
				slot = (method - NV097_SET_TEXGEN_T) / 16;
				unsigned int reg = (slot < 2) ? NV_PGRAPH_CSV1_A
					: NV_PGRAPH_CSV1_B;
				unsigned int mask = (slot % 2) ? NV_PGRAPH_CSV1_A_T1_T
					: NV_PGRAPH_CSV1_A_T0_T;
				SET_MASK(pgraph.regs[reg], mask, kelvin_map_texgen(parameter, 1));
				break;
			}
			CASE_4(NV097_SET_TEXGEN_R, 16) : {
				slot = (method - NV097_SET_TEXGEN_R) / 16;
				unsigned int reg = (slot < 2) ? NV_PGRAPH_CSV1_A
					: NV_PGRAPH_CSV1_B;
				unsigned int mask = (slot % 2) ? NV_PGRAPH_CSV1_A_T1_R
					: NV_PGRAPH_CSV1_A_T0_R;
				SET_MASK(pgraph.regs[reg], mask, kelvin_map_texgen(parameter, 2));
				break;
			}
			CASE_4(NV097_SET_TEXGEN_Q, 16) : {
				slot = (method - NV097_SET_TEXGEN_Q) / 16;
				unsigned int reg = (slot < 2) ? NV_PGRAPH_CSV1_A
					: NV_PGRAPH_CSV1_B;
				unsigned int mask = (slot % 2) ? NV_PGRAPH_CSV1_A_T1_Q
					: NV_PGRAPH_CSV1_A_T0_Q;
				SET_MASK(pgraph.regs[reg], mask, kelvin_map_texgen(parameter, 3));
				break;
			}
			CASE_4(NV097_SET_TEXTURE_MATRIX_ENABLE, 4) :
				slot = (method - NV097_SET_TEXTURE_MATRIX_ENABLE) / 4;
			pgraph.texture_matrix_enable[slot] = parameter;
			break;

		default:
			EmuWarning("EmuNV2A: Unknown NV_KELVIN_PRIMITIVE Method: 0x%08X\n", method);
		}
		break;
	}

	default:
		EmuWarning("EmuNV2A: Unknown Graphics Class/Method 0x%08X/0x%08X\n", object->graphics_class, method);
		break;
	}
}

static void* pfifo_puller_thread()
{
	Cache1State *state = &pfifo.cache1;

	while (true) {
		// Scope the lock so that it automatically unlocks at tne end of this block
		{
			std::unique_lock<std::mutex> lk(state->mutex);

			while (state->cache.empty() || !state->pull_enabled) {
				state->cache_cond.wait(lk);
			}

			// Copy cache to working_cache
			while (!state->cache.empty()) {
				state->working_cache.push(state->cache.front());
				state->cache.pop();
			}
		}

		while (!state->working_cache.empty()) {
			CacheEntry* command = state->working_cache.front();
			state->working_cache.pop();

			if (command->method == 0) {
				// qemu_mutex_lock_iothread();
				RAMHTEntry entry = ramht_lookup(command->parameter);
				assert(entry.valid);

				assert(entry.channel_id == state->channel_id);
				// qemu_mutex_unlock_iothread();

				switch (entry.engine) {
				case ENGINE_GRAPHICS:
					pgraph_context_switch(entry.channel_id);
					pgraph_wait_fifo_access();
					pgraph_method(command->subchannel, 0, entry.instance);
					break;
				default:
					assert(false);
					break;
				}

				/* the engine is bound to the subchannel */
				std::lock_guard<std::mutex> lk(pfifo.cache1.mutex);
				state->bound_engines[command->subchannel] = entry.engine;
				state->last_engine = entry.engine;
			} else if (command->method >= 0x100) {
				/* method passed to engine */

				uint32_t parameter = command->parameter;

				/* methods that take objects.
				* TODO: Check this range is correct for the nv2a */
				if (command->method >= 0x180 && command->method < 0x200) {
					//qemu_mutex_lock_iothread();
					RAMHTEntry entry = ramht_lookup(parameter);
					assert(entry.valid);
					assert(entry.channel_id == state->channel_id);
					parameter = entry.instance;
					//qemu_mutex_unlock_iothread();
				}

				// qemu_mutex_lock(&state->cache_lock);
				enum FIFOEngine engine = state->bound_engines[command->subchannel];
				// qemu_mutex_unlock(&state->cache_lock);

				switch (engine) {
				case ENGINE_GRAPHICS:
					pgraph_wait_fifo_access();
					pgraph_method(command->subchannel, command->method, parameter);
					break;
				default:
					assert(false);
					break;
				}

				// qemu_mutex_lock(&state->cache_lock);
				state->last_engine = state->bound_engines[command->subchannel];
				// qemu_mutex_unlock(&state->cache_lock);
			}

			free(command);
		}
	}

	return NULL;
}

DEVICE_READ32(PMC)
{
	DEVICE_READ32_SWITCH() {
	case NV_PMC_BOOT_0:	// chipset and stepping: NV2A, A02, Rev 0
		result = 0x02A000A2;
		break;
	case NV_PMC_INTR_0:
		result = pmc.pending_interrupts;
		break;
	case NV_PMC_INTR_EN_0:
		result = pmc.enabled_interrupts;
		break;
	default:
		result = 0;
		//DEVICE_READ32_REG(pmc); // Was : DEBUG_READ32_UNHANDLED(PMC);
		break;
	}

	DEVICE_READ32_END(PMC);
}

DEVICE_WRITE32(PMC)
{
	switch(addr) {
	case NV_PMC_INTR_0:
		pmc.pending_interrupts &= ~value;
		update_irq();
		break;
	case NV_PMC_INTR_EN_0:
		pmc.enabled_interrupts = value;
		update_irq();
		break;

	default: 
		//DEVICE_WRITE32_REG(pmc); // Was : DEBUG_WRITE32_UNHANDLED(PMC);
		break;
	}

	DEVICE_WRITE32_END(PMC);
}


DEVICE_READ32(PBUS)
{
	DEVICE_READ32_SWITCH() {
	case NV_PBUS_PCI_NV_0:
		result = 0x10de;	// PCI_VENDOR_ID_NVIDIA	(?where to return PCI_DEVICE_ID_NVIDIA_NV2A = 0x01b7)

		break;
	case NV_PBUS_PCI_NV_1:
		result = 1; // NV_PBUS_PCI_NV_1_IO_SPACE_ENABLED
		break;
	case NV_PBUS_PCI_NV_2:
		result = (0x02 << 24) | 161; // PCI_CLASS_DISPLAY_3D (0x02) Rev 161 (0xA1) 
		break;

	default: 
		DEBUG_READ32_UNHANDLED(PBUS); // TODO : DEVICE_READ32_REG(pbus);
		break;
	}

	DEVICE_READ32_END(PBUS);
}

DEVICE_WRITE32(PBUS)
{
	switch(addr) {
	case NV_PBUS_PCI_NV_1:
		// TODO : Handle write on NV_PBUS_PCI_NV_1 with  1 (NV_PBUS_PCI_NV_1_IO_SPACE_ENABLED) + 4 (NV_PBUS_PCI_NV_1_BUS_MASTER_ENABLED)
		break;
	default: 
		DEBUG_WRITE32_UNHANDLED(PBUS); // TODO : DEVICE_WRITE32_REG(pbus);
		break;
	}

	DEVICE_WRITE32_END(PBUS);
}


DEVICE_READ32(PFIFO)
{
	DEVICE_READ32_SWITCH() {
	case NV_PFIFO_RAMHT:
		result = 0x03000100; // = NV_PFIFO_RAMHT_SIZE_4K | NV_PFIFO_RAMHT_BASE_ADDRESS(NumberOfPaddingBytes >> 12) | NV_PFIFO_RAMHT_SEARCH_128
		break;
	case NV_PFIFO_RAMFC:
		result = 0x00890110; // = ? | NV_PFIFO_RAMFC_SIZE_2K | ?
		break;
	case NV_PFIFO_INTR_0:
		result = pfifo.pending_interrupts;
		break;
	case NV_PFIFO_INTR_EN_0:
		result = pfifo.enabled_interrupts;
		break;
	case NV_PFIFO_RUNOUT_STATUS:
		result = NV_PFIFO_RUNOUT_STATUS_LOW_MARK; /* low mark empty */
		break;
	case NV_PFIFO_CACHE1_PUSH0:
		result = pfifo.cache1.push_enabled;
		break;
	case NV_PFIFO_CACHE1_PUSH1:
		SET_MASK(result, NV_PFIFO_CACHE1_PUSH1_CHID, pfifo.cache1.channel_id);
		SET_MASK(result, NV_PFIFO_CACHE1_PUSH1_MODE, pfifo.cache1.mode);
		break;
	case NV_PFIFO_CACHE1_STATUS: {
		std::lock_guard<std::mutex> lk(pfifo.cache1.mutex);

		if (pfifo.cache1.cache.empty()) {
			result |= NV_PFIFO_CACHE1_STATUS_LOW_MARK; /* low mark empty */
		}

	}	break;
	case NV_PFIFO_CACHE1_DMA_PUSH:
		SET_MASK(result, NV_PFIFO_CACHE1_DMA_PUSH_ACCESS,
			pfifo.cache1.dma_push_enabled);
		SET_MASK(result, NV_PFIFO_CACHE1_DMA_PUSH_STATUS,
			pfifo.cache1.dma_push_suspended);
		SET_MASK(result, NV_PFIFO_CACHE1_DMA_PUSH_BUFFER, 1); /* buffer emoty */
		break;
	case NV_PFIFO_CACHE1_DMA_STATE:
		SET_MASK(result, NV_PFIFO_CACHE1_DMA_STATE_METHOD_TYPE,
			pfifo.cache1.method_nonincreasing);
		SET_MASK(result, NV_PFIFO_CACHE1_DMA_STATE_METHOD,
			pfifo.cache1.method >> 2);
		SET_MASK(result, NV_PFIFO_CACHE1_DMA_STATE_SUBCHANNEL,
			pfifo.cache1.subchannel);
		SET_MASK(result, NV_PFIFO_CACHE1_DMA_STATE_METHOD_COUNT,
			pfifo.cache1.method_count);
		SET_MASK(result, NV_PFIFO_CACHE1_DMA_STATE_ERROR,
			pfifo.cache1.error);
		break;
	case NV_PFIFO_CACHE1_DMA_INSTANCE:
		SET_MASK(result, NV_PFIFO_CACHE1_DMA_INSTANCE_ADDRESS,
			pfifo.cache1.dma_instance >> 4);
		break;
	case NV_PFIFO_CACHE1_DMA_PUT:
		result = user.channel_control[pfifo.cache1.channel_id].dma_put;
		break;
	case NV_PFIFO_CACHE1_DMA_GET:
		result = user.channel_control[pfifo.cache1.channel_id].dma_get;
		break;
	case NV_PFIFO_CACHE1_DMA_SUBROUTINE:
		result = pfifo.cache1.subroutine_return
			| pfifo.cache1.subroutine_active;
		break;
	case NV_PFIFO_CACHE1_PULL0: {
		std::lock_guard<std::mutex> lk(pfifo.cache1.mutex);
		result = pfifo.cache1.pull_enabled;
	} break;
	case NV_PFIFO_CACHE1_ENGINE: {
		std::lock_guard<std::mutex> lk(pfifo.cache1.mutex);
		for (int i = 0; i < NV2A_NUM_SUBCHANNELS; i++) {
			result |= pfifo.cache1.bound_engines[i] << (i * 2);
		}
		
	}	break;
	case NV_PFIFO_CACHE1_DMA_DCOUNT:
		result = pfifo.cache1.dcount;
		break;
	case NV_PFIFO_CACHE1_DMA_GET_JMP_SHADOW:
		result = pfifo.cache1.get_jmp_shadow;
		break;
	case NV_PFIFO_CACHE1_DMA_RSVD_SHADOW:
		result = pfifo.cache1.rsvd_shadow;
		break;
	case NV_PFIFO_CACHE1_DMA_DATA_SHADOW:
		result = pfifo.cache1.data_shadow;
		break;
	default:
		DEVICE_READ32_REG(pfifo); // Was : DEBUG_READ32_UNHANDLED(PFIFO);
		break;
	}

	DEVICE_READ32_END(PFIFO);
}

DEVICE_WRITE32(PFIFO)
{
	switch(addr) {
		case NV_PFIFO_INTR_0:
			pfifo.pending_interrupts &= ~value;
			update_irq();
			break;
		case NV_PFIFO_INTR_EN_0:
			pfifo.enabled_interrupts = value;
			update_irq();
			break;
		case NV_PFIFO_CACHE1_PUSH0:
			pfifo.cache1.push_enabled = value & NV_PFIFO_CACHE1_PUSH0_ACCESS;
			break;
		case NV_PFIFO_CACHE1_PUSH1:
			pfifo.cache1.channel_id = GET_MASK(value, NV_PFIFO_CACHE1_PUSH1_CHID);
			pfifo.cache1.mode = (FifoMode)GET_MASK(value, NV_PFIFO_CACHE1_PUSH1_MODE);
			assert(pfifo.cache1.channel_id < NV2A_NUM_CHANNELS);
			break;
		case NV_PFIFO_CACHE1_DMA_PUSH:
			pfifo.cache1.dma_push_enabled = GET_MASK(value, NV_PFIFO_CACHE1_DMA_PUSH_ACCESS);
			if (pfifo.cache1.dma_push_suspended	&& !GET_MASK(value, NV_PFIFO_CACHE1_DMA_PUSH_STATUS)) {
				pfifo.cache1.dma_push_suspended = false;
				pfifo_run_pusher();
			}
			pfifo.cache1.dma_push_suspended = GET_MASK(value, NV_PFIFO_CACHE1_DMA_PUSH_STATUS);
			break;
		case NV_PFIFO_CACHE1_DMA_STATE:
			pfifo.cache1.method_nonincreasing =	GET_MASK(value, NV_PFIFO_CACHE1_DMA_STATE_METHOD_TYPE);
			pfifo.cache1.method = GET_MASK(value, NV_PFIFO_CACHE1_DMA_STATE_METHOD) << 2;
			pfifo.cache1.subchannel = GET_MASK(value, NV_PFIFO_CACHE1_DMA_STATE_SUBCHANNEL);
			pfifo.cache1.method_count =	GET_MASK(value, NV_PFIFO_CACHE1_DMA_STATE_METHOD_COUNT);
			pfifo.cache1.error = GET_MASK(value, NV_PFIFO_CACHE1_DMA_STATE_ERROR);
			break;
		case NV_PFIFO_CACHE1_DMA_INSTANCE:
			pfifo.cache1.dma_instance =	GET_MASK(value, NV_PFIFO_CACHE1_DMA_INSTANCE_ADDRESS) << 4;
			break;
		case NV_PFIFO_CACHE1_DMA_PUT:
			user.channel_control[pfifo.cache1.channel_id].dma_put = value;
			break;
		case NV_PFIFO_CACHE1_DMA_GET:
			user.channel_control[pfifo.cache1.channel_id].dma_get = value;
			break;
		case NV_PFIFO_CACHE1_DMA_SUBROUTINE:
			pfifo.cache1.subroutine_return = (value & NV_PFIFO_CACHE1_DMA_SUBROUTINE_RETURN_OFFSET);
			pfifo.cache1.subroutine_active = (value & NV_PFIFO_CACHE1_DMA_SUBROUTINE_STATE);
			break;
		case NV_PFIFO_CACHE1_PULL0: {
			std::lock_guard<std::mutex> lk(pfifo.cache1.mutex);

			if ((value & NV_PFIFO_CACHE1_PULL0_ACCESS)
				&& !pfifo.cache1.pull_enabled) {
				pfifo.cache1.pull_enabled = true;

				/* the puller thread should wake up */
				pfifo.cache1.cache_cond.notify_all();
			} else if (!(value & NV_PFIFO_CACHE1_PULL0_ACCESS)
				&& pfifo.cache1.pull_enabled) {
				pfifo.cache1.pull_enabled = false;
			}
		}	break;
		case NV_PFIFO_CACHE1_ENGINE: {
			std::lock_guard<std::mutex> lk(pfifo.cache1.mutex);

			for (int i = 0; i < NV2A_NUM_SUBCHANNELS; i++) {
				pfifo.cache1.bound_engines[i] = (FIFOEngine)((value >> (i * 2)) & 3);
			}

		} break;
		case NV_PFIFO_CACHE1_DMA_DCOUNT:
			pfifo.cache1.dcount = (value & NV_PFIFO_CACHE1_DMA_DCOUNT_VALUE);
			break;
		case NV_PFIFO_CACHE1_DMA_GET_JMP_SHADOW:
			pfifo.cache1.get_jmp_shadow = (value & NV_PFIFO_CACHE1_DMA_GET_JMP_SHADOW_OFFSET);
			break;
		case NV_PFIFO_CACHE1_DMA_RSVD_SHADOW:
			pfifo.cache1.rsvd_shadow = value;
			break;
		case NV_PFIFO_CACHE1_DMA_DATA_SHADOW:
			pfifo.cache1.data_shadow = value;
			break;
		default:
			DEVICE_WRITE32_REG(pfifo); // Was : DEBUG_WRITE32_UNHANDLED(PFIFO);
			break;
	}

	DEVICE_WRITE32_END(PFIFO);
}

DEVICE_READ32(PRMA)
{
	DEVICE_READ32_SWITCH() {
	default:
		DEBUG_READ32_UNHANDLED(PRMA); // TODO : DEVICE_READ32_REG(prma);
		break;
	}

	DEVICE_READ32_END(PRMA);
}

DEVICE_WRITE32(PRMA)
{
	switch(addr) {
	default: 
		DEBUG_WRITE32_UNHANDLED(PRMA); // TODO : DEVICE_WRITE32_REG(prma);
		break;
	}

	DEVICE_WRITE32_END(PRMA);
}


DEVICE_READ32(PVIDEO)
{
	DEVICE_READ32_SWITCH() {

	case NV_PVIDEO_STOP:
		result = 0;
		break;
	default:
		DEVICE_READ32_REG(pvideo);
		break;
	}

	DEVICE_READ32_END(PVIDEO);
}

DEVICE_WRITE32(PVIDEO)
{
	switch (addr) {
	case NV_PVIDEO_BUFFER:
		pvideo.regs[addr] = value;
		// TODO: vga.enable_overlay = true;
		// pvideo_vga_invalidate(d);
		break;
	case NV_PVIDEO_STOP:
		pvideo.regs[NV_PVIDEO_BUFFER] = 0;
		// TODO: vga.enable_overlay = false;
		//pvideo_vga_invalidate(d);
		break;
	default:
		DEVICE_WRITE32_REG(pvideo);
		break;
	}

	DEVICE_WRITE32_END(PVIDEO);
}

DEVICE_READ32(PCOUNTER)
{
	DEVICE_READ32_SWITCH() {
	default:
		DEBUG_READ32_UNHANDLED(PCOUNTER); // TODO : DEVICE_READ32_REG(pcounter);
		break;
	}

	DEVICE_READ32_END(PCOUNTER);
}

DEVICE_WRITE32(PCOUNTER)
{
	switch (addr) {
	default:
		DEBUG_WRITE32_UNHANDLED(PCOUNTER); // TODO : DEVICE_WRITE32_REG(pcounter);
		break;
	}

	DEVICE_WRITE32_END(PCOUNTER);
}

static inline uint64_t muldiv64(uint64_t a, uint32_t b, uint32_t c)
{
	union {
		uint64_t ll;
		struct {
			uint32_t low, high;
		} l;
	} u, res;
	uint64_t rl, rh;

	u.ll = a;
	rl = (uint64_t)u.l.low * (uint64_t)b;
	rh = (uint64_t)u.l.high * (uint64_t)b;
	rh += (rl >> 32);
	res.l.high = rh / c;
	res.l.low = (((rh % c) << 32) + (rl & 0xffffffff)) / c;
	return res.ll;
}

/* PIMTER - time measurement and time-based alarms */
static uint32_t ptimer_get_clock()
{
	// Get time in nanoseconds
	long int time = static_cast<long int>(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count());
	
	return muldiv64(time, pramdac.core_clock_freq * ptimer.numerator, CLOCKS_PER_SEC * ptimer.denominator);
}

DEVICE_READ32(PTIMER)
{
	DEVICE_READ32_SWITCH() {
	case NV_PTIMER_INTR_0:
		result = ptimer.pending_interrupts;
		break;
	case NV_PTIMER_INTR_EN_0:
		result = ptimer.enabled_interrupts;
		break;
	case NV_PTIMER_NUMERATOR:
		result = ptimer.numerator;
		break;
	case NV_PTIMER_DENOMINATOR:
		result = ptimer.denominator;
		break;
	case NV_PTIMER_TIME_0:
		result = (ptimer_get_clock() & 0x7ffffff) << 5;
		break;
	case NV_PTIMER_TIME_1:
		result = (ptimer_get_clock() >> 27) & 0x1fffffff;
		break;
	default: 
		result = 0;
		//DEVICE_READ32_REG(ptimer); // Was : DEBUG_READ32_UNHANDLED(PTIMER);
		break;
	}

	DEVICE_READ32_END(PTIMER);
}


DEVICE_WRITE32(PTIMER)
{
	switch (addr) {

	case NV_PTIMER_INTR_0:
		ptimer.pending_interrupts &= ~value;
		update_irq();
		break;
	case NV_PTIMER_INTR_EN_0:
		ptimer.enabled_interrupts = value;
		update_irq();
		break;
	case NV_PTIMER_DENOMINATOR:
		ptimer.denominator = value;
		break;
	case NV_PTIMER_NUMERATOR:
		ptimer.numerator = value;
		break;
	case NV_PTIMER_ALARM_0:
		ptimer.alarm_time = value;
		break;
	default: 
		//DEVICE_WRITE32_REG(ptimer); // Was : DEBUG_WRITE32_UNHANDLED(PTIMER);
		break;
	}

	DEVICE_WRITE32_END(PTIMER);
}


DEVICE_READ32(PVPE)
{
	DEVICE_READ32_SWITCH() {
	default:
		DEBUG_READ32_UNHANDLED(PVPE); // TODO : DEVICE_READ32_REG(pvpe);
		break;
	}

	DEVICE_READ32_END(PVPE);
}


DEVICE_WRITE32(PVPE)
{
	switch (addr) {
	default:
		DEBUG_WRITE32_UNHANDLED(PVPE); // TODO : DEVICE_WRITE32_REG(pvpe);
		break;
	}

	DEVICE_WRITE32_END(PVPE);
}


DEVICE_READ32(PTV)
{
	DEVICE_READ32_SWITCH() {
	default:
		DEBUG_READ32_UNHANDLED(PTV); // TODO : DEVICE_READ32_REG(ptv);
		break;
	}

	DEVICE_READ32_END(PTV);
}

DEVICE_WRITE32(PTV)
{
	switch (addr) {
	default:
		DEBUG_WRITE32_UNHANDLED(PTV); // TODO : DEVICE_WRITE32_REG(ptv);
		break;
	}

	DEVICE_WRITE32_END(PTV);
}


DEVICE_READ32(PRMFB)
{
	DEVICE_READ32_SWITCH() {
	default:
		DEBUG_READ32_UNHANDLED(PRMFB); // TODO : DEVICE_READ32_REG(prmfb);
		break;
	}

	DEVICE_READ32_END(PRMFB);
}

DEVICE_WRITE32(PRMFB)
{
	switch (addr) {
	default:
		DEBUG_WRITE32_UNHANDLED(PRMFB); // TODO : DEVICE_WRITE32_REG(prmfb);
		break;
	}

	DEVICE_WRITE32_END(PRMFB);
}


DEVICE_READ32(PRMVIO)
{
	DEVICE_READ32_SWITCH() {
	default:
		DEBUG_READ32_UNHANDLED(PRMVIO); // TODO : DEVICE_READ32_REG(prmvio);
		break;
	}

	DEVICE_READ32_END(PRMVIO);
}

DEVICE_WRITE32(PRMVIO)
{
	switch (addr) {
	default:
		DEBUG_WRITE32_UNHANDLED(PRMVIO); // TODO : DEVICE_WRITE32_REG(prmvio);
		break;
	}

	DEVICE_WRITE32_END(PRMVIO);
}


DEVICE_READ32(PFB)
{
	DEVICE_READ32_SWITCH() {
	case NV_PFB_CFG0:
		result = 3; // = NV_PFB_CFG0_PART_4
		break;
	case NV_PFB_CSTATUS:
		result = CONTIGUOUS_MEMORY_SIZE;
		break;
	case NV_PFB_WBC:
		result = 0; // = !NV_PFB_WBC_FLUSH
		break;
	default:
		DEVICE_READ32_REG(pfb);
		break;
	}

	DEVICE_READ32_END(PFB);
}

DEVICE_WRITE32(PFB)
{
	switch (addr) {
	default:
		DEVICE_WRITE32_REG(pfb);
		break;
	}

	DEVICE_WRITE32_END(PFB);
}


DEVICE_READ32(PSTRAPS)
{
	DEVICE_READ32_SWITCH() {
	default:
		DEBUG_READ32_UNHANDLED(PSTRAPS);
		break;
	}

	DEVICE_READ32_END(PSTRAPS);
}

DEVICE_WRITE32(PSTRAPS)
{
	switch (addr) {
	default:
		DEBUG_WRITE32_UNHANDLED(PSTRAPS);
		break;
	}

	DEVICE_WRITE32_END(PSTRAPS);
}


DEVICE_READ32(PGRAPH)
{
	std::lock_guard<std::mutex> lk(pgraph.mutex);

	DEVICE_READ32_SWITCH() {
	case NV_PGRAPH_INTR:
		result = pgraph.pending_interrupts;
		break;
	case NV_PGRAPH_INTR_EN:
		result = pgraph.enabled_interrupts;
		break;
	case NV_PGRAPH_NSOURCE:
		result = pgraph.notify_source;
		break;
	case NV_PGRAPH_CTX_USER:
		SET_MASK(result, NV_PGRAPH_CTX_USER_CHANNEL_3D, pgraph.context[pgraph.channel_id].channel_3d);
		SET_MASK(result, NV_PGRAPH_CTX_USER_CHANNEL_3D_VALID, 1);
		SET_MASK(result, NV_PGRAPH_CTX_USER_SUBCH, pgraph.context[pgraph.channel_id].subchannel << 13);
		SET_MASK(result, NV_PGRAPH_CTX_USER_CHID, pgraph.channel_id);
		break;
	case NV_PGRAPH_TRAPPED_ADDR:
		SET_MASK(result, NV_PGRAPH_TRAPPED_ADDR_CHID, pgraph.trapped_channel_id);
		SET_MASK(result, NV_PGRAPH_TRAPPED_ADDR_SUBCH, pgraph.trapped_subchannel);
		SET_MASK(result, NV_PGRAPH_TRAPPED_ADDR_MTHD, pgraph.trapped_method);
		break;
	case NV_PGRAPH_TRAPPED_DATA_LOW:
		result = pgraph.trapped_data[0];
		break;
	case NV_PGRAPH_FIFO:
		SET_MASK(result, NV_PGRAPH_FIFO_ACCESS, pgraph.fifo_access);
		break;
	case NV_PGRAPH_CHANNEL_CTX_TABLE:
		result = pgraph.context_table >> 4;
		break;
	case NV_PGRAPH_CHANNEL_CTX_POINTER:
		result = pgraph.context_address >> 4;
		break;
	default:
		DEVICE_READ32_REG(pgraph); // Was : DEBUG_READ32_UNHANDLED(PGRAPH);
	}

	DEVICE_READ32_END(PGRAPH);
}

static void pgraph_set_context_user(uint32_t value)
{
	pgraph.channel_id = (value & NV_PGRAPH_CTX_USER_CHID) >> 24;
	pgraph.context[pgraph.channel_id].channel_3d = GET_MASK(value, NV_PGRAPH_CTX_USER_CHANNEL_3D);
	pgraph.context[pgraph.channel_id].subchannel = GET_MASK(value, NV_PGRAPH_CTX_USER_SUBCH);
}

DEVICE_WRITE32(PGRAPH)
{
	std::lock_guard<std::mutex> lk(pgraph.mutex);

	switch (addr) {
	case NV_PGRAPH_INTR:
		pgraph.pending_interrupts &= ~value;
		pgraph.interrupt_cond.notify_all();
		break;
	case NV_PGRAPH_INTR_EN:
		pgraph.enabled_interrupts = value;
		break;
	case NV_PGRAPH_CTX_CONTROL:
		pgraph.channel_valid = (value & NV_PGRAPH_CTX_CONTROL_CHID);
		break;
	case NV_PGRAPH_CTX_USER:
		pgraph_set_context_user(value);
		break;
	case NV_PGRAPH_INCREMENT:
		if (value & NV_PGRAPH_INCREMENT_READ_3D) {
			SET_MASK(pgraph.regs[NV_PGRAPH_SURFACE],
				NV_PGRAPH_SURFACE_READ_3D,
				(GET_MASK(pgraph.regs[NV_PGRAPH_SURFACE],
					NV_PGRAPH_SURFACE_READ_3D) + 1)
				% GET_MASK(pgraph.regs[NV_PGRAPH_SURFACE],
					NV_PGRAPH_SURFACE_MODULO_3D));

			pgraph.flip_3d.notify_all();
		}
		break;
	case NV_PGRAPH_FIFO:
		pgraph.fifo_access = GET_MASK(value, NV_PGRAPH_FIFO_ACCESS);
		pgraph.fifo_access_cond.notify_all();
		break;
	case NV_PGRAPH_CHANNEL_CTX_TABLE:
		pgraph.context_table = (value & NV_PGRAPH_CHANNEL_CTX_TABLE_INST) << 4;
		break;
	case NV_PGRAPH_CHANNEL_CTX_POINTER:
		pgraph.context_address =
			(value & NV_PGRAPH_CHANNEL_CTX_POINTER_INST) << 4;
		break;
	case NV_PGRAPH_CHANNEL_CTX_TRIGGER:
		if (value & NV_PGRAPH_CHANNEL_CTX_TRIGGER_READ_IN) {
			printf("PGRAPH: read channel %d context from %0x08X\n",
				pgraph.channel_id, pgraph.context_address);

			uint8_t *context_ptr = (uint8_t*)(NV2A_ADDR + NV_PRAMIN_ADDR + pgraph.context_address);
			uint32_t context_user = ldl_le_p((uint32_t*)context_ptr);

			printf("    - CTX_USER = 0x%x\n", context_user);


			pgraph_set_context_user(context_user);
		}
		if (value & NV_PGRAPH_CHANNEL_CTX_TRIGGER_WRITE_OUT) {
			/* do stuff ... */
		}

		break;
	default: 
		DEVICE_WRITE32_REG(pgraph); // Was : DEBUG_WRITE32_UNHANDLED(PGRAPH);
		break;
	}

	DEVICE_WRITE32_END(PGRAPH);
}


DEVICE_READ32(PCRTC)
{
	DEVICE_READ32_SWITCH() {

	case NV_PCRTC_INTR_0:
		result = pcrtc.pending_interrupts;
		break;
	case NV_PCRTC_INTR_EN_0:
		result = pcrtc.enabled_interrupts;
		break;
	case NV_PCRTC_START:
		result = pcrtc.start;
		break;
	default: 
		result = 0;
		//DEVICE_READ32_REG(pcrtc); // Was : DEBUG_READ32_UNHANDLED(PCRTC);
		break;
	}

	DEVICE_READ32_END(PCRTC);
}

DEVICE_WRITE32(PCRTC)
{
	switch (addr) {

	case NV_PCRTC_INTR_0:
		pcrtc.pending_interrupts &= ~value;
		update_irq();
		break;
	case NV_PCRTC_INTR_EN_0:
		pcrtc.enabled_interrupts = value;
		update_irq();
		break;
	case NV_PCRTC_START:
		pcrtc.start = value &= 0x07FFFFFF;
		break;

	default: 
		DEVICE_WRITE32_REG(pcrtc); // Was : DEBUG_WRITE32_UNHANDLED(PCRTC);
		break;
	}

	DEVICE_WRITE32_END(PCRTC);
}


DEVICE_READ32(PRMCIO)
{
	DEVICE_READ32_SWITCH() {
	default:
		DEBUG_READ32_UNHANDLED(PRMCIO);
		break;
	}

	DEVICE_READ32_END(PRMCIO);
}

DEVICE_WRITE32(PRMCIO)
{
	switch (addr) {
	default:
		DEBUG_WRITE32_UNHANDLED(PRMCIO); // TODO : DEVICE_WRITE32_REG(prmcio);
		break;
	}

	DEVICE_WRITE32_END(PRMCIO);
}


DEVICE_READ32(PRAMDAC)
{
	DEVICE_READ32_SWITCH() {

	case NV_PRAMDAC_NVPLL_COEFF:
		result = pramdac.core_clock_coeff;
		break;
	case NV_PRAMDAC_MPLL_COEFF:
		result = pramdac.memory_clock_coeff;
		break;
	case NV_PRAMDAC_VPLL_COEFF:
		result = pramdac.video_clock_coeff;
		break;
	case NV_PRAMDAC_PLL_TEST_COUNTER:
		/* emulated PLLs locked instantly? */
		result = NV_PRAMDAC_PLL_TEST_COUNTER_VPLL2_LOCK
			| NV_PRAMDAC_PLL_TEST_COUNTER_NVPLL_LOCK
			| NV_PRAMDAC_PLL_TEST_COUNTER_MPLL_LOCK
			| NV_PRAMDAC_PLL_TEST_COUNTER_VPLL_LOCK;
		break;

	default: 
		//DEVICE_READ32_REG(pramdac); // Was : DEBUG_READ32_UNHANDLED(PRAMDAC);
		break;
	}

	DEVICE_READ32_END(PRAMDAC);
}

DEVICE_WRITE32(PRAMDAC)
{
	switch (addr) {

	uint32_t m, n, p;
	case NV_PRAMDAC_NVPLL_COEFF:
		pramdac.core_clock_coeff = value;

		m = value & NV_PRAMDAC_NVPLL_COEFF_MDIV;
		n = (value & NV_PRAMDAC_NVPLL_COEFF_NDIV) >> 8;
		p = (value & NV_PRAMDAC_NVPLL_COEFF_PDIV) >> 16;

		if (m == 0) {
			pramdac.core_clock_freq = 0;
		}
		else {
			pramdac.core_clock_freq = (NV2A_CRYSTAL_FREQ * n)
				/ (1 << p) / m;
		}

		break;
	case NV_PRAMDAC_MPLL_COEFF:
		pramdac.memory_clock_coeff = value;
		break;
	case NV_PRAMDAC_VPLL_COEFF:
		pramdac.video_clock_coeff = value;
		break;

	default: 
		//DEVICE_WRITE32_REG(pramdac); // Was : DEBUG_WRITE32_UNHANDLED(PRAMDAC);
		break;
	}

	DEVICE_WRITE32_END(PRAMDAC);
}


DEVICE_READ32(PRMDIO)
{
	DEVICE_READ32_SWITCH() {
	default:
		DEBUG_READ32_UNHANDLED(PRMDIO);
		break;
	}

	DEVICE_READ32_END(PRMDIO);
}

DEVICE_WRITE32(PRMDIO)
{
	switch (addr) {
	default:
		DEBUG_WRITE32_UNHANDLED(PRMDIO);
		break;
	}

	DEVICE_WRITE32_END(PRMDIO);
}


DEVICE_READ32(PRAMIN)
{
	DEVICE_READ32_SWITCH() {
	default:
		DEVICE_READ32_REG(pramin);
		break;
	}

	DEVICE_READ32_END(PRAMIN);
}

DEVICE_WRITE32(PRAMIN)
{
	switch (addr) {
	default:
		DEVICE_WRITE32_REG(pramin);
		break;
	}

	DEVICE_WRITE32_END(PRAMIN);
}


DEVICE_READ32(USER)
{
	unsigned int channel_id = addr >> 16;
	assert(channel_id < NV2A_NUM_CHANNELS);

	ChannelControl *control = &user.channel_control[channel_id];
	uint32_t channel_modes = pfifo.regs[NV_PFIFO_MODE];

	/* PIO Mode */
	if (!channel_modes & (1 << channel_id)) {
		assert(false);
	}

	/* DMA Mode */
	addr &= 0xFFFF;
	DEVICE_READ32_SWITCH() {
		case NV_USER_DMA_PUT:
			result = control->dma_put;
			break;
		case NV_USER_DMA_GET:
			result = control->dma_get;
			break;
		case NV_USER_REF:
			result = control->ref;
			break;
		default:
			DEBUG_READ32_UNHANDLED(USER);
			break;
	}

	DEVICE_READ32_END(USER);
}

DEVICE_WRITE32(USER)
{
	unsigned int channel_id = addr >> 16;
	assert(channel_id < NV2A_NUM_CHANNELS);

	ChannelControl *control = &user.channel_control[channel_id];

	uint32_t channel_modes = pfifo.regs[NV_PFIFO_MODE];

	if (channel_modes & (1 << channel_id)) {
		/* DMA Mode */
		switch (addr & 0xFFFF) {
		case NV_USER_DMA_PUT:
			control->dma_put = value;

			if (pfifo.cache1.push_enabled) {
				pfifo_run_pusher();
			}
			break;
		case NV_USER_DMA_GET:
			control->dma_get = value;
			break;
		case NV_USER_REF:
			control->ref = value;
			break;
		default:
			DEBUG_WRITE32_UNHANDLED(USER);
			break;
		}
	}
	else {
		/* PIO Mode */
		assert(false);
	}

	DEVICE_WRITE32_END(USER);
}


typedef struct NV2ABlockInfo {
		uint32_t offset;
		uint32_t size;
		uint32_t(*read)(xbaddr addr);
		void(*write)(xbaddr addr, uint32_t value);
} NV2ABlockInfo;

static const NV2ABlockInfo regions[] = {{
		/* card master control */
		NV_PMC_ADDR, // = 0x000000
		NV_PMC_SIZE, // = 0x001000
		EmuNV2A_PMC_Read32,
		EmuNV2A_PMC_Write32,
	}, {
		/* bus control */
		NV_PBUS_ADDR, // = 0x001000
		NV_PBUS_SIZE, // = 0x001000
		EmuNV2A_PBUS_Read32,
		EmuNV2A_PBUS_Write32,
	}, {
		/* MMIO and DMA FIFO submission to PGRAPH and VPE */
		NV_PFIFO_ADDR, // = 0x002000
		NV_PFIFO_SIZE, // = 0x002000
		EmuNV2A_PFIFO_Read32,
		EmuNV2A_PFIFO_Write32,
	}, {
		/* access to BAR0/BAR1 from real mode */
		NV_PRMA_ADDR, // = 0x007000
		NV_PRMA_SIZE, // = 0x001000
		EmuNV2A_PRMA_Read32,
		EmuNV2A_PRMA_Write32,
	}, {
		/* video overlay */
		NV_PVIDEO_ADDR, // = 0x008000
		NV_PVIDEO_SIZE, // = 0x001000
		EmuNV2A_PVIDEO_Read32,
		EmuNV2A_PVIDEO_Write32,
	}, {
		/* time measurement and time-based alarms */
		NV_PTIMER_ADDR, // = 0x009000
		NV_PTIMER_SIZE, // = 0x001000
		EmuNV2A_PTIMER_Read32,
		EmuNV2A_PTIMER_Write32,
	}, {
		/* performance monitoring counters */
		NV_PCOUNTER_ADDR, // = 0x00a000
		NV_PCOUNTER_SIZE, // = 0x001000
		EmuNV2A_PCOUNTER_Read32,
		EmuNV2A_PCOUNTER_Write32,
	}, {
		/* MPEG2 decoding engine */
		NV_PVPE_ADDR, // = 0x00b000
		NV_PVPE_SIZE, // = 0x001000
		EmuNV2A_PVPE_Read32,
		EmuNV2A_PVPE_Write32,
	},	{
		/* TV encoder */
		NV_PTV_ADDR, // = 0x00d000
		NV_PTV_SIZE, // = 0x001000
		EmuNV2A_PTV_Read32,
		EmuNV2A_PTV_Write32,
	}, {
		/* aliases VGA memory window */
		NV_PRMFB_ADDR, // = 0x0a0000
		NV_PRMFB_SIZE, // = 0x020000
		EmuNV2A_PRMFB_Read32,
		EmuNV2A_PRMFB_Write32,
	}, {
		/* aliases VGA sequencer and graphics controller registers */
		NV_PRMVIO_ADDR, // = 0x0c0000
		NV_PRMVIO_SIZE, // = 0x001000
		EmuNV2A_PRMVIO_Read32,
		EmuNV2A_PRMVIO_Write32,
	},{
		/* memory interface */
		NV_PFB_ADDR, // = 0x100000
		NV_PFB_SIZE, // = 0x001000
		EmuNV2A_PFB_Read32,
		EmuNV2A_PFB_Write32,
	}, {
		/* straps readout / override */
		NV_PSTRAPS_ADDR, // = 0x101000
		NV_PSTRAPS_SIZE, // = 0x001000
		EmuNV2A_PSTRAPS_Read32,
		EmuNV2A_PSTRAPS_Write32,
	}, {
		/* accelerated 2d/3d drawing engine */
		NV_PGRAPH_ADDR, // = 0x400000
		NV_PGRAPH_SIZE, // = 0x002000
		EmuNV2A_PGRAPH_Read32,
		EmuNV2A_PGRAPH_Write32,
	}, {
		/* more CRTC controls */
		NV_PCRTC_ADDR, // = 0x600000
		NV_PCRTC_SIZE, // = 0x001000
		EmuNV2A_PCRTC_Read32,
		EmuNV2A_PCRTC_Write32,
	}, {
		/* aliases VGA CRTC and attribute controller registers */
		NV_PRMCIO_ADDR, // = 0x601000
		NV_PRMCIO_SIZE, // = 0x001000
		EmuNV2A_PRMCIO_Read32,
		EmuNV2A_PRMCIO_Write32,
	}, {
		/* RAMDAC, cursor, and PLL control */
		NV_PRAMDAC_ADDR, // = 0x680000
		NV_PRAMDAC_SIZE, // = 0x001000
		EmuNV2A_PRAMDAC_Read32,
		EmuNV2A_PRAMDAC_Write32,
	}, {
		/* aliases VGA palette registers */
		NV_PRMDIO_ADDR, // = 0x681000
		NV_PRMDIO_SIZE, // = 0x001000
		EmuNV2A_PRMDIO_Read32,
		EmuNV2A_PRMDIO_Write32,
	}, {
		/* RAMIN access */
		NV_PRAMIN_ADDR, // = 0x700000
		NV_PRAMIN_SIZE, // = 0x100000
		EmuNV2A_PRAMIN_Read32,
		EmuNV2A_PRAMIN_Write32,
	},{
		/* PFIFO MMIO and DMA submission area */
		NV_USER_ADDR, // = 0x800000,
		NV_USER_SIZE, // = 0x800000,
		EmuNV2A_USER_Read32,
		EmuNV2A_USER_Write32,
	}, {
		0xFFFFFFFF,
		0,
		nullptr,
		nullptr,
	},
};

const NV2ABlockInfo* EmuNV2A_Block(xbaddr addr) 
{
	// Find the block in the block table
	const NV2ABlockInfo* block = &regions[0];
	int i = 0;

	while (block->read != nullptr) {
		if (addr >= block->offset && addr < block->offset + block->size) {
			return block;
		}

		block = &regions[++i];
	}

	return nullptr;
}

uint32_t EmuNV2A_Read(xbaddr addr, int size)
{
	const NV2ABlockInfo* block = EmuNV2A_Block(addr);

	if (block != nullptr) {
		switch (size) {
			case 8:
				return block->read(addr - block->offset) & 0xFF;
			case 16:
				return block->read(addr - block->offset) & 0xFFFF;
			case 32:
				return block->read(addr - block->offset);
			default:
				EmuWarning("EmuNV2A_Read: Invalid read size: %d", size);
				return 0;
		}
	}

	EmuWarning("EmuNV2A_Read%d: Unhandled Read Address %08X", size, addr);
	return 0;
}

void EmuNV2A_Write(xbaddr addr, uint32_t value, int size)
{
	const NV2ABlockInfo* block = EmuNV2A_Block(addr);

	if (block != nullptr) {
		int shift = 0;
		xbaddr aligned_addr = 0;
		uint32_t aligned_value = 0;
		uint32_t mask = 0;
		switch (size) {
			case 8:
				shift = (addr & 3) * 8;
				aligned_addr = addr & ~3;
				aligned_value = block->read(aligned_addr - block->offset);
				mask = 0xFF << shift;

				// TODO : Must the second byte be written to the next DWORD?		
				block->write(aligned_addr - block->offset, (aligned_value & ~mask) | (value << shift));
				return;
			case 16:
				assert((addr & 1) == 0);
				
				shift = (addr & 2) * 16;
				aligned_addr = addr & ~3;
				aligned_value = block->read(addr - block->offset);
				 mask = 0xFFFF << shift;
				
				// TODO : Must the second byte be written to the next DWORD?		
				block->write(aligned_addr - block->offset, (aligned_value & ~mask) | (value << shift));
				return;
			case 32:
				block->write(addr - block->offset, value);
				return;
			default:
				EmuWarning("EmuNV2A_Read: Invalid read size: %d", size);
				return;
		}
	}

	EmuWarning("EmuNV2A_Write%d: Unhandled Write Address %08X (value %08X)", size, addr, value);
	return;
}

//
// OPENGL
//

// 
#define X_D3DTS_STAGECOUNT 4

HDC g_EmuWindowsDC = 0;
GLuint VertexProgramIDs[4] = { 0, 0, 0, 0 };
uint ActiveVertexProgramID = 0;
GLuint TextureIDs[X_D3DTS_STAGECOUNT]= { 0, 0, 0, 0 };

// Vertex shader header, mapping Xbox1 registers to the ARB syntax (original version by KingOfC).
// Note about the use of 'conventional' attributes in here: Since we prefer to use only one shader
// for both immediate and deferred mode rendering, we alias all attributes to conventional inputs
// as much as possible. Only when there's no conventional attribute available, we use generic attributes.
// So in the following header, we use conventional attributes first, and generic attributes for the
// rest of the vertex attribute slots. This makes it possible to support immediate and deferred mode
// rendering with the same shader, and the use of the OpenGL fixed-function pipeline without a shader.
std::string DxbxVertexShaderHeader =
    "!!ARBvp1.0\n"
    "TEMP R0,R1,R2,R3,R4,R5,R6,R7,R8,R9,R10,R11,R12;\n"
    "ADDRESS A0;\n"
#ifdef DXBX_OPENGL_CONVENTIONAL
    "ATTRIB v0 = vertex.position;\n" // Was: vertex.attrib[0] (See "conventional" note above)
    "ATTRIB v1 = vertex.%s;\n" // Note : We replace this with "weight" or "attrib[1]" depending GL_ARB_vertex_blend
    "ATTRIB v2 = vertex.normal;\n" // Was: vertex.attrib[2]
    "ATTRIB v3 = vertex.color.primary;\n" // Was: vertex.attrib[3]
    "ATTRIB v4 = vertex.color.secondary;\n" // Was: vertex.attrib[4]
    "ATTRIB v5 = vertex.fogcoord;\n" // Was: vertex.attrib[5]
    "ATTRIB v6 = vertex.attrib[6];\n"
    "ATTRIB v7 = vertex.attrib[7];\n"
    "ATTRIB v8 = vertex.texcoord[0];\n" // Was: vertex.attrib[8]
    "ATTRIB v9 = vertex.texcoord[1];\n" // Was: vertex.attrib[9]
    "ATTRIB v10 = vertex.texcoord[2];\n" // Was: vertex.attrib[10]
    "ATTRIB v11 = vertex.texcoord[3];\n" // Was: vertex.attrib[11]
#else
    "ATTRIB v0 = vertex.attrib[0];\n"
    "ATTRIB v1 = vertex.attrib[1];\n"
    "ATTRIB v2 = vertex.attrib[2];\n"
    "ATTRIB v3 = vertex.attrib[3];\n"
    "ATTRIB v4 = vertex.attrib[4];\n"
    "ATTRIB v5 = vertex.attrib[5];\n"
    "ATTRIB v6 = vertex.attrib[6];\n"
    "ATTRIB v7 = vertex.attrib[7];\n"
    "ATTRIB v8 = vertex.attrib[8];\n"
    "ATTRIB v9 = vertex.attrib[9];\n"
    "ATTRIB v10 = vertex.attrib[10];\n"
    "ATTRIB v11 = vertex.attrib[11];\n"
#endif
    "ATTRIB v12 = vertex.attrib[12];\n"
    "ATTRIB v13 = vertex.attrib[13];\n"
    "ATTRIB v14 = vertex.attrib[14];\n"
    "ATTRIB v15 = vertex.attrib[15];\n"
    "OUTPUT oPos = result.position;\n"
    "OUTPUT oD0 = result.color.front.primary;\n"
    "OUTPUT oD1 = result.color.front.secondary;\n"
    "OUTPUT oB0 = result.color.back.primary;\n"
    "OUTPUT oB1 = result.color.back.secondary;\n"
    "OUTPUT oPts = result.pointsize;\n"
    "OUTPUT oFog = result.fogcoord;\n"
    "OUTPUT oT0 = result.texcoord[0];\n"
    "OUTPUT oT1 = result.texcoord[1];\n"
    "OUTPUT oT2 = result.texcoord[2];\n"
    "OUTPUT oT3 = result.texcoord[3];\n"
    "PARAM c[] = { program.env[0..191] };\n" // All constants in 1 array declaration (requires NV_gpu_program4?)
    "PARAM mvp[4] = { state.matrix.mvp };\n";

void SetupPixelFormat(HDC DC)
{
	const PIXELFORMATDESCRIPTOR pfd = {
		 /* .nSize = */ sizeof(PIXELFORMATDESCRIPTOR), // size
		 /* .nVersion = */ 1,   // version
		 /* .dwFlags = */ PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER, // support double-buffering
		 /* .iPixelType = */ PFD_TYPE_RGBA, // color type
		 /* .cColorBits = */ 32,   // preferred color depth
		 /* .cRedBits = */ 0,
		 /* .cRedShift = */ 0, // color bits (ignored)
		 /* .cGreenBits = */ 0,
		 /* .cGreenShift = */ 0,
		 /* .cBlueBits = */ 0,
		 /* .cBlueShift = */ 0,
		 /* .cAlphaBits = */ 0,
		 /* .cAlphaShift = */ 0,   // no alpha buffer
		 /* .cAccumBits = */ 0,
		 /* .cAccumRedBits = */ 0,    // no accumulation buffer,
		 /* .cAccumGreenBits = */ 0,      // accum bits (ignored)
		 /* .cAccumBlueBits = */ 0,
		 /* .cAccumAlphaBits = */ 0,
		 /* .cDepthBits = */ 16,   // depth buffer
		 /* .cStencilBits = */ 0,   // no stencil buffer
		 /* .cAuxBuffers = */ 0,   // no auxiliary buffers
		 /* .iLayerType= */ PFD_MAIN_PLANE,   // main layer
		 /* .bReserved = */ 0,
		 /* .dwLayerMask = */ 0,
		 /* .dwVisibleMask = */ 0,
		 /* .dwDamageMask = */ 0                    // no layer, visible, damage masks
	};

	int PixelFormat = ChoosePixelFormat(DC, &pfd);
	if (PixelFormat == 0)
		return;

	if (SetPixelFormat(DC, PixelFormat, &pfd) != TRUE)
		return;
}

// From https://github.com/inolen/redream/blob/master/src/video/gl_backend.c
static int rb_compile_shader(const char *source, GLenum shader_type, GLuint *shader)
{
	size_t sourceLength = strlen(source);

	*shader = glCreateShader(shader_type);
	glShaderSource(*shader, 1, (const GLchar **)&source,
		(const GLint *)&sourceLength);
	glCompileShader(*shader);

	GLint compiled;
	glGetShaderiv(*shader, GL_COMPILE_STATUS, &compiled);

	if (!compiled) {
//		rb_print_shader_log(*shader);
		glDeleteShader(*shader);
		return 0;
	}

	return 1;
}

void DxbxCompileShader(std::string Shader)
{
	int GLErrorPos;

//	if (MayLog(lfUnit))
//		DbgPrintf("  NV2A: New vertex program :\n" + Shader);

/*
glProgramStringARB(GL_VERTEX_PROGRAM_ARB, GL_PROGRAM_FORMAT_ASCII_ARB, Shader.size(), Shader.c_str());

	// errors are catched
	glGetIntegerv(GL_PROGRAM_ERROR_POSITION_ARB, &GLErrorPos);
	*/
	GLuint shader;

	GLErrorPos = rb_compile_shader(Shader.c_str(), GL_VERTEX_SHADER, &shader); // TODO : GL_VERTEX_SHADER_ARB ??
	/*
	if (GLErrorPos > 0) 
	{
		Shader.insert(GLErrorPos, "{ERROR}");
		EmuWarning("Program error at position %d:", GLErrorPos);
		EmuWarning((char*)glGetString(GL_PROGRAM_ERROR_STRING_ARB));
		EmuWarning(Shader.c_str());
	}
	*/
}
void InitOpenGLContext()
{
	HGLRC RC;
	std::string szCode;

	g_EmuWindowsDC = GetDC(g_hEmuWindow); // Actually, you can use any windowed control here
	SetupPixelFormat(g_EmuWindowsDC);

	RC = wglCreateContext(g_EmuWindowsDC); // makes OpenGL window out of DC
	wglMakeCurrent(g_EmuWindowsDC, RC);   // makes OpenGL window active
	//ReadImplementationProperties(); // Determine a set of booleans indicating which OpenGL extensions are available
	//ReadExtensions(); // Assign all OpenGL extension API's (DON'T call them if the extension is not available!)

	// Initialize the viewport :
	//Viewport.X = 0;
	//Viewport.Y = 0;
	//Viewport.Width = g_EmuCDPD.pPresentationParameters.BackBufferWidth;
	//Viewport.Height = g_EmuCDPD.pPresentationParameters.BackBufferHeight;
	//Viewport.MinZ = -1.0;
	//Viewport.MaxZ = 1.0;

	//DxbxUpdateTransformProjection();
	//DxbxUpdateViewport();


	//glutInit();
	{ // rb_init_context();
	  /* link in gl functions at runtime */
		glewExperimental = GL_TRUE;
		GLenum err = glewInit();
		if (err != GLEW_OK) {
			EmuWarning("GLEW initialization failed: %s", glewGetErrorString(err));
			return;
		}
	}

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	// Switch to left-handed coordinate space (as per http://www.opengl.org/resources/faq/technical/transformations.htm) :
	//  glScalef(1.0, 1.0, -1.0);

	// Set some defaults :
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL); // Nearer Z coordinates cover further Z

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glAlphaFunc(GL_GEQUAL, 0.5);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // GL_LINE for wireframe

/*
	// TODO : The following code only works on cards that support the
	// vertex program extensions (NVidia cards mainly); So for ATI we
	// have to come up with another solution !!!
	glGenProgramsARB(4, &VertexProgramIDs[0]);
*/

#ifdef DXBX_OPENGL_CONVENTIONAL
	if (GL_ARB_vertex_blend)
		DxbxVertexShaderHeader = sprintf(DxbxVertexShaderHeader, "weight");
	else
		DxbxVertexShaderHeader = sprintf(DxbxVertexShaderHeader, "attrib[1]");
#endif

	// Precompiled shader for the fixed function pipeline :
	szCode = DxbxVertexShaderHeader +
		"# This part adjusts the vertex position by the super-sampling scale & offset :\n"
		"MOV R0, v0;\n"
		"RCP R0.w, R0.w;\n"
		"MUL R0, R0, c[0];\n" // c[-96] in D3D speak - applies SuperSampleScale
								// Note : Use R12 instead of oPos because this is not yet the final assignment :
		"ADD R12, R0, c[1];\n" // c[-95] in D3D speak - applies SuperSampleOffset

		"# This part just reads all other components and passes them to the output :\n"
		"MOV oD0, v3;\n"
		"MOV oD1, v4;\n"
		"MOV oFog, v4.w;\n" // specular fog
							  //    "MOV oFog, v0.z;\n" // z fog
							  //    "RCP oFog, v0.w;\n" // w fog
		"MOV oPts, v1.x;\n"
		"MOV oB0, v7;\n"
		"MOV oB1, v8;\n"
		"MOV oT0, v9;\n"
		"MOV oT1, v10;\n"
		"MOV oT2, v11;\n"
		"MOV oT3, v12;\n"

		"# This part applies the screen-space transform (not present when '#pragma screenspace' was used) :\n"
		"MUL R12.xyz, R12, c[58];\n" // c[-38] in D3D speak - see EmuNV2A_ViewportScale,
		"RCP R1.x, R12.w;\n" // Originally RCC, but that"s not supported in ARBvp1.0 (use "MIN R1, R1, 0" and "MAX R1, R1, 1"?)
		"MAD R12.xyz, R12, R1.x, c[59];\n" // c[-37] in D3D speak - see EmuNV2A_ViewportOffset

		"# Dxbx addition : Transform the vertex to clip coordinates :\n"
		"DP4 R0.x, mvp[0], R12;\n"
		"DP4 R0.y, mvp[1], R12;\n"
		"DP4 R0.z, mvp[2], R12;\n"
		"DP4 R0.w, mvp[3], R12;\n"
		"MOV R12, R0;\n"

		"# Apply Z coord mapping\n"
		"ADD R12.z, R12.z, R12.z;\n"
		"ADD R12.z, R12.z, -R12.w;\n"

		"# Here""s the final assignment to oPos :\n"
		"MOV oPos, R12;\n"
		"END\n"; // TODO : Check if newline is required?

//	glBindProgramARB(GL_VERTEX_PROGRAM_ARB, VertexProgramIDs[0]);
	DxbxCompileShader(szCode);
}

// HACK: Until we implement VGA/proper interrupt generation
// we simulate VBLANK by calling the interrupt at 60Hz
std::thread vblank_thread;
extern clock_t GetNextVBlankTime();
static void nv2a_vblank_thread()
{
	clock_t nextVBlankTime = GetNextVBlankTime();

	while (true) {
		// Handle VBlank
		if (clock() > nextVBlankTime) {
			pcrtc.pending_interrupts |= NV_PCRTC_INTR_0_VBLANK;
			update_irq();
			nextVBlankTime = GetNextVBlankTime();
		}
	}
}

void EmuNV2A_Init()
{
	// Allocate PRAMIN Region
	VirtualAlloc((void*)(NV2A_ADDR + NV_PRAMIN_ADDR), NV_PRAMIN_SIZE, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

	pcrtc.start = 0;

	pramdac.core_clock_coeff = 0x00011c01; /* 189MHz...? */
	pramdac.core_clock_freq = 189000000;
	pramdac.memory_clock_coeff = 0;
	pramdac.video_clock_coeff = 0x0003C20D; /* 25182Khz...? */

	pfifo.puller_thread = std::thread(pfifo_puller_thread);
	
	vblank_thread = std::thread(nv2a_vblank_thread);;
}
