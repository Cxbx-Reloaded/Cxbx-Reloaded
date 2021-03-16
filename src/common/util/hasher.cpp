#include "hasher.h"

#include "xxhash.h"
#include "crc32c.h"
#include <cstdio>

enum {
    HASH_NONE = 0,
    HASH_XXH3,
    HASH_CRC32C
};

static int g_HashAlgorithm = HASH_NONE;

void InitHasher()
{
    // Detect the best hashing algorithm to use for the host machine
    // TODO/Future Improvement: This could be expanded to support even more hash algorithims
    // And we could hash a random buffer to calculate the fastest hash to use on a given host
    printf("Selecting hash algorithm: ");
    if (crc32c_hw_available()) {
        printf("CRC32C\n");
        g_HashAlgorithm = HASH_CRC32C;
    } else {
        printf("XXH3\n");
        g_HashAlgorithm = HASH_XXH3;
    }
}

uint64_t ComputeHash(const void* data, size_t len)
{
    if (g_HashAlgorithm == HASH_NONE) {
        InitHasher();
    }

    switch (g_HashAlgorithm) {
        case HASH_XXH3: return XXH3_64bits(data, len);
        case HASH_CRC32C: return crc32c_append(0, (uint8_t*)data, len);
    }

    return 0;
}
