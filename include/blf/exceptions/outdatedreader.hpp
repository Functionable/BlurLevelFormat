#pragma once

#include "../version.hpp"

#include <exception>
#include <cstdio>
#include <string>

namespace blf
{
    class OutdatedReaderException : public std::exception
    {
        Version m_readerVersion;
        Version m_fileVersion;

        std::string exceptionString;

        public:
            OutdatedReaderException(const Version& readerVersion, const Version& fileVersion) 
            : m_readerVersion(readerVersion), m_fileVersion(fileVersion)
                {
                    std::string readerVersionString = std::to_string(readerVersion.major) + "." + std::to_string(readerVersion.minor) + "." + std::to_string(readerVersion.fix);

                    std::string fileVersionString = std::to_string(fileVersion.major) + "." + std::to_string(fileVersion.minor) + "." + std::to_string(fileVersion.fix);

                    exceptionString = "File version " + fileVersionString + " is newer than the expected version " + readerVersionString + " by the reader";
                }; 


            const char* what() const throw()
            {
                return exceptionString.c_str();
            }
    };
}