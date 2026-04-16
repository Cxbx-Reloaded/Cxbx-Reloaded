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
// *  All rights reserved
// *
// ******************************************************************
#include "EmuD3D8_common.h"


void CxbxImGui_RenderD3D(ImGuiUI* m_imgui, IDirect3DSurface* renderTarget)
{
#ifdef CXBX_USE_D3D11
	ImGui_ImplDX11_NewFrame();
#else
	ImGui_ImplDX9_NewFrame();
#endif
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	m_imgui->DrawMenu();
	m_imgui->DrawWidgets();

	ImGui::EndFrame();

	ImGui::Render();
	ImDrawData* drawData = ImGui::GetDrawData();
	if (drawData->TotalVtxCount > 0) {
#ifdef CXBX_USE_D3D11
		(void)CxbxSetRenderTarget(renderTarget);
		ImGui_ImplDX11_RenderDrawData(drawData);
		(void)CxbxSetRenderTarget(nullptr);
#else
		IDirect3DSurface* pExistingRenderTarget = CxbxGetCurrentRenderTarget();
		if (pExistingRenderTarget) {
			(void)CxbxSetRenderTarget(renderTarget);
			ImGui_ImplDX9_RenderDrawData(drawData);
			(void)CxbxSetRenderTarget(pExistingRenderTarget);
			pExistingRenderTarget->Release();
		}
#endif
	}
}
