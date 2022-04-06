#include "blf/foreignattributetable.hpp"

namespace blf
{
    void ForeignAttributeTable::addAttribute(ObjectAttribute definition)
    {
        attributes.push_back(definition);
    }
}