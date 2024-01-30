#include "test.hpp"

#include <string>

#include "blf/localobjecttable.hpp"
#include "blf/datatable.hpp"

struct Foo
{
    std::string value1;
    uint64_t value2;
    uint32_t value3;
};

struct Bar
{
    bool value1;
    float value2;
    float value3;
    float value4;
};

bool test()
{
    blf::LocalObjectTable localObjectTable;


    Bar bars[] = {
        {false, 2, 3, 7},
        {true, 4, 3, 8},
        {false, 8, 8, 8},
        {true, 9, 9, 3}
    };

    Foo foos[] = {
        {"Hello World!", 88, 823828343},
        {"TestString3", 233242734723477, 22}
    };

    auto fooDefinition = localObjectTable.define<Foo>("Foo Definition", 
        blf::arg("value1", &Foo::value1),
        blf::arg("value2", &Foo::value2),
        blf::arg("value3", &Foo::value3)
    );

    auto barDefinition = localObjectTable.define<Bar>("Bar Definition!",
        blf::arg("value1", &Bar::value1),
        blf::arg("value2", &Bar::value2),
        blf::arg("value3", &Bar::value3),
        blf::arg("value4", &Bar::value4)
    );

    blf::UnbakedDataTable data;

    for(int i = 0; i < sizeof(foos); i++)
    {
        data.add(fooDefinition, foos[i]);
    }

    for(int i = 0; i < sizeof(bars); i++)
    {
        data.add(barDefinition, bars[i]);
    }

    return true;
}