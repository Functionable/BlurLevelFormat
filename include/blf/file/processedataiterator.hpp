#pragma once

namespace blf
{
    /**
     * This iterator class is responsible for iterating over a 
     * select type of object in the data retrieved from a parsed BLF file.
     *
     * This iterator's function is to do the conversion necessary to convert the
     * ObjectData read by the Reader into full objects, all of select class T.
     *
     * Given an array of ObjectData references (that have been grouped by type) and the LocalObjectTable,
     * this class will be able to offer an iterator which 
     */
    template<typename T>
    class ProcessedDataIterator
    {

    };
}