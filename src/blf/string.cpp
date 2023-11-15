#include "blf/string.hpp"

#include <cstdlib>
#include <cstring>

namespace blf
{
    String::String(const char* str)
    {
        m_length = std::strlen(str);
        m_buffer = new char[m_length + 1];
        std::memcpy(m_buffer, str, m_length);
        m_buffer[m_length] = 0;
        //m_buffer = strdup(str);
    }

    String& String::operator=(const String& other)
    {
        if( this == &other ) { return *this; }

        m_length = other.m_length;
        char* buffer = new char[m_length + 1];
        std::memcpy(buffer, other.m_buffer, m_length);
        buffer[m_length] = 0;

        delete[] m_buffer;

        m_buffer = buffer;

        return *this;
    }

    String& String::operator=(String&& other)
    {
        std::swap(m_buffer, other.m_buffer);
        std::swap(m_length, other.m_length);

        return *this;
    }

    bool String::operator==(const String& other) const
    {
        return strcmp(m_buffer, other.m_buffer) == 0;
    }

    bool String::operator==(const char* other) const
    {
        return strcmp(m_buffer, other) == 0;
    }

    bool String::operator==(const std::string& other) const
    {
        return (std::string)*this == other;
    }

    String::~String()
    {
        //free(m_buffer);
        delete[] m_buffer;
    }
}