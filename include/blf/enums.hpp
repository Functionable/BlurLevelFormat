#pragma once

#include <cstdint>

namespace blf
{
	enum BLF_TYPE
	{
		TYPE_NULL 				= 0, /** Represents a nullptr */
		TYPE_INT 				= 1, /** Represents an int32_t */
		TYPE_FLOAT 				= 2,
		TYPE_BOOL 				= 4,
		TYPE_DOUBLE 			= 8,
		TYPE_STRING 			= 16,
		TYPE_OBJECTREFERENCE 	= 32, /** The blf version of pointers. */
		TYPE_LONG 				= 64, /**Represents an int64_t*/
		TYPE_COMPLEX			= 128, /** Represents small structs */
	};

    enum BLF_TYPECATEGORY
    {
        CATEGORY_NONE,
        CATEGORY_OBJECTREFERENCE,
        CATEGORY_NUMERICFLOAT,
        CATEGORY_NUMERICINTEGER,
        CATEGORY_STRING,
    };
    
	enum BLF_FLAG
	{
		FLAG_EMPTY		    = 0, /** No flag */
		FLAG_VERBOSE 		= 1, /** Should it print output logs */
		FLAG_TYPESTRICT 	= 2, /** Aborts loading as soon as the file has conflicting object definition types. */
		FLAG_COMPRESSED		= 4, /** GZIP Compressed */
	};

	constexpr bool hasFlag(BLF_FLAG set, BLF_FLAG needle)
	{
		return (set & needle) == needle;
	}

	constexpr BLF_FLAG operator &(BLF_FLAG one, BLF_FLAG two) { return (BLF_FLAG)((unsigned int)one & (unsigned int)two); }
	constexpr BLF_FLAG operator |(BLF_FLAG one, BLF_FLAG two) { return (BLF_FLAG)((unsigned int)one | (unsigned int)two); }

	const BLF_FLAG DEFAULT_FLAG = FLAG_COMPRESSED;

	const int SIZE_DYNAMIC = -1;

	int8_t 				getTypeSize(BLF_TYPE type);
	uint8_t				getAbsoluteTypeSize(BLF_TYPE type);
	
    BLF_TYPECATEGORY 	getTypeCategory(BLF_TYPE type);
}
