#pragma once

//#include <zlib.h>

#include <sstream>
#include <fstream>
#include <cstring>

#include "string.hpp"
#include "datatable.hpp"
#include "commontable.hpp"
#include "objecttable.hpp"
#include "templateobject.hpp"

#include "informationheader.hpp"

namespace blf
{
	class Writer
	{
		String m_outputPath;
		std::stringstream m_headerStream;
		std::stringstream m_writeStream;
		std::stringstream* m_currentStream;

		bool m_allowCompression = false; // The option is there, if for some reason you want it.
		bool m_verbose          = false;
		
		public:

			template<typename T>
			void write(T data)
			{
				static_assert(std::is_pod<T>::value, "Write only supports POD types.");
				m_currentStream->write(reinterpret_cast<char*>(&data), sizeof(T));
			}

			void writeString(const char* value)
			{
				StringLength size = strlen(value);
				write(size);
				m_currentStream->write(value, size);
			}

			void writeString(const String& value)
			{
				StringLength size = value.getLength();

				write(size);
				m_currentStream->write(value.getBuffer(), size);
			}

			void writeIndexer(uint64_t index, uint8_t size)
			{
				switch (size)
				{
					case 1:
						write((uint8_t)index);
                        break;

					case 2:
						write((uint16_t)index);
                        break;

					case 4:
						write((uint32_t)index);
                        break;

					case 8:
						write(index);
                        break;
				}
			}

			void writeInformationHeader(const InformationHeader& header)
			{
				m_currentStream = &m_headerStream;
				m_headerStream.write(header.signature.getBuffer(), 4);
				write(header.major);
				write(header.minor);
				write(header.fix);
				write<uint8_t>((header.compressionFlags | m_allowCompression));
				write(header.headerFlagCount);
				m_currentStream = &m_writeStream;
			}

			void writeObjectTable(const ObjectTable& table)
			{
				uint8_t indexerSize = table.getIndexerSize();
				int64_t size = table.getSize();

				write(indexerSize);
				write(size);
				for (int i = 0; i < size; i++)
				{
					blf::ObjectDefinition* definition = table.getDefinitionFromIndex(i);
					writeString(definition->identifier);
					uint8_t attributeCount = definition->attributes.size();
					write(attributeCount);
					for (int j = 0; j < attributeCount; j++)
					{
						ObjectAttribute* attribute = &(definition->attributes[j]);
						//dynamicWrite(&attribute->name);
						writeString(attribute->name);
						uint8_t type = attribute->attribType;
						write(type);
					}
				}
			}

			/**
			 *	Writes the common table to file, i'll take this as another opportunity to say, if for whatever reason you're implementing
			 *	an alternate saving mechanism other than the one that BLF already provides, then please do be careful when using common tables,
			 *  if you don't remove duplicate references from the list before you build it, you will have many duplicates that will take away from the size of the file.
			 *
			 *	You have been warned.
			 */
			void writeCommonTable(const CommonTable& table, const ObjectTable& objects);

			void writeDataTable(const DataTable& dataTable, const ObjectTable& objects, const CommonTable& common)
			{
				for (DataTable::ConstIterator it = dataTable.begin(); it != dataTable.end(); it++)
				{
					write((uint8_t)(0xAA));
					writeObject(*it, objects, common);
				}
				write((uint8_t)255);
			}

			void writeForeignAttributes(const ForeignAttributeTable& foreignAttributes, const CommonTable& common)
			{
				int i = 0;
				//write((uint32_t)(foreignAttributes.getSize()));
				for(ObjectAttribute attribute : foreignAttributes )
				{
					void* location = attribute.offset;
					//dynamicWrite(&attribute.name);
					writeString(attribute.name);
					//write<int16_t>(i);
					write<int8_t>((int8_t)attribute.attribType);
					int size = getTypeSize(attribute.attribType);
					if (size != -1)
					{
						m_currentStream->write((char*)location, size);
					}
					else
					{
                        if( attribute.attribType == TYPE_STRING )
                        {
                            //dynamicWrite(((String*)location));
							writeString(*(String*)location);
                        }
                        else if(attribute.attribType == TYPE_OBJECTREFERENCE)
                        {
							TemplateObject* object = (TemplateObject*)location;
							writeIndexer(object->commonTableIndex, common.getIndexerSize());
						}
					}
					i++;
				}	
			}

			/**
			 *	Writes the object to the file, make sure you stored your defined blf::ObjectTable first, using storeObjectTable(blf::ObjectTable).
			 *	After you've done that, the blf::Writer will manage the rest.
			 */
			void writeObject(const TemplateObject* obj, const ObjectTable& object, const CommonTable& common)
			{
				const char* objectName = obj->getObjectName();
				ObjectDefinition* objectDefinition = object.getDefinitionFromIdentifier(obj->getObjectName());
				// Writing the identifier to the file.
				//dynamicWrite(&objectName);
				//writeString(objectName);
				//std::cout << (int)object.getIndexerSize() << std::endl;
				writeIndexer(objectDefinition->arrayIndex+1, object.getIndexerSize());
				//write((uint32_t)(obj->getForeignAttributes().getSize()));
				//std::cout << objectDefinition->identifier << ": " << objectDefinition->arrayIndex << ", " << object.getSize() << " q" << (int)object.getIndexerSize() << std::endl;

				for (uint32_t i = 0; i < objectDefinition->attributes.size(); i++)
				{
					ObjectAttribute* objectAttribute = &objectDefinition->attributes[i];
					void* offset = getOffsetFromPointers(objectDefinition->templatePointer, objectAttribute->offset);
					void* location = getPointerFromOffset(obj->dataPtr(), offset);
					int8_t size = getTypeSize(objectAttribute->attribType);

					//write<uint32_t>(i);
					if (size != -1)
					{
						m_currentStream->write((char*)location, size);
					}
					else
					{
                        if( objectAttribute->attribType == TYPE_STRING )
                        {
                            //dynamicWrite(((String*)location));
							writeString(*(String*)location);
                        }
                        else if(objectAttribute->attribType == TYPE_OBJECTREFERENCE)
                        {
							TemplateObject* object = (TemplateObject*)location;
							writeIndexer(object->commonTableIndex, common.getIndexerSize());
						}
					}
				}

				//writeForeignAttributes(obj->getForeignAttributes(), common);
			}

			void flushToFile();

			Writer(String filePath, BLF_FLAG flags = FLAG_EMPTY);
			~Writer();
	};
}
