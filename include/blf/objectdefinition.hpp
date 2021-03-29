#pragma once

#include "string.hpp"
#include "objectattribute.hpp"
#include "templatecreator.hpp"

#include <vector>

namespace blf
{
	struct ObjectDefinition
	{
		// This is what you use to detect object types in C++ when reading.
		blf::String identifier;
		void* templatePointer;

		/**
		 *	If an object is foreign, all it's attributes are also foreign.
		 */
		bool isForeign = false;

		// Takes care of instantiating new instances of the object.
		blf::TemplateCreator* creator;

		/**
		 * A list of blf::ObjectAttribute objects that the object wants to be available.
		 */
		std::vector<ObjectAttribute> attributes;

		/**
		 * Never use this variable yourself, it only should ever be handled
		 * by the blf::Reader. Shows which attributes are active, or in other
		 * words, which ones are defined and supported by the file.
		 */
		std::vector<int> activeAttributeIndexes;  
        
        uint32_t activeAttributeCount = 0;
	};
    
    ObjectAttribute* getObjectAttributeByIdentifier(ObjectDefinition* definition, String identifier);
}
