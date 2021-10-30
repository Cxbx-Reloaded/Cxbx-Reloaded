#ifndef __STEELBATTALIONCONTROLLER_H__
#define __STEELBATTALIONCONTROLLER_H__

#include "InputDevice.h"
#include "LibusbDevice.h"


namespace SBC
{
#pragma region Buttons.h

	enum class ControllerLEDEnum
	{
		EmergencyEject = 4,			//	 4
		CockpitHatch,				//	 5
		Ignition,					//	 6
		Start,						//	 7
		OpenClose,					//	 8
		MapZoomInOut,				//	 9
		ModeSelect,					//	10
		SubMonitorModeSelect,		//	11
		MainMonitorZoomIn,			//	12
		MainMonitorZoomOut,			//	13
		ForecastShootingSystem,		//	14
		Manipulator,				//	15
		LineColorChange,			//	16
		Washing,					//	17
		Extinguisher,				//	18
		Chaff,						//	19
		TankDetach,					//	20
		Override,					//	21
		NightScope,					//	22
		F1,							//	23
		F2,							//	24
		F3,							//	25
		MainWeaponControl,			//	26
		SubWeaponControl,			//	27
		MagazineChange,				//	28
		Comm1,						//	29
		Comm2,						//	30
		Comm3,						//	31
		Comm4,						//	32
		Comm5,						//	33

		GearR = 35,					//	35
		GearN,						//	36
		Gear1,						//	37
		Gear2,						//	38
		Gear3,						//	39
		Gear4,						//	40
		Gear5						//	41
	};

	enum class ButtonEnum
	{
		MainWeapon,					//	 0
		Fire,						//	 1
		LockOn,						//	 2
		CockpitHatch,				//	 3
		Ignition,					//	 4
		Start,						//	 5
		EmergencyEject,				//	 6
		OpenClose,					//	 7
		MapZoomInOut,				//	 8
		ModeSelect,					//	 9
		SubMonitorModeSelect,		//	10
		MainMonitorZoomIn,			//	11
		MainMonitorZoomOut,			//	12
		Washing,					//	13
		Extinguisher,				//	14
		Chaff,						//	15
		MainWeaponControl,			//	16
		SubWeaponControl,			//	17
		MagazineChange,				//	18
		ForecastShootingSystem,		//	19
		Manipulator,				//	20
		LineColorChange,			//	21
		TankDetach,					//	22
		Override,					//	23
		NightScope,					//	24
		F1,							//	25
		F2,							//	26
		F3,							//	27
		Comm1,						//	28
		Comm2,						//	29
		Comm3,						//	30
		Comm4,						//	31
		Comm5,						//	32
		SightChange,				//	33
		FilterControl,				//	34
		OxygenSupply,				//	35
		FuelFlowRate,				//	36
		BufferMaterial,				//	37
		VtLocationMeasurement,		//	38
		TunerDialStateChanged,		//	39
		GearLeverStateChange		//	40
	};

	enum class AxisType
	{
		AimingAxis,
		SightChangeAxis,
		RotationLeverAxis,
		PedalAxis
	};

	enum class AxisDirection
	{
		Positive,
		Negative
	};

	struct ButtonMask
	{
		int bytePos;
		int maskValue;

		ButtonMask(int bytePos, int maskValue);
	};

	struct ButtonMasks
	{
		std::map<ButtonEnum, ButtonMask*> Masks;
		void Init();
		void deInit();
	};

	struct ButtonState
	{
		ButtonEnum button;
		bool currentState;
		bool changed;
	};

#pragma endregion Buttons.h

	class SteelBattalionController : public InputDevice, libusb::LibusbDevice
	{
	public:
		SteelBattalionController(libusb_device* dev);
		std::string GetDeviceName() const override;
		std::string GetAPI() const override;
		void SetLEDBuffer(const uint8_t* data, size_t length);
		const uint8_t* GetRawControlData() const;
		bool UpdateInput() override;
		~SteelBattalionController();

		static void PopulateDevices();

	private:
		/// <summary>
		/// Used for all normal buttons and toggle switches
		/// </summary>
		class Button : public InputDevice::Input
		{
		private:
			ButtonEnum btn;
			ButtonMask mask;
			const uint8_t* controlBuffer;
		public:
			Button(ButtonEnum btn, ButtonMask mask, const uint8_t* controlBuffer) : btn(btn), mask(mask), controlBuffer(controlBuffer) {}
			ControlState GetState() const override;
			std::string GetName() const override;
		};

		/// <summary>
		/// Used for Tuner Dial positions, and Shifter positions
		/// </summary>
		class AxisButton : public InputDevice::Input
		{
			std::string name;
			int index;
			int bufferOffset;
			uint8_t axisPosition;
			const uint8_t* controlBuffer;
		public:
			AxisButton(std::string name, int bufferOffset, uint8_t axisPosition, const uint8_t* controlBuffer) : name(name), bufferOffset(bufferOffset), axisPosition(axisPosition), controlBuffer(controlBuffer) {}
			ControlState GetState() const override;
			std::string GetName() const override;
		};

		/// <summary>
		/// Used for all Axes that behave as axes
		/// </summary>
		class Axis : public InputDevice::Input
		{
			std::string name;
			int bufferOffset;
			AxisType axisType;
			AxisDirection axisDirection;
			const uint8_t* controlBuffer;
		public:
			Axis(std::string name, int bufferOffset, AxisType axisType, AxisDirection axisDirection, const uint8_t* controlBuffer) : name(name), bufferOffset(bufferOffset), axisType(axisType), axisDirection(axisDirection), controlBuffer(controlBuffer) {}
			ControlState GetState() const override;
			std::string GetName() const override;
		};

		ButtonMasks masks;
		std::vector<Input*> mInputs;
		void initInputDevice();
		void resetLEDs();
	};
}

#endif
