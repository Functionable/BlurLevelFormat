#include "blf/enums.hpp"

namespace blf
{
    int8_t typeSize(const BLF_TYPE type)
    {
        int8_t typeSizes[] = {
            [TYPE_NULL] = 0,
            [TYPE_UNKNOWN] = 0,
            [TYPE_BYTE] = 1,
            [TYPE_SHORT] = 2,
            [TYPE_INT] = 4,
            [TYPE_LONG] = 8,
            [TYPE_FLOAT] = 4,
            [TYPE_DOUBLE] = 8,

            [TYPE_STRING] = DYNAMIC_TYPE,
            [TYPE_OBJECT] = DYNAMIC_TYPE,
            [TYPE_OBJECTREFERENCE] = DYNAMIC_TYPE
        };

        return typeSizes[type];
    }
}