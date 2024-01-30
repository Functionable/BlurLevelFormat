#pragma once

#include "bakedlist.hpp"
#include "unbakedlist.hpp"

#include "object/objectdefinition.hpp"
#include "object/foreignobjectdefinition.hpp"

#include <initializer_list>

namespace blf
{
    class ObjectTable;

    /**
     * A dynamic list meant to store ObjectDefinitions.
     * Can be 'baked' into an ObjectTable.
     */
    class UnbakedObjectTable : public UnbakedList<ForeignObjectDefinition>
    {   
        private:
            using List = UnbakedList<ForeignObjectDefinition>;
        public:
            UnbakedObjectTable() : List()
            {}

            UnbakedObjectTable(const UnbakedObjectTable& table) 
                : List(table)
            {}

            UnbakedObjectTable(const std::initializer_list<ForeignObjectDefinition>& initializerList) 
                : List(initializerList)
            {}

            /**
             * Bakes the UnbakedObjectTable into a static list.
             */
            ObjectTable bake();
    };


    class ObjectTable : public UnbakedList<ForeignObjectDefinition>::BakedList
    {
        private:
            void validateTable();

        public:

            /**
             * Takes an UnbakedObjectTable and bakes it into
             * a baked object table.
             *
             * Will throw std::invalid_argument if the UnbakedObjectTable
             * contained a definition with name equal to NULL_OBJECT_NAME.
             */
            ObjectTable(const UnbakedObjectTable& table)
                : UnbakedList<ForeignObjectDefinition>::BakedList(table)
            {
                validateTable();
            }

            /**
             * Returns the first ObjectDefinition with the given name.
             *
             * Throws std::out_of_range if the name cannot be found.
             */
            const ForeignObjectDefinition& findDefinition(const std::string& name) const;
    };
}