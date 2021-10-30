#ifndef __LIBUSBXBOXCONTROLLER_H__
#define __LIBUSBXBOXCONTROLLER_H__

#include "InputDevice.h"
#include "LibusbDevice.h"

namespace libusbXbox
{
	struct vidpid
	{
		uint16_t VID;
		uint16_t PID;
		std::string Name;
	};

	class LibusbXboxController : public InputDevice, libusb::LibusbDevice
	{
	public:
		LibusbXboxController(libusb_device *dev);
		std::string GetDeviceName() const override;
		std::string GetAPI() const override;
		void SetRumble(const uint8_t* data, size_t bufferSize);
		const uint8_t* GetRawControlData() const;
		bool UpdateInput() override;
		~LibusbXboxController();
		static void PopulateDevices();

	private:
		class Button : public InputDevice::Input
		{
		public:
			Button(LibusbXboxController* Parent, uint16_t ButtonMask, std::string Name) : m_Parent(Parent), m_ButtonMask(ButtonMask), m_Name(Name) { }
			ControlState GetState() const override;
			std::string GetName() const override;
		private:
			LibusbXboxController* m_Parent;
			uint16_t m_ButtonMask;
			std::string m_Name;
		};

		class AxisButton : public InputDevice::Input
		{
		public:
			AxisButton(LibusbXboxController* Parent, int Index, std::string Name) : m_Parent(Parent), m_Index(Index), m_Name(Name) { }
			ControlState GetState() const override;
			std::string GetName() const override;
		private:
			LibusbXboxController* m_Parent;
			int m_Index;
			std::string m_Name;
		};

		class Axis : public InputDevice::Input
		{
		public:
			Axis(LibusbXboxController* Parent, std::string Name, int Index) : m_Parent(Parent), m_Name(Name), m_Index(Index) { }
			ControlState GetState() const override;
			std::string GetName() const override;
		private:
			LibusbXboxController* m_Parent;
			std::string m_Name;
			int m_Index;
		};

		class Motor : public InputDevice::Output
		{
		public:
			Motor(LibusbXboxController* Parent)
				: m_Parent(Parent) {}
			std::string GetName() const override;
			void SetState(ControlState StateLeft, ControlState StateRight) override;

		private:
			// parent controller
			LibusbXboxController* m_Parent;
		};

		std::string name;
		void initInputDevice();
		static std::vector<vidpid> vidpids;
		static void populateVidPids();
	};
}

#endif
