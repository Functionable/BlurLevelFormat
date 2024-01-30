#include "blf/io/writer.hpp"

#include "blf/localobjecttable.hpp"
#include <type_traits>

namespace blf
{
    void Writer::writeObjectTable(const ObjectTable& table)
    {
        blf::LocalObjectTable localTable;

        auto attributeDefinition = localTable.define<ObjectAttribute>("BLFObjectAttribute",
            arg("name", &ObjectAttribute::getName, &ObjectAttribute::setName),
            arg("type", &ObjectAttribute::getType, &ObjectAttribute::setType),
            arg("id", &ObjectAttribute::getObject, &ObjectAttribute::setID)
        );

        auto objDef = localTable.define<ForeignObjectDefinition>("BLFObjectDefinition", 
            arg<ForeignObjectDefinition>("name", &ForeignObjectDefinition::getName, 
                                                 &ForeignObjectDefinition::setName),
            arg("attributes", &ForeignObjectDefinition::getAttributes, 
                              &ForeignObjectDefinition::setAttributes,
               attributeDefinition) 
        );
    }
}