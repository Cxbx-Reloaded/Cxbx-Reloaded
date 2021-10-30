#ifndef __LIBUSB_DEVICE_H__
#define __LIBUSB_DEVICE_H__

#include <thread>
#include <mutex>
#include <vector>
#include <atomic>

struct libusb_context;
struct libusb_device_handle;
struct libusb_device;

namespace libusb
{
	class LibusbDevice
	{
	public:
		LibusbDevice(libusb_device* dev);
		bool IsConnected() const;
		~LibusbDevice();

	protected:
		
		const uint8_t* GetBufferIn() const;
		void SetBufferOut(const uint8_t* buffer, size_t bufferSize);
		static std::vector<libusb_device*> GetDevices(uint16_t VID, uint16_t PID);
		static void RefreshDeviceList();
		bool poll();
		
	private:
		static libusb_context* ctx;
		static int libusb_error;
		static libusb_device** list;
		static int deviceCount;
		std::thread* updateThread;
		std::atomic_bool shutdown;
		int pollingInterval;
		bool isConnected;

		libusb_device* dev;
		libusb_device_handle* device;

		uint8_t endpointIn;
		uint8_t* bufferIn;
		uint16_t bufferInSize;

		uint8_t endpointOut;
		uint8_t* bufferOut;
		uint16_t bufferOutSize;

		void pollLoop();
		static void threadStart(LibusbDevice* lud);
	};
}

#endif
