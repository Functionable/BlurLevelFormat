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
		uint8_t m_indexerSize = 1;
		size_t m_size;
		ObjectDefinition** m_definitions;
		bool m_empty = true;

		void copyFromTable(const ObjectTable& otherTable)
		{
			m_indexerSize	= otherTable.m_indexerSize;
			m_empty			= otherTable.m_empty;
			m_size			= otherTable.m_size;
			m_definitions	= new ObjectDefinition * [m_size];

			for (size_t i = 0; i < m_size; i++)
			{
				m_definitions[i] = new ObjectDefinition();
				(*m_definitions[i]) = (*otherTable.m_definitions[i]);
			}
		};

		public:

			ObjectTable() : m_size(0), m_definitions(nullptr) {}

			// COPY CONSTRUCTOR, COPY CONSTRUCTOR, NEEDED, IT IS.
			ObjectTable(const ObjectTable& table) : ObjectTable()
			{
				copyFromTable(table);
			}

			ObjectTable(std::initializer_list<ObjectDefinition> definitions) : ObjectTable()
			{
				setContent(definitions);
			}

			ObjectTable& operator =(const ObjectTable& otherTable)
			{
				copyFromTable(otherTable);
				return *this;
			}

			~ObjectTable()
			{
				if( !m_empty )
				{
					for (size_t i = 0; i < m_size; i++)
					{
						delete m_definitions[i];
					}
					delete[] m_definitions;
				}
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
					m_definitions[i] = new ObjectDefinition();
					for (ObjectAttribute attribute : definition.attributes)
					{
						ObjectAttribute nAttr = attribute;
						nAttr.attribType = attribute.attribType;
						m_definitions[i]->attributes.push_back(nAttr);
					}
					m_definitions[i]->identifier = definition.identifier;
					m_definitions[i]->templatePointer = definition.templatePointer;
					m_definitions[i]->creator = definition.creator;
					m_definitions[i]->activeAttributeCount = definition.activeAttributeCount;
					//m_definitions[i]->activeAttributeIndexes = definition.activeAttributeIndexes;
					m_definitions[i]->isForeign = definition.isForeign;
					m_definitions[i]->arrayIndex = i;
					i++;
				}

				computeIndexerSize();
			};

			ObjectDefinition** begin() const { return &m_definitions[0]; }
			ObjectDefinition** end() const { return &m_definitions[m_size]; }

			/*
			 *	Should be called after setContent was.
			 */
			void insertDefinition(ObjectDefinition definition)
			{
				size_t oldSize = m_size;

				m_empty = false;
				m_size++;

				ObjectDefinition** oldDefinitions = m_definitions;
				m_definitions = new ObjectDefinition*[m_size];

				// Copying the array over.
				if( m_size > 1 )
				{
					for (size_t i = 0; i < m_size - 1; i++)
					{
						(m_definitions[i]) = (oldDefinitions[i]);
					}
				}

				ObjectDefinition* defPtr = new ObjectDefinition();
				for (ObjectAttribute attribute : definition.attributes)
				{
					defPtr->attributes.push_back(attribute);
					//std::cout << attribute.activeIndex << std::endl;
					defPtr->attributes[defPtr->attributes.size() - 1].activeIndex = attribute.activeIndex;
				}
				defPtr->identifier = definition.identifier;
				defPtr->templatePointer = definition.templatePointer;
				defPtr->creator = definition.creator;
				defPtr->isForeign = definition.isForeign;
				defPtr->activeAttributeCount = definition.activeAttributeCount;
				defPtr->arrayIndex = oldSize;
				defPtr->foreignIndex = definition.foreignIndex;

				if( oldDefinitions != nullptr )
				{
					delete[] oldDefinitions;
				}
				
				m_definitions[oldSize] = defPtr;
				computeIndexerSize();
			}

			void computeIndexerSize()
			{
				uint64_t size = getSize();
				if (size < UINT8_MAX)
				{
					m_indexerSize = 1;
					return;
				}
				else if (size < UINT16_MAX)
				{
					m_indexerSize = 2;
					return;
				}
				else if (size < UINT32_MAX)
				{
					m_indexerSize = 4;
					return;
				}		
				else if (size < UINT64_MAX)
				{
					m_indexerSize = 8;
					return;
				}
			}

			ObjectDefinition* getDefinitionFromIndex(int index) const
			{
				return m_definitions[index];
			}

			ObjectDefinition* getDefinitionFromForeignIndex(int foreignIndex) const
			{
				for (size_t i = 0; i < m_size; i++)
				{
					ObjectDefinition* definition = m_definitions[i];
					//std::cout << "F" << foreignIndex << "/" << definition->foreignIndex << std::endl;
					if (foreignIndex == definition->foreignIndex)
					{
						return definition;
					}
				}

				//std::cout << "Warning! Definition " << foreignIndex << " could not be found!" << std::endl;
				return nullptr;
			}

			ObjectDefinition* getDefinitionFromIdentifier(blf::String identifier) const
			{
				for (size_t i = 0; i < m_size; i++)
				{
					ObjectDefinition* definition = m_definitions[i];
					//std::cout << i << identifier << "/" << definition->identifier << std::endl;
					if (identifier == definition->identifier)
					{
						return definition;
					}
				}

				//std::cout << "Warning! Definition '" << identifier << "' could not be found!" << std::endl;
				return nullptr;
			}
	};
}
