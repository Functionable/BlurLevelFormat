#include "test.hpp"

#include "blf/object/foreignobjectdefinition.hpp"

// How to make references work.
// 1 -> All data is gathered into a container. Unbaked data table?
// 2 -> CommonTable is initialized.
// 3 -> Walk unbaked data table with commontable.
//     \-> e.g CommonTable::populateReferences(UnbakedDataTable&, ObjectTable&)
// Note: As a consequence of this setup, the unbakeddatatable would have to be a vector of
// char array pointers. This way it could work with both foreign/local definitions.
// 4 -> CommonTable is populated with reference info, it can compute the indexer size as well as a
//      map of what reference should be inlined/replaced with an indexer. 
// 5 -> DataTable can now be serialized. A copy of CommonTable has to be passed.
// 
// Deserializing references: 
// 1 -> CommonTable is read before DataTable, thus indexer size is known before reading any object.
//     \-> All objects with multiple references can also be extracted.
// 2 -> DataTable has to be read with a CommonTable&.

bool test()
{
    blf::UnbakedList<blf::ObjectAttribute> attrList = {
        {"X", blf::TYPE_FLOAT},
        {"Y", blf::TYPE_FLOAT},
        {"Z", blf::TYPE_FLOAT}
    };

    blf::CommonTable table = blf::CommonTable::empty();
    blf::SerializationContext ctx = {table};

    blf::ForeignObjectDefinition vectorDefinition("Vector", attrList.bake());

    // Data for Vector(1.2f, 3.4f, 5.6f);
    unsigned char data[12] = {
        0x9a, 0x99, 0x99, 0x3f,   //1.2f
        0x9a, 0x99, 0x59, 0x49,   //3.4f
        0x33, 0x33, 0xb3, 0x40  //5.6f
    };

    unsigned char read[12];

    vectorDefinition.deserialize(ctx, (char*)read, (const char*)data);

    bool readCorrect = true;
    for(int i = 0; i < 12; i++)
    {
        readCorrect &= read[i] == data[i];
    }

    return true;
}