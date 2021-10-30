#include "SteelBattalionController.h"
#include "InputManager.h"
#include "libusb.h"

#define USB_VID 0x0A7B
#define USB_PID 0xD000

#define min(a, b) ((a < b) ? a : b)

#define CONTROLDATASTART 2
#define CONTROLDATASIZE 24
#define LEDDATASTART 2
#define LEDDATASIZE 19

#define AIMING_X 9
#define AIMING_Y 11
#define ROTATION_LEVER 13
#define SIGHT_CHANGE_X 15
#define SIGHT_CHANGE_Y 17
#define LEFT_PEDAL 19
#define MIDDLE_PEDAL 21
#define RIGHT_PEDAL 23
#define SHIFTER 25
#define TUNER_DIAL 24

std::string getString(SBC::ButtonEnum btn);
ControlState getAimingAxis(uint8_t raw);
ControlState getRotationLeverAxis(uint8_t raw);
ControlState getSightChangeAxis(uint8_t raw);
ControlState getPedalAxis(uint8_t raw);

void SBC::SteelBattalionController::PopulateDevices()
{
	auto devices = libusb::LibusbDevice::GetDevices(USB_VID, USB_PID);
	for (auto dev : devices)
	{
		auto sbc = std::make_shared<SBC::SteelBattalionController>(dev);
		g_InputDeviceManager.AddDevice(sbc);
	}
}

bool SBC::SteelBattalionController::UpdateInput()
{
	return IsConnected();
}

std::string SBC::SteelBattalionController::GetDeviceName() const
{
	return "Steel Battalion Controller";
}

std::string SBC::SteelBattalionController::GetAPI() const
{
	return "libusb";
}

SBC::SteelBattalionController::SteelBattalionController(libusb_device *dev) : libusb::LibusbDevice(dev)
{
	masks.Init();
	
	initInputDevice();
}

ControlState SBC::SteelBattalionController::Button::GetState() const
{
	return (controlBuffer[mask.bytePos] & mask.maskValue) != 0 ? 1 : 0;
}

std::string SBC::SteelBattalionController::Button::GetName() const
{
	return getString(btn);
}

ControlState SBC::SteelBattalionController::AxisButton::GetState() const
{
	return controlBuffer[bufferOffset] == axisPosition ? 1 : 0;
}

std::string SBC::SteelBattalionController::AxisButton::GetName() const
{
	return name;
}

ControlState SBC::SteelBattalionController::Axis::GetState() const
{
	ControlState result = 0;
	uint8_t raw = controlBuffer[bufferOffset];
	switch (axisType)
	{
	case AxisType::AimingAxis:
		result = getAimingAxis(raw);
		break;
	case AxisType::SightChangeAxis:
		result = getSightChangeAxis(raw);
		break;
	case AxisType::RotationLeverAxis:
		result = getRotationLeverAxis(raw);
		break;
	case AxisType::PedalAxis:
		result = getPedalAxis(raw);
		break;
	}

	if (axisDirection == AxisDirection::Negative)
		return -result;
	return result;
}

std::string SBC::SteelBattalionController::Axis::GetName() const
{
	return name;
}

void SBC::SteelBattalionController::initInputDevice()
{
	auto rawControlData = GetBufferIn();

	// Create Input objects for each button
	for (auto it = masks.Masks.begin(); it != masks.Masks.end(); it++)
		AddInput(new Button(it->first, *it->second, rawControlData));

	// Create Input objects for each axis
	AddInput(new Axis("Aiming X+", AIMING_X, AxisType::AimingAxis, AxisDirection::Positive, rawControlData)); // AimingX
	AddInput(new Axis("Aiming X-", AIMING_X, AxisType::AimingAxis, AxisDirection::Negative, rawControlData)); // AimingX
	AddInput(new Axis("Aiming Y+", AIMING_Y, AxisType::AimingAxis, AxisDirection::Positive, rawControlData)); // AimingY
	AddInput(new Axis("Aiming Y-", AIMING_Y, AxisType::AimingAxis, AxisDirection::Negative, rawControlData)); // AimingY
	AddInput(new Axis("Rotation Lever+", ROTATION_LEVER, AxisType::RotationLeverAxis, AxisDirection::Positive, rawControlData)); // RotationLever
	AddInput(new Axis("Rotation Lever-", ROTATION_LEVER, AxisType::RotationLeverAxis, AxisDirection::Negative, rawControlData)); // RotationLever
	AddInput(new Axis("Sight Change X+", SIGHT_CHANGE_X, AxisType::SightChangeAxis, AxisDirection::Positive, rawControlData)); // SightChangeX
	AddInput(new Axis("Sight Change X-", SIGHT_CHANGE_X, AxisType::SightChangeAxis, AxisDirection::Negative, rawControlData)); // SightChangeX
	AddInput(new Axis("Sight Change Y+", SIGHT_CHANGE_Y, AxisType::SightChangeAxis, AxisDirection::Positive, rawControlData)); // SightChangeY
	AddInput(new Axis("Sight Change Y-", SIGHT_CHANGE_Y, AxisType::SightChangeAxis, AxisDirection::Negative, rawControlData)); // SightChangeY
	AddInput(new Axis("Left Pedal", LEFT_PEDAL, AxisType::PedalAxis, AxisDirection::Positive, rawControlData)); // Left Pedal
	AddInput(new Axis("Middle Pedal", MIDDLE_PEDAL, AxisType::PedalAxis, AxisDirection::Positive, rawControlData)); // Middle Pedal
	AddInput(new Axis("Right Pedal", RIGHT_PEDAL, AxisType::PedalAxis, AxisDirection::Positive, rawControlData)); // Right Pedal

	// Create input objects for the Gears (it's an axis, but we're mapping it as buttons)
	AddInput(new AxisButton("Gear R", SHIFTER, 254, rawControlData));
	AddInput(new AxisButton("Gear N", SHIFTER, 255, rawControlData));
	AddInput(new AxisButton("Gear 1", SHIFTER, 1, rawControlData));
	AddInput(new AxisButton("Gear 2", SHIFTER, 2, rawControlData));
	AddInput(new AxisButton("Gear 3", SHIFTER, 3, rawControlData));
	AddInput(new AxisButton("Gear 4", SHIFTER, 4, rawControlData));
	AddInput(new AxisButton("Gear 5", SHIFTER, 5, rawControlData));

	// Create input objects for the Tuner (it's an axis, but we're mapping it as buttons)
	AddInput(new AxisButton("Tuner Dial 1", TUNER_DIAL, 0, rawControlData));
	AddInput(new AxisButton("Tuner Dial 2", TUNER_DIAL, 1, rawControlData));
	AddInput(new AxisButton("Tuner Dial 3", TUNER_DIAL, 2, rawControlData));
	AddInput(new AxisButton("Tuner Dial 4", TUNER_DIAL, 3, rawControlData));
	AddInput(new AxisButton("Tuner Dial 5", TUNER_DIAL, 4, rawControlData));
	AddInput(new AxisButton("Tuner Dial 6", TUNER_DIAL, 5, rawControlData));
	AddInput(new AxisButton("Tuner Dial 7", TUNER_DIAL, 6, rawControlData));
	AddInput(new AxisButton("Tuner Dial 8", TUNER_DIAL, 7, rawControlData));
	AddInput(new AxisButton("Tuner Dial 9", TUNER_DIAL, 8, rawControlData));
	AddInput(new AxisButton("Tuner Dial 10", TUNER_DIAL, 9, rawControlData));
	AddInput(new AxisButton("Tuner Dial 11", TUNER_DIAL, 10, rawControlData));
	AddInput(new AxisButton("Tuner Dial 12", TUNER_DIAL, 11, rawControlData));
	AddInput(new AxisButton("Tuner Dial 13", TUNER_DIAL, 12, rawControlData));
	AddInput(new AxisButton("Tuner Dial 14", TUNER_DIAL, 13, rawControlData));
	AddInput(new AxisButton("Tuner Dial 15", TUNER_DIAL, 14, rawControlData));
	AddInput(new AxisButton("Tuner Dial 16", TUNER_DIAL, 15, rawControlData));
}

void SBC::SteelBattalionController::SetLEDBuffer(const uint8_t *newData, size_t length)
{
	uint8_t buffer[LEDDATASTART + LEDDATASIZE];
	memset(buffer, 0, LEDDATASTART);
	memcpy(&buffer[LEDDATASTART], newData, min(length, LEDDATASIZE));
	SetBufferOut(buffer, LEDDATASTART + min(length, LEDDATASIZE));
}

void SBC::SteelBattalionController::resetLEDs()
{
	uint8_t buffer[LEDDATASTART + LEDDATASIZE];
	memset(buffer, 0, LEDDATASTART + LEDDATASIZE);
	SetBufferOut(buffer, LEDDATASTART + LEDDATASIZE);
}

const uint8_t* SBC::SteelBattalionController::GetRawControlData() const
{
	const uint8_t* bufferIn = GetBufferIn();
	return &bufferIn[CONTROLDATASTART];
}

SBC::SteelBattalionController::~SteelBattalionController()
{
	masks.deInit();
}

#pragma region axis helpers

ControlState getAimingAxis(uint8_t raw)
{
	return (ControlState(raw) - 127) / 128;
}

ControlState getRotationLeverAxis(uint8_t raw)
{
	auto cs = ControlState(raw);
	return (cs < 128 ? cs : (cs - 255)) / 128;
}

ControlState getSightChangeAxis(uint8_t raw)
{
	auto cs = ControlState(raw);
	return (cs < 128 ? cs : (cs - 255)) / 128;
}

ControlState getPedalAxis(uint8_t raw)
{
	return ControlState(raw) / 255;
}

#pragma endregion axis helpers

#pragma region Buttons.h

SBC::ButtonMask::ButtonMask(int bytePos, int maskValue) : bytePos(bytePos), maskValue(maskValue)
{
}

void SBC::ButtonMasks::Init()
{
	Masks[ButtonEnum::MainWeapon] = new ButtonMask(2, 0x01);
	Masks[ButtonEnum::Fire] = new ButtonMask(2, 0x02);
	Masks[ButtonEnum::LockOn] = new ButtonMask(2, 0x04);
	Masks[ButtonEnum::EmergencyEject] = new ButtonMask(2, 0x08);
	Masks[ButtonEnum::CockpitHatch] = new ButtonMask(2, 0x10);
	Masks[ButtonEnum::Ignition] = new ButtonMask(2, 0x20);
	Masks[ButtonEnum::Start] = new ButtonMask(2, 0x40);
	Masks[ButtonEnum::OpenClose] = new ButtonMask(2, 0x80);
	Masks[ButtonEnum::MapZoomInOut] = new ButtonMask(3, 0x01);
	Masks[ButtonEnum::ModeSelect] = new ButtonMask(3, 0x02);
	Masks[ButtonEnum::SubMonitorModeSelect] = new ButtonMask(3, 0x04);
	Masks[ButtonEnum::MainMonitorZoomIn] = new ButtonMask(3, 0x08);
	Masks[ButtonEnum::MainMonitorZoomOut] = new ButtonMask(3, 0x10);
	Masks[ButtonEnum::ForecastShootingSystem] = new ButtonMask(3, 0x20);
	Masks[ButtonEnum::Manipulator] = new ButtonMask(3, 0x40);
	Masks[ButtonEnum::LineColorChange] = new ButtonMask(3, 0x80);
	Masks[ButtonEnum::Washing] = new ButtonMask(4, 0x01);
	Masks[ButtonEnum::Extinguisher] = new ButtonMask(4, 0x02);
	Masks[ButtonEnum::Chaff] = new ButtonMask(4, 0x04);
	Masks[ButtonEnum::TankDetach] = new ButtonMask(4, 0x08);
	Masks[ButtonEnum::Override] = new ButtonMask(4, 0x10);
	Masks[ButtonEnum::NightScope] = new ButtonMask(4, 0x20);
	Masks[ButtonEnum::F1] = new ButtonMask(4, 0x40);
	Masks[ButtonEnum::F2] = new ButtonMask(4, 0x80);
	Masks[ButtonEnum::F3] = new ButtonMask(5, 0x01);
	Masks[ButtonEnum::MainWeaponControl] = new ButtonMask(5, 0x02);
	Masks[ButtonEnum::SubWeaponControl] = new ButtonMask(5, 0x04);
	Masks[ButtonEnum::MagazineChange] = new ButtonMask(5, 0x08);
	Masks[ButtonEnum::Comm1] = new ButtonMask(5, 0x10);
	Masks[ButtonEnum::Comm2] = new ButtonMask(5, 0x20);
	Masks[ButtonEnum::Comm3] = new ButtonMask(5, 0x40);
	Masks[ButtonEnum::Comm4] = new ButtonMask(5, 0x80);
	Masks[ButtonEnum::Comm5] = new ButtonMask(6, 0x01);
	Masks[ButtonEnum::SightChange] = new ButtonMask(6, 0x02);
	Masks[ButtonEnum::FilterControl] = new ButtonMask(6, 0x04);
	Masks[ButtonEnum::OxygenSupply] = new ButtonMask(6, 0x08);
	Masks[ButtonEnum::FuelFlowRate] = new ButtonMask(6, 0x10);
	Masks[ButtonEnum::BufferMaterial] = new ButtonMask(6, 0x20);
	Masks[ButtonEnum::VtLocationMeasurement] = new ButtonMask(6, 0x40);
	Masks[ButtonEnum::TunerDialStateChanged] = new ButtonMask(24, 0x0F);
	Masks[ButtonEnum::GearLeverStateChange] = new ButtonMask(25, 0xFF);
}

void SBC::ButtonMasks::deInit()
{
	for (auto it = Masks.begin(); it != Masks.end(); it++) {
		delete it->second;
		it->second = nullptr;
	}
	Masks.clear();
}

std::string getString(SBC::ButtonEnum btn)
{
	switch (btn)
	{
	case SBC::ButtonEnum::MainWeapon: return "MainWeapon";
	case SBC::ButtonEnum::Fire: return "Fire";
	case SBC::ButtonEnum::LockOn: return "LockOn";
	case SBC::ButtonEnum::CockpitHatch: return "CockpitHatch";
	case SBC::ButtonEnum::Ignition: return "Ignition";
	case SBC::ButtonEnum::Start: return "Start";
	case SBC::ButtonEnum::EmergencyEject: return "EmergencyEject";
	case SBC::ButtonEnum::OpenClose: return "OpenClose";
	case SBC::ButtonEnum::MapZoomInOut: return "MapZoomInOut";
	case SBC::ButtonEnum::ModeSelect: return "ModeSelect";
	case SBC::ButtonEnum::SubMonitorModeSelect: return "SubMonitorModeSelect";
	case SBC::ButtonEnum::MainMonitorZoomIn: return "MainMonitorZoomIn";
	case SBC::ButtonEnum::MainMonitorZoomOut: return "MainMonitorZoomOut";
	case SBC::ButtonEnum::Washing: return "Washing";
	case SBC::ButtonEnum::Extinguisher: return "Extinguisher";
	case SBC::ButtonEnum::Chaff: return "Chaff";
	case SBC::ButtonEnum::MainWeaponControl: return "MainWeaponControl";
	case SBC::ButtonEnum::SubWeaponControl: return "SubWeaponControl";
	case SBC::ButtonEnum::MagazineChange: return "MagazineChange";
	case SBC::ButtonEnum::ForecastShootingSystem: return "ForecastShootingSystem";
	case SBC::ButtonEnum::Manipulator: return "Manipulator";
	case SBC::ButtonEnum::LineColorChange: return "LineColorChange";
	case SBC::ButtonEnum::TankDetach: return "TankDetach";
	case SBC::ButtonEnum::Override: return "Override";
	case SBC::ButtonEnum::NightScope: return "NightScope";
	case SBC::ButtonEnum::F1: return "F1";
	case SBC::ButtonEnum::F2: return "F2";
	case SBC::ButtonEnum::F3: return "F3";
	case SBC::ButtonEnum::Comm1: return "Comm1";
	case SBC::ButtonEnum::Comm2: return "Comm2";
	case SBC::ButtonEnum::Comm3: return "Comm3";
	case SBC::ButtonEnum::Comm4: return "Comm4";
	case SBC::ButtonEnum::Comm5: return "Comm5";
	case SBC::ButtonEnum::SightChange: return "SightChange";
	case SBC::ButtonEnum::FilterControl: return "FilterControl";
	case SBC::ButtonEnum::OxygenSupply: return "OxygenSupply";
	case SBC::ButtonEnum::FuelFlowRate: return "FuelFlowRate";
	case SBC::ButtonEnum::BufferMaterial: return "BufferMaterial";
	case SBC::ButtonEnum::VtLocationMeasurement: return "VtLocationMeasurement";
	case SBC::ButtonEnum::TunerDialStateChanged: return "TunerDialStateChanged";
	case SBC::ButtonEnum::GearLeverStateChange: return "GearLeverStateChange";
	default:
		return "Unknown";
	}
}

#pragma endregion Buttons.h
