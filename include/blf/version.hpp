#pragma once

#include <cstdint>

namespace blf
{
    // Version 2.0.0
    const int32_t VERSION_MAJOR = 2;
    const int32_t VERSION_MINOR = 0;
    const int32_t VERSION_FIX   = 0;

    inline const char* SIGNATURE = "BLFF";

    struct Version
    {
        int32_t major;
        int32_t minor;
        int32_t fix;

        Version(int32_t majorVer, int32_t minorVer, int32_t fixVer) 
        : major(majorVer), minor(minorVer), fix(fixVer) {}

        bool operator ==(const Version& otherVersion)
        {
            return (major == otherVersion.major) && (minor == otherVersion.minor) && (fix == otherVersion.fix);
        }

        bool operator !=(const Version& otherVersion)
        {
            return !((major == otherVersion.major) && (minor == otherVersion.minor) && (fix == otherVersion.fix));
        }
    };
}