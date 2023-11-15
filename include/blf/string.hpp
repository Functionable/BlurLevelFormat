#pragma once

#include <cstddef>
#include <cstdint>
#include <utility>
#include <iostream>

namespace blf
{
    class String
    {
        private: 
            size_t m_length;
            char* m_buffer;

        public:
            String(const char* str);
            String(char* str) 
                : String((const char*)str)
            {}

            String(const String& string)
                : String(string.m_buffer)
            {}

            String(const std::string& string)
                : String(string.c_str())
            {}

            String(String&& string)
                : m_length(std::move(string.m_length)),
                  m_buffer(std::move(string.m_buffer))
            {
                string.m_buffer = nullptr;
            }

            String& operator=(const String& other);
            String& operator=(String&& other);

            bool operator==(const String& other) const;
            bool operator==(const char* other) const;
            bool operator==(const std::string& other) const;

            operator std::string() const { return std::string(m_buffer, m_length); }

            friend std::ostream& operator<<(std::ostream& os, const String& str)
            {
                return os << str.m_buffer;
            } 

            virtual ~String();

            size_t getLength() const { return m_length; }
            const char* getBuffer() const { return m_buffer; }
    };
}