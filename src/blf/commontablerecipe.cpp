#include "blf/commontable.hpp"

namespace blf
{
    void CommonTableRecipe::add(char* ptr)
    {
        bool present = m_commonEntries.find(ptr) != m_commonEntries.end();

        if( !present )
        {
            m_commonEntries.insert({ptr, CommonEntry()});
        }
        else 
        {
            CommonEntry& entry = m_commonEntries.at(ptr);

            if( entry.objectID == NULL_OBJECT )
            {
                entry.objectID = ++m_namedEntries;
            }
        }
    }

    bool CommonTableRecipe::query(char* ptr) const
    {
        return (m_commonEntries.find(ptr) != m_commonEntries.end()
            && m_commonEntries.at(ptr).objectID != NULL_OBJECT);
    }
}