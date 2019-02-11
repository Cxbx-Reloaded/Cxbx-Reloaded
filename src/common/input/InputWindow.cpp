#include "InputWindow.h"
#include <future>

#define INPUT_TIMEOUT 5000


InputDevice::Input* InputWindow::Detect(InputDevice* const Device)
{
	using namespace std::chrono;

	auto now = system_clock::now();
	auto timeout = now + milliseconds(INPUT_TIMEOUT);
	std::vector<InputDevice::Input*>::const_iterator i = Device->GetInputs().begin(),
		e = Device->GetInputs().end();

	while (now <= timeout) {
		for (; i != e; i++) {
			if ((*i)->GetState()) {
				return *i; // user pressed a button
			}
		}
		std::this_thread::sleep_for(milliseconds(10));
		now += milliseconds(10);
	}

	return nullptr; // no input
}

void InputWindow::BindButton(int ControlID, std::string DeviceName)
{
	auto dev = g_InputDeviceManager.FindDevice(DeviceName);
	if (dev != nullptr) {
		// Don't block the message processing loop
		std::thread([this, &dev, ControlID]() {
			char current_text[50];
			Button* xbox_button = m_DeviceConfig->FindButton(ControlID);
			xbox_button->GetText(current_text, sizeof(current_text));
			xbox_button->UpdateText("...");
			std::future<InputDevice::Input*> fut = std::async(std::launch::async, &InputWindow::Detect, this, dev.get());
			InputDevice::Input* dev_button = fut.get();
			if (dev_button) {
				dev->SetBindings(xbox_button->GetIndex(), dev_button);
				xbox_button->UpdateText(dev_button->GetName().c_str());
			}
			else {
				xbox_button->UpdateText(current_text);
			}
		});
	}
}

void InputWindow::ConstructEmuDevice(int Type, HWND hwnd)
{
	m_DeviceConfig = new EmuDevice(Type, hwnd);
}

void InputWindow::DestroyEmuDevice()
{
	delete m_DeviceConfig;
	m_DeviceConfig = nullptr;
}
