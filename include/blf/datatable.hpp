#pragma once

#include <cstddef>
#include <utility>

#include "bakedlist.hpp"
#include "objectdata.hpp"
#include "unbakedlist.hpp"

namespace blf
{
    using UnbakedDataList = UnbakedList<ObjectData>;

    class DataTable
    {
        private:
            BakedList<ObjectData> m_objectViews;

        public:
            DataTable(const BakedList<ObjectData>& data)
                : m_objectViews(data)
            {}

            DataTable(const DataTable& table)
                : m_objectViews(table.m_objectViews)
            {}

            DataTable(DataTable&& table)
                : m_objectViews(std::move(table.m_objectViews))
            {}

            size_t size() const { return m_objectViews.size(); }
    };
}