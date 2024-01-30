#pragma once

#include "../objecttable.hpp"

namespace blf
{
    class Writer
    {
        private:
            void writeObjectTable(const ObjectTable& table);
    };
}