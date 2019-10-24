#ifndef CRC32C_H
#define CRC32C_H

#define CRC32C_API

#include <stdint.h>

/*
    Computes CRC-32C (Castagnoli) checksum. Uses Intel's CRC32 instruction if it is available.
    Otherwise it uses a very fast software fallback.
*/
extern "C" CRC32C_API uint32_t crc32c_append(
    uint32_t crc,               // Initial CRC value. Typically it's 0.
                                // You can supply non-trivial initial value here.
                                // Initial value can be used to chain CRC from multiple buffers.
    const uint8_t *input,       // Data to be put through the CRC algorithm.
    size_t length);             // Length of the data in the input buffer.


/*
	Software fallback version of CRC-32C (Castagnoli) checksum.
*/
extern "C" CRC32C_API uint32_t crc32c_append_sw(uint32_t crc, const uint8_t *input, size_t length);

/*
	Hardware version of CRC-32C (Castagnoli) checksum. Will fail, if CPU does not support related instructions. Use a crc32c_append version instead of.
*/
extern "C" CRC32C_API uint32_t crc32c_append_hw(uint32_t crc, const uint8_t *input, size_t length);

/*
	Checks is hardware version of CRC-32C is available.
*/
extern "C" CRC32C_API int crc32c_hw_available();

#endif
