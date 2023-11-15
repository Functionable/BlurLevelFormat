cmake_minimum_required(VERSION 3.8)

set(BLF_SRC ./src/blf)
project(blf)

add_library(blf STATIC 
    ${BLF_SRC}/enums.cpp ${BLF_SRC}/string.cpp
)

set_property(TARGET blf PROPERTY CXX_STANDARD 17)
find_package(ZLIB)

target_include_directories(blf PUBLIC include)
target_link_libraries(blf PUBLIC ZLIB::ZLIB)