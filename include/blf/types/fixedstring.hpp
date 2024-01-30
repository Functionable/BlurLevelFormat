#pragma once

#include "../string.hpp"

#include <cstring>


namespace blf
{
    /*
     * Class representing a string of fixed length.
     */
    class FixedString
    {
        private:
            const char* m_string;
            int m_length;

            const char* copyString(const char* string, int len)
            {
                char* newString = new char[len+1];
                std::memcpy(newString, string, len);
                newString[len] = 0;
                return newString;
            }   

        public:
            FixedString(const char* string, int len)
                : m_string(copyString(string, len)), m_length(len)
            {}

            FixedString(const char* string)
                : FixedString(string, strlen(string))
            {}

            FixedString(const std::string& string)
                : FixedString(string.c_str(), string.size())
            {}
    }
}