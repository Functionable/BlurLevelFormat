#pragma once

#include <exception>
#include <cstdio>
#include <string>

namespace blf
{
    class OutdatedReaderException : public std::exception
    {
        int32_t m_readerMajor,  m_readerMinor,  m_readerFix;
        int32_t m_fileMajor,    m_fileMinor,    m_fileFix;

        std::string exceptionString;

        public:
            OutdatedReaderException(
                int32_t readerMajor, int32_t readerMinor, int32_t readerFix,
                int32_t fileMajor,   int32_t fileMinor,   int32_t fileFix) 
            :   m_readerMajor(readerMajor), m_readerMinor(readerMinor), m_readerFix(readerFix),
                m_fileMajor(fileMajor),     m_fileMinor(fileMinor),     m_fileFix(fileFix) 
                
                {
                    std::string readerVersion = std::to_string(m_readerMajor) + "." + std::to_string(m_readerMinor) + "." + std::to_string(m_readerFix);

                    std::string fileVersion = std::to_string(m_fileMajor) + "." + std::to_string(m_fileMinor) + "." + std::to_string(m_fileFix);

                    exceptionString = "File version " + fileVersion + " is above the expected version of " + readerVersion + " by the reader";
                }; 


            const char* what() const throw()
            {
                return exceptionString.c_str();
            }
    };
}