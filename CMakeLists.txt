cmake_minimum_required(VERSION 3.8)

project(blf)

set(BLF_SRC ./src/blf)

add_library(blf STATIC 
$(BLF_SRC)/blffile.cpp $(BLF_SRC)/commontable.cpp
$(BLF_SRC)/datatable.cpp $(BLF_SRC)/enums.cpp
$(BLF_SRC)/objectattribute.cpp $(BLF_SRC)/objectdefinition.cpp
$(BLF_SRC)/reader.cpp $(BLF_SRC)/string.cpp 
$(BLF_SRC)/templatearrayconstiterator.cpp
$(BLF_SRC)/writer.cpp) 

target_include_directories(blf PUBLIC include)