#include "LibusbXboxController.h"
#include "InputManager.h"
#include "libusb.h"

#define CONTROLDATASTART 2
#define RUMBLEDATASTART 0

#define min(a, b) ((a < b) ? a : b)
#define max(a, b) ((a < b) ? b : a)

std::vector<libusbXbox::vidpid> libusbXbox::LibusbXboxController::vidpids;

void libusbXbox::LibusbXboxController::populateVidPids()
{
	if (vidpids.empty())
	{
		vidpids.push_back({ 0x0d2f, 0x0002, "Andamiro Pump It Up pad" });
		vidpids.push_back({ 0x045e, 0x0202, "Xbox Controller" });
		vidpids.push_back({ 0x045e, 0x0285, "Xbox Controller S" });
		vidpids.push_back({ 0x045e, 0x0287, "Xbox Controller S" });
		vidpids.push_back({ 0x045e, 0x0289, "Xbox Controller S" });
		vidpids.push_back({ 0x046d, 0xca84, "Logitech Xbox Cordless Controller" });
		vidpids.push_back({ 0x046d, 0xca88, "Logitech Compact Controller for Xbox" });
		vidpids.push_back({ 0x05fd, 0x1007, "Mad Catz Controller (unverified)" });
		vidpids.push_back({ 0x05fd, 0x107a, "InterAct 'PowerPad Pro' X-Box pad (Germany)" });
		vidpids.push_back({ 0x0738, 0x4516, "Mad Catz Control Pad" });
		vidpids.push_back({ 0x0738, 0x4522, "Mad Catz LumiCON" });
		vidpids.push_back({ 0x0738, 0x4526, "Mad Catz Control Pad Pro" });
		vidpids.push_back({ 0x0738, 0x4536, "Mad Catz MicroCON" });
		vidpids.push_back({ 0x0738, 0x4556, "Mad Catz Lynx Wireless Controller" });
		vidpids.push_back({ 0x0c12, 0x8802, "Zeroplus Xbox Controller" });
		vidpids.push_back({ 0x0c12, 0x8810, "Zeroplus Xbox Controller" });
		vidpids.push_back({ 0x0c12, 0x9902, "HAMA VibraX - *FAULTY HARDWARE*" });
		vidpids.push_back({ 0x0e4c, 0x1097, "Radica Gamester Controller" });
		vidpids.push_back({ 0x0e4c, 0x2390, "Radica Games Jtech Controller" });
		vidpids.push_back({ 0x0e6f, 0x0003, "Logic3 Freebird wireless Controller" });
		vidpids.push_back({ 0x0e6f, 0x0005, "Eclipse wireless Controller" });
		vidpids.push_back({ 0x0e6f, 0x0006, "Edge wireless Controller" });
		vidpids.push_back({ 0x0e8f, 0x0201, "SmartJoy Frag Xpad/PS2 adaptor" });
		vidpids.push_back({ 0x0f30, 0x0202, "Joytech Advanced Controller" });
		vidpids.push_back({ 0x0f30, 0x8888, "BigBen XBMiniPad Controller" });
		vidpids.push_back({ 0x102c, 0xff0c, "Joytech Wireless Advanced Controller" });
		vidpids.push_back({ 0x044f, 0x0f07, "Thrustmaster, Inc. Controller" });
		vidpids.push_back({ 0x0e8f, 0x3008, "Generic xbox control (dealextreme)" });
	}
}

void libusbXbox::LibusbXboxController::PopulateDevices()
{
	populateVidPids();

	// Enumerate all Xbox Controllers (Duke)
	for (auto vidpid : vidpids)
	{
		auto devices = libusb::LibusbDevice::GetDevices(vidpid.VID, vidpid.PID);
		for (auto dev : devices)
		{
			auto xbc = std::make_shared<libusbXbox::LibusbXboxController>(dev);
			g_InputDeviceManager.AddDevice(xbc);
		}
	}
}


bool libusbXbox::LibusbXboxController::UpdateInput()
{
	//poll();
	return IsConnected();
}

std::string libusbXbox::LibusbXboxController::GetDeviceName() const
{
	return "Xbox Controller";
}

std::string libusbXbox::LibusbXboxController::GetAPI() const
{
	return "libusb";
}

libusbXbox::LibusbXboxController::LibusbXboxController(libusb_device* dev) : libusb::LibusbDevice(dev)
{
	initInputDevice();

	libusb_device_descriptor descriptor;
	libusb_get_device_descriptor(dev, &descriptor);
	for (auto vidpid : vidpids)
	{
		if (descriptor.idVendor == vidpid.VID && descriptor.idProduct == vidpid.PID)
		{
			name = vidpid.Name;
			break;
		}
	}
}

void libusbXbox::LibusbXboxController::initInputDevice()
{
	AddInput(new Button(this, 0x0001, "D Pad Up"));
	AddInput(new Button(this, 0x0002, "D Pad Down"));
	AddInput(new Button(this, 0x0004, "D Pad Left"));
	AddInput(new Button(this, 0x0008, "D Pad Right"));
	AddInput(new Button(this, 0x0010, "Start"));
	AddInput(new Button(this, 0x0020, "Back"));
	AddInput(new Button(this, 0x0040, "L Thumb"));
	AddInput(new Button(this, 0x0080, "R Thumb"));

	AddInput(new AxisButton(this, 0, "A"));
	AddInput(new AxisButton(this, 1, "B"));
	AddInput(new AxisButton(this, 2, "X"));
	AddInput(new AxisButton(this, 3, "Y"));
	AddInput(new AxisButton(this, 4, "Black"));
	AddInput(new AxisButton(this, 5, "White"));

	AddInput(new Axis(this, "L Trigger", 14));
	AddInput(new Axis(this, "R Trigger", 15));
	AddInput(new Axis(this, "Left Axis X+", 16));
	AddInput(new Axis(this, "Left Axis X-", 17));
	AddInput(new Axis(this, "Left Axis Y+", 18));
	AddInput(new Axis(this, "Left Axis Y-", 19));
	AddInput(new Axis(this, "Right Axis X+", 20));
	AddInput(new Axis(this, "Right Axis X-", 21));
	AddInput(new Axis(this, "Right Axis Y+", 22));
	AddInput(new Axis(this, "Right Axis Y-", 23));

	AddOutput(new Motor(this));
}

std::string libusbXbox::LibusbXboxController::Motor::GetName() const
{
	return "LeftRight";
}

#pragma pack(push, 1)
struct XpadOutputReport {
	uint8_t bReportId;
	uint8_t bLength;
	uint8_t bReserved0;
	uint8_t bLeftMotor;
	uint8_t bReserved1;
	uint8_t bRightMotor;
};
#pragma pack(pop)

void libusbXbox::LibusbXboxController::Motor::SetState(ControlState leftMotor, ControlState rightMotor)
{
	XpadOutputReport output;

	// Got the idea from the linux kernel driver for xpad
	output.bReportId = 0;
	output.bLength = sizeof(XpadOutputReport);
	output.bReserved0 = 0;
	output.bLeftMotor = (uint8_t)(min(255, max(0, leftMotor * 255)));
	output.bReserved1 = 0;
	output.bRightMotor = (uint8_t)(min(255, max(0, rightMotor * 255)));

	m_Parent->SetRumble((const uint8_t*)&output, sizeof(XpadOutputReport));
}

std::string libusbXbox::LibusbXboxController::Button::GetName() const
{
	return m_Name;
}

ControlState libusbXbox::LibusbXboxController::Button::GetState() const
{
	auto controlData = m_Parent->GetRawControlData();
	auto input = (const XpadInput*)controlData;
	return (input->wButtons & m_ButtonMask) == 0 ? 0 : 1;
}

std::string libusbXbox::LibusbXboxController::AxisButton::GetName() const
{
	return m_Name;
}

ControlState libusbXbox::LibusbXboxController::AxisButton::GetState() const
{
	auto controlData = m_Parent->GetRawControlData();
	auto input = (const XpadInput*)controlData;
	return (input->bAnalogButtons[m_Index] > 32) == 0 ? 0 : 1;
}

std::string libusbXbox::LibusbXboxController::Axis::GetName() const
{
	return m_Name;
}

ControlState libusbXbox::LibusbXboxController::Axis::GetState() const
{
	auto controlData = m_Parent->GetRawControlData();
	auto input = (const XpadInput*)controlData;

	switch (m_Index)
	{
	case 14:
		return ControlState(input->bAnalogButtons[6]) / 255;
	case 15:
		return ControlState(input->bAnalogButtons[7]) / 255;
	case 16:
		return ControlState(input->sThumbLX) / 32768;
	case 17:
		return ControlState(input->sThumbLX) / -32768;
	case 18:
		return ControlState(input->sThumbLY) / 32768;
	case 19:
		return ControlState(input->sThumbLY) / -32768;
	case 20:
		return ControlState(input->sThumbRX) / 32768;
	case 21:
		return ControlState(input->sThumbRX) / -32768;
	case 22:
		return ControlState(input->sThumbRY) / 32768;
	case 23:
		return ControlState(input->sThumbRY) / -32768;
	default:
		return 0;
	}
}

void libusbXbox::LibusbXboxController::SetRumble(const uint8_t* newData, size_t bufferSize)
{
	SetBufferOut(newData, bufferSize);
}

const uint8_t* libusbXbox::LibusbXboxController::GetRawControlData() const
{
	const uint8_t* rawControlData = GetBufferIn();
	return &rawControlData[CONTROLDATASTART];
}

libusbXbox::LibusbXboxController::~LibusbXboxController()
{
}

