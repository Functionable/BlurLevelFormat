#include "test.hpp"

#include "blf/localobjecttable.hpp"

#include <vector>

struct ImportantStruct
{
    float importantVal1;
    float importantVal2;
    float importantVal3;
    float importantVal4;

    ImportantStruct()
    {
        importantVal1 = (rand() % 1000000) / 8.0f;
        importantVal2 = (rand() % 1000000) / 8.0f;
        importantVal3 = (rand() % 1000000) / 8.0f;
        importantVal4 = (rand() % 1000000) / 8.0f;
    }

    bool operator==(const ImportantStruct& other) const
    {
        return other.importantVal1 == importantVal1
            && other.importantVal2 == importantVal2
            && other.importantVal3 == importantVal3
            && other.importantVal4 == importantVal4;
    }
};

struct StorageStruct
{
    std::vector<ImportantStruct> structs;
    std::string string;

    bool operator==(const StorageStruct& storage) const
    {
        if(structs.size() != storage.structs.size())
        {
            return false;
        }

        bool allEqual = true;
        for(size_t i = 0; i < structs.size(); i++)
        {
            allEqual &= storage.structs.at(i) == structs.at(i);
        }


        return allEqual && string == storage.string;
    }
};

struct OtherStruct
{
    std::vector<int> numbers;
    int qualityDescriptor;
};

const int TEST_COUNT = 100000;

bool test()
{
    bool correct = true;
    char buf[sizeof(StorageStruct) * TEST_COUNT + 4 + 200];
    StorageStruct storage;
    storage.string = "The Storage String";
    storage.structs.reserve(TEST_COUNT);
    for(int i = 0; i < TEST_COUNT; i++)
    {
        storage.structs.push_back(ImportantStruct());
    }

    std::vector<int> ints;
    ints.reserve(TEST_COUNT);
    for(int i = 0; i < TEST_COUNT; i++)
    {
        ints.push_back(rand() % 4000);
    }

    OtherStruct otherStruct = {
        ints,
        rand() % 100
    };

    blf::LocalObjectTable local;

    static_assert(blf::is_list<std::vector<ImportantStruct>>::value, "is_list doesn't work!");


    auto importantDefinition = local.define<ImportantStruct>("ImportantStruct",
        blf::arg("Important1", &ImportantStruct::importantVal1),    
        blf::arg("Important2", &ImportantStruct::importantVal2),    
        blf::arg("Important3", &ImportantStruct::importantVal3),    
        blf::arg("Important4", &ImportantStruct::importantVal4)
    );

    auto storageDefintion = local.define<StorageStruct>("StorageStruct",
        blf::arg("AString", &StorageStruct::string),
        blf::arg("AList", &StorageStruct::structs, &importantDefinition)
    );

    auto otherDefinition = local.define<OtherStruct>("OtherStruct",
        blf::arg("Numbers", &OtherStruct::numbers),
        blf::arg("Quality", &OtherStruct::qualityDescriptor)
    );

    blf::SerializationContext ctx = {};

    std::cout << "The written size of OtherStruct will be: " << 
        otherDefinition.serializedLength(ctx, &otherStruct) << std::endl;

    std::cout << "The written size of StorageStruct will be: " << 
        storageDefintion.serializedLength(ctx, &storage) << std::endl;

    StorageStruct storage2;

    {
        PROFILE_SCOPE("Serializing StorageStruct");
        storageDefintion.serialize(ctx, &storage, buf);
    }

    {
        PROFILE_SCOPE("Deserializing StorageStruct");
        storageDefintion.deserialize(ctx, &storage2, buf);
    }

    correct &= storage2 == storage;

    return correct;
}