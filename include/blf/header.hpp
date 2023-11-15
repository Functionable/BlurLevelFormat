#pragma once

#include "string.hpp"

#include <cstdint>
#include <vector>

namespace blf
{
    struct Header
    {
        String signature = "BLFF";
        uint32_t major;
        uint32_t minor;
        uint32_t fix;

        uint8_t compressionFlags;
    };

    // SHOULD THIS EXIST?
    struct FlagInfo
    {
        // Unused prior to 2.0.0.
        // A headerflag is a single object with a 
        // name labelling it.
        uint16_t flagCount;
        //std::vector<Flag>...
    };
}