#include "blf/blffile.hpp"

#include "blf/version.hpp"

#include "blf/writer.hpp"
#include "blf/reader.hpp"

#include "blf/exceptions/outdatedreader.hpp"

#include <chrono>

#define BLF_DEBUG_TIME

namespace blf
{
    void writeFile(const BLFFile& file, BLF_FLAG flag)
    {
        writeFile(file.path, file.header, (const ObjectTable&)file.objects, file.common, file.data, flag);
    }

    void writeFile(const char* const path, const ObjectTable& objects, DataTable& data, BLF_FLAG flag)
    {
        InformationHeader header = {SIGNATURE, VERSION_MAJOR, VERSION_MINOR, VERSION_FIX};
        CommonTable common;

        data.computeCommonTable(common, objects);
        data.buildArray();
        common.buildCommonObjectArray();

        writeFile(path, header, objects, common, data, flag);
    }

    void writeFile(const char* const path, const InformationHeader& header, const ObjectTable& objects, const CommonTable& common, const DataTable& data, BLF_FLAG flag)
    {
        #ifdef BLF_DEBUG_TIME
            auto t1 = std::chrono::high_resolution_clock::now();
        #endif
        blf::Writer writer(path, flag);

        writer.writeInformationHeader(header);
        writer.writeObjectTable(objects);
        writer.writeCommonTable(common, objects);
        writer.writeDataTable(data, objects, common);
        #ifdef BLF_DEBUG_TIME
            auto t2 = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
            if( hasFlag(flag, FLAG_VERBOSE))
            {
                std::cout << "Write completed in: " << duration << " milliseconds." << std::endl;
            }
        #endif

        writer.flushToFile();
    }

    BLFFile readFile(const char* const path, const blf::ObjectTable& objects, BLF_FLAG flag)
    {
        BLFFile file;
        file.path    = path;
        file.objects = objects;

        readFile(file, flag);

        return file;
    }

    void readFile(blf::BLFFile& file, BLF_FLAG flag)
    {
        #ifdef BLF_DEBUG_TIME
            auto t3 = std::chrono::high_resolution_clock::now();
        #endif
        blf::Reader levelReader(file.path, flag);
        file.header = levelReader.readHeader();
        if( file.header.compressionFlags & 1 == 1 )
        {
            levelReader.decompress();
        }

        bool larger = (file.header.major > VERSION_MAJOR || (file.header.major >= VERSION_MAJOR && file.header.minor > VERSION_MINOR ));

        if( larger )
        {
            throw OutdatedReaderException(Version(VERSION_MAJOR, VERSION_MINOR, VERSION_FIX), Version(file.header.major, file.header.minor, file.header.fix));
        }
        levelReader.readObjectTable(file.objects);
        levelReader.readCommonTable(file.common, file.objects);
        file.common.buildCommonObjectArray();

        blf::DataTable data;
        #ifdef BLF_DEBUG_TIME
            auto t5 = std::chrono::high_resolution_clock::now();
        #endif
        levelReader.readDataTable(&file.data, file.objects, file.common);
        #ifdef BLF_DEBUG_TIME
            auto t6 = std::chrono::high_resolution_clock::now();
        #endif
        file.data.buildArray();

        #ifdef BLF_DEBUG_TIME
            auto t4 = std::chrono::high_resolution_clock::now();
            auto duration2 = std::chrono::duration_cast<std::chrono::milliseconds>(t4 - t3).count();
            auto duration3 = std::chrono::duration_cast<std::chrono::milliseconds>(t6 - t5).count();
            if( hasFlag(flag, FLAG_VERBOSE))
            {
                std::cout << "Read completed in: " << duration2 << " milliseconds." << std::endl;
                std::cout << "Data-Table read in: " << duration3 << " milliseconds." << std::endl;
            }
        #endif
    }

    void readFile(blf::BLFFile& file, const ObjectTable& objects, BLF_FLAG flag)
    {
        file.objects = objects;
        readFile(file, flag);
    }

    void readFile(const char* path, blf::BLFFile& file, const ObjectTable& objects, BLF_FLAG flag)
    {
        file.objects = objects;
        file.path    = path;
        readFile(file, flag);
    }
}