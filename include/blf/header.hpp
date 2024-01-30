#pragma once

#include "string.hpp"
#include "version.hpp"
#include "bakedlist.hpp"

#include <cstdint>
#include <vector>

namespace blf
{
    struct HeaderFlag
    {
        String name;
        String value;
    };

    struct Header
    {
        String signature = "BLFF";
        Version fileVersion;

        BakedList<HeaderFlag> flags;
    };
}