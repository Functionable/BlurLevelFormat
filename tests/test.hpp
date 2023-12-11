#pragma once

#include "test_memory.hpp"

#include <chrono>
#include <string>
#include <iostream>
#include <exception>

// Helper library for tests.

bool test();
inline int successStatus(bool isSuccessful) { return !isSuccessful; }
inline const char* successString(bool isSuccessful) { return isSuccessful ? "PASS" : "FAIL"; }

int main()
{
    bool status = successStatus(test());

    //std::cout << "HEAP SIZE: " << std::to_string(heapSize()) << " byes";

    if( heapSize() != 0 )
    {
        std::cout << "FAILURE: Potential memory leak, heap is " << heapSize() <<
            " bytes bigger" << std::endl;

        return 1;
    }

    return status;
}

class TinyProfiler
{
    std::chrono::high_resolution_clock::time_point m_start;
    std::string m_name;

    public:
        TinyProfiler(const char* scopeName)
            : m_start(std::chrono::high_resolution_clock::now()),
              m_name(scopeName)
        {}

        ~TinyProfiler()
        {
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - m_start);

            std::cout << "Scope '" << m_name << "' took " << duration.count() << "\xC2\xB5s." << std::endl;
        }
};

#define PROFILE_SCOPE(name) auto ___scopeProfiler = TinyProfiler(name)