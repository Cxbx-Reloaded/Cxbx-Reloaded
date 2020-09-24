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
// *  (c) 2019 ergo720
// *
// *  All rights reserved
// *
// ******************************************************************

#ifndef INPUT_DEVICE
#define INPUT_DEVICE

#include <string>
#include <vector>
#include <atomic>
#include <map>
#include <functional>
#include <condition_variable>
#include "SDL.h"

#define PORT_INVALID     -1
#define PORT_1            0
#define PORT_2            1
#define PORT_3            2
#define PORT_4            3

#define PORT_INC(port) ((port) + 1)
#define PORT_DEC(port) ((port) - 1)

#define DIRECTION_IN      0
#define DIRECTION_OUT     1

#define MO_AXIS_DEFAULT_RANGE   10l
#define MO_WHEEL_DEFAULT_RANGE  80l

typedef double ControlState;

/* enum indicating the device type to attach to the virtual xbox */
typedef enum class _XBOX_INPUT_DEVICE : int {
	DEVICE_INVALID = -1,
	MS_CONTROLLER_DUKE,
	MS_CONTROLLER_S,
	LIGHT_GUN,
	STEERING_WHEEL,
	MEMORY_UNIT,
	IR_DONGLE,
	STEEL_BATTALION_CONTROLLER,
	DEVICE_MAX,
}
XBOX_INPUT_DEVICE;

// Flags that indicate that WM_MOUSELEAVE and WM_MOUSEMOVE respectively are being tracked in the rendering window procedure
inline bool g_bIsTrackingMoLeave = false;
inline bool g_bIsTrackingMoMove = false;

// Lookup array used to translate a gui port to an xbox usb port and vice versa
extern int Gui2XboxPortArray[4];

// Global function used to retrieve the printable name of a xid type
std::string GetInputDeviceName(int dev_type);

/* Abstract class which represents a host device usable for input/output */
class InputDevice
{
public:
	virtual ~InputDevice();

	class IoControl
	{
	public:
		virtual std::string GetName() const = 0;
	};

	class Input : public IoControl
	{
	public:
		virtual ControlState GetState() const = 0;
	};

	class Output : public IoControl
	{
	public:
		virtual void SetState(ControlState StateLeft, ControlState StateRight) = 0;
	};

	// retrieves the vector of input controls
	const std::vector<Input*>& GetInputs() const { return m_Inputs; }
	// retrieves the vector of output controls
	const std::vector<Output*>& GetOutputs() const { return m_Outputs; }
	// retrieves all the i/o controls of this device
	const std::vector<IoControl*> GetIoControls();
	// retrieves the full name of the device (API/ID/API-specific name)
	std::string GetQualifiedName() const;
	// retrieves the API-specific name of the device
	virtual std::string GetDeviceName() const = 0;
	// retrieves the API used to control this device
	virtual std::string GetAPI() const = 0;
	// updates the state of the device and checks if its state has changed
	virtual bool UpdateInput() = 0;
	// retrieves the ID of this device
	int GetId() const { return m_ID; }
	// retrieves the sdl id or -1 for non-sdl devices
	virtual SDL_JoystickID GetId(SDL_JoystickID id) const { return -1; }
	// sets the ID of this device
	void SetId(int ID) { m_ID = ID; }
	// retrieves the port this device is attached to
	bool GetPort(int Port) const { return m_XboxPort[Port]; }
	// sets the port this device is attached to
	void SetPort(int Port, bool Connect) { m_XboxPort[Port] = Connect; }


protected:
	// adds an input control to the device
	void AddInput(Input* const In);
	// adds an output control to the device
	void AddOutput(Output* const Out);
	// indicates that the device has new input data available
	bool m_bDirty;
	// lock for the bindings map
	mutable std::mutex m_BindingsMtx;

public:
	// retrieves the map of input bindings
	const std::map<int, IoControl*> GetBindings() const {
		std::lock_guard<std::mutex> lck(m_BindingsMtx);
		return m_Bindings;
	}
	// sets a pair in the map of the input bindings
	void SetBindings(int XButton, IoControl* Control) {
		std::lock_guard<std::mutex> lck(m_BindingsMtx);
		m_Bindings[XButton] = Control;
	}

private:
	// arbitrary ID assigned by to the device
	int m_ID;
	// all the input controls detected and usable on this device
	std::vector<Input*> m_Inputs;
	// all the output controls detected and usable on this device
	std::vector<Output*> m_Outputs;
	// xbox port(s) this device is attached to
	bool m_XboxPort[4] = { false };
	// button bindings to the xbox device buttons
	std::map<int, IoControl*> m_Bindings;
};

#endif
