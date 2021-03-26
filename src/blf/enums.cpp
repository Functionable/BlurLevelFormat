#include "blf/enums.hpp"

namespace blf
{
	int8_t sizeOfType(BLF_TYPE type)
	{
		switch (type)
		{
			case TYPE_NULL:
				return 0;

			case TYPE_INT:
				return 4;

			case TYPE_FLOAT:
				return 4;

			case TYPE_BOOL:
				return 1;

			case TYPE_DOUBLE:
				return 8;

			case TYPE_STRING:
				return SIZE_DYNAMIC;

			case TYPE_OBJECTREFERENCE:
				// The size of this is decided by the size of the common table indexer.
				return SIZE_DYNAMIC;

			case TYPE_LONG:
				return 8;
		}
	}
}