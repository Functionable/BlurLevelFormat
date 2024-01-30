#include "test.hpp"

#include "blf/localobjecttable.hpp"
#include <string>

class Vector
{
    public:
        float x;
        float y;
        float z;

        Vector(float theX = 0, float theY = 0, float theZ = 0)
            : x(theX), y(theY), z(theZ)
        {}

        operator std::string() const 
        { 
            return "(" + std::to_string(x) 
                + ", " + std::to_string(y)
                + ", " + std::to_string(z) + ")";
        }

        bool operator ==(const Vector& other) const
        {
            return x == other.x && y == other.y && z == other.z;
        }
};

class GameObject
{
    public:
        std::string name;
        Vector position;
        Vector size;

        GameObject(const std::string& name = "NULL", Vector thePos=0, Vector theSize=0)
            : name(name), position(thePos), size(theSize)
        {}

        operator std::string() const
        {
            return "Object '" + name + "' at: " + (std::string)position + " has a size of: " + (std::string)size;
        }

        bool operator ==(const GameObject& other) const
        {
            return other.name == name && position == other.position && size == other.size;
        }

        bool operator !=(const GameObject& other) const
        {
            return !(*this == other);
        }

        void print() const
        {
            std::cout << (std::string)(*this) << std::endl;
        }
};

bool performTest(char* databuf, GameObject& object, blf::LocalObjectDefinition<GameObject>& objectDefinition, blf::LocalObjectTable& table)
{
    GameObject defaultObj;

    std::cout << "Test case: \n";
    object.print();

    blf::SerializationContext ctx = {};

    objectDefinition.serialize(ctx, &object, databuf);

    const size_t measuredSpan = objectDefinition.dataLength(ctx, databuf);
    const size_t calculatedSpan = objectDefinition.serializedLength(ctx, &object);

    if( measuredSpan != calculatedSpan )
    {
        std::cout << "Serialized span (" << measuredSpan 
            << ") vs calculated span (" << calculatedSpan << ") does not match. Test failed." << std::endl;
        return false;
    }
    else 
    {
        std::cout << "Data length: OK" << std::endl;
    }

    objectDefinition.deserialize(ctx, &defaultObj, databuf);
    if( defaultObj != object )
    {
        std::cout << "Object was not the same after deserialization.\nOriginal:\n";
        object.print();
        std::cout << "Deserialized (mismatch detected):\n";
        defaultObj.print();
        std::cout << "Test failed." << std::endl;
        return false;
    }
    else 
    {
        std::cout << "Deserialization Integrity (using LocalObjectDefinition<GameObject>::deserialize): OK" << std::endl;
    }


    GameObject* init1 = (GameObject*)table.fromData<GameObject>(ctx, objectDefinition, databuf);

    if( *init1 != object )
    {
        std::cout << "Object was not the same after table::fromData.\nOriginal:\n";
        object.print();
        std::cout << "Deserialized (mismatch detected):\n";
        defaultObj.print();
        std::cout << "Test failed." << std::endl;
        return false;
    }
    else 
    {
        std::cout << "Deserialization Integrity (using LocalObjectTable::fromData): OK" << std::endl;
    }

    return true;
}

bool test()
{
    bool success = true;

    char buf[512];

    blf::LocalObjectTable objectTable;
    blf::LocalObjectDefinition<Vector>& vectorDefinition = objectTable.define<Vector>(
        "Vector",
        blf::arg("X", &Vector::x),
        blf::arg("Y", &Vector::y),
        blf::arg("Z", &Vector::z)
    );

    blf::LocalObjectDefinition<GameObject>& objectDefinition = objectTable.define<GameObject>(
        "GameObject",
        blf::arg("Position", &GameObject::position, vectorDefinition),
        blf::arg("Name", &GameObject::name),
        blf::arg("Size", &GameObject::size, vectorDefinition)
    );

    GameObject foo = GameObject("Foo", Vector(2, 2), Vector(3, 3, 3));
    GameObject bar = GameObject("BarBar", Vector(200, 60.2, -2e9), Vector(5, 5, 8));
   
    success &= performTest(buf, foo, objectDefinition, objectTable);
    success &= performTest(buf, bar, objectDefinition, objectTable);

    return success;
}