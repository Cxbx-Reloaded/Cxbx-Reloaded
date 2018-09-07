// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   src->devices->video->nv2a.h
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
// *  (c) 2016-2018 Luke Usher <luke.usher@outlook.com>
// *  (c) 2018 Patrick van Logchem <pvanlogchem@gmail.com>
// *
// *  All rights reserved
// *
// ******************************************************************
#pragma once

#undef USE_SHADER_CACHE

#ifdef USE_SHADER_CACHE
#include "glib_compat.h" // For GHashTable, g_hash_table_new, g_hash_table_lookup, g_hash_table_insert
#endif

#include "Cxbx.h" // For xbaddr
#include "devices\PCIDevice.h" // For PCIDevice

#include <queue>
#include <thread>

#include <GL/glew.h>

#include "swizzle.h"
#include "nv2a_int.h"
#include "nv2a_debug.h" // For HWADDR_PRIx, NV2A_DPRINTF, NV2A_GL_DPRINTF, etc.
#include "CxbxKrnl/gloffscreen/gloffscreen.h"
#include "qemu-thread.h" // For qemu_mutex, etc
#include "nv2a_shaders.h" // For ShaderBinding

#define NV_PMC_SIZE                 0x001000
#define _NV_PFIFO_SIZE              0x002000 // Underscore prefix to prevent clash with NV_PFIFO_SIZE
#define NV_PVIDEO_SIZE              0x001000
#define NV_PTIMER_SIZE              0x001000
#define NV_PFB_SIZE                 0x001000
#define NV_PGRAPH_SIZE              0x002000
#define NV_PCRTC_SIZE               0x001000
#define NV_PRAMDAC_SIZE             0x001000

typedef xbaddr hwaddr; // Compatibility; Cxbx uses xbaddr, xqemu and OpenXbox use hwaddr 
typedef uint32_t value_t; // Compatibility; Cxbx values are uint32_t (xqemu and OpenXbox use uint64_t)

#ifdef __cplusplus
template <size_t N> struct ArraySizeHelper { char _[N]; };
template <typename T, size_t N>
ArraySizeHelper<N> makeArraySizeHelper(T(&)[N]);
#  define ARRAY_SIZE(a)  sizeof(makeArraySizeHelper(a))
#else
// The expression ARRAY_SIZE(a) is a compile-time constant of type
// size_t which represents the number of elements of the given
// array. You should only use ARRAY_SIZE on statically allocated
// arrays.

#define ARRAY_SIZE(a)                               \
  ((sizeof(a) / sizeof(*(a))) /                     \
  static_cast<size_t>(!(sizeof(a) % sizeof(*(a)))))
#endif

#define VSH_TOKEN_SIZE 4 // Compatibility; TODO : Move this to nv2a_vsh.h
#define MAX(a,b) ((a)>(b) ? (a) : (b)) // Compatibility
#define MIN(a,b) ((a)<(b) ? (a) : (b)) // Compatibility

#define g_free(x) free(x) // Compatibility
#define g_malloc(x) malloc(x) // Compatibility
#define g_malloc0(x) calloc(1, x) // Compatibility
#define g_realloc(x, y) realloc(x, y) // Compatibility

#undef USE_TEXTURE_CACHE

#if __cplusplus >= 201402L
#  define NV2A_CONSTEXPR constexpr
#else
#  define NV2A_CONSTEXPR static
#endif

// GCC implementation of FFS
static int ffs(register int valu)
{
	register int bit;

	if (valu == 0)
		return 0;

	for (bit = 1; !(valu & 1); bit++)
		valu >>= 1;

	return bit;
}

#define GET_MASK(v, mask) (((v) & (mask)) >> (ffs(mask)-1))

#define SET_MASK(v, mask, val)                                       \
    do {                                                             \
        (v) &= ~(mask);                                              \
        (v) |= ((val) << (ffs(mask)-1)) & (mask);                    \
    } while (0)

// Power-of-two CASE statements
#define CASE_1(v, step) case (v)
#define CASE_2(v, step) CASE_1(v, step) : CASE_1(v + (step) * 1, step)
#define CASE_4(v, step) CASE_2(v, step) : CASE_2(v + (step) * 2, step)
#define CASE_8(v, step) CASE_4(v, step) : CASE_4(v + (step) * 4, step)
#define CASE_16(v, step) CASE_8(v, step) : CASE_8(v + (step) * 8, step)
#define CASE_32(v, step) CASE_16(v, step) : CASE_16(v + (step) * 16, step)
#define CASE_64(v, step) CASE_32(v, step) : CASE_32(v + (step) * 32, step)
#define CASE_128(v, step) CASE_64(v, step) : CASE_64(v + (step) * 64, step)

// Non-power-of-two CASE statements
#define CASE_3(v, step) CASE_2(v, step) : CASE_1(v + (step) * 2, step)
#define CASE_12(v, step) CASE_8(v, step) : CASE_4(v + (step) * 8, step)
#define CASE_13(v, step) CASE_8(v, step) : CASE_3(v + (step) * 8, step)
#define CASE_28(v, step) CASE_16(v, step) : CASE_12(v + (step) * 16, step)
#define CASE_29(v, step) CASE_16(v, step) : CASE_13(v + (step) * 16, step)
#define CASE_61(v, step) CASE_32(v, step) : CASE_29(v + (step) * 32, step)
#define CASE_78(v, step) CASE_64(v, step) : CASE_12(v + (step) * 64, step)
#define CASE_125(v, step) CASE_64(v, step) : CASE_61(v + (step) * 64, step)
#define CASE_132(v, step) CASE_128(v, step) : CASE_4(v + (step) * 128, step)
#define CASE_253(v, step) CASE_128(v, step) : CASE_125(v + (step) * 128, step)

#define NV2A_DEVICE(obj) \
    OBJECT_CHECK(NV2AState, (obj), "nv2a")

//void reg_log_read(int block, hwaddr addr, uint64_t val);
//void reg_log_write(int block, hwaddr addr, uint64_t val);

enum FifoMode {
	FIFO_PIO = 0,
	FIFO_DMA = 1,
};

enum FIFOEngine {
	ENGINE_SOFTWARE = 0,
	ENGINE_GRAPHICS = 1,
	ENGINE_DVD = 2,
};

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


typedef struct PGRAPHState {
	bool opengl_enabled; // == bLLE_GPU
	QemuMutex lock;

	uint32_t pending_interrupts;
	uint32_t enabled_interrupts;
	QemuCond interrupt_cond;

	xbaddr context_table;
	xbaddr context_address;


	unsigned int trapped_method;
	unsigned int trapped_subchannel;
	unsigned int trapped_channel_id;
	uint32_t trapped_data[2];
	uint32_t notify_source;

	bool fifo_access;
	QemuCond fifo_access_cond;

	QemuCond flip_3d;

	unsigned int channel_id;
	bool channel_valid;
	GraphicsContext context[NV2A_NUM_CHANNELS];

	xbaddr dma_color, dma_zeta;
	Surface surface_color, surface_zeta;
	unsigned int surface_type;
	SurfaceShape surface_shape;
	SurfaceShape last_surface_shape;

	xbaddr dma_a, dma_b;
#ifdef USE_TEXTURE_CACHE
	GLruCache *texture_cache;
#endif
	bool texture_dirty[NV2A_MAX_TEXTURES];
	TextureBinding *texture_binding[NV2A_MAX_TEXTURES];

#ifdef USE_SHADER_CACHE
	GHashTable *shader_cache;
#endif
	ShaderBinding *shader_binding;

	bool texture_matrix_enable[NV2A_MAX_TEXTURES];

	/* FIXME: Move to NV_PGRAPH_BUMPMAT... */
	float bump_env_matrix[NV2A_MAX_TEXTURES - 1][4]; /* 3 allowed stages with 2x2 matrix each */

	GloContext *gl_context;
	QemuMutex gl_lock;

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

	uint32_t program_data[NV2A_MAX_TRANSFORM_PROGRAM_LENGTH][VSH_TOKEN_SIZE];

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

	VertexAttribute vertex_attributes[NV2A_VERTEXSHADER_ATTRIBUTES];

	unsigned int inline_array_length;
	uint32_t inline_array[NV2A_MAX_BATCH_LENGTH];
	GLuint gl_inline_array_buffer;

	unsigned int inline_elements_length;
	uint16_t inline_elements[NV2A_MAX_BATCH_LENGTH]; // Cxbx-Reloaded TODO : Restore uint32_t once HLE_draw_inline_elements can using that


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
} PGRAPHState;

#define lockGL(x) lockGL_(x, __LINE__)
static void lockGL_(PGRAPHState* pg, unsigned int line) {
	//printf("Locking from line %d\n", line);
	qemu_mutex_lock(&pg->gl_lock);
	if (pg->opengl_enabled) {
		glo_set_current(pg->gl_context);
	}
}

#define unlockGL(x) unlockGL_(x, __LINE__)
static void unlockGL_(PGRAPHState* pg, unsigned int line) {
	//printf("Unlocking from line %d\n", line);
	if (pg->opengl_enabled) {
		glo_set_current(NULL);
	}
	qemu_mutex_unlock(&pg->gl_lock);
}

typedef struct CacheEntry {
	//QSIMPLEQ_ENTRY(CacheEntry) entry;
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
	QemuMutex cache_lock;
	QemuCond cache_cond;
	std::queue<CacheEntry*> cache;
	std::queue<CacheEntry*> working_cache;
} Cache1State;

typedef struct OverlayState {
	bool video_buffer_use;
	int pitch;
	bool is_transparent;
#ifdef DEBUG
	hwaddr base;
	hwaddr limit;
#endif
	hwaddr offset;
	uint32_t in_height;
	uint32_t in_width;
	int out_x;
	int out_y;
	int out_width;
	int out_height;

	bool covers_framebuffer;
	int old_in_width;
	int old_in_height;
	int old_pitch;
	GLuint gl_texture;
} OverlayState;

typedef struct ChannelControl {
	xbaddr dma_put;
	xbaddr dma_get;
	uint32_t ref;
} ChannelControl;

typedef struct NV2AState {
    // PCIDevice dev;
    // qemu_irq irq;
    bool exiting;
	bool enable_overlay = false;

    // VGACommonState vga;
    // GraphicHwOps hw_ops;
    // QEMUTimer *vblank_timer;

    // MemoryRegion *vram;
    // MemoryRegion vram_pci;
    uint8_t *vram_ptr;
    size_t vram_size;
    // MemoryRegion ramin;
	struct {
		uint8_t *ramin_ptr;
		size_t ramin_size;
	} pramin;

    // MemoryRegion mmio;
    // MemoryRegion block_mmio[NV_NUM_BLOCKS];

    struct {
        uint32_t pending_interrupts;
        uint32_t enabled_interrupts;
		uint32_t regs[NV_PMC_SIZE]; // Not in xqemu/openxbox? TODO : union
    } pmc;

    struct {
		std::thread puller_thread;
        uint32_t pending_interrupts;
        uint32_t enabled_interrupts;
        Cache1State cache1;
		uint32_t regs[_NV_PFIFO_SIZE]; // TODO : union
    } pfifo;

    struct {
		uint32_t pending_interrupts;
		uint32_t enabled_interrupts;
		//QemuCond interrupt_cond; // pvideo.interrupt_cond not used (yet)
		OverlayState overlays[2]; // NV2A supports 2 video overlays
		uint32_t regs[NV_PVIDEO_SIZE]; // TODO : union
    } pvideo;

    struct {
        uint32_t pending_interrupts;
        uint32_t enabled_interrupts;
        uint32_t numerator;
        uint32_t denominator;
        uint32_t alarm_time;
		uint32_t regs[NV_PTIMER_SIZE]; // Not in xqemu/openxbox? TODO : union
    } ptimer;

    struct {
		uint32_t regs[NV_PFB_SIZE]; // TODO : union
    } pfb;

    struct PGRAPHState pgraph;

    struct {
        uint32_t pending_interrupts;
        uint32_t enabled_interrupts;
        hwaddr start;
		uint32_t regs[NV_PCRTC_SIZE]; // Not in xqemu/openxbox? TODO : union
    } pcrtc;

    struct {
        uint32_t core_clock_coeff;
        uint64_t core_clock_freq;
        uint32_t memory_clock_coeff;
        uint32_t video_clock_coeff;
		uint32_t regs[NV_PRAMDAC_SIZE]; // Not in xqemu/openxbox? TODO : union
    } pramdac;

    struct {
        ChannelControl channel_control[NV2A_NUM_CHANNELS];
    } user;

	// PRMCIO (Actually the VGA controller)
	struct {
		uint8_t cr_index;
		uint8_t cr[256]; /* CRT registers */
	} prmcio; // Not in xqemu/openxbox?
} NV2AState;

typedef value_t(*read_func)(NV2AState *d, hwaddr addr); //, unsigned int size);
typedef void(*write_func)(NV2AState *d, hwaddr addr, value_t val); //, unsigned int size);

typedef struct {
	read_func read;
	write_func write;
} MemoryRegionOps;

typedef struct NV2ABlockInfo {
	const char* name;
	hwaddr offset;
	uint64_t size;
	MemoryRegionOps ops;
} NV2ABlockInfo;

const NV2ABlockInfo* EmuNV2A_Block(xbaddr addr);

#if 0
// Valid after PCI init :
#define NV20_REG_BASE_KERNEL 0xFD000000

typedef volatile DWORD *PPUSH;

typedef struct {
	DWORD Ignored[0x10];
	PPUSH Put; // On Xbox1, this field is only written to by the CPU (the GPU uses this as a trigger to start executing from the given address)
	PPUSH Get; // On Xbox1, this field is only read from by the CPU (the GPU reflects in here where it is/stopped executing)
	PPUSH Reference; // TODO : xbaddr / void* / DWORD ? 
	DWORD Ignored2[0x7ED];
} Nv2AControlDma;

#define PUSH_TYPE_MASK         0x00000002 // 2 bits
#define PUSH_TYPE_SHIFT        0
#define PUSH_TYPE_METHOD        0 // method
#define PUSH_TYPE_JMP_FAR       1 // jump far
#define PUSH_TYPE_CALL_FAR      2 // call far
#define PUSH_TYPE_METHOD_UNUSED 3 // method (unused)
#define PUSH_METHOD_MASK       0x00001FFC // 12 bits
#define PUSH_METHOD_SHIFT      0 // Dxbx note : Not 2, because methods are actually DWORD offsets (and thus defined with increments of 4)
#define PUSH_SUBCH_MASK        0x0000E000 // 3 bits
#define PUSH_SUBCH_SHIFT       13
#define PUSH_COUNT_MASK        0x1FFC0000 // 11 bits
#define PUSH_COUNT_SHIFT       18
#define PUSH_INSTR_MASK        0xE0000000 // 3 bits
#define PUSH_INSTR_SHIFT       29
#define PUSH_INSTR_IMM_INCR     0 // immediate, increment
#define PUSH_INSTR_JMP_NEAR     1 // near jump
#define PUSH_INSTR_IMM_NOINC    2 // immediate, no-increment
#define PUSH_ADDR_FAR_MASK     0xFFFFFFFC // 30 bits
#define PUSH_ADDR_FAR_SHIFT    0
#define PUSH_ADDR_NEAR_MASK    0x1FFFFFFC // 27 bits
#define PUSH_ADDR_NEAR_SHIFT   0 // Cxbx note : Not 2, because methods are actually DWORD offsets (and thus defined with increments of 4)

#define PUSH_TYPE(dwPushCommand) ((dwPushCommand & PUSH_TYPE_MASK) >> PUSH_TYPE_SHIFT)
#define PUSH_METHOD(dwPushCommand) ((dwPushCommand & PUSH_METHOD_MASK) >> PUSH_METHOD_SHIFT)
#define PUSH_SUBCH(dwPushCommand) ((dwPushCommand & PUSH_SUBCH_MASK) >> PUSH_SUBCH_SHIFT)
#define PUSH_COUNT(dwPushCommand) ((dwPushCommand & PUSH_COUNT_MASK) >> PUSH_COUNT_SHIFT)
#define PUSH_INSTR(dwPushCommand) ((dwPushCommand & PUSH_INSTR_MASK) >> PUSH_INSTR_SHIFT)
#define PUSH_ADDR_FAR(dwPushCommand) ((dwPushCommand & PUSH_ADDR_FAR_MASK) >> PUSH_ADDR_FAR_SHIFT)
#define PUSH_ADDR_NEAR(dwPushCommand) ((dwPushCommand & PUSH_ADDR_NEAR_MASK) >> PUSH_ADDR_NEAR_SHIFT)

#define PUSH_METHOD_MAX ((PUSH_METHOD_MASK | 3) >> PUSH_METHOD_SHIFT) // = 8191
#define PUSH_SUBCH_MAX (PUSH_SUBCH_MASK >> PUSH_SUBCH_SHIFT) // = 7
#define PUSH_COUNT_MAX (PUSH_COUNT_MASK >> PUSH_COUNT_SHIFT) // = 2047

// Decode push buffer conmmand (inverse of D3DPUSH_ENCODE)
inline void D3DPUSH_DECODE(const DWORD dwPushCommand, DWORD &dwMethod, DWORD &dwSubCh, DWORD &dwCount)
{
	dwMethod = PUSH_METHOD(dwPushCommand);
	dwSubCh = PUSH_SUBCH(dwPushCommand);
	dwCount = PUSH_COUNT(dwPushCommand);
}
#endif

void CxbxReserveNV2AMemory(NV2AState *d);

GLuint create_gl_shader(GLenum gl_shader_type,
	const char *code,
	const char *name); // forward to nv2a_shaders.cpp

class NV2ADevice : public PCIDevice {
public:
	// constructor
	NV2ADevice();
	// destructor
	~NV2ADevice();

	// PCI Device functions
	void Init();
	void Reset();

	// State Getter: Used for HLE reading of device state
	NV2AState* GetDeviceState() { return m_nv2a_state; };

	uint32_t IORead(int barIndex, uint32_t port, unsigned size);
	void IOWrite(int barIndex, uint32_t port, uint32_t value, unsigned size);
	uint32_t BlockRead(const NV2ABlockInfo* block, uint32_t addr, unsigned size);
	uint32_t MMIORead(int barIndex, uint32_t addr, unsigned size);
	void BlockWrite(const NV2ABlockInfo* block, uint32_t addr, uint32_t value, unsigned size);
	void MMIOWrite(int barIndex, uint32_t addr, uint32_t value, unsigned size);

	static void UpdateHostDisplay(NV2AState *d);
private:
	NV2AState *m_nv2a_state;
};
