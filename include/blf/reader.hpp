#pragma once

#include "string.hpp"
#include "datatable.hpp"
#include "objecttable.hpp"
#include "dynamicreadable.hpp"
#include "templateobject.hpp"
#include "commontable.hpp"

#include <fstream>
#include <type_traits>

namespace blf
{
	class Reader
	{
		std::ifstream* readStream;
		bool m_strictTypeChecking = false; // By default type differences end up in warnings.

		public:
			Reader(blf::String filePath, BLF_FLAG readerFlags = FLAG_NOTHING);
			~Reader();

			/**
			 *	Used to read more basic POD types. It reads the sizeof() bytes of the object. 
			 */
			template<typename T>
			T read()
			{
				static_assert(std::is_pod<T>::value, "Read only supports POD types.");
				T newInstance;
				readStream->read(reinterpret_cast<char*>(&newInstance), sizeof(T));

				return newInstance;
			}

			/**
			 *	Used to read more complex types that don't always have the
			 *  same size, such as a const char* or blf::String.
			 *  In order to do this, this function uses blf::DynamicReadable
			 */
			template<typename T>
			T dynamicRead()
			{
				DynamicReadable<T> readable;
				readable.read(readStream);
				return T(readable.getValue());
			}

			uint64_t readIndexer(uint8_t size)
			{
                
                std::cout << "gonna read" << std::endl;
				switch (size)
				{
                    
					case 1:
                        std::cout << "read one so far" << std::endl;
						return read<uint8_t>();

					case 2:
                        std::cout << "read two so far" << std::endl;
						return read<uint16_t>();

					case 4:
                        std::cout << "read four so far" << std::endl;
						return read<uint32_t>();

					case 8:
                        std::cout << "read eight so far" << std::endl;
						return read<uint64_t>();
				}
			}
			
			void readDataTable(DataTable* dataTable, ObjectTable* objectTable, CommonTable* commonTable)
            {      
                uint8_t byte = read<uint8_t>();
                while( byte == 0xAA )
                {
                    std::cout << "BB" << (int)byte << std::endl;
                    dataTable->addObject(readObject(objectTable, commonTable->getIndexerSize()));
                    byte = read<uint8_t>();
                    std::cout << "AA" << (int)byte << std::endl;
                }
            }

			ObjectTable* readObjectTable(ObjectTable* definedTable)
			{
				std::vector<ObjectDefinition> definitions;
				definedTable->setIndexerSize(read<uint8_t>());
				size_t size = read<int64_t>();
				for (int i = 0; i < size; i++)
				{
					blf::ObjectDefinition definition; 
					definition.identifier = dynamicRead<String>();
					uint8_t attributeCount = read<uint8_t>();
					for (int j = 0; j < attributeCount; j++)
					{
						ObjectAttribute attribute;
						attribute.name = dynamicRead<String>();
						attribute.attribType = (BLF_TYPE)read<uint8_t>();
						definition.attributes.push_back(attribute);
					}
					definitions.push_back(definition);
				}

				// Looping to check integrity of both tables
				for (int i = 0; i < definitions.size(); i++)
				{
					ObjectDefinition readDefinition = definitions[i];
					ObjectDefinition* nativeDefinition = definedTable->getDefinitionFromIdentifier(readDefinition.identifier);
					
					if (nativeDefinition == nullptr)
					{
						std::cout << "Definition was nullptr, wasn't found, marking as foreign." << std::endl;
						readDefinition.isForeign = true;
						for (ObjectAttribute attribute : readDefinition.attributes)
						{
							attribute.isActive = true;
							attribute.isForeign = true;
						}
						definedTable->insertDefinition(readDefinition);
						continue;
					}

					this->processDefinitionAttributes(nativeDefinition, &definitions[i]);
				}

				return definedTable;
			}

			void processDefinitionAttributes(ObjectDefinition* nativeDefinition, ObjectDefinition* readDefinition)
			{
				for (int i = 0; i < nativeDefinition->attributes.size(); i++)
				{
					ObjectAttribute nativeAttribute = nativeDefinition->attributes[i];
					bool foundMatchingAttribute = false;
					ObjectAttribute readAttribute;
					int j = 0;
					for (ObjectAttribute fileAttribute : readDefinition->attributes)
					{
						if (fileAttribute.name == nativeAttribute.name)
						{
							foundMatchingAttribute = true;
							readAttribute = fileAttribute;
							readDefinition->attributes[j].isActive = true;
						}
						readDefinition->attributes[j].isForeign = !readDefinition->attributes[j].isActive;
						j++;
					}

					// If matching definition couldn't be found.
					if (foundMatchingAttribute == false)
					{
						// Warning the user.
						std::cout << "Warning: Expected attribute for name '"
							<< nativeAttribute.name <<
							"' not found in file! Attribute won't be activated." << std::endl;
						continue;
					}

					// Activating attribute as both the file and the program have defined it.
					nativeAttribute.isActive = true;

					if (readAttribute.attribType != nativeAttribute.attribType)
					{
						std::cout << "Warning: Differing types for attribute name '"
							<< nativeAttribute.name << "', might be fatal. Carrying on."
							<< std::endl;
					}

					nativeDefinition->attributes[i] = nativeAttribute;
				}

				for (ObjectAttribute attrib : readDefinition->attributes)
				{
					if (attrib.isActive == false)
					{
						std::cout << "Assuming attribute '" << attrib.name << "' is foreign!" << std::endl;
                        attrib.isForeign = true;
                        nativeDefinition->attributes.push_back(attrib);
                    }
				}
			}

			TemplateObject* readObject(ObjectTable* objectTable, uint8_t commonTableIndexerSize)
			{
				// Reading the name of the object (used as a reference to the object table)
				const char* objectName = dynamicRead<const char*>();

				ObjectDefinition* objectDefinition = objectTable->getDefinitionFromIdentifier(objectName);
				
				if (objectDefinition == nullptr)
				{
					//	return nullptr;
					std::cout << "Fatal! Definition for '" << objectName << "'" << std::endl;
					return nullptr;
				}

				bool foreign = objectDefinition->isForeign;

				ForeignAttributeTable foreignAttributes;

				TemplateObject* obj = objectDefinition->creator->createNew();
				
				std::cout << "Reading object of type '" << objectName << "'" << std::endl;

				for (int i = 0; i < objectDefinition->attributes.size(); i++)
				{
                    blf::String attributeName = dynamicRead<blf::String>();
					ObjectAttribute* objectAttribute = getObjectAttributeByIdentifier(objectDefinition, attributeName);
                    int8_t size = sizeOfType(objectAttribute->attribType);
                    std::cout << attributeName << "!IDK" << objectAttribute->name << std::endl;
                    std::cout << "is foreign? " << objectAttribute->isForeign << std::endl;
                    if( objectAttribute->isForeign ) 
                    { 
                        std::cout << attributeName << "IDK" << std::endl;
                        if (size != -1)
                        {
                            char* test = new char[size];
                            readStream->read(test, size);
                            delete[] test;
                        }
                        else
                        {
                            if (objectAttribute->attribType == TYPE_STRING)
                            {
                                String str = dynamicRead<String>();
                            }							
                            else if(objectAttribute->attribType == TYPE_OBJECTREFERENCE)
                            {
                                int commonTableReference = readIndexer(commonTableIndexerSize);
                                std::cout << "thing " << commonTableReference << std::endl;
                            }
                        }
                        continue; 
                    }
					void* offset = getOffsetFromPointers(objectDefinition->templatePointer, objectAttribute->offset);
					void* location = getPointerFromOffset(obj, offset);
					if (size != -1)
					{
						readStream->read((char*)location, size);
					}
					else
					{
						if (objectAttribute->attribType == TYPE_STRING)
						{
							String str = dynamicRead<String>();
							(*(blf::String*)location) = str;
						}							
						else if(objectAttribute->attribType == TYPE_OBJECTREFERENCE)
						{
							int commonTableReference = readIndexer(commonTableIndexerSize);
                            std::cout << "thing " << commonTableReference << std::endl;
						}
					}
				}
				return obj;
			}

			CommonTable readCommonTable(ObjectTable* objectTable)
			{
				CommonTable table;

				uint8_t indexerSize = read<uint8_t>();

				uint64_t tableSize = read<uint64_t>();

				std::cout << "Reading Common Table of Size: " << tableSize << std::endl;

				for (int i = 0; i < tableSize; i++)
				{
					blf::TemplateObject* object = readObject(objectTable, indexerSize);
					table.addCommonObject(object);
				}
				return table;
			}

			std::ifstream& getStream() { return *readStream; }
	};
}
