#include "Button.h"


void Button::EnableControl(bool enable)
{
	EnableWindow(m_button_hwnd, enable);
}

void Button::UpdateText(const char* text)
{
	SendMessage(m_button_hwnd, WM_SETTEXT, 0, reinterpret_cast<LPARAM>(text));
}

void Button::GetText(char* const text, size_t size)
{
	SendMessage(m_button_hwnd, WM_GETTEXT, size, reinterpret_cast<LPARAM>(text));
}
