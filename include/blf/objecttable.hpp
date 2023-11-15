#pragma once

#include "bakedlist.hpp"
#include "unbakedlist.hpp"

#include "object/objectdefinition.hpp"

#include <initializer_list>

namespace blf
{
    //template<typename T>
    using BakedObjectTable = UnbakedList<ObjectDefinition>::BakedList;

    //template<typename T>
    using ObjectTable = UnbakedList<ObjectDefinition>;

    /*class BakedObjectTable : private BakedList<ObjectDefinition>
    {
        private:
            using List = UnbakedList<ObjectDefinition>::BakedList;

        public:
            BakedObjectTable(const List& list) 
                : List(list)
            {}

            using List::get;
            using List::begin;
            using List::end;
    };

    class ObjectTable : private UnbakedList<ObjectDefinition>
    {   
        private:
            using List = UnbakedList<ObjectDefinition>;
        public:
            ObjectTable() : List()
            {}

            ObjectTable(const ObjectTable& table) 
                : List(table)
            {}

            ObjectTable(const std::initializer_list<ObjectDefinition>& initializerList) 
                : List(initializerList)
            {}

            using List::add;
            using List::emplace;
            using List::get;

            BakedObjectTable bake()
            {
                return BakedObjectTable(*this); 
            }

            using List::size;
            using List::begin;
            using List::end;
    };*/
}