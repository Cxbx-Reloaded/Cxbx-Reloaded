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
