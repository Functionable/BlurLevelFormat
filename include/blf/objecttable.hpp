#pragma once

#include <cstdint>
#include <vector>
#include <iostream>
#include <initializer_list>

#include "string.hpp"		
#include "objectdefinition.hpp"

namespace blf
{
	class ObjectTable
	{
		uint8_t m_indexerSize;
		size_t m_size;
		ObjectDefinition** m_definitions;
		bool m_empty = true;

		public:

			ObjectTable() {}

			ObjectTable(std::initializer_list<ObjectDefinition> definitions)
			{
				setContent(definitions);
			}

			~ObjectTable()
			{
				for (int i = 0; i < m_size; i++)
				{
				//	delete (m_definitions[i]);
				}
				delete[] m_definitions;
			}

			size_t getSize() const { return m_size; }
			uint8_t getIndexerSize() const { return m_indexerSize; }

			// Don't touch these unless you know what you're doing
			void setIndexerSize(uint8_t indexerSize) { m_indexerSize = indexerSize; }
			void setContent(std::initializer_list<ObjectDefinition> definitions)
			{
				m_empty = false;
				m_size = definitions.size();
				m_definitions = new ObjectDefinition * [m_size];

				int i = 0;
				for (ObjectDefinition definition : definitions)
				{
					m_definitions[i] = new ObjectDefinition;
					for (ObjectAttribute attribute : definition.attributes)
					{
						ObjectAttribute nAttr = attribute;
						nAttr.attribType = attribute.attribType;
						m_definitions[i]->attributes.push_back(nAttr);
					}
					m_definitions[i]->identifier = definition.identifier;
					m_definitions[i]->templatePointer = definition.templatePointer;
					m_definitions[i]->creator = definition.creator;
					m_definitions[i]->activeAttributeIndexes = definition.activeAttributeIndexes;
					m_definitions[i]->isForeign = definition.isForeign;
					i++;
				}
			};

			/*
			 *	Should be called after setContent was.
			 */
			void insertDefinition(ObjectDefinition definition)
			{
				int oldSize = m_size;

				m_empty = true;
				m_size += 1;

				ObjectDefinition** newDefinitions = new ObjectDefinition*[oldSize + 1];

				// Copying the array over.
				for (int i = 0; i < oldSize; i++)
				{
					(newDefinitions[i]) = (m_definitions[i]);
					m_definitions[i] = nullptr;
				}

				ObjectDefinition* defPtr = new ObjectDefinition;
				//defPtr->attributes = definition.attributes;
				for (ObjectAttribute attribute : definition.attributes)
				{
					defPtr->attributes.push_back(attribute);
				}
				defPtr->identifier = definition.identifier;
				defPtr->templatePointer = definition.templatePointer;
				defPtr->creator = definition.creator;
				defPtr->activeAttributeIndexes = definition.activeAttributeIndexes;
				defPtr->isForeign = definition.isForeign;

				delete[] m_definitions;

				m_definitions = newDefinitions;

				m_definitions[m_size - 1] = defPtr;
			}

			ObjectDefinition* getDefinitionFromIndex(int index) const
			{
				return m_definitions[index];
			}

			ObjectDefinition* getDefinitionFromIdentifier(blf::String identifier) const
			{
				ObjectDefinition* definition = m_definitions[0];
				for (int i = 0; i < m_size; i++, definition = m_definitions[i])
				{
					if (identifier == definition->identifier)
					{
						return definition;
					}
				}

				std::cout << "Warning! Definition '" << identifier << "' could not be found!" << std::endl;
				return nullptr;
			}
	};
}