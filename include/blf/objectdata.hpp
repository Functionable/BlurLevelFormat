#pragma once

#include <cstddef>
#include <cstring>
#include <utility>

namespace blf
{
    class ObjectData
    {
        private:
            size_t m_size;
            char* m_data;

        public:
            ObjectData(char* data, size_t size)
                : m_size(size)
            {
                m_data = new char[size];
                std::memcpy(m_data, data, size);
            }

            ObjectData(const ObjectData& data)
                : m_size(data.m_size)
            {
                m_data = new char[data.m_size];
                std::memcpy(m_data, data.m_data, data.m_size);
            }

            ObjectData(ObjectData&& data)
                : m_size(std::move(data.m_size)),
                  m_data(std::move(data.m_data))
            {
                data.m_data = nullptr;
            }

            ~ObjectData()
            {
                delete[] m_data;
            }

            ObjectData& operator=(const ObjectData& other)
            {
                delete[] m_data;
                m_size = other.m_size;
                m_data = new char[m_size];
                std::memcpy(m_data, other.m_data, m_size);

                return *this;
            }

            ObjectData& operator=(ObjectData&& other)
            {
                std::swap(m_data, other.m_data);
                std::swap(m_size, other.m_size);

                return *this;
            }

            size_t getSize() const { return m_size; }
            char* getData() const { return m_data; }
    };

    class DataView
    {
        private:
            size_t m_size;
            char* m_data;

        public:
            DataView(size_t size, char* data)
                : m_size(size), m_data(data)
            {}

            DataView(const DataView& view)
                : m_size(view.m_size), m_data(view.m_data)
            {}

            DataView(DataView&& view)
                : m_size(std::move(view.m_size)),
                  m_data(std::move(view.m_data))
            {}  

            size_t getSize() const { return m_size; }
            char* getData() const { return m_data; }
    };
}