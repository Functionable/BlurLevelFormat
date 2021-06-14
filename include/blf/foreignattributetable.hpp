#pragma once

#include <cstdint>
#include <vector>

#include "objectattribute.hpp"

namespace blf
{
	class ForeignAttributeTable
	{
		std::vector<ObjectAttribute> attributes;
		public:
			size_t getSize() const { return attributes.size(); }
			void addAttribute(ObjectAttribute attribute);

			const ObjectAttribute* begin() const
			{
				return ((attributes.size() > 0) ? &(attributes[0]) : nullptr);
			}

			const ObjectAttribute* end() const
			{
				return ((attributes.size() > 0) ? &(attributes[attributes.size()]) : nullptr);
			}
	};
}
