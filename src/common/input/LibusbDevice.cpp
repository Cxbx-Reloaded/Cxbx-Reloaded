#include "LibusbDevice.h"
#include "libusb.h"

#define LOG_PREFIX _CXBXR_MODULE::IO
#include "core\kernel\exports\EmuKrnl.h" // For EmuLog

#define min(a, b) ((a < b) ? a : b)

libusb_context* libusb::LibusbDevice::ctx = nullptr;
libusb_device** libusb::LibusbDevice::list = nullptr;
int libusb::LibusbDevice::libusb_error = 0;
int libusb::LibusbDevice::deviceCount = 0;

std::string GetErrorString(int errorCode);

void libusb::LibusbDevice::RefreshDeviceList()
{
	if (ctx == nullptr)
		libusb_error = libusb_init(&ctx);

	if (list != nullptr)
	{
		libusb_free_device_list(list, 1);
		list = nullptr;
	}

	deviceCount = libusb_get_device_list(ctx, &list);
}

std::vector<libusb_device*> libusb::LibusbDevice::GetDevices(uint16_t VID, uint16_t PID)
{
	std::vector<libusb_device*> devices;

	RefreshDeviceList();

	if (libusb_error == 0)
	{
		for (int i = 0; i < deviceCount; i++)
		{
			libusb_device* dev = list[i];
			libusb_device_descriptor descriptor;
			libusb_get_device_descriptor(dev, &descriptor);
			{
				if (descriptor.idVendor == VID && descriptor.idProduct == PID)
				{
					devices.push_back(dev);
				}
			}
		}
	}

	return devices;
}

libusb::LibusbDevice::LibusbDevice(libusb_device* dev) :
	device(nullptr),
	endpointIn(0),
	bufferInSize(0),
	bufferIn(nullptr),
	endpointOut(0),
	bufferOutSize(0),
	bufferOut(nullptr),
	updateThread(nullptr),
	pollingInterval(0),
	isConnected(true),
	dev(dev)
{

	std::unique_lock<std::mutex>(mtx);

	if (ctx == nullptr)
		libusb_error = libusb_init(&ctx);
	if (libusb_error == 0)
	{
		if (device == nullptr)
		{
			libusb_error = libusb_open(dev, &device);
			libusb_config_descriptor* desc;
			libusb_get_active_config_descriptor(dev, &desc);
			if (desc != nullptr)
			{
				if (desc->bNumInterfaces > 0)
				{
					for (uint8_t i = 0; i < desc->bNumInterfaces; i++)
					{
						auto iface = desc->interface[i];
						if (iface.num_altsetting > 0)
						{
							for (int j = 0; j < iface.num_altsetting; j++)
							{
								auto setting = iface.altsetting[i];
								if (setting.bNumEndpoints > 0)
								{
									for (uint8_t k = 0; k < setting.bNumEndpoints; k++)
									{
										auto endpoint = setting.endpoint[k];
										if (endpoint.bmAttributes & LIBUSB_ENDPOINT_TRANSFER_TYPE_INTERRUPT)
										{
											if (endpoint.bEndpointAddress & 0x80 && endpointIn == 0)
											{
												if (endpointIn == 0)
												{
													endpointIn = endpoint.bEndpointAddress;
													bufferInSize = endpoint.wMaxPacketSize;
													bufferIn = new uint8_t[bufferInSize];
													pollingInterval = endpoint.bInterval;
												}
											}
											else
											{
												if (endpointOut == 0)
												{
													endpointOut = endpoint.bEndpointAddress;
													bufferOutSize = endpoint.wMaxPacketSize;
													bufferOut = new uint8_t[bufferOutSize];
												}
											}
											if (endpointIn != 0 && endpointOut != 0)
												break;
										}
									}
								}
							}
						}
					}
				}
				libusb_free_config_descriptor(desc);
			}
		}

		if (device != nullptr)
		{
			libusb_claim_interface(device, 0);
			updateThread = new std::thread(threadStart, this);
		}
	}
	else
		EmuLog(LOG_LEVEL::ERROR2, "Error initializing libusb: %s", GetErrorString(libusb_error).c_str());
}

void libusb::LibusbDevice::threadStart(LibusbDevice* lud)
{
	lud->pollLoop();
}

void libusb::LibusbDevice::pollLoop()
{
	bool connected = true;
	while (!shutdown.load(std::memory_order_acquire))
	{
		if (connected)
		{
			connected = poll();
		}
		else
		{
			if (!libusb_open(dev, &device))
			{
				connected = (!libusb_claim_interface(device, 0));
				if(connected)
					EmuLog(LOG_LEVEL::INFO, "Device Reconnected");
			}
		}
	}
}

bool libusb::LibusbDevice::poll()
{
	int32_t done = 0;

	int error = libusb_bulk_transfer(device, endpointIn, bufferIn, bufferInSize, &done, 20);//pollingInterval);		// bulk transfer the bytes over from the device into memory
	if (error != 0 && error != LIBUSB_ERROR_TIMEOUT)
	{
		if (error == LIBUSB_ERROR_IO || error == LIBUSB_ERROR_NO_DEVICE) // Disconnected
		{
			EmuLog(LOG_LEVEL::INFO, "Device Disconnected");
			return false;
		}
		else
		{
			EmuLog(LOG_LEVEL::WARNING, "Read Error: %s", GetErrorString(error).c_str());
		}
	}
	return true;
}

bool libusb::LibusbDevice::IsConnected() const
{
	return isConnected;
}

const uint8_t* libusb::LibusbDevice::GetBufferIn() const
{
	return bufferIn;
}

void libusb::LibusbDevice::SetBufferOut(const uint8_t* buffer, size_t length)
{
	// Constrain the write to ensure that it doesn't overrun the buffer
	size_t end = length;
	if (end > bufferOutSize)
		length = bufferOutSize;

	int done = 0;
	libusb_bulk_transfer(device, endpointOut, (uint8_t*)buffer, length, &done, 0); // Begin the bulk transfer of bytes from the device to the buffer in memory.
}

libusb::LibusbDevice::~LibusbDevice()
{
	if (device != nullptr)
	{
		if (updateThread != nullptr)
		{
			shutdown.store(true, std::memory_order_release);
			updateThread->join();
			updateThread = nullptr;
		}

		libusb_release_interface(device, 0);
		libusb_close(device);
		
		device = nullptr;
	}		
}

std::string GetErrorString(int errorCode)
{
	switch (errorCode)
	{
	case libusb_error::LIBUSB_SUCCESS: return "LIBUSB_SUCCESS";
	case libusb_error::LIBUSB_ERROR_IO: return "LIBUSB_ERROR_IO";
	case libusb_error::LIBUSB_ERROR_INVALID_PARAM: return "LIBUSB_ERROR_INVALID_PARAM";
	case libusb_error::LIBUSB_ERROR_ACCESS: return "LIBUSB_ERROR_ACCESS";
	case libusb_error::LIBUSB_ERROR_NO_DEVICE: return "LIBUSB_ERROR_NO_DEVICE";
	case libusb_error::LIBUSB_ERROR_NOT_FOUND: return "LIBUSB_ERROR_NOT_FOUND";
	case libusb_error::LIBUSB_ERROR_BUSY: return "LIBUSB_ERROR_BUSY";
	case libusb_error::LIBUSB_ERROR_TIMEOUT: return "LIBUSB_ERROR_TIMEOUT";
	case libusb_error::LIBUSB_ERROR_OVERFLOW: return "LIBUSB_ERROR_OVERFLOW";
	case libusb_error::LIBUSB_ERROR_PIPE: return "LIBUSB_ERROR_PIPE";
	case libusb_error::LIBUSB_ERROR_INTERRUPTED: return "LIBUSB_ERROR_INTERRUPTED";
	case libusb_error::LIBUSB_ERROR_NO_MEM: return "LIBUSB_ERROR_NO_MEM";
	case libusb_error::LIBUSB_ERROR_NOT_SUPPORTED: return "LIBUSB_ERROR_NOT_SUPPORTED";
	case libusb_error::LIBUSB_ERROR_OTHER: return "LIBUSB_ERROR_OTHER";
	default:
		return "UNKNOWN";
	}
}
