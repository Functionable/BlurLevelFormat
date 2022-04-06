#include <ios>
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

#include "blf.hpp"

const int TEST_TILES = 100000;
const char* BLF_TILE_NAME = "Tile";

class Texture : public blf::TemplateObject
{
	public:
		blf::String filePath;

		Texture() { }

		Texture(blf::String filePath)
		{
			this->filePath = filePath;
		}

		const char* getObjectName() const  override
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
			for (blf::ObjectAttribute attr : table)
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

		const char* getObjectName() const override
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
			for (blf::ObjectAttribute attr : table)
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

		const char* getObjectName() const override
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

		/* NOTE! A class deriving fromForeignObject does NOT require you to override
		storeForeignAttributes in order to use it, however it might come in useful
		when you need to use it to log these foreign attributes, alternatively you may need to insert equivalent objects into an object in a scripting language running at runtime, or other edge cases which this virtual function should hopefully cover. If you don't need such functionality, you need not worry about foreign attribute tables. 
		
		TL;DR: You most likely don't need to override storeForeignAttributes/getForeignAttributes, and if you do, read what it enables in the full paragraph.
		
		*/
		void storeForeignAttributes(blf::ForeignAttributeTable table) override
		{
			// Nowhere to store these, instead we're gonna print them
			for (blf::ObjectAttribute attr : table)
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
				{"x", &x, blf::TYPE_DOUBLE },
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

void writetest()
{
	const int names = 4;
	blf::String identifiers[names] = 
	{
		"one",
		"two",
		"three",
		"four"
	};

	// WRITING
	std::vector<Tile> tiles;

	blf::DataTable data;

	blf::ObjectTable objects = {
		blf::createDefinition<testObject>(),
		blf::createDefinition<Texture>(),
		blf::createDefinition<Tile>(),
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

		blf::ForeignAttributeTable attributeTable;
		attributeTable.addAttribute({
			identifiers[rand() % 4],
			&identifiers[rand() % 4],
			blf::TYPE_STRING
		});

		tiles.emplace_back(
            test,
            rand() % 200,
            rand() % 200,
            rand() % 200,
            &textures[rand() % 4]
        );
		//tiles[i].storeForeignAttributes(
		//	attributeTable
		//);

		data.addObject(&(tiles[i]));
	}

	blf::writeFile("level.blf", objects, data);
}


void readtest(const char* name)
{
	// READING
	blf::BLFFile readFile;

	blf::ObjectTable objects = {
		blf::createDefinition<Texture>(),
		blf::createDefinition<StateTile>(),
	};

	blf::readFile(name, readFile, objects);
}

/*
	TASKS:
	-	Finish write logic. [Y.D]
	-	Translate that logic to make matching reading logic. [Y.D]

	-	Document all the classes * - keep till last
	-	Move the header defined functions in reader.hpp and writer.hpp into their respective source files.
	-	Make a function that can simplify this entire process. [Y.D]

	-	Create the DataTable class. [Y.D]
	
	-   Debug output, might be useful in the future.

	-	Group read objects by type, allow DataTable to return list of pointers to all objects of a group. [Y.D]
*/

int main(int argc, char** argv)
{
	std::string readPath = "level.blf";

	if( argc > 1 )
	{
		readPath = argv[1];
	}

	if( argc <= 1 )
	{
		writetest();
	}
	readtest(readPath.c_str());
}