#pragma once

#include "blf/templateobject.hpp"

#include <vector>
#include <stdexcept>

namespace blf
{
    template<class DataGroup>
    class DataGroupIterator
    {
        public: 
            using IndexType = typename DataGroup::IndexType;
            using ValueType = typename DataGroup::ValueType;
            using PointerType = ValueType*;
            using ReferenceType = ValueType&;
            using IndexArrayReference = typename DataGroup::IndicesArrayType&;

        private:
            IndexType   m_index;
            IndexType   m_maxSize;
            IndexArrayReference m_indices;
            TemplateObject** m_dataArray;
        
        public:
            DataGroupIterator(IndexType index, IndexType maxSize, IndexArrayReference indices, TemplateObject** dataArray) : m_index(index), m_maxSize(maxSize), m_indices(indices), m_dataArray(dataArray) { };
            
            DataGroupIterator& operator++()
            {
                m_index++;
                return *this;
            }
            
            DataGroupIterator operator++(int)
            {
                DataGroup iterator = *this;
                ++(*this);
                return iterator;
            }
            
            DataGroupIterator& operator--()
            {
                m_index--;
                return *this;
            }
            
            DataGroupIterator operator--(int)
            {
                DataGroup iterator = *this;
                --(*this);
                return iterator;
            }
            
            ReferenceType operator[](int index)
            {
                return m_dataArray[m_indices[index]];
            }
            
            PointerType operator->()
            {
                return &m_dataArray[m_indices[m_index]];
            }
            
            ValueType operator*()
            {
                return (ValueType)(m_dataArray[m_indices[m_index]]);
            }
            
            bool operator==(const DataGroupIterator& other) const
            {
                return m_index == other.m_index;
            }
            
            bool operator!=(const DataGroupIterator& other) const
            {
                return !(*this == other);
            }
    };
    
    /*
     *  Stores all indices of objects of a certain type inside of a DataTable
     */
    class BasicDataGroup
    {
        private:
            TemplateObject** m_dataArray;
            
        protected:
            TemplateObject** getDataArray() const
            {
                return m_dataArray;
            }
        
        public:
            BasicDataGroup(TemplateObject** const dataArray)
            {
                m_dataArray = dataArray;
            }
            
            virtual void addIndex(int index);
            virtual int getSize();
    };
    
    template<class T>
    class DataGroup : BasicDataGroup
    {
        public:
            using IndexType = int;
            using ValueType = T*;
            using IndicesArrayType = std::vector<IndexType>;
            using Iterator = DataGroupIterator<DataGroup<T>>;
            
        private:
            std::vector<IndexType> m_indices;
            
            IndexType m_size;
            
        public:
            DataGroup(TemplateObject** const dataArray) : BasicDataGroup(dataArray)
            {
                m_size = 0;
            }
            
            void addIndex(IndexType index) override
            {
                m_indices.push_back(index);
                m_size++;
            }
            
            IndexType getSize() override
            {
                return m_size;
            }
            
            T* operator[](const IndexType index)
            {
                return (T*)(getDataArray()[m_indices[index]]);
            }
            
            Iterator begin() 
            { 
                return Iterator(0, m_size, m_indices, getDataArray()); 
            }
            
            Iterator end()
            {
                return Iterator(m_size, m_size, m_indices, getDataArray());
            }
    };
}
