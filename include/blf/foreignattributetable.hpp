#pragma once

#include <cstdint>
#include <vector>

#include "objectattribute.hpp"

namespace blf
{
	class ForeignAttributeTable
	{
		std::vector<ObjectAttribute> attributes = std::vector<ObjectAttribute>();

		int reserved = 0;
		public:
			ForeignAttributeTable(unsigned int n = 0)
			{
				reserved = n;
				attributes.reserve(n);
			}

			unsigned int getReserved() const { return reserved; }

			size_t getSize() const { return attributes.size(); }
			void addAttribute(ObjectAttribute attribute);

			const ObjectAttribute* begin() const
			{
				return ((attributes.size() > 0) ? &(attributes[0]) : nullptr);
			}

			const ObjectAttribute* end() const
			{
				return ((attributes.size() > 0) ? &(attributes[attributes.size()-1]) : nullptr);
			}
	};
}
