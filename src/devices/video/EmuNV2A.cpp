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
#define _XBOXKRNL_DEFEXTRN_

#define LOG_PREFIX "NV2A"

#include <process.h> // For __beginthreadex(), etc.
#include "vga.h"

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

#include "CxbxKrnl\CxbxKrnl.h"
#include "CxbxKrnl\Emu.h"
#include "CxbxKrnl\EmuFS.h"
#include "CxbxKrnl\EmuKrnl.h"
#include "CxbxKrnl\HLEIntercept.h"

#include "EmuNV2A.h"
#include "nv2a.h" // For NV2AState
#include "nv2a_int.h" // from https://github.com/espes/xqemu/tree/xbox/hw/xbox

//#include <gl\glew.h>
#include <gl\GL.h>
#include <gl\GLU.h>
#include <cassert>
//#include <gl\glut.h>


static void update_irq(NV2AState *d)
{
	/* PFIFO */
	if (d->pfifo.pending_interrupts & d->pfifo.enabled_interrupts) {
		d->pmc.pending_interrupts |= NV_PMC_INTR_0_PFIFO;
	} else {
		d->pmc.pending_interrupts &= ~NV_PMC_INTR_0_PFIFO;
	}

	/* PCRTC */
	if (d->pcrtc.pending_interrupts & d->pcrtc.enabled_interrupts) {
		d->pmc.pending_interrupts |= NV_PMC_INTR_0_PCRTC;
	} else {
		d->pmc.pending_interrupts &= ~NV_PMC_INTR_0_PCRTC;
	}

	/* PGRAPH */
	if (d->pgraph.pending_interrupts & d->pgraph.enabled_interrupts) {
		d->pmc.pending_interrupts |= NV_PMC_INTR_0_PGRAPH;
	} else {
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
	} else {
		HalSystemInterrupts[3].Assert(false);
	}

	SwitchToThread();
}


#include "EmuNV2A_DEBUG.cpp"


#define DEBUG_READ32(DEV)            DbgPrintf("X86 : Rd32 NV2A " #DEV "(0x%08X) = 0x%08X [Handled %s]\n", addr, result, DebugNV_##DEV##(addr))
#define DEBUG_READ32_UNHANDLED(DEV)  { DbgPrintf("X86 : Rd32 NV2A " #DEV "(0x%08X) = 0x%08X [Unhandled %s]\n", addr, result, DebugNV_##DEV##(addr)); return result; }

#define DEBUG_WRITE32(DEV)           DbgPrintf("X86 : Wr32 NV2A " #DEV "(0x%08X, 0x%08X) [Handled %s]\n", addr, value, DebugNV_##DEV##(addr))
#define DEBUG_WRITE32_UNHANDLED(DEV) { DbgPrintf("X86 : Wr32 NV2A " #DEV "(0x%08X, 0x%08X) [Unhandled %s]\n", addr, value, DebugNV_##DEV##(addr)); return; }

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

static inline void stl_le_p(uint32_t *p, uint32 v)
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
	assert(dma_obj_address < d->pramin.ramin_size);

	DMAObject dma = nv_dma_load(d, dma_obj_address);

	/* TODO: Handle targets and classes properly */
	NV2A_DPRINTF("dma_map %x, %x, %x %x\n",
		dma.dma_class, dma.dma_target, dma.address, dma.limit);

	dma.address &= 0x07FFFFFF;

	// assert(dma.address + dma.limit < memory_region_size(d->vram));
	*len = dma.limit;
	return (void*)(PHYSICAL_MAP_BASE  + dma.address);
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
	// Terminating entry
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

NV2AState g_nv2a_state;
NV2AState *d = &g_nv2a_state;

uint32_t EmuNV2A_Read(xbaddr addr, int size)
{
	const NV2ABlockInfo* block = EmuNV2A_Block(addr);

	if (block != nullptr) {
		switch (size) {
			case sizeof(uint8_t):
				return block->ops.read(d, addr - block->offset) & 0xFF;
			case sizeof(uint16_t) :
				return block->ops.read(d, addr - block->offset) & 0xFFFF;
			case sizeof(uint32_t) :
				return block->ops.read(d, addr - block->offset);
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
			case sizeof(uint8_t) :
				shift = (addr & 3) * 8;
				aligned_addr = addr & ~3;
				aligned_value = block->ops.read(d, aligned_addr - block->offset);
				mask = 0xFF << shift;

				// TODO : Must the second byte be written to the next DWORD?		
				block->ops.write(d, aligned_addr - block->offset, (aligned_value & ~mask) | (value << shift));
				return;
			case sizeof(uint16_t) :
				assert((addr & 1) == 0);
				
				shift = (addr & 2) * 16;
				aligned_addr = addr & ~3;
				aligned_value = block->ops.read(d, addr - block->offset);
				mask = 0xFFFF << shift;
				
				// TODO : Must the second byte be written to the next DWORD?		
				block->ops.write(d, aligned_addr - block->offset, (aligned_value & ~mask) | (value << shift));
				return;
			case sizeof(uint32_t) :
				block->ops.write(d, addr - block->offset, value);
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
extern std::chrono::time_point<std::chrono::steady_clock, std::chrono::duration<double, std::nano>> GetNextVBlankTime();
static void nv2a_vblank_thread(NV2AState *d)
{
	CxbxSetThreadName("Cxbx NV2A VBlank");

	auto nextVBlankTime = GetNextVBlankTime();

	while (true) {
		// Handle VBlank
		if (std::chrono::steady_clock::now() > nextVBlankTime) {
			d->pcrtc.pending_interrupts |= NV_PCRTC_INTR_0_VBLANK;
			update_irq(d);
			nextVBlankTime = GetNextVBlankTime();
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

void EmuNV2A_Init()
{
	CxbxReserveNV2AMemory(d);

	d->pcrtc.start = 0;

	d->vram_ptr = (uint8_t*)PHYSICAL_MAP_BASE;
	d->vram_size = g_SystemMaxMemory;

	d->pramdac.core_clock_coeff = 0x00011c01; /* 189MHz...? */
	d->pramdac.core_clock_freq = 189000000;
	d->pramdac.memory_clock_coeff = 0;
	d->pramdac.video_clock_coeff = 0x0003C20D; /* 25182Khz...? */

	d->pfifo.puller_thread = std::thread(pfifo_puller_thread, d);
	
	// Only spawn VBlank thread when LLE is enabled
	if (bLLE_GPU) {
		vblank_thread = std::thread(nv2a_vblank_thread, d);
	}
}
