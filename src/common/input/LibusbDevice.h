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
// *  (c) 2021 ergo720
// *
// *  All rights reserved
// *
// ******************************************************************

#pragma once

#include "InputDevice.h"
// Suppress warning in libusb about zero sized array
#pragma warning(push)
#pragma warning(disable: 4200)
#include "libusb.h"
#pragma warning(pop)


namespace Libusb
{
	typedef enum _INIT_STATUS : int
	{
		NOT_INIT = -2,
		INIT_ERROR,
		INIT_SUCCESS,
	}
	INIT_STATUS;

	extern int InitStatus;

	// initialize Libusb
	void Init(std::mutex &Mtx);
	// shutdown Libusb
	void DeInit();
	// refresh the device list in response to a refresh command from the input GUI
	void PopulateDevices();
	// update the device list
	void GetDeviceChanges();

	class LibusbDevice : public InputDevice
	{
	public:
		~LibusbDevice();
		bool UpdateInput() override;
		bool ExecuteIo(void *Buffer, int Direction);

		LibusbDevice(libusb_device_descriptor *Desc, libusb_device *Dev);

		std::string GetDeviceName() const override;
		std::string GetAPI() const override;
		bool IsLibusb() const override { return m_Type != XBOX_INPUT_DEVICE::DEVICE_INVALID; }
		XBOX_INPUT_DEVICE GetLibusbType() const { return m_Type; }
		uint8_t GetUcType() { return m_UcType; }
		uint8_t GetUcSubType() { return m_UcSubType; }


	private:
		XBOX_INPUT_DEVICE m_Type;
		uint8_t m_UcType;
		uint8_t m_UcSubType;
		std::string m_Name;
		libusb_device_handle *m_hDev;
		unsigned char m_EndpointIn;
		unsigned char m_EndpointOut;
		uint8_t m_IntervalIn;
		uint8_t m_IntervalOut;
		uint8_t m_BufferInSize;
		uint8_t m_BufferOutSize;
		uint8_t m_IfaceNum;
		bool m_HasEndpointOut;
	};
}
