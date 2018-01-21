#ifndef _SMDEVICE_H_
#define _SMDEVICE_H_

#include <cstdint>

class SMDevice {
	public:
		virtual void Init() = 0;
		virtual void Reset() = 0;

		virtual void QuickCommand(bool read) = 0;
		virtual uint8_t ReceiveByte() = 0;
		virtual uint8_t ReadByte(uint8_t command) = 0;
		virtual uint16_t ReadWord(uint8_t command) = 0;
		virtual int ReadBlock(uint8_t command, uint8_t *data) = 0;

		virtual void SendByte(uint8_t data) = 0;
		virtual void WriteByte(uint8_t command, uint8_t value) = 0;
		virtual void WriteWord(uint8_t command, uint16_t value) = 0;
		virtual void WriteBlock(uint8_t command, uint8_t* data, int length) = 0;
};

#endif