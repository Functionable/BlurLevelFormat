#pragma once

#include <cstdint>
#include <vector>

#include "objectattribute.hpp"

namespace blf
{
	class ForeignAttributeTable
	{
		public:
			std::vector<blf::ObjectAttribute> attributes;
	};
}
