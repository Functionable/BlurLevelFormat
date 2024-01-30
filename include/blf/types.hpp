#pragma once

#include <string>
#include <cstdint>

namespace blf
{
    using ObjectID = uint16_t;
    using ObjectName = std::string;
    using stringsize_t = uint32_t;
    using listsize_t = uint32_t;

    const ObjectID NULL_OBJECT = 0;
    const ObjectName NULL_OBJECT_NAME = "__NULL!";
}