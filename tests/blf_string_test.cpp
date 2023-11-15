#include <iostream>
#include <string>

#include "blf/string.hpp"

int main(int argc, char** argv)
{
    bool successful = true;

    std::string s1 = "Hello World";
    const char* s2 = "Goodbye World";

    blf::String blString1 = s1;
    blf::String blString2 = s2;
    blf::String blString3 = s2;
    successful &= blString1 == s1;
    successful &= blString2 == s2;
    successful &= blString2 == blString3;

    blString3 = s1;

    successful &= blString3 == blString1;

    return successful == 0;
}