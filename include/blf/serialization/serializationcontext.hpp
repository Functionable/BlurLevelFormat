#pragma once

#include "../localobjecttable.hpp"

namespace blf
{
    struct SerializationContext
    {
        const LocalObjectTable& table;
    };
}