#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <vector>
#include <cstring>
#include <algorithm>
#include <functional>

#include <cstdlib>
#include <ctime>

#include "blf/string.hpp"
#include "blf/reader.hpp"
#include "blf/writer.hpp"
#include "blf/datatable.hpp"
#include "blf/objecttable.hpp"
#include "blf/commontable.hpp"
#include "blf/templateobject.hpp"
#include "blf/templatecreator.hpp"
#include "blf/objectattribute.hpp"
#include "blf/objectdefinition.hpp"
#include "blf/foreignattributetable.hpp"

#include "blf/basereadable.hpp"
#include "blf/basewriteable.hpp"
#include "blf/dynamicwriteable.hpp"
#include "blf/dynamicreadable.hpp"

// BLF version info. VERSION 1.0.0
const int32_t BLFMAJOR = 1;
const int32_t BLFMINOR = 0;
const int32_t BLFFIX   = 0;

const char* BLFSIGNATURE = "BLFF";
const int TEST_TILES = 5;

const char* BLF_TILE_NAME = "Tile";

// Defining STRDUP as different functions depending on the platform.
// This is needed because some windows compilers see strdup as deprecated
// and use _strdup.

#ifdef _WIN32
#define STRDUP(x) _strdup(x)
#else
#define STRDUP(x) strdup(x)
#endif

// A way of storing object data without knowing what kind of object it is.
class Object
{
	public:
		std::vector<blf::ObjectAttribute> attributes;
		blf::ForeignAttributeTable foreignAttributes;
		blf::ObjectDefinition* definition;

};

class Texture : public blf::TemplateObject
{
	public:
		blf::String filePath;

		Texture() { }

		Texture(blf::String filePath)
		{
			this->filePath = filePath;
		}

		const char* getObjectName() override
		{
			return "Texture";
		}

		std::vector<blf::ObjectAttribute> getAttributeMap() override
		{
			return
			{
				{"filePath", &filePath, blf::TYPE_STRING}
			};
		}

		void storeForeignAttributes(blf::ForeignAttributeTable table) override
		{
			// Nowhere to store these, instead we're gonna print them
			for (blf::ObjectAttribute attr : table.attributes)
			{
				std::cout << "Attribute of type " << attr.attribType << " which has the name of: " << attr.name << std::endl;
			}
		}
};

class testObject : public blf::TemplateObject
{
	public:

		testObject() {}

		// Some properties for this test class.
		int aRandomNumber;
		blf::String aString;

		const char* getObjectName() override
		{
			return "testObject";
		};

		std::vector<blf::ObjectAttribute> getAttributeMap() override
		{
			return
			{
				{"aRandomNumber", &aRandomNumber, blf::TYPE_INT},
				{"aString", &aString, blf::TYPE_STRING}
			};
		}

		void storeForeignAttributes(blf::ForeignAttributeTable table) override
		{
			// Nowhere to store these, instead we're gonna print them
			for (blf::ObjectAttribute attr : table.attributes)
			{
				std::cout << "Attribute of type " << attr.attribType << " which has the name of: " << attr.name << std::endl;
			}
		}

};

class Tile : public blf::TemplateObject
{
	public:
		blf::String ID;
		double x;
		double y;
		double z;
		Texture* texture;

		Tile() {}

		Tile(blf::String ID, double x, double y, double z, Texture* texture)
		{
			this->ID = ID;
			this->x = x;
			this->y = y;
			this->z = z;
			this->texture = texture;
		}

		const char* getObjectName() override
		{
			return "Tile";
		};

		std::vector<blf::ObjectAttribute> getAttributeMap() override
		{
			return
			{
				{"ID", &ID, blf::TYPE_STRING},
				{"x", &x, blf::TYPE_DOUBLE},
				{"y", &y, blf::TYPE_DOUBLE},
				{"z", &z, blf::TYPE_DOUBLE},
				{"texture", &texture, blf::TYPE_OBJECTREFERENCE, "Texture"}
			};
		}

		void storeForeignAttributes(blf::ForeignAttributeTable table) override
		{
			// Nowhere to store these, instead we're gonna print them
			for (blf::ObjectAttribute attr : table.attributes)
			{
				std::cout << "Attribute of type " << attr.attribType << " which has the name of: " << attr.name << std::endl;
			}
		}
};

class StateTile : public Tile
{
	public:

		int state;

		std::vector<blf::ObjectAttribute> getAttributeMap() override
		{
			return
			{
				{"x", &x, blf::TYPE_FLOAT },
				{"state", &state, blf::TYPE_INT},
				{"y", &y, blf::TYPE_DOUBLE},
				{"z", &z, blf::TYPE_DOUBLE},
				{"texture", &texture, blf::TYPE_OBJECTREFERENCE, "Texture"},
				{"ID", &ID, blf::TYPE_STRING},
			};
		}

		using Tile::storeForeignAttributes;
		using Tile::getObjectName;
};

/**
 * Copies object data to a object definition that can be later managed by blf classes.
 */
void initializeDefinition(blf::TemplateObject* obj, blf::ObjectDefinition* definition)
{
	definition->identifier = obj->getObjectName();
	definition->templatePointer = obj;
	definition->attributes = obj->getAttributeMap();
	std::cout << obj->getObjectName() << std::endl;
}

template<typename T, typename = std::enable_if<std::is_base_of<blf::TemplateObject, T>::value>>
blf::ObjectDefinition createDefinition()
{
	blf::ObjectDefinition definition;
	T object;
	initializeDefinition(&object, &definition);
	definition.creator = new blf::SpecialisedTemplateCreator<T>();
	return definition;
}

void writetest()
{
	// WRITING
	std::vector<Tile> tiles;

	// Initializing the Information Header.
	blf::InformationHeader header = {
		BLFSIGNATURE, BLFMAJOR, BLFMINOR, BLFFIX
	};

	blf::CommonTable common;

	blf::DataTable data;

	blf::ObjectTable objects = {
		createDefinition<testObject>(),
		createDefinition<Texture>(),
		createDefinition<StateTile>(),
	};

	srand(time(0));

	Texture textures[4];
	textures[0] = Texture("grass.png");
	textures[1] = Texture("dirt.png");
	textures[2] = Texture("crate.png");
	textures[3] = Texture("metalblock.png");

	std::cout << "Write test started with " << TEST_TILES << " tile objects." << std::endl;

    tiles.reserve(TEST_TILES);
    
	for (int i = 0; i < TEST_TILES; i++)
	{
		std::string test = "Tile " + std::to_string(i);

		tiles.emplace_back(
            //new Tile(
                test,
                5,
                6,
                4,
                &textures[rand() % 4]
                //)
        );

		data.addObject(&(tiles[i]));
	}

	int64_t count = TEST_TILES;

	auto t1 = std::chrono::high_resolution_clock::now();

	blf::Writer writer("level.blf");

	writer.storeObjectTable(&objects);
	data.computeCommonTable(common, objects);
	data.buildArray();

	//testObject test;
	//test.aString = "VROOOOOOOOOOOOOOOOOOOOOOOOOM\0";
	//common.addCommonObject(&test);

	common.buildCommonObjectArray();
	writer.storeCommonTable(&common);
	std::cout << "Common Table Size: " << common.getArraySize() << std::endl;

    std::cout << "1" << std::endl;
    
	writer.dynamicWrite(&header);
    
    std::cout << "2" << std::endl;
    
	writer.writeObjectTable(&objects);
    
    std::cout << "3" << std::endl;

	writer.writeCommonTable(&common); // ERROR HERE

    std::cout << "4" << std::endl;

    std::cout << "Writing Table Size: " << data.getArraySize() << std::endl;
    writer.writeDataTable(&data);

    std::cout << "5" << std::endl;
    //writer.wr

	auto t2 = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();

	std::cout << "Write completed in: " << duration << " milliseconds." << std::endl;

	std::cout << "Common objects:" << std::endl;
	if (common.isArrayBuilt() && common.getArraySize() > 0)
	{
		for (blf::TemplateObject* tex : common)
		{
			for (blf::ObjectAttribute attrib : tex->getAttributeMap())
			{
				std::cout << attrib.name << ": ";
				switch (attrib.attribType)
				{
					case blf::TYPE_STRING:
						blf::String* string = (blf::String*)attrib.offset;
						std::cout << *string;
						break;
				}
				std::cout << std::endl;
			}
		}
	}
	else
	{
		std::cout << "none" << std::endl;
	}
}


void readtest()
{
	// READING

	// Variables that will be written to.
	int32_t blurVersion;
	int64_t blurTileCount = 0;
	char* blurSignature = new char[4];
	std::vector<Tile> blurTiles;

	auto t3 = std::chrono::high_resolution_clock::now();
	blf::Reader levelReader("level.blf");
	blf::InformationHeader fileInfo = levelReader.dynamicRead<blf::InformationHeader>();
	blf::ObjectTable objects = {
		createDefinition<Texture>(),
		createDefinition<Tile>(),
	};

	std::cout << "Object Table" << std::endl;

	levelReader.readObjectTable(&objects);
	for (int i = 0; i < objects.getSize(); i++)
	{
		blf::ObjectDefinition* definition = objects.getDefinitionFromIndex(i);
		
		std::cout << "Definition: " << i
			<< "\t - ATTRIBUTES: " 
			<< definition->attributes.size()
			<< "\t - FOREIGN: " << ((definition->isForeign) ? ("Y") : ("N")) << std::endl;
		std::cout << "- ID: " << definition->identifier << std::endl;
		for (int j = 0; j < definition->attributes.size(); j++)
		{
			blf::ObjectAttribute& attribute = definition->attributes[j];
			std::cout << "\t- Attribute " << j << std::endl;
			std::cout << "\t  - NAME: " << attribute.name << std::endl;
			std::cout << "\t  - FOREIGN: " << attribute.isForeign << std::endl;
			std::cout << "\t  - TYPE: " << attribute.attribType << std::endl;
			if (attribute.attribType == blf::TYPE_OBJECTREFERENCE)
			{
				const char* referencedType = attribute.referencedType;
				std::cout << "\t    - REFTYPE: " << ((referencedType == nullptr) ? "Invalid/NULL" : referencedType) << std::endl;
			}
		}
	}
	
	blf::CommonTable common = levelReader.readCommonTable(&objects);
    common.buildCommonObjectArray();

	std::cout << "Common Table Size: " << common.getArraySize() << std::endl;

	for (blf::TemplateObject* tex : common)
	{
		for (blf::ObjectAttribute attrib : tex->getAttributeMap())
		{
			std::cout << attrib.name << ": ";
			switch (attrib.attribType)
			{
			case blf::TYPE_STRING:
				blf::String* string = (blf::String*)attrib.offset;
				std::cout << *string;
				break;
			}
			std::cout << std::endl;
		}
	}
	
	blf::DataTable data;
	levelReader.readDataTable(&data, &objects, &common);
    data.buildArray();
    
    std::cout << "Array: " << data.getArraySize() << std::endl;
    
    /*for( blf::TemplateObject* obj : data )
    {
        for( blf::ObjectAttribute attrib : obj->getAttributeMap())
        {
            std::cout << attrib.name << ": ";
			switch (attrib.attribType)
			{
			case blf::TYPE_STRING:
				blf::String* string = (blf::String*)attrib.offset;
				std::cout << *string;
				break;
			}
			std::cout << std::endl;
        }
    }*/

	auto t4 = std::chrono::high_resolution_clock::now();
	auto duration2 = std::chrono::duration_cast<std::chrono::milliseconds>(t4 - t3).count();
	std::cout << "Read completed in: " << duration2 << " milliseconds." << std::endl;
}

/*
	TASKS:
	-	Finish write logic. [Y.D]
	-	Translate that logic to make matching reading logic.

	-	Document all the classes
	-	Move the header defined functions in reader.hpp and writer.hpp into their respective source files.
	-	Make a function that can simplify this entire process.

	-	Create the DataTable class.

	-	Group read objects by type, allow DataTable to return list of pointers to all objecs of a group.
*/

int main()
{
	std::cout << "begin" << std::endl;
	writetest();
	readtest();
	//for (Tile tile : tiles)
	{
		//std::cout << tile.ID << ", " << tile.x << ", " << tile.y << ", " << tile.z << std::endl;
	}
	std::cout << "end" << std::endl;
}
