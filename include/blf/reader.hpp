#pragma once

#include "string.hpp"
#include "version.hpp"
#include "datatable.hpp"
#include "commontable.hpp"
#include "objecttable.hpp"
#include "foreignobject.hpp"
#include "templateobject.hpp"
#include "informationheader.hpp"

#include <fstream>
#include <sstream>
#include <type_traits>

namespace blf
{
	class Reader
	{
		std::stringstream m_readStream;

		bool m_strictTypeChecking = false; // By default type differences end up in warnings.
		bool m_verbose            = false;

		// mainly just used for backwards compatibility
		Version m_loadedVersion = Version(0, 0, 0);

		public:
			Reader(String filePath, BLF_FLAG readerFlags = FLAG_EMPTY);
			~Reader();

			/**
			 *	Used to read more basic POD types. It reads the sizeof() bytes of the object. 
			 */
			template<typename T>
			T read()
			{
				static_assert(std::is_pod<T>::value, "Read only supports POD types.");
				T newInstance;
				m_readStream.read(reinterpret_cast<char*>(&newInstance), sizeof(T));

				return newInstance;
			}

			String readString()
			{
				StringLength size = read<StringLength>();

				char* id = new char[size + 1];

				m_readStream.read(&id[0], size);
				id[size] = (char)'\0';

				return id;
			}

			void decompress();

			InformationHeader readHeader()
			{
				InformationHeader header;

				char signature[5];
				m_readStream.read(&signature[0], 4);
				signature[4] = '\0';

				header.signature = blf::String((const char*)signature);
				header.major = read<int32_t>();
				header.minor = read<int32_t>();
				header.fix   = read<int32_t>();

				m_loadedVersion.major = header.major;
				m_loadedVersion.minor = header.minor;
				m_loadedVersion.fix   = header.fix;

				if( m_loadedVersion != Version(1, 0, 0))
				{
					header.compressionFlags = read<uint8_t>();
					header.headerFlagCount  = read<uint16_t>();
				}
				else
				{
					header.compressionFlags = 0;
					header.headerFlagCount = 0;
				}

				return header;
			}

			uint64_t readIndexer(uint8_t size)
			{
				switch (size)
				{
					case 1:
						return read<uint8_t>();

					case 2:
						return read<uint16_t>();

					case 4:
						return read<uint32_t>();

					case 8:
						return read<uint64_t>();
				}

				return 0;
			}
			
			void readDataTable(DataTable* dataTable, const ObjectTable& objectTable, const CommonTable& commonTable)
            {      
                uint8_t byte = read<uint8_t>();
                while( byte == 0xAA )
                {
					//std::cout << "==== READ ======" << std::endl;
					blf::TemplateObject* object = readObject(objectTable, commonTable.getIndexerSize());
					if( object == nullptr )
					{
						std::cout << "Error: Abandoned reading due to a read error" << std::endl;
						break;
					}
                    dataTable->addObject(object);
                    byte = read<uint8_t>();
                }
            }

			int readObjectTable(ObjectTable& definedTable)
			{
				std::vector<ObjectDefinition> definitions;
				definedTable.setIndexerSize(read<uint8_t>());
				size_t size = read<int64_t>();
				if( size <= 0)
				{
					return -1;
				}
				for (int i = 0; i < size; i++)
				{
					ObjectDefinition definition; 
					definition.identifier = readString();
					definition.foreignIndex = i+1;
					//std::cout << definition.identifier << ", " << i+1 << std::endl;
					uint8_t attributeCount = read<uint8_t>();
					if( attributeCount == 0 )
					{
						return -1;
					}
					for (int j = 0; j < attributeCount; j++)
					{
						ObjectAttribute attribute;
						attribute.name = readString();
						attribute.attribType = (BLF_TYPE)read<uint8_t>();
						attribute.activeIndex = j+1;
						definition.attributes.push_back(attribute);
					}
					definitions.push_back(definition);
				}

				// Looping to check integrity of both tables
				for (int i = 0; i < definitions.size(); i++)
				{
					ObjectDefinition readDefinition = definitions[i];
					ObjectDefinition* nativeDefinition = definedTable.getDefinitionFromIdentifier(readDefinition.identifier);
					
					if (nativeDefinition == nullptr)
					{
						if( m_verbose )
						{
							std::cout << "Definition was nullptr, wasn't found, marking as foreign." << std::endl;
						}
						readDefinition.isForeign = true;
						for (ObjectAttribute attribute : readDefinition.attributes)
						{
							attribute.isActive = true;
							attribute.isForeign = true;
							readDefinition.activeAttributeCount++;
						}
						//std::cout << readDefinition.foreignIndex << std::endl;
						definedTable.insertDefinition(readDefinition);
						continue;
					}

					nativeDefinition->foreignIndex = readDefinition.foreignIndex;

					this->processDefinitionAttributes(nativeDefinition, &definitions[i]);
				}

				return 0;
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
							uint64_t activeIndex = fileAttribute.activeIndex;
							foundMatchingAttribute = true;
							readAttribute = fileAttribute;
							nativeAttribute.activeIndex = activeIndex;
							readDefinition->attributes[j].isActive = true;
							break;
						}
						j++;
					}

					// If matching definition couldn't be found.
					if (foundMatchingAttribute == false)
					{
						if( m_verbose )
						{
							// Warning the user.
							std::cout << "Warning: Expected attribute for name '"
								<< nativeAttribute.name <<
								"' not found in file! Attribute won't be activated." << std::endl;
						}
						continue;
					}

					// Activating attribute as both the file and the program have defined it.
					nativeAttribute.isActive = true;

					BLF_TYPE nativeAttribType = nativeAttribute.attribType;
					BLF_TYPE readAttribType   = readAttribute.attribType;
					if (readAttribType != nativeAttribType)
					{
						if( m_verbose )
						{
							std::cout << "File and native definitions differ, opting to use file's attrib type for: "
							<< nativeAttribute.name << std::endl;                                
						}
						if( getTypeCategory(nativeAttribType) == getTypeCategory(readAttribType))
						{
							nativeAttribute.attribType = readAttribType;
							//int sizeDiff = getTypeSize(nativeAttribType) - getTypeSize(readAttribType);
							//nativeAttribute.offset += sizeDiff;
							if( getTypeSize(readAttribType) > getTypeSize(nativeAttribType))
							{
								if( m_verbose )
								{
									std::cout << "File definition type is larger than native type! Opting to mark as foreign in order to prevent memory errors" << std::endl;
								}
								nativeAttribute.isForeign = true;
							}
						}
						else
						{
							if( m_verbose )
							{
								std::cout << "FATAL: Fatal type mismatch detected! Marking attribute as foreign in order to avoid errors" << std::endl;
							}
							nativeAttribute.isForeign = true;
						}
					}

					if (nativeAttribute.isActive && nativeAttribute.isForeign)
					{
						nativeDefinition->foreignAttributeCount++;
					}

					nativeDefinition->attributes[i] = nativeAttribute;
				}

				for (ObjectAttribute attrib : readDefinition->attributes)
				{
					if (attrib.isActive == false)
					{
						if( m_verbose )
						{
							std::cout << "Assuming attribute '" << attrib.name << "' is foreign!" << std::endl;
						}
                        attrib.isForeign = true;
                        nativeDefinition->attributes.push_back(attrib);
                    }
                    else
                    {
                        nativeDefinition->activeAttributeCount++;
                    }
				}
			}
			
			void readAttribute(ObjectAttribute* objectAttribute, int commonTableIndexerSize, void* addr, ForeignAttributeTable* foreignAttributes)
            {
                int8_t size = getTypeSize(objectAttribute->attribType);
                if( objectAttribute->isForeign == true ) 
                { 
                    void* foreignLoc = nullptr;
                    if (size != SIZE_DYNAMIC)
                    {
                        foreignLoc = new char[size];
                        m_readStream.read((char*)foreignLoc, size);
                    }
                    else
                    {
                        if (objectAttribute->attribType == TYPE_STRING)
                        {
                            foreignLoc = new char[sizeof(String)];
                            *((String*)foreignLoc) = readString();
                        }							
                        else if(objectAttribute->attribType == TYPE_OBJECTREFERENCE)
                        {
                            int commonTableReference = readIndexer(commonTableIndexerSize);
                            foreignLoc = new char[sizeof(commonTableReference)];
                            *((int*)foreignLoc) = commonTableReference;
                        }
                    }
                    foreignAttributes->addAttribute({objectAttribute->name, foreignLoc, objectAttribute->attribType, objectAttribute->referencedType});
                    return; 
                }
				if (size != SIZE_DYNAMIC)
				{
					m_readStream.read((char*)addr, size);
				}
				else
				{
					if (objectAttribute->attribType == TYPE_STRING)
					{
						String str = readString();
							
						(*(String*)addr) = str;
					}
					else if (objectAttribute->attribType == TYPE_OBJECTREFERENCE)
					{
						int commonTableReference = readIndexer(commonTableIndexerSize);
						*((int*)addr) = commonTableReference;
					}
				}
            }
			
			void readForeignAttributeTable(ForeignAttributeTable* foreignAttributeTable, ObjectDefinition* obj, int commonTableIndexerSize)
			{
				uint32_t size;
				if (m_loadedVersion == Version(1, 0, 0))
				{
					size = read<uint32_t>();
				}
				else
				{
					size = foreignAttributeTable->getReserved() - obj->foreignAttributeCount;
				}

				for( size_t index = 0; index < size; index++ )
				{
					String name = readString();
					BLF_TYPE type = (BLF_TYPE)read<int8_t>();
					ObjectAttribute attribute = {name, nullptr, type, nullptr, false, true};
					readAttribute(&attribute, commonTableIndexerSize, nullptr, foreignAttributeTable);
				}
			}

			TemplateObject* readObject(const ObjectTable& objectTable, uint8_t commonTableIndexerSize)
			{
				//std::cout << objectTable.getSize() << std::endl;
				ObjectDefinition* objectDefinition;

				// The number of foreign objects that we know about, if we know the exact amount
				// then reading will take less time. Of course that's impossible with older versions of BLF.
				uint32_t knownForeignObjects = 0;

				// Reading the name of the object (used as a reference to the object table)
				if( m_loadedVersion == Version(1, 0, 0))
				{
					String objectName = readString();

					objectDefinition = objectTable.getDefinitionFromIdentifier(objectName);	
				}
				else
				{
					uint64_t foreignIndex = readIndexer(objectTable.getIndexerSize());
					//std::cout << "F" << foreignIndex << std::endl;
					objectDefinition = objectTable.getDefinitionFromForeignIndex(foreignIndex);
					//knownForeignObjects = read<uint32_t>();
				}

				if (objectDefinition == nullptr)
				{
					return nullptr;
				}

				knownForeignObjects += objectDefinition->foreignAttributeCount;

				bool foreign = objectDefinition->isForeign;

				ForeignAttributeTable foreignAttributes(knownForeignObjects);

				TemplateObject* obj = (foreign) ? new ForeignObject(objectDefinition->identifier, objectDefinition->attributes) : objectDefinition->creator->createNew();

				for (int i = 0; i < objectDefinition->activeAttributeCount; i++)
				{
					ObjectAttribute* objectAttribute=nullptr;
					if (m_loadedVersion == Version(1, 0, 0))
					{
						String attributeName = readString();
						objectAttribute = getAttributeFromIdentifier(objectDefinition, attributeName);
					}
					else
					{
						uint64_t activeIndex = i+1;
						objectAttribute = getAttributeFromActiveIndex(objectDefinition, activeIndex);
					}

					if( objectAttribute == nullptr )
					{
						return nullptr;
					}

                    void* offset = getOffsetFromPointers(objectDefinition->templatePointer, objectAttribute->offset);
                    void* location = getPointerFromOffset(obj->dataPtr(), offset);
                    
                    readAttribute(objectAttribute, commonTableIndexerSize, location, &foreignAttributes);
                }

				//readForeignAttributeTable(&foreignAttributes, objectDefinition, commonTableIndexerSize);

				obj->storeForeignAttributes(foreignAttributes);

				return obj;
			}

			void readCommonTable(CommonTable& table, const ObjectTable& objectTable)
			{
				uint8_t indexerSize = read<uint8_t>();

				uint64_t tableSize = read<uint64_t>();

				for (int i = 0; i < tableSize; i++)
				{
					TemplateObject* object = readObject(objectTable, indexerSize);
					table.addCommonObject(object);
				}
			}
	};
}