
static void RunOnWndMsgThread(const std::function<void()>& func)
{
	const void* param = &func;
	SendMessage(g_hEmuWindow, WM_CXBXR_RUN_ON_MESSAGE_THREAD, reinterpret_cast<WPARAM>(param), 0);
}


const char *D3DErrorString(HRESULT hResult)
{
	static char buffer[1024];
	buffer[0] = 0; // Reset static buffer!

	const char* errorCodeString = DXGetErrorString(hResult);
	if (errorCodeString)
	{
		strcat(buffer, errorCodeString);
		strcat(buffer, ": ");
	}

	const char* errorDescription = CxbxGetErrorDescription(hResult);
	if (errorDescription)
		strcat(buffer, errorDescription);
	else
		strcat(buffer, "Unknown D3D error.");

	return buffer;
}

void CxbxInitWindow()
{
    g_EmuShared->GetVideoSettings(&g_XBVideo);

    if(g_XBVideo.bFullScreen)
        CxbxKrnl_hEmuParent = NULL;

/* TODO : Port this Dxbx code :
  // create vblank handling thread
    {
        dwThreadId = 0;
        {hThread :=} CreateThread(nullptr, 0, EmuThreadHandleVBlank, nullptr, 0, &dwThreadId);
    }
*/
    // create window message processing thread
    {
		HANDLE hStartEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
		HANDLE hRenderWindowThread = CreateThread(nullptr, 0, EmuRenderWindow, &hStartEvent, 0, nullptr);

		if (hRenderWindowThread == NULL) {
			char szBuffer[1024] = { 0 };
			sprintf(szBuffer, "Creating EmuRenderWindowThread Failed: %08X", GetLastError());
			PopupFatal(nullptr, szBuffer);
			EmuShared::Cleanup();
			ExitProcess(0);
		}
		g_AffinityPolicy->SetAffinityOther(hRenderWindowThread);

		// Wait for the window to create
		WaitForSingleObject(hStartEvent, INFINITE);
		CloseHandle(hStartEvent);
		CloseHandle(hRenderWindowThread);
    }

	SetFocus(g_hEmuWindow);
	g_renderbase = std::unique_ptr<RenderBase>(new RenderBase());
	g_renderbase->Initialize();

	ImGui_ImplWin32_Init(g_hEmuWindow);
	g_renderbase->SetWindowRelease([] {
		ImGui_ImplWin32_Shutdown();
	});

	(void) g_ShaderSources.Update();
	g_ShaderSources.InitShaderHotloading();
	
}

void DrawUEM(HWND hWnd)
{
	// Draw the universal error message (UEM)
	// See https://xboxdevwiki.net/Fatal_Error
	// Only call this from WM_PAINT message!

	PAINTSTRUCT ps;

	BeginPaint(hWnd, &ps);

	HDC hDC = GetDC(hWnd);
	HDC hMemDC = CreateCompatibleDC(hDC);
	HBITMAP hUEMBmp = CreateCompatibleBitmap(hDC, 640, 480);
	HBITMAP hOriUEMBmp = (HBITMAP)SelectObject(hMemDC, hUEMBmp);


	int nHeight = -MulDiv(8, GetDeviceCaps(hMemDC, LOGPIXELSY), 72);

	HFONT hFont = CreateFont(nHeight, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, FF_ROMAN, "Verdana");

	HGDIOBJ tmpObj = SelectObject(hMemDC, hFont);

	SetBkColor(hMemDC, RGB(0, 0, 0));

	SetTextColor(hMemDC, RGB(0, 204, 0));

	std::wstring utf16str = utf8_to_utf16(uem_str.c_str());

	// Unfortunately, DrawTextW doesn't support vertical alignemnt, so we have to do the calculation
	// ourselves. See here: https://social.msdn.microsoft.com/Forums/vstudio/en-US/abd89aae-16a0-41c6-8db6-b119ea90b42a/win32-drawtext-how-center-in-vertical-with-new-lines-and-tabs?forum=vclanguage

	RECT rect = { 0, 0, 640, 480 };
	RECT textrect = { 0, 0, 640, 480 };
	DrawTextW(hMemDC, utf16str.c_str(), utf16str.length(), &textrect, DT_CALCRECT);
	rect.top = (rect.bottom - textrect.bottom) / 2;
	DrawTextW(hMemDC, utf16str.c_str(), utf16str.length(), &rect, DT_CENTER);


	// Draw the Xbox error code

	SetTextColor(hMemDC, RGB(255, 255, 255));
	std::string err_str(std::to_string(g_CxbxFatalErrorCode));
	rect.left = 20;
	DrawText(hMemDC, err_str.c_str(), err_str.length(), &rect, DT_LEFT);

	GetClientRect(hWnd, &rect);
	SetStretchBltMode(hDC, COLORONCOLOR);
	StretchBlt(hDC, rect.left, rect.top, rect.right, rect.bottom, hMemDC, 0, 0, 640, 480, SRCCOPY);

	SelectObject(hMemDC, hOriUEMBmp);
	SelectObject(hDC, tmpObj);

	DeleteObject(hUEMBmp);
	DeleteObject(hFont);
	DeleteObject(hMemDC);

	if (hDC != NULL)
		ReleaseDC(hWnd, hDC);

	EndPaint(hWnd, &ps);
}

void CxbxClipCursor(HWND hWnd)
{
	RECT wnd_rect;
	GetWindowRect(hWnd, &wnd_rect);
	ClipCursor(&wnd_rect);
}

void CxbxReleaseCursor()
{
	ClipCursor(nullptr);
}

static void CxbxUpdateCursor(bool forceShow = false) {
	// Getting cursor info is a requirement in order to prevent a bug occur with ShowCursor redundant calls.
	CURSORINFO cursorInfo;
	cursorInfo.cbSize = sizeof(cursorInfo);
	if (!GetCursorInfo(&cursorInfo)) {
		// If cursor info is not available, then ignore the cursor update.
		return;
	}

	if ((g_renderbase && g_renderbase->IsImGuiFocus()) || forceShow) {
		if (cursorInfo.flags == 0) {
			ShowCursor(TRUE);
		}
	}
	else {
		if ((cursorInfo.flags & CURSOR_SHOWING) != 0) {
			ShowCursor(FALSE);
		}
	}
}

inline DWORD GetXboxCommonResourceType(const xbox::dword_xt XboxResource_Common)
{
	DWORD dwCommonType = XboxResource_Common & X_D3DCOMMON_TYPE_MASK;
	return dwCommonType;
}

inline DWORD GetXboxCommonResourceType(const xbox::X_D3DResource* pXboxResource)
{
	// Don't pass in unassigned Xbox resources
	assert(pXboxResource != xbox::zeroptr);

	return GetXboxCommonResourceType(pXboxResource->Common);
}

inline int GetXboxPixelContainerDimensionCount(const xbox::X_D3DPixelContainer *pXboxPixelContainer)
{
	// Don't pass in unassigned Xbox pixel container
	assert(pXboxPixelContainer != xbox::zeroptr);

	return (xbox::X_D3DFORMAT)((pXboxPixelContainer->Format & X_D3DFORMAT_DIMENSION_MASK) >> X_D3DFORMAT_DIMENSION_SHIFT);
}

