#include "test.hpp"

#include "blf/localobjecttable.hpp"

#include <string>

struct Texture
{
    std::string path;
    std::string name;
    uint8_t filteringMode;
};

class WorldObject
{
    std::string m_name;
    const Texture* m_texture;

    public:
        WorldObject(const std::string& name, const Texture* tex)
            : m_name(name), m_texture(tex)
        {}

        void setName(const std::string& name) { m_name = name; }
        void setTexture(const Texture* texture) { m_texture = texture; }

        const std::string& getName() const { return m_name; }
        const Texture* getTexture() const { return m_texture; }

};

const uint8_t FILTER_LINEAR = 12;
const uint8_t FILTER_NEAREST = 59;

const Texture NULL_TEXTURE = {"!NULL", "NULL_TEXTURE", FILTER_NEAREST};

template<>
struct blf::DefaultConstructor<WorldObject>
{
    static void* function()
    {
        return new WorldObject("Undefined", &NULL_TEXTURE);
    }
};

bool test()
{
    // Initializing a basic number generator.
    srand(time(NULL));

    blf::LocalObjectTable objectTable;

    Texture textures[4] = {
        {"./engine/textures/grass.tex", "GRASS_1", FILTER_LINEAR},
        {"./engine/textures/wall.tex", "WALL_GENERIC", FILTER_LINEAR},
        {"./engine/textures/ui_cursor.tex", "CURSOR_GENERIC", FILTER_NEAREST},
        {"./engine/textures/ui_closebutton.tex", "CLOSE_BUTTON", FILTER_NEAREST}
    };

    WorldObject* objects[1024];
    for(int i = 0; i < 1024; i++)
    {
        std::string objectName = "Object #" + std::to_string(i + 1);
        objects[i] = new WorldObject(objectName, &textures[rand() % 4]);
    }

    /*blf::LocalObjectDefinition<Texture>& textureDefinition = objectTable.define<Texture>("Texture",
        blf::arg("Path", &Texture::path),
        blf::arg("Name", &Texture::name),
        blf::arg("FilteringMode", &Texture::filteringMode)
    );

    blf::LocalObjectDefinition<WorldObject>& worldObjectDefintion = objectTable.define<WorldObject>("WorldObject",
        blf::arg("Name", &WorldObject::getName, &WorldObject::setName),
        blf::arg("Texture", &WorldObject::getTexture, &WorldObject::setTexture, textureDefinition)
    );*/


    for(int i = 0; i < 1024; i++)
    {
        delete objects[i];
    }

    return true;
}