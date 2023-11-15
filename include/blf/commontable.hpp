#pragma once

#include <cstdint>

#include "bakedlist.hpp"
#include "objectdata.hpp"

namespace blf
{
    class CommonTable
    {
        private:
            uint8_t m_indexerSize;
            BakedList<DataView> m_objectData;
    };
}