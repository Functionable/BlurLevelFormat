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
        for( ForeignObjectDefinition definition : *this )
        {
            if( definition.getName() == NULL_OBJECT_NAME )
            {
                throw std::invalid_argument("Invalid definition (with null name) in ObjectTable.");
            }
        }
    }

    const ForeignObjectDefinition& ObjectTable::findDefinition(const std::string& name) const
    {
        // TODO: clean this up
        size_t i = 0;
        for( const ForeignObjectDefinition& definition : *this )
        {
            if( definition.getName() == name )
            { 
                return definition;
            }
            i++;
        }

        throw std::out_of_range("Definition name not found in ObjectTable.");
    }
}