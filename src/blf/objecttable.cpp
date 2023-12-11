#include "blf/objecttable.hpp"

#include "blf/types.hpp"

#include <stdexcept>

namespace blf
{
    ObjectTable UnbakedObjectTable::bake()
    {
        return ObjectTable(*this);
    }

    void ObjectTable::validateTable()
    {
        for( ObjectDefinition* definition : *this )
        {
            if( definition->getName() == NULL_OBJECT_NAME )
            {
                throw std::invalid_argument("Invalid definition (with null name) in ObjectTable.");
            }
        }
    }

    ObjectDefinition& ObjectTable::findDefinition(const std::string& name) const
    {
        for( ObjectDefinition* definition : *this )
        {
            if( definition->getName() == name )
            { 
                return *definition;
            }
        }

        throw std::out_of_range("Definition name not found in ObjectTable.");
    }
}