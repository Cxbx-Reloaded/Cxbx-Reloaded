#include"Button.h"
#include "InputManager.h"
#include "common\util\CxbxUtil.h"
#include "..\..\gui\ResCxbx.h"

static int button_ctrl_id[] = {
	IDC_SET_DPAD_UP,
	IDC_SET_DPAD_DOWN,
	IDC_SET_DPAD_LEFT,
	IDC_SET_DPAD_RIGHT,
	IDC_SET_START,
	IDC_SET_BACK,
	IDC_SET_LTHUMB,
	IDC_SET_RTHUMB,
	IDC_SET_A,
	IDC_SET_B,
	IDC_SET_X,
	IDC_SET_Y,
	IDC_SET_BLACK,
	IDC_SET_WHITE,
	IDC_SET_LTRIGGER,
	IDC_SET_RTRIGGER,
	IDC_SET_LEFT_POSX,
	IDC_SET_LEFT_NEGX,
	IDC_SET_LEFT_POSY,
	IDC_SET_LEFT_NEGY,
	IDC_SET_RIGHT_POSX,
	IDC_SET_RIGHT_NEGX,
	IDC_SET_RIGHT_POSY,
	IDC_SET_RIGHT_NEGY,
	IDC_SET_LMOTOR,
	IDC_SET_RMOTOR,
};

static const char* button_names[] = {
	"D Pad Up",
	"D Pad Down",
	"D Pad Left",
	"D Pad Right",
	"Start",
	"Back",
	"L Thumb",
	"R Thumb",
	"A",
	"B",
	"X",
	"Y",
	"Black",
	"White",
	"L Trigger",
	"R Trigger",
	"Left Axis X+",
	"Left Axis X-",
	"Left Axis Y+",
	"Left Axis Y-",
	"Right Axis X+",
	"Right Axis X-",
	"Right Axis Y+",
	"Right Axis Y-",
	"L Motor",
	"R Motor",
};

static_assert((ARRAY_SIZE(button_ctrl_id) == ARRAY_SIZE(button_names)),
	"button_ctrl_id and button_names have different sizes!");


EmuDevice::EmuDevice(int type, HWND hwnd)
{
	switch (type)
	{
		case to_underlying(XBOX_INPUT_DEVICE::MS_CONTROLLER_DUKE): {
			m_name = "MS Controller Duke";
			m_hwnd = hwnd;
			for (int i = 0; i < ARRAY_SIZE(button_ctrl_id); i++) {
				m_buttons.push_back(new Button(button_names[i], button_ctrl_id[i], i, hwnd));
			}
		}
		break;

		default:
			break;
	}
}

EmuDevice::~EmuDevice()
{
	for (auto button : m_buttons) {
		delete button;
	}
}

Button* EmuDevice::FindButton(int id)
{
	auto it = std::find_if(m_buttons.begin(), m_buttons.end(), [&id](const auto button) {
		if (button->GetId() == id) {
			return true;
		}
		return false;
	});
	assert(it != m_buttons.end());
	return *it;
}
