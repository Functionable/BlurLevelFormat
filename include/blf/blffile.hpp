#pragma once

#include "informationheader.hpp"
#include "objecttable.hpp"
#include "commontable.hpp"
#include "datatable.hpp"

namespace blf
{
    struct BLFFile
    {
        // Information about the files.
        const char* path;
        
        // These contain the data in the file.
        InformationHeader  header;
        ObjectTable        objects;
        CommonTable        common;
        DataTable          data;
    };

    void    writeFile(const BLFFile& file, BLF_FLAG flag = DEFAULT_FLAG);
    void    writeFile(const char* const path, const ObjectTable& objects, DataTable& data, BLF_FLAG flag = DEFAULT_FLAG);
    void    writeFile(const char* const path, const InformationHeader& header, const ObjectTable& objects, const CommonTable& common, const DataTable& data, BLF_FLAG flag = DEFAULT_FLAG);
    
    BLFFile readFile(const char* const path, const blf::ObjectTable& objects, BLF_FLAG flag = DEFAULT_FLAG);

    void    readFile(const char* path, blf::BLFFile& file, const ObjectTable& objects, BLF_FLAG flag = DEFAULT_FLAG);
    /**
     *  Assumes all required objects are found within the BLFFile object.
     */
    void    readFile(blf::BLFFile& file, BLF_FLAG flag = DEFAULT_FLAG);
    void    readFile(blf::BLFFile& file, const ObjectTable& objects, BLF_FLAG flag = DEFAULT_FLAG);
}