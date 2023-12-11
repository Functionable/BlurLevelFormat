#include <iostream>
#include "test.hpp"

#include "blf/localobjecttable.hpp"

class Foo
{
    public:
        std::string s_1;
        std::string s_2;
        std::string s_3;

        Foo(std::string s1 = "These", 
            std::string s2 = "are the",
            std::string s3 = "defaults")
            : s_1(s1), s_2(s2), s_3(s3)
        {}

        ~Foo() = default;

        bool operator==(const Foo& other) const
        {
            return s_1 == other.s_1 && 
                s_2 == other.s_2 && 
                s_3 == other.s_3;
        }

        bool operator!=(const Foo& other) const
        {
            return !(*this == other);
        }

        void print() const
        {
            std::cout << "foo-s_1: " << s_1 << "\n";
            std::cout << "foo-s_2: " << s_2 << "\n";
            std::cout << "foo-s_3: " << s_3 << std::endl;
        }
};

bool checkMismatch(const Foo& f1, const Foo& f2)
{
    if( f1 != f2 )
    {
        std::cout << "Mismatch between two Foo instances: \nFirst:\n";
        f1.print();
        std::cout << "Second:\n";
        f2.print();
    }

    return f1 != f2;
}

bool testDeserialization(blf::LocalObjectTable& localTable, blf::LocalObjectDefinition<Foo>& definition,
    const Foo& value, char* databuf)
{
    Foo temp = Foo();
    std::cout << "Test data: \n";
    value.print();

    blf::SerializationContext ctx = {blf::CommonTable::empty()};

    std::cout << "CASE 1: blf::LocalObjectDefinition::deserialize()\n";
    definition.deserialize(ctx, &temp, databuf);
    bool success1 = !checkMismatch(value, temp);
    std::cout << "CASE 1 RESULT: " << successString(success1) << "\n\n";

    std::cout << "CASE 2: blf::LocalObjectTable::fromData(Definition, void*)\n";
    Foo* newInstance = (Foo*)localTable.fromData(ctx, definition, databuf);
    bool success2 = !checkMismatch(*newInstance, temp);
    std::cout << "CASE 2 RESULT: " << successString(success2) << "\n\n";

    std::cout << "CASE 3: blf::LocalObjectTable::fromData(std::string, void*)\n";
    Foo* newInstance2 = (Foo*)localTable.fromData(ctx, definition, databuf);
    bool success3 = !checkMismatch(*newInstance, temp);
    std::cout << "CASE 3 RESULT: " << successString(success3) << "\n\n";

    return success3 && success2 && success1;
}

bool test()
{
    bool successful = true;

    char dataBuf[512];// = {0};

    blf::LocalObjectTable localObjectTable;

    Foo f1 = Foo("These are", "some different", "values compared to the default");
    Foo f2 = Foo("These are some", "completely different", "values compared to the instance 'f1'");
    Foo theDefault = Foo();

    blf::LocalObjectDefinition<Foo> fooDefinition = localObjectTable.define<Foo>("Foo",
        blf::arg("s_1", &Foo::s_1),
        blf::arg("s_2", &Foo::s_2),
        blf::arg("s_3", &Foo::s_3)
    );

    blf::SerializationContext ctx = {blf::CommonTable::empty()};

    fooDefinition.serialize(ctx, &f1, dataBuf);
    successful &= testDeserialization(localObjectTable, fooDefinition, f1, dataBuf);

    fooDefinition.serialize(ctx, &f2, dataBuf);
    successful &= testDeserialization(localObjectTable, fooDefinition, f2, dataBuf);

    fooDefinition.serialize(ctx, &theDefault, dataBuf);
    successful &= testDeserialization(localObjectTable, fooDefinition, theDefault, dataBuf);

    return successful;
}