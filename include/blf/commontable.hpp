#pragma once

#include <map>
#include <cstdint>

#include "types.hpp"

#include "bakedlist.hpp"
#include "objectdata.hpp"
#include "unbakedlist.hpp"

namespace blf
{
    struct CommonEntry
    {
        ObjectID objectID;

        CommonEntry() 
            : objectID(NULL_OBJECT)
        {}
    };

    // Used to determine what should be stored within the commontable.
    // Used to create a commontable.
    class CommonTableRecipe
    {
        private:
            uint8_t m_indexerSize;          

            size_t m_namedEntries = 0;
            std::map<char*, CommonEntry> m_commonEntries;

        public:
            CommonTableRecipe()
                : m_indexerSize(0), m_commonEntries()
            {}

            ~CommonTableRecipe() = default;

            void add(char* ptr);

            /**
             * Queries the CommonTable whether the given ptr will be included within the commontable.
             */
            bool query(char* ptr) const;

            size_t size() const { return m_namedEntries; }

            // Passing through const_iterator functions of the entries map.
            auto begin() const { return m_commonEntries.begin(); }
            auto end() const { return m_commonEntries.end(); }        
    };

    class CommonTable
    {
        private:
            BakedList<char*> m_list;

            CommonTable()
                : m_list(BakedList<char*>::empty())
            {}

        public:
            static CommonTable empty() { return CommonTable(); }

            CommonTable(CommonTableRecipe& recipe) : CommonTable()
            {                
                std::vector<char*> storedPtrs(recipe.size());
                
                for( auto&& [ptr, entry] : recipe )
                {
                    storedPtrs[entry.objectID - 1] = ptr;
                }

                UnbakedList<char*> unbakedPtrs = UnbakedList<char*>(storedPtrs);
                m_list = unbakedPtrs.bake();
            }
    };
}