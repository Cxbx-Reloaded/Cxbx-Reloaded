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
// *   src->devices->video->nv2a.cpp
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
// *  nv2a.cpp is heavily based on code from XQEMU
// *  Copyright(c) 2012 espes
// *  Copyright(c) 2015 Jannik Vogel
// *  https://github.com/espes/xqemu/blob/xbox/hw/xbox/nv2a.c
// *  (c) 2017-2018 Luke Usher <luke.usher@outlook.com>
// *  (c) 2018 Patrick van Logchem <pvanlogchem@gmail.com>
// *
// *  All rights reserved
// *
// ******************************************************************
#define _XBOXKRNL_DEFEXTRN_

#define LOG_PREFIX "NV2A"

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

#include <string> // For std::string
#include <distorm.h> // For uint32_t
#include <process.h> // For __beginthreadex(), etc.

#include "CxbxKrnl\CxbxKrnl.h" // For XBOX_MEMORY_SIZE, DWORD, etc
#include "CxbxKrnl\Emu.h"
#include "CxbxKrnl\EmuFS.h"
#include "CxbxKrnl\EmuKrnl.h"
#include "CxbxKrnl\HLEIntercept.h"
#include "CxbxKrnl\EmuKrnlAvModes.h"

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

#include "CxbxKrnl/gloffscreen/glextensions.h" // for glextensions_init


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

	SwitchToThread();
}


#include "EmuNV2A_DEBUG.cpp"


#define DEBUG_READ32(DEV)              DbgPrintf("X86 : Read32  NV2A " #DEV "(0x%08X) = 0x%08X [Handle%s]\n", addr, result, DebugNV_##DEV##(addr))
#define DEBUG_READ32_UNHANDLED(DEV)  { DbgPrintf("X86 : Read32  NV2A " #DEV "(0x%08X) = 0x%08X [Unhandle%s]\n", addr, result, DebugNV_##DEV##(addr)); return result; }

#define DEBUG_WRITE32(DEV)             DbgPrintf("X86 : Write32 NV2A " #DEV "(0x%08X, 0x%08X) [Handle%s]\n", addr, value, DebugNV_##DEV##(addr))
#define DEBUG_WRITE32_UNHANDLED(DEV) { DbgPrintf("X86 : Write32 NV2A " #DEV "(0x%08X, 0x%08X) [Unhandle%s]\n", addr, value, DebugNV_##DEV##(addr)); return; }

#define DEVICE_READ32(DEV) uint32_t EmuNV2A_##DEV##_Read32(NV2AState *d, xbaddr addr)
#define DEVICE_READ32_SWITCH() uint32_t result = 0; switch (addr) 
#define DEVICE_READ32_REG(dev) result = d->dev.regs[addr]
#define DEVICE_READ32_END(DEV) DEBUG_READ32(DEV); return result

#define DEVICE_WRITE32(DEV) void EmuNV2A_##DEV##_Write32(NV2AState *d, xbaddr addr, uint32_t value)
#define DEVICE_WRITE32_REG(dev) d->dev.regs[addr] = value
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

static DMAObject nv_dma_load(NV2AState *d, xbaddr dma_obj_address)
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

static void *nv_dma_map(NV2AState *d, xbaddr dma_obj_address, xbaddr *len)
{
	DMAObject dma = nv_dma_load(d, dma_obj_address);

	/* TODO: Handle targets and classes properly */
	NV2A_DPRINTF("dma_map %x, %x, %x %x\n",
		dma.dma_class, dma.dma_target, dma.address, dma.limit);

	dma.address &= 0x07FFFFFF;

	// assert(dma.address + dma.limit < memory_region_size(d->vram));
	*len = dma.limit;
	return d->vram_ptr + dma.address;
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

const NV2ABlockInfo* EmuNV2A_Block(xbaddr addr)
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
extern std::chrono::time_point<std::chrono::steady_clock, std::chrono::duration<double, std::nano>> GetNextVBlankTime();

extern ULONG AvpCurrentMode;		// Current AV Mode
extern ULONG g_AvDisplayModeFormat;		// Current AV FrameBuffer Format

void AvDisplayModeFormatToGL(ULONG displayModeFormat, GLenum* internalFormat, GLenum* format, GLenum* type)
{
#define D3DFMT_LIN_A1R5G5B5   0x00000010
#define D3DFMT_LIN_X1R5G5B5   0x0000001C
#define D3DFMT_LIN_R5G6B5     0x00000011
#define D3DFMT_LIN_A8R8G8B8   0x00000012
#define D3DFMT_LIN_X8R8G8B8   0x0000001E

	switch (displayModeFormat) {
	case D3DFMT_LIN_A1R5G5B5:
		*internalFormat = GL_RGB5_A1;
		*format = GL_BGRA;
		*type = GL_UNSIGNED_SHORT_1_5_5_5_REV;
		break;
	case D3DFMT_LIN_X1R5G5B5:
		*internalFormat = GL_RGB5;
		*format = GL_BGRA;
		*type = GL_UNSIGNED_SHORT_1_5_5_5_REV;
		break;
	case D3DFMT_LIN_R5G6B5:
		*internalFormat = GL_RGB;
		*format = GL_RGB;
		*type = GL_UNSIGNED_SHORT_5_6_5;
		break;
	case D3DFMT_LIN_X8R8G8B8:
		*internalFormat = GL_RGB8;
		*format = GL_BGRA;
		*type = GL_UNSIGNED_INT_8_8_8_8_REV;
		break;
	case D3DFMT_LIN_A8R8G8B8:
		*internalFormat = GL_RGBA8;
		*format = GL_BGRA;
		*type = GL_UNSIGNED_INT_8_8_8_8_REV;
		break;
	default:
		*internalFormat = GL_RGBA;
		*format = GL_RGBA;
		*type = GL_UNSIGNED_INT_8_8_8_8_REV;
	}
}

void AvGetFormatSize(ULONG mode, int* width, int* height)
{
	// Iterate through the display mode table until we find a matching mode
	for (unsigned int i = 0; i < g_DisplayModeCount; i++) {
		if (g_DisplayModes[i].DisplayMode == mode) {
			*width = g_DisplayModes[i].Width;
			*height = g_DisplayModes[i].Height;
			return;
		}
	}

	// if we couldn't find a valid mode, fallback to 640x480
	*width = 640;
	*height = 480;
}

void NV2ADevice::SwapBuffers(NV2AState *d)
{
	lockGL(&d->pgraph);

	NV2A_GL_DGROUP_BEGIN("VGA Frame");

	static ULONG PreviousAvDisplayModeFormat = 0;
	static GLenum internalFormat = GL_RGBA;
	static GLenum format = GL_RGBA;
	static GLenum type = GL_UNSIGNED_INT_8_8_8_8;
	static int framebufferWidth = 640;
	static int framebufferHeight = 480;

	static GLuint texture = -1;

	// Convert AV Format to OpenGl format details & destroy the texture if format changed..
	// This is required for titles that use a non ARGB framebuffer, such was Beats of Rage
	if (g_AvDisplayModeFormat != PreviousAvDisplayModeFormat) {
		AvDisplayModeFormatToGL(g_AvDisplayModeFormat, &internalFormat, &format, &type);
		AvGetFormatSize(AvpCurrentMode, &framebufferWidth, &framebufferHeight);
		if (texture != -1) {
			glDeleteTextures(1, &texture);
			texture = -1;
		}
	}
	
	// If we need to create a new texture, do so, otherwise, update the existing
	if (texture == -1) {
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, framebufferWidth, framebufferHeight, 0, format, type, (void*)(0x80000000 | d->pcrtc.start));
	} else {
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, framebufferWidth, framebufferHeight, format, type, (void*)(0x80000000 | d->pcrtc.start));
	}

	PreviousAvDisplayModeFormat = g_AvDisplayModeFormat;

	// If we need to create an OpenGL framebuffer, do so
	static GLuint framebuffer = -1;
	if (framebuffer == -1) {
		glGenFramebuffers(1, &framebuffer);
	}

	// Draw to screen..
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	// TODO: Use window size/actual framebuffer size rather than hard coding 640x480
	glBlitFramebuffer(0, 0, framebufferWidth, framebufferHeight, 0, 480, 640, 0, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	glo_swap(d->pgraph.gl_context);

	// Restore previous framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, d->pgraph.gl_framebuffer);

	NV2A_GL_DGROUP_END();
	unlockGL(&d->pgraph);

}

// TODO: Fix this properl
static void nv2a_vblank_thread(NV2AState *d)
{
	CxbxSetThreadName("Cxbx NV2A VBLANK");
	auto nextVBlankTime = GetNextVBlankTime();

	while (true) {
		// Handle VBlank
		if (std::chrono::steady_clock::now() > nextVBlankTime) {
			d->pcrtc.pending_interrupts |= NV_PCRTC_INTR_0_VBLANK;
			update_irq(d);
			nextVBlankTime = GetNextVBlankTime();

			// TODO: We should swap here for the purposes of supporting overlays + direct framebuffer access
			// But it causes crashes on AMD hardware for reasons currently unknown...
			//NV2ADevice::SwapBuffers(d);
		}
	}
}

// See NV2ABlockInfo regions[] PRAMIN
#define NV_PRAMIN_ADDR   0x00700000
#define NV_PRAMIN_SIZE              0x100000

void CxbxReserveNV2AMemory(NV2AState *d)
{
	// Reserve NV2A memory :
	void *memory = (void *)VirtualAllocEx(
		GetCurrentProcess(),
		(void *)NV2A_ADDR,
		NV2A_SIZE,
		MEM_RESERVE, // Don't allocate actual physical storage in memory
		PAGE_NOACCESS); // Any access must result in an access violation exception (handled in EmuException/EmuX86_DecodeException)
	if (memory == NULL) {
		EmuWarning("Couldn't reserve NV2A memory, continuing assuming we'll receive (and handle) access violation exceptions anyway...");
		return;
	}

	printf("[0x%.4X] INIT: Reserved %d MiB of Xbox NV2A memory at 0x%.8X to 0x%.8X\n",
		GetCurrentThreadId(), NV2A_SIZE / ONE_MB, NV2A_ADDR, NV2A_ADDR + NV2A_SIZE - 1);

	// Allocate PRAMIN Region
	d->pramin.ramin_size = NV_PRAMIN_SIZE;
	d->pramin.ramin_ptr = (uint8_t*)VirtualAllocEx(
		GetCurrentProcess(),
		(void*)(NV2A_ADDR + NV_PRAMIN_ADDR),
		d->pramin.ramin_size,
		MEM_COMMIT, // No MEM_RESERVE |
		PAGE_READWRITE);
	if (d->pramin.ramin_ptr == NULL) {
		EmuWarning("Couldn't allocate NV2A PRAMIN memory");
		return;
	}

	printf("[0x%.4X] INIT: Allocated %d MiB of Xbox NV2A PRAMIN memory at 0x%.8X to 0x%.8X\n",
		GetCurrentThreadId(), d->pramin.ramin_size / ONE_MB, d->pramin.ramin_ptr, d->pramin.ramin_ptr + d->pramin.ramin_size - 1);
}

/* NV2ADevice */

NV2ADevice::NV2ADevice()
{
	m_nv2a_state = new NV2AState();
	m_nv2a_state->pgraph.opengl_enabled = bLLE_GPU;
}

NV2ADevice::~NV2ADevice()
{
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
	qemu_mutex_init(&d->pfifo.cache1.cache_lock);
	qemu_cond_init(&d->pfifo.cache1.cache_cond);

	pgraph_init(m_nv2a_state);

	// Only spawn VBlank thread when LLE is enabled
	if (bLLE_GPU) {
		vblank_thread = std::thread(nv2a_vblank_thread, m_nv2a_state);
	}

	d->pfifo.puller_thread = std::thread(pfifo_puller_thread, d);
}

void NV2ADevice::Reset()
{
}

uint32_t NV2ADevice::IORead(int barIndex, uint32_t port, unsigned size)
{
	return 0;
}

void NV2ADevice::IOWrite(int barIndex, uint32_t port, uint32_t value, unsigned size)
{
}

uint32_t NV2ADevice::MMIORead(int barIndex, uint32_t addr, unsigned size)
{ 

	switch (barIndex) {
	case 0: {
		// Access NV2A regardless weither HLE is disabled or not (ignoring bLLE_GPU)
		const NV2ABlockInfo* block = EmuNV2A_Block(addr);

		if (block != nullptr) {
			switch (size) {
			case sizeof(uint8_t) :
				return block->ops.read(m_nv2a_state, addr - block->offset) & 0xFF;
			case sizeof(uint16_t) :
				assert((addr & 1) == 0); // TODO : What if this fails?	

				return block->ops.read(m_nv2a_state, addr - block->offset) & 0xFFFF;
			case sizeof(uint32_t) :
				assert((addr & 3) == 0); // TODO : What if this fails?	

				return block->ops.read(m_nv2a_state, addr - block->offset);
			}
		}
		break;
	}
	case 1: {
		// TODO : access physical memory
		break;
	}
	}

	EmuWarning("NV2ADevice::MMIORead: Unhandled barIndex %d, addr %08X, size %d", barIndex, addr, size);
	return 0;
}

void NV2ADevice::MMIOWrite(int barIndex, uint32_t addr, uint32_t value, unsigned size)
{
	switch (barIndex) {
	case 0: {
		// Access NV2A regardless whether HLE is disabled or not (ignoring bLLE_GPU)
		const NV2ABlockInfo* block = EmuNV2A_Block(addr);

		if (block != nullptr) {
			xbaddr aligned_addr;
			uint32_t aligned_value;
			int shift;
			uint32_t mask;

			switch (size) {
			case sizeof(uint8_t) :
				aligned_addr = addr & ~3;
				aligned_value = block->ops.read(m_nv2a_state, aligned_addr - block->offset);
				shift = (addr & 3) * 8;
				mask = 0xFF << shift;
				block->ops.write(m_nv2a_state, aligned_addr - block->offset, (aligned_value & ~mask) | (value << shift));
				return;
			case sizeof(uint16_t) :
				assert((addr & 1) == 0); // TODO : What if this fails?				

				aligned_addr = addr & ~3;
				aligned_value = block->ops.read(m_nv2a_state, aligned_addr - block->offset);
				shift = (addr & 2) * 16;
				mask = 0xFFFF << shift;				
				block->ops.write(m_nv2a_state, aligned_addr - block->offset, (aligned_value & ~mask) | (value << shift));
				return;
			case sizeof(uint32_t) :
				assert((addr & 3) == 0); // TODO : What if this fails?	

				block->ops.write(m_nv2a_state, addr - block->offset, value);
				return;
			}
		}

		break;	
	}
	case 1: {
		// TODO : access physical memory
		break;
	}
	}

	EmuWarning("NV2ADevice::MMIOWrite: Unhandled barIndex %d, addr %08X, value %08X, size %d", barIndex, addr, value, size);
}
