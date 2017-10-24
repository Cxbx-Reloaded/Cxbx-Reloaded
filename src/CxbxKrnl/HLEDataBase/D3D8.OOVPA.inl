// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->HLEDatabase->D3D8.OOVPA.inl
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
// *  (c) 2017 jarupxx
// *
// *  All rights reserved
// *
// ******************************************************************

// Titles which did compiled with full libary version
//   [LibV] Title Name                       |  Verify   |   Comments
//-----------------------------------------------------------------------
// * [3925] Azurik                           |   100%    | Missing D3D_SetPushBufferSize due locate in BINK section, have 182/196 library.
// * [3948] Shikigami no Shiro               |   100%    | have 178/196 library.
// * [4034] Dark Summit                      |   100%    | have 113/195 library.
// * [4039] New Legends                      |   100%    | have 117/195 library.
// * [4134] Double-S.T.E.A.L                 |   100%    | have 195/202 library.
// * [4242] NFL Blitz 2002                   |   100%    | have 180/202 library.
// * [4361] Hunter The Reckoning             |   100%    | have 117/204 library.
// * [4432] RedCard 2003                     |   100%    | have 184/206 library.
// * [4531] NHL HITZ 2003                    |   100%    | have 196/206 library.
// * [4627] MLB SlugFest 2003                |   100%    | have 216/227 library.
// * [4721] Terminator - Dawn of Fate        |   100%    | have 202/227 library.
// * [4831] Frogger Beyond                   |   100%    | have 200/227 library.
// * [4928] Drihoo                           |   100%    | have 201/228 library.
// * [5028] Shikigami no Shiro Evolution     |   100%    | have 201/229 library.
// * [5120] N.U.D.E.@                        |   100%    | have 211/229 library.
// * [5233] Evil Dead                        |   100%    | have 208/230 library.
// * [5344] Gladius DEMO                     |   100%    | have 202/229 library.
// * [5455] Dinosaur Hunting                 |   100%    | have 207/229 library.
// * [5558] NHL HITZ Pro 2004                |   100%    | have 218/229 library.
// * [5659] Midway Arcade Treasures Paperboy |   100%    | have 212/229 library.
// * [5788] Digimon Battle Chronicle         |   100%    | have 210/229 library.
// * [5849] Nickelodeon Tak 2                |   100%    | have 210/229 library.

// TODO: Known D3D8 OOVPA issue list
//   * CMiniport_IsFlipPending is XREF in v1 database, any reason for this?
// * Following OOVPA revision are not verified
//   * D3DCubeTexture_GetCubeMapSurface (4721, 4928, 5233, 5455)
//   * D3DDevice_ApplyStateBlock (4039, 4721, 5028, 5120)
//   * D3DDevice_BeginPushBuffer (3948, 4039, 4242, 5028)
//   * D3DDevice_BeginStateBlock (4039, 4721, 5028, 5120)
//   * D3DDevice_BlockOnFence (4039)
//   * D3DDevice_CaptureStateBlock (4039, 4432, 4721, 4831, 5028, 5120)
//   * D3DDevice_CreateCubeTexture (4721, 4928, 5028, 5233, 5455)
//   * D3DDevice_CreateImageSurface (4721, 4928, 5028, 5120, 5233, 5455)
//   * D3DDevice_CreatePalette (4721, 4831, 4928, 5120, 5233, 5455)
//   * D3DDevice_CreateStateBlock (3948, 4039, 4242, 4432, 4721, 4928, 5028, 5120)
//   * D3DDevice_CreateTexture (4721, 4928, 5233, 5455)
//   * D3DDevice_CreateVolumeTexture (4721, 4928, 5028, 5120, 5233, 5455)
//   * D3DDevice_DeleteStateBlock (4039, 4721, 4831, 5028, 5120)
//   * D3DDevice_DrawRectPatch (3948 and higher, only use by 3925?)
//   * D3DDevice_EnableOverlay (4039)
//   * D3DDevice_EndPushBuffer (3948, 4039, 4242, 5028)
//   * D3DDevice_EndStateBlock (4039, 4721, 4831, 5028, 5120)
//   * D3DDevice_FlushVertexCache (4039)
//   * D3DDevice_GetBackBuffer (4721, 4831, 4928, 5028, 5120, 5233, 5455)
//   * D3DDevice_GetBackMaterial (4039)
//   * D3DDevice_GetLightEnable (4039)
//   * D3DDevice_GetMaterial (4039)
//   * D3DDevice_GetModelView (4039)
//   * D3DDevice_GetOverlayUpdateStatus (4039, 4242, 4432)
//   * D3DDevice_GetProjectionViewportMatrix (4039)
//   * D3DDevice_GetPushBufferOffset (3948, 4039, 4242, 5028)
//   * D3DDevice_GetRenderTarget (5028)
//   * D3DDevice_GetScissors (4039)
//   * D3DDevice_GetShaderConstantMode (4039)
//   * D3DDevice_GetTile (4039)
//   * D3DDevice_GetVertexShaderFunction (4039)
//   * D3DDevice_GetVertexShaderInput (4039)
//   * D3DDevice_GetVertexShaderType (4039)
//   * D3DDevice_InsertCallback (4039)
//   * D3DDevice_IsFencePending (4039)
//   * D3DDevice_LoadVertexShaderProgram (4039)
//   * D3DDevice_PrimeVertexCache (4039)
//   * D3DDevice_RunPushBuffer (5028)
//   * D3DDevice_RunVertexStateShader (4039)
//   * D3DDevice_SetBackBufferScale (4039)
//   * D3DDevice_SetBackMaterial (4039)
//   * D3DDevice_SetModelView (4039)
//   * D3DDevice_SetPixelShaderProgram (4039)
//   * D3DDevice_SetVertexData2s (4039)
//   * D3DDevice_SetVertexData4s (4039)
//   * D3DDevice_SetVertexData4ub (4039)
//   * D3DDevice_SetVertexShaderConstant (4928)
//   * D3DDevice_SetVertexShaderInput (4039)
//   * D3DDevice_SwitchTexture (4039)
//   * D3DDevice_UpdateOverlay (4039)
//   * D3DPalette_Lock (4721, 4831, 4928, 5028, 5120, 5233)
//   * D3DTexture_GetSurfaceLevel (4928, 5455)
//   * D3DVertexBuffer_Lock (4721, 4831, 4928, 5120, 5233, 5455)
//   * D3D_ClearStateBlockFlags (4039, 4721, 5028, 5120)
//   * D3D_RecordStateBlock (4039, 4721, 5028, 5120)
//   * CMiniport_IsFlipPending (4039, 4134)

#ifndef D3D8_OOVPA_INL
#define D3D8_OOVPA_INL

#include "../OOVPA.h"

#include "D3D8.1.0.3911.inl"
#include "D3D8.1.0.3925.inl"
#include "D3D8.1.0.3948.inl"
#include "D3D8.1.0.4034.inl"
#include "D3D8.1.0.4134.inl"
#include "D3D8.1.0.4242.inl"
#include "D3D8.1.0.4361.inl"
#include "D3D8.1.0.4432.inl"
#include "D3D8.1.0.4531.inl"
#include "D3D8.1.0.4627.inl"
#include "D3D8.1.0.4831.inl"
#include "D3D8.1.0.4928.inl"
#include "D3D8.1.0.5028.inl"
#include "D3D8.1.0.5120.inl"
#include "D3D8.1.0.5233.inl"
#include "D3D8.1.0.5344.inl"
#include "D3D8.1.0.5455.inl"
#include "D3D8.1.0.5558.inl"
#include "D3D8.1.0.5659.inl"
#include "D3D8.1.0.5788.inl"
#include "D3D8.1.0.5849.inl"


// ******************************************************************
// * D3D8_OOVPA
// ******************************************************************
OOVPATable D3D8_OOVPAV2[] = {

	REGISTER_OOVPAS(CMiniport_CreateCtxDmaObject, PATCH, 3911, 4034),
	REGISTER_OOVPAS(CMiniport_InitHardware, PATCH, 3911, 4034/*, 4361, 4627*/, 5455),
	REGISTER_OOVPAS(CMiniport_IsFlipPending, UNPATCHED, 3911, 4242, 4627, 4928),
	REGISTER_OOVPAS(D3DBaseTexture_GetLevelCount, PATCH, 3911),
	REGISTER_OOVPAS(D3DCubeTexture_GetCubeMapSurface, PATCH, 3911/*, 4134*/, 4627/*, 5558*/), // Called D3DCubeTexture_GetCubeMapSurface2 (from 4627's comment) NOTE: Use D3DCubeTexture_GetCubeMapSurface2 for 4627 and above
	REGISTER_OOVPAS(D3DCubeTexture_GetCubeMapSurface2, PATCH, 4627),
	REGISTER_OOVPAS(D3DCubeTexture_LockRect, PATCH, 3911), // Just calls Lock2DSurface (from 4134, 4432's comment)
	REGISTER_OOVPAS(D3DDevice_AddRef, PATCH, 3911, 4039, 4134, 4242, 4627, 5028, 5344, 5558, 5659),
	REGISTER_OOVPAS(D3DDevice_ApplyStateBlock, PATCH, 3911/*, 4361*/, 4627),
	REGISTER_OOVPAS(D3DDevice_Begin, PATCH, 3911, 4039),
	REGISTER_OOVPAS(D3DDevice_BeginPush, PATCH, 4134/*, 4432*/, 4627, 5028),
	REGISTER_OOVPAS(D3DDevice_BeginPushBuffer, PATCH, 3911, 4134), // Not implemented yet. (from 5788's comment)
	REGISTER_OOVPAS(D3DDevice_BeginStateBig, XREF, 5028),
	REGISTER_OOVPAS(D3DDevice_BeginStateBlock, PATCH, 3911, 4134),
	REGISTER_OOVPAS(D3DDevice_BeginVisibilityTest, PATCH, 3911, 4034/*, 4361, 4627*/),
	REGISTER_OOVPAS(D3DDevice_BlockOnFence, PATCH, 3911/*, 4039, 4134, 5233*/),
	REGISTER_OOVPAS(D3DDevice_BlockUntilVerticalBlank, PATCH, 3911, 4034, 4134, 4242, 4432, 4627, 5028, 5233, 5344, 5455, 5558, 5659/*, 5788*/),
	REGISTER_OOVPAS(D3DDevice_CaptureStateBlock, PATCH, 3911, 4134/*, 4627, 5455*/),
	REGISTER_OOVPAS(D3DDevice_Clear, PATCH, 3911, 4034/*, 4134, 4627, 5028*/),
	REGISTER_OOVPAS(D3DDevice_CopyRects, PATCH, 3911, 4034/*, 4134*/, 4627, 5120),
	REGISTER_OOVPAS(D3DDevice_CreateCubeTexture, PATCH, 3911/*, 4361*/, 4627),
	REGISTER_OOVPAS(D3DDevice_CreateImageSurface, PATCH, 3911, 4034/*, 4134*/, 4627),//TODO 4721,4928 (from 4627's comment)
	REGISTER_OOVPAS(D3DDevice_CreateIndexBuffer, UNPATCHED, 3911, 4627/*, 5558*/), // TODO: This needs to be verified on 4361
	REGISTER_OOVPAS(D3DDevice_CreateIndexBuffer2, UNPATCHED, 4627, 5344),
	REGISTER_OOVPAS(D3DDevice_CreatePalette, PATCH, 3911, 4627), // Called D3DDevice_CreatePalette2 (from 4627's comment) NOTE: Use D3DDevice_CreatePalette2 for 4627 and above
	REGISTER_OOVPAS(D3DDevice_CreatePalette2, PATCH, 4627, 5344, 5455),
	REGISTER_OOVPAS(D3DDevice_CreatePixelShader, PATCH, 3911, 5344/*, 5788*/),
	REGISTER_OOVPAS(D3DDevice_CreateStateBlock, PATCH, 3911/*, 4627*/),
	REGISTER_OOVPAS(D3DDevice_CreateTexture, PATCH, 3911, 4627),// Called D3DDevice_CreateTexture2 (from 4627's comment) NOTE: Use D3DDevice_CreateTexture2 for 4627 and above
	REGISTER_OOVPAS(D3DDevice_CreateTexture2, PATCH, 4627, 4831/*, 5028, 5233*/), // Was 4627 (from 5344's comment)
	REGISTER_OOVPAS(D3DDevice_CreateVertexBuffer, UNPATCHED, 3911, 4627),
	REGISTER_OOVPAS(D3DDevice_CreateVertexBuffer2, UNPATCHED, 4627, 5344, 5455),
	REGISTER_OOVPAS(D3DDevice_CreateVertexShader, PATCH, 3911),
	REGISTER_OOVPAS(D3DDevice_CreateVolumeTexture, PATCH, 3911, 4627),
	REGISTER_OOVPAS(D3DDevice_DeletePatch, UNPATCHED, 3911/*, 4039*/), // (TODO) (from 4034's comment)
	REGISTER_OOVPAS(D3DDevice_DeletePixelShader, PATCH, 3911, 5344),
	REGISTER_OOVPAS(D3DDevice_DeleteStateBlock, PATCH, 3911/*, 4361, 5788*/),
	REGISTER_OOVPAS(D3DDevice_DeleteVertexShader, PATCH, 3911, 5344),
	REGISTER_OOVPAS(D3DDevice_DrawIndexedVertices, PATCH, 3911, 4034, 4627, 5028),
	REGISTER_OOVPAS(D3DDevice_DrawIndexedVerticesUP, PATCH, 3911, 4039/*, 4134, 4627*/, 5028, 5344/*, 5558, 5788*/),
	REGISTER_OOVPAS(D3DDevice_DrawRectPatch, PATCH, 3911), // TODO: Unused? (from 4034's comment)
	REGISTER_OOVPAS(D3DDevice_DrawTriPatch, UNPATCHED, 3911),
	REGISTER_OOVPAS(D3DDevice_DrawVertices, PATCH, 3911/*, 4034, 5028*/),
	REGISTER_OOVPAS(D3DDevice_DrawVerticesUP, PATCH, 3911, 4039/*, 4134, 4627*/, 5344/*, 5558, 5788*/),
	REGISTER_OOVPAS(D3DDevice_EnableOverlay, PATCH, 3911, 4134),
	REGISTER_OOVPAS(D3DDevice_End, PATCH, 3911, 4039/*, 4627, 5028*/, 5344/*, 5558*/),
	REGISTER_OOVPAS(D3DDevice_EndPush, PATCH, 4134/*, 4627*/), // Was 4627 (from 5233's comment)
	REGISTER_OOVPAS(D3DDevice_EndPushBuffer, PATCH, 3911/*, 4134*/), // Not implemented yet. (from 5788's comment)
	REGISTER_OOVPAS(D3DDevice_EndStateBlock, PATCH, 3911, 4134),
	REGISTER_OOVPAS(D3DDevice_EndVisibilityTest, PATCH, 3911/*, 4034, 4361, 4627, 5788*/),
	REGISTER_OOVPAS(D3DDevice_FlushVertexCache, PATCH, 3911, 4134),
	REGISTER_OOVPAS(D3DDevice_GetBackBuffer, PATCH, 3911, 4034, 4134, 4627/*, 5558*/), // Called D3DDevice_GetBackBuffer2 (from 4627's comment) NOTE: Use D3DDevice_GetBackBuffer2 for 4627 and above
	REGISTER_OOVPAS(D3DDevice_GetBackBuffer2, PATCH, 4627/*, 5028, 5344, 5455, 5558, 5788*/), // 5233 (from 5344's comment)
	REGISTER_OOVPAS(D3DDevice_GetBackMaterial, PATCH, 3911, 4134, 4627, 5344/*, 5455*/, 5558, 5659/*, 5788*/),
	REGISTER_OOVPAS(D3DDevice_GetCreationParameters, PATCH, 3911),
	REGISTER_OOVPAS(D3DDevice_GetDepthStencilSurface, PATCH, 3911, 4627), // Called D3DDevice_GetDepthStencilSurface2 (from 4627's comment) NOTE: Use D3DDevice_GetDepthStencilSurface2 for 4627 and above
	REGISTER_OOVPAS(D3DDevice_GetDepthStencilSurface2, PATCH, 4627/*, 5028, 5788*/),
	REGISTER_OOVPAS(D3DDevice_GetDeviceCaps, UNPATCHED, 3911),
	REGISTER_OOVPAS(D3DDevice_GetDisplayFieldStatus, PATCH, 3911/*, 4039, 4432, 5233, 5788*/),
	REGISTER_OOVPAS(D3DDevice_GetDisplayMode, PATCH, 3911, 4039/*, 4432, 4627*/),
	REGISTER_OOVPAS(D3DDevice_GetGammaRamp, PATCH, 3911/*, 3925, 4034*/),
	REGISTER_OOVPAS(D3DDevice_GetLight, PATCH, 3911),
	REGISTER_OOVPAS(D3DDevice_GetLightEnable, PATCH, 3911, 5344),
	REGISTER_OOVPAS(D3DDevice_GetMaterial, PATCH, 3911, 4134, 4627, 5344/*, 5455*/, 5558, 5659/*, 5788*/),
	REGISTER_OOVPAS(D3DDevice_GetModelView, PATCH, 3911, 4134),
	REGISTER_OOVPAS(D3DDevice_GetOverlayUpdateStatus, PATCH, 3911),
	REGISTER_OOVPAS(D3DDevice_GetPersistedSurface2, PATCH, 4928), // For only on Unreal Championship (from 4627's comment)
	REGISTER_OOVPAS(D3DDevice_GetPixelShader, PATCH, 3911, 4039, 4134, 5028, 5558, 5659/*, 5788*/),
	REGISTER_OOVPAS(D3DDevice_GetProjectionViewportMatrix, PATCH, 3911, 4134/*, 4432*/, 4627, 5344, 5558), // For 5455 (from 5558's comment)
	REGISTER_OOVPAS(D3DDevice_GetPushBufferOffset, PATCH, 3911, 4627),//TODO 4831 (from 4627's comment)
	REGISTER_OOVPAS(D3DDevice_GetRenderTarget, PATCH, 3911, 4039, 4134/*, 4361, 4432*/, 4627), // Called D3DDevice_GetRenderTarget2 (from 4627's comment) NOTE: Use D3DDevice_GetRenderTarget2 for 4627 and above
	REGISTER_OOVPAS(D3DDevice_GetRenderTarget2, PATCH, 4627/*, 5028*/),
	REGISTER_OOVPAS(D3DDevice_GetScissors, PATCH, 3911/*, 5788*/),
	REGISTER_OOVPAS(D3DDevice_GetShaderConstantMode, PATCH, 3911, 4134, 4627, 5028, 5344, 5558, 5659/*, 5788*/),
	REGISTER_OOVPAS(D3DDevice_GetStreamSource2, PATCH, 4627),
	REGISTER_OOVPAS(D3DDevice_GetTexture2, PATCH, 3911, 4134, 4627, 5344, 5558, 5659/*, 5788*/),
	REGISTER_OOVPAS(D3DDevice_GetTile, PATCH, 3911/*, 4134, 4627, 5028, 5344*/, 5455),
	REGISTER_OOVPAS(D3DDevice_GetTransform, PATCH, 3911, 4034),
	REGISTER_OOVPAS(D3DDevice_GetVertexShader, PATCH, 3911, 4039, 4134, 5028, 5558, 5659/*5788*/),
	REGISTER_OOVPAS(D3DDevice_GetVertexShaderConstant, PATCH, 3911, 4039, 5028),
	REGISTER_OOVPAS(D3DDevice_GetVertexShaderDeclaration, PATCH, 3911/*, 4134, 4627, 5455*/),
	REGISTER_OOVPAS(D3DDevice_GetVertexShaderFunction, PATCH, 3911/*, 4134*/),
	REGISTER_OOVPAS(D3DDevice_GetVertexShaderInput, PATCH, 3911/*, 4134, 4627*/),
	REGISTER_OOVPAS(D3DDevice_GetVertexShaderSize, PATCH, 3911),
	REGISTER_OOVPAS(D3DDevice_GetVertexShaderType, PATCH, 3911),
	REGISTER_OOVPAS(D3DDevice_GetViewport, PATCH, 3911/*, 4034, 4134, 4627, 5344*/, 5455/*, 5788*/), // Was 4627 (from 5344's comment)
	REGISTER_OOVPAS(D3DDevice_GetViewportOffsetAndScale, PATCH, 4432/*, 5558*/),
	REGISTER_OOVPAS(D3DDevice_GetVisibilityTestResult, PATCH, 3911, 5233),
	REGISTER_OOVPAS(D3DDevice_InsertCallback, PATCH, 3911, 4134/*, 4627*/, 5028),
	REGISTER_OOVPAS(D3DDevice_InsertFence, PATCH, 3911),
	REGISTER_OOVPAS(D3DDevice_IsBusy, PATCH, 3911/*, 4034*/, 5028/*, 5344*/),
	REGISTER_OOVPAS(D3DDevice_IsFencePending, PATCH, 3911, 4039, 5028/*, 5558*/),
	REGISTER_OOVPAS(D3DDevice_KickOff, PATCH, 3911, 4034/*, 4134*/, 4531, 5028, 5455/*, 5788*/),
	REGISTER_OOVPAS(D3DDevice_KickPushBuffer, PATCH, 3911/*, 4627*/),
	REGISTER_OOVPAS(D3DDevice_LazySetStateVB, XREF, 5028),
	REGISTER_OOVPAS(D3DDevice_LightEnable, PATCH, 3911/*, 5028*/, 5344), // Was 5233 (from 5344's comment)
	REGISTER_OOVPAS(D3DDevice_LoadVertexShader, PATCH, 3911, 4034, 4627, 5028),
	REGISTER_OOVPAS(D3DDevice_LoadVertexShaderProgram, PATCH, 3911/*, 4134, 4627, 5558*/),
	REGISTER_OOVPAS(D3DDevice_MakeSpace, UNPATCHED, 4134),
	REGISTER_OOVPAS(D3DDevice_PersistDisplay, PATCH, 3911, 4039, 4627, 4831, 5455, 5558),
	REGISTER_OOVPAS(D3DDevice_Present, PATCH, 3911),
	REGISTER_OOVPAS(D3DDevice_PrimeVertexCache, PATCH, 3911, 4134/*, 4361, 4627*/),
	REGISTER_OOVPAS(D3DDevice_Release, PATCH, 3911),
	REGISTER_OOVPAS(D3DDevice_Reset, PATCH, 3911/*, 4039*/),
	REGISTER_OOVPAS(D3DDevice_RunPushBuffer, PATCH, 3911, 4039, 4627, 5120, 5558), // for 5455 (from 5558's comment)
	REGISTER_OOVPAS(D3DDevice_RunVertexStateShader, PATCH, 3911, 4134),
	REGISTER_OOVPAS(D3DDevice_SelectVertexShader, PATCH, 3911, 4034/*, 4627, 5344*/, 5455),
	REGISTER_OOVPAS(D3DDevice_SelectVertexShaderDirect, PATCH, 4361),
	REGISTER_OOVPAS(D3DDevice_SetBackBufferScale, PATCH, 4134),
	REGISTER_OOVPAS(D3DDevice_SetBackMaterial, PATCH, 3911, 4134, 4627, 5344/*, 5455*/, 5558, 5659/*, 5788*/),
	REGISTER_OOVPAS(D3DDevice_SetDepthClipPlanes, PATCH, 4432/*, 4627, 5788, 5849*/),
	REGISTER_OOVPAS(D3DDevice_SetFlickerFilter, PATCH, 3911, 4034, 4134),
	REGISTER_OOVPAS(D3DDevice_SetGammaRamp, PATCH, 3911, 4627),
	REGISTER_OOVPAS(D3DDevice_SetIndices, UNPATCHED, 3911, 4034/*, 4134, 5028, 5558*/),
	REGISTER_OOVPAS(D3DDevice_SetLight, PATCH, 3911/*, 4034, 4134, 5028*/, 5344/*, 5558*/),
	REGISTER_OOVPAS(D3DDevice_SetMaterial, PATCH, 3911, 4034, 4134, 4627, 5344/*, 5455*/, 5558, 5659/*, 5788*/), // Was 4627 (from 5344's comment)
	REGISTER_OOVPAS(D3DDevice_SetModelView, PATCH, 3911, 4134, 4627),
	REGISTER_OOVPAS(D3DDevice_SetPalette, PATCH, 3911, 4034/*, 4134, 4432, 4627, 5233, 5344, 5558, 5788*/),
	REGISTER_OOVPAS(D3DDevice_SetPixelShader, PATCH, 3911, 4034/*, 4134, 4432*/, 4627),
	REGISTER_OOVPAS(D3DDevice_SetPixelShaderConstant, PATCH, 3911/*, 4039*/, 4831/*, 5233, 5558*/),
	REGISTER_OOVPAS(D3DDevice_SetPixelShaderProgram, PATCH, 3911/*, 4361, 4627, 5558*/),
	REGISTER_OOVPAS(D3DDevice_SetRenderState_BackFillMode, PATCH, 3911, 4034/*, 4134, 4531, 5788*/),
	REGISTER_OOVPAS(D3DDevice_SetRenderState_CullMode, PATCH, 3911, 4034),
	REGISTER_OOVPAS(D3DDevice_SetRenderState_Deferred, UNPATCHED, 3911),
	REGISTER_OOVPAS(D3DDevice_SetRenderState_DoNotCullUncompressed, PATCH, 3911),
	REGISTER_OOVPAS(D3DDevice_SetRenderState_Dxt1NoiseEnable, PATCH, 3911, 4034/*, 4134*/, 4627/*, 5028, 5344, 5558*/), // Was 5233 (from 5344's comment)
	REGISTER_OOVPAS(D3DDevice_SetRenderState_EdgeAntiAlias, PATCH, 3911, 4034/*, 4134*/),
	REGISTER_OOVPAS(D3DDevice_SetRenderState_FillMode, PATCH, 3911, 4034/*, 4134*/),
	REGISTER_OOVPAS(D3DDevice_SetRenderState_FogColor, PATCH, 3911, 4034/*, 4134*/),
	REGISTER_OOVPAS(D3DDevice_SetRenderState_FrontFace, PATCH, 3911, 4034/*, 4134*/),
	REGISTER_OOVPAS(D3DDevice_SetRenderState_LineWidth, PATCH, 3911, 4034),
	REGISTER_OOVPAS(D3DDevice_SetRenderState_LogicOp, PATCH, 3911, 4034/*, 4134, 4627*/),
	REGISTER_OOVPAS(D3DDevice_SetRenderState_MultiSampleAntiAlias, PATCH, 3911, 4034/*, 4134, 4432*/, 4627),
	REGISTER_OOVPAS(D3DDevice_SetRenderState_MultiSampleMask, PATCH, 3911, 4034/*, 4134*/, 4627),
	REGISTER_OOVPAS(D3DDevice_SetRenderState_MultiSampleMode, PATCH, 3925, 4034, 4134, 4627, 5233/*, 5455, 5558, 5788*/),
	REGISTER_OOVPAS(D3DDevice_SetRenderState_MultiSampleRenderTargetMode, PATCH, 4034, 4134, 4627/*, 5233*/, 5344/*, 5455, 5558, 5788*/), // Was 5233 (from 5344's comment)
	REGISTER_OOVPAS(D3DDevice_SetRenderState_NormalizeNormals, PATCH, 3911, 4034/*, 4134*/),
	REGISTER_OOVPAS(D3DDevice_SetRenderState_OcclusionCullEnable, PATCH, 3911, 4034),
	REGISTER_OOVPAS(D3DDevice_SetRenderState_PSTextureModes, PATCH, 3911, 4034),
	REGISTER_OOVPAS(D3DDevice_SetRenderState_RopZCmpAlwaysRead, PATCH, 3911),
	REGISTER_OOVPAS(D3DDevice_SetRenderState_RopZRead, PATCH, 3911),
	REGISTER_OOVPAS(D3DDevice_SetRenderState_SampleAlpha, PATCH, 4627),
	REGISTER_OOVPAS(D3DDevice_SetRenderState_ShadowFunc, PATCH, 3911, 4034/*, 4134*/),
	REGISTER_OOVPAS(D3DDevice_SetRenderState_Simple, PATCH, 3911, 4034),
	REGISTER_OOVPAS(D3DDevice_SetRenderState_StencilCullEnable, PATCH, 3911, 4034),
	REGISTER_OOVPAS(D3DDevice_SetRenderState_StencilEnable, PATCH, 3911, 4034/*, 4134*/, 5849),
	REGISTER_OOVPAS(D3DDevice_SetRenderState_StencilFail, PATCH, 3911, 4034/*, 4134*/, 5849),
	REGISTER_OOVPAS(D3DDevice_SetRenderState_TextureFactor, PATCH, 3911, 4034/*, 4134, 5028, 5558, 5788*/),
	REGISTER_OOVPAS(D3DDevice_SetRenderState_TwoSidedLighting, PATCH, 3911, 4034/*, 4134*/, 5344/*, 5558*/), // Beware of the typo... (from 4627's comment)
	REGISTER_OOVPAS(D3DDevice_SetRenderState_VertexBlend, PATCH, 3911, 4034/*, 4134*/),
	REGISTER_OOVPAS(D3DDevice_SetRenderState_YuvEnable, PATCH, 3911, 4034/*, 4134*/),
	REGISTER_OOVPAS(D3DDevice_SetRenderState_ZBias, PATCH, 3911),
	REGISTER_OOVPAS(D3DDevice_SetRenderState_ZEnable, PATCH, 3911, 4034, 4134, 4432/*, 4627, 5028*/),
	REGISTER_OOVPAS(D3DDevice_SetRenderTarget, PATCH, 3911, 3948, 4039/*, 4134*/, 4627, 5344, 5455/*, 5558*/),
	REGISTER_OOVPAS(D3DDevice_SetRenderTargetFast, PATCH, 5233/*, 5344*/),
	REGISTER_OOVPAS(D3DDevice_SetScissors, PATCH, 3911/*, 4039, 4361, 4627, 5233*/, 5344, 5558), // Was 5233 (from 5344's comment)
	REGISTER_OOVPAS(D3DDevice_SetScreenSpaceOffset, PATCH, 4034/*, 5233, 5344*/, 5455), // Was 5233 (from 5344's comment)
	REGISTER_OOVPAS(D3DDevice_SetShaderConstantMode, PATCH, 3911, 4034/*, 4134, 4361, 4627, 5028*/),
	REGISTER_OOVPAS(D3DDevice_SetSoftDisplayFilter, PATCH, 3911, 4034, 4134),
	REGISTER_OOVPAS(D3DDevice_SetStateUP, UNPATCHED, 3911, 4627),
	REGISTER_OOVPAS(D3DDevice_SetStateVB, UNPATCHED, 3911, 4627),
	REGISTER_OOVPAS(D3DDevice_SetStipple, PATCH, 4627),
	REGISTER_OOVPAS(D3DDevice_SetStreamSource, PATCH, 3911, 4034),
	REGISTER_OOVPAS(D3DDevice_SetSwapCallback, PATCH, 4134, 4242, 4432, 4627, 5028, 5233, 5344, 5455, 5558, 5659),
	REGISTER_OOVPAS(D3DDevice_SetTexture, PATCH, 3911, 4034/*, 4627*/, 4831/*, 5344*/), // Was 4928 (from 5344's comment)
	REGISTER_OOVPAS(D3DDevice_SetTextureState_BorderColor, PATCH, 3911, 4034/*, 4361*/),
	REGISTER_OOVPAS(D3DDevice_SetTextureState_BumpEnv, PATCH, 3911, 4034),
	REGISTER_OOVPAS(D3DDevice_SetTextureState_ColorKeyColor, PATCH, 3911, 4034/*, 4134*/),
	REGISTER_OOVPAS(D3DDevice_SetTextureState_TexCoordIndex, PATCH, 3911, 4034, 4242, 4627),
	REGISTER_OOVPAS(D3DDevice_SetTile, PATCH, 3911, 4034/*, 4134*/, 4627/*, 5028, 5455, 5788*/), // Then it has changed calls ?SetTileNoWait@D3D@@YGXKPBU_D3DTILE@@@Z in XDK 4627 and higher.
	REGISTER_OOVPAS(D3DDevice_SetTransform, PATCH, 3911, 4034/*, 4134*/, 5344, 5558), // Was 4134 (from 5344's comment)
	REGISTER_OOVPAS(D3DDevice_SetVertexData2f, PATCH, 3911, 4039), // Was 4627 (from 5344's comment)
	REGISTER_OOVPAS(D3DDevice_SetVertexData2s, PATCH, 3911, 4134/*, 4361*/),
	REGISTER_OOVPAS(D3DDevice_SetVertexData4f, PATCH, 3911, 4039),
	REGISTER_OOVPAS(D3DDevice_SetVertexData4s, PATCH, 3911, 4134/*, 4361*/),
	REGISTER_OOVPAS(D3DDevice_SetVertexData4ub, PATCH, 3911, 4134),
	REGISTER_OOVPAS(D3DDevice_SetVertexDataColor, PATCH, 3911, 4039/*, 4361*/),
	REGISTER_OOVPAS(D3DDevice_SetVertexShader, PATCH, 3911, 4034, 4134, 5028, 5455),
	REGISTER_OOVPAS(D3DDevice_SetVertexShaderConstant, PATCH, 3911, 4034, 4627),
	REGISTER_OOVPAS(D3DDevice_SetVertexShaderConstant1, PATCH, 4627/*, 5558, 5788*/),
	REGISTER_OOVPAS(D3DDevice_SetVertexShaderConstant1Fast, PATCH, 4627),
	REGISTER_OOVPAS(D3DDevice_SetVertexShaderConstant4, PATCH, 4627),
	REGISTER_OOVPAS(D3DDevice_SetVertexShaderConstantNotInline, PATCH, 4627, 5028, 5558),
	REGISTER_OOVPAS(D3DDevice_SetVertexShaderConstantNotInlineFast, PATCH, 4627),
	REGISTER_OOVPAS(D3DDevice_SetVertexShaderInput, PATCH, 3911, 4134/*, 4361*/),
	REGISTER_OOVPAS(D3DDevice_SetVertexShaderInputDirect, PATCH, 4361),
	REGISTER_OOVPAS(D3DDevice_SetVerticalBlankCallback, PATCH, 3911, 4039, 4134, 4242, 4432, 4627, 5028, 5233, 5344, 5455, 5558, 5659), // Was 5233 (from 5344's comment)
	REGISTER_OOVPAS(D3DDevice_SetViewport, PATCH, 3911, 4034/*, 4627, 5028*/, 5344, 5455), // Was 5233 (from 5344's comment)
	REGISTER_OOVPAS(D3DDevice_Swap, PATCH, 4034, 4531, 4627/*, 5028, 5233*/),
	REGISTER_OOVPAS(D3DDevice_SwitchTexture, PATCH, 3911/*, 4361*/),
	REGISTER_OOVPAS(D3DDevice_UpdateOverlay, PATCH, 3911, 4531,/* 5028,*/ 5233/*, 5344, 5558*/),
	REGISTER_OOVPAS(D3DPalette_Lock, PATCH, 3911, 4531, 4627/*, 5344*/), // Called D3DPalette_Lock2 (from 4627's comment) NOTE: Use D3DPalette_Lock2 for 4627 and above
	REGISTER_OOVPAS(D3DPalette_Lock2, PATCH, 4627),
	REGISTER_OOVPAS(D3DResource_AddRef, PATCH, 3911),
	REGISTER_OOVPAS(D3DResource_BlockUntilNotBusy, PATCH, 3911),
	REGISTER_OOVPAS(D3DResource_GetType, UNPATCHED, 3911, 4034), // Probably not even necessary... (from 3925's comment) // I forget why I had this in here... (from 5028's comment)
	REGISTER_OOVPAS(D3DResource_IsBusy, PATCH, 3911, 4039),
	REGISTER_OOVPAS(D3DResource_Register, PATCH, 3911),
	REGISTER_OOVPAS(D3DResource_Release, PATCH, 3911/*, 4361*/),
	REGISTER_OOVPAS(D3DSurface_GetDesc, PATCH, 3911),
	REGISTER_OOVPAS(D3DSurface_LockRect, PATCH, 3911),
	REGISTER_OOVPAS(D3DTexture_GetSurfaceLevel, PATCH, 3911/*, 4432*/, 4627),
	REGISTER_OOVPAS(D3DTexture_GetSurfaceLevel2, PATCH, 4627),
	REGISTER_OOVPAS(D3DTexture_LockRect, PATCH, 3911),
	REGISTER_OOVPAS(D3DVertexBuffer_GetDesc, UNPATCHED, 3911),
	REGISTER_OOVPAS(D3DVertexBuffer_Lock, PATCH, 3911, 4034, 4531, 4627/*, 5788*/),
	REGISTER_OOVPAS(D3DVertexBuffer_Lock2, PATCH, 4627/*, 5788*/),
	REGISTER_OOVPAS(D3DVolumeTexture_LockBox, PATCH, 3911), // Just calls Lock3DSurface (from 4134, 4432's comment)
	REGISTER_OOVPAS(D3D_AllocContiguousMemory, UNPATCHED, 3911, 5455/*, 5788*/), // Just calls MmAllocateContiguousMemory. Was PATCH (from 3925, 5788's comment)
	REGISTER_OOVPAS(D3D_BlockOnResource, PATCH, 3911, 4034),
	REGISTER_OOVPAS(D3D_BlockOnTime, PATCH, 3911, 4034/*, 4361*/, 4627, 5028, 5558/*, 5849*/),
	REGISTER_OOVPAS(D3D_CMiniport_GetDisplayCapabilities, PATCH, 3911),
	REGISTER_OOVPAS(D3D_CheckDeviceFormat, UNPATCHED, 3911/*, 4134*/),
	REGISTER_OOVPAS(D3D_ClearStateBlockFlags, XREF, 3911/*, 5788*/),
	REGISTER_OOVPAS(D3D_CommonSetRenderTarget, XREF, 4627, 5028/*, 5233*/), // Used between 4627 to 5233 (from 5344's comment)
	REGISTER_OOVPAS(D3D_CommonSetRenderTargetB, XREF, 4627),
	REGISTER_OOVPAS(D3D_CreateDeviceX, UNPATCHED, 4627),
	REGISTER_OOVPAS(D3D_CreateStandAloneSurface, XREF, 4034),
	REGISTER_OOVPAS(D3D_EnumAdapterModes, UNPATCHED, 3911),
	REGISTER_OOVPAS(D3D_GetAdapterDisplayMode, UNPATCHED, 3911, 4627),
	REGISTER_OOVPAS(D3D_GetAdapterIdentifier, UNPATCHED, 3911),
	REGISTER_OOVPAS(D3D_GetAdapterModeCount, UNPATCHED, 3911),
	REGISTER_OOVPAS(D3D_GetDeviceCaps, UNPATCHED, 3911),
	REGISTER_OOVPAS(D3D_KickOffAndWaitForIdle, PATCH, 3911, 4034, 4627, 5028),
	REGISTER_OOVPAS(D3D_KickOffAndWaitForIdle2, PATCH, 4627),
	REGISTER_OOVPAS(D3D_LazySetPointParams, PATCH, 3911, 4034),
	REGISTER_OOVPAS(D3D_RecordStateBlock, XREF, 3911/*, 5788*/),
	REGISTER_OOVPAS(D3D_SetCommonDebugRegisters, PATCH, 3911),
	REGISTER_OOVPAS(D3D_SetFence, XREF, 3911, 4034, 4134, 5028, 5558),
	REGISTER_OOVPAS(D3D_SetPushBufferSize, UNPATCHED, 3911),
	REGISTER_OOVPAS(D3D_SetTileNoWait, XREF, 4627, 5455),
	// REGISTER_OOVPAS(D3D_Unknown, XREF, 5788), // This one D3DDevice_MakeSpace
	REGISTER_OOVPAS(Direct3D_CheckDeviceMultiSampleType, UNPATCHED, 3911),
	REGISTER_OOVPAS(Direct3D_CreateDevice, PATCH, 3911, 5028),
	REGISTER_OOVPAS(Get2DSurfaceDesc, PATCH, 3911, 4034/*, 4134, 4627, 5028, 5344, 5558, 5788, 5849*/), // Was 5233 (from 5344's comment)
	REGISTER_OOVPAS(Lock2DSurface, PATCH, 3911),
	REGISTER_OOVPAS(Lock3DSurface, PATCH, 3911),
	REGISTER_OOVPAS(D3D_MakeRequestedSpace, PATCH, 4134, 5028, 5558/*, 5788*/),
	REGISTER_OOVPAS(XMETAL_StartPush, UNPATCHED, 3911),
	REGISTER_OOVPAS(IDirect3DVertexBuffer8_Lock, UNPATCHED, 4627),
};

// ******************************************************************
// * D3D8_OOVPA_SIZE
// ******************************************************************
uint32 D3D8_OOVPA_SIZEV2 = sizeof(D3D8_OOVPAV2);

#endif
