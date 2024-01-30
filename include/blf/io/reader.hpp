#pragma once

#include "../types.hpp"
#include "../objectdata.hpp"
#include "../localobjecttable.hpp"

#include <map>
#include <vector>

namespace blf
{
    /**
     * This class contains vital logic needed to parse a BLF file.
     * It also stores read objects and manages their lifetimes.
     *
     * You must have initialized your LocalObjectTable beforehand
     * and passed it to Reader's constructor if you wish for it to be used.
     */
    class Reader
    {
        private:
            std::map<ObjectID, ObjectData> m_readObjects;

        public:
            // TODO: copy table?
            Reader(const LocalObjectTable& table)
            {}

            /**
             * Opens a file, retrieves its buffer and calls readFromMemory.
             */
            void readFile(const char* path);

            /**
             * Reads a file from the memory buffer given.
             * Reads all object data into an internal array of ObjectData instances.
             */
            void readFromMemory(const char* buffer);

            /**
             * Returns the first element of a certain object type.
             *
             * Will throw if type cannot be found in the LocalObjectTable.
             * Will throw if the LocalObjectTable cannot be found.
             * Will throw if no file has been read yet and thus there is no objects.
             */
            template<typename T>
            T begin()
            {

            }

    };
}