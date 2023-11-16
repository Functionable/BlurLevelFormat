# BlurLevelFormat 2.0
A binary data storage file format written for BlurEngine.

## Why rewrite?
BLF 2.0 started as an effort to clean up the old codebase of BLF. It ended up morphing into a complete rewrite of the project. This time, a different approach has been taken to (de)serializing data. The focus of this rewrite is code cleanliness. It is designed with easier integration with Lua in mind.

The aim is for code for BLF 1.x to be easily adapted to work with BLF 2.0. The most noticable change for the user will be in how attributes for an object are defined. In BLF 1.x it was required for an object to extend `blf::TemplateObject` (similar functionality *MAY* be supported to make porting code based on BLF 1.x easier) and overriding `getAttributeMap` in order to define its attributes. In BLF 2.0, attributes are going to be defined as a list that is going to be passed to the constructor of your ObjectDefinition. The preferred way to create your attributes is when you create your ObjectTable and definitions. So, entirely within the scope of you using blf to read/write. This will allow you to keep all blf-specific code self-contained unlike with BLF 1.x.

Backwards compatibility is (at the moment) planned to be maintained between BLF 1.x files and BLF 2.x files.

Unfortunately, BLF 2.0 is currently __**in very early stages and has not reached a usable state**__. As of now, only basic (de)serialization functionality has been added. Out of the three tables, only the ObjectTable has been implemented in any way, and even then not fully.

## Planned structural changes
- Tables will have local/foreign variants instead of objects. This should make the resolution of definition conflicts easier and less error prone.
    - When writing, the user of the library will be required to instantiate a blf::LocalObjectTable and when reading from a file, a blf::ForeignTable instead (the exception to this is a blf::LocalObjectTable which is required to be initialized when reading as it provides useful information on how objects should be instantiated and how data should be applied to an instance of an object).
- (As mentioned before) the process of writing a definition no longer will require for blf-specific code to be put within a class. Instead, this code may be put in a read method alongside the defintion of a blf::LocalObjectTable.
    - Type will no longer need to be specified when defining an attribute.

## What has to be done?
These things have to be done to achieve BLF feature parity.
- [ ] Serialization/Deserialization of structs (containing all BLF types)
    - [x] Serialization of built-in types (e.g: ints, floats, bools and strings)
    - [x] Inline serialization of classes containing classes.
    - [ ] Serializing pointers/references to classes (with multiple references) as references to the common table. (Where this can be collapsed down to an inline reference to a class if an instance is only referred to once).
        - [ ] Implement CommonTable
- [ ] Serialization/Deserialization of objects not explicitly covered by a LocalObjectDefinition.
    - [ ] Implement ForeignObjectDefinition
    - [ ] Implement ForeignObjectTable.
- [ ] Serialization/Deserialization of all tables.
    - [ ] Implement deserialization of ForeignObjectTable.
    - [ ] Allow ForeignObjectTable and LocalObjectTable to be combined, into something like an ObjectDeserializationContext. This class will try to connect attributes from an object in the foreign table to the local table. Which will allow the local table to do the heavy lifting for reinterpreting stored data while strictly adhering to the data layout set out by the foreign data.
        - [ ] Implement type checking, allow for some type mismatches to be dismissed with a warning (which have to be explicitly enabled in the file). Larger mismatches will result in BLF aborting the read operation.
        - [ ] Implement a flag which aborts the read operation if not every foreign attribute in an object is matched to a corresponding attribute within an object in the local table.
    - [ ] Implement deserialization of a CommonTable by the use of the ObjectDeserializationContext.
    - [ ] Implement deserialization of a DataTable via the use of an ObjectDeserializationContext.
    - [ ] Implement the serialization of a LocalObjectTable.
    - [ ] Implement the serialization of a DataTable and CommonTable with the use of a LocalObjectTable.
- [ ] Backwards compatibility with BLF 1.x files.
- [ ] Implement compression.

Additional Features (nice to haves for BLF 2.0)
- [x] Allow for an ObjectAttribute to pull data through getters/setters
- [ ] Allow for an ObjectAttribute to pull data from an object through an std::function.
- [ ] Implement an Array datatype (Would allow to specify whether it is restricted to a single type).
- [ ] Implement a Dictionary datatype (Would allow to specify whether it is restricted to a single key/value type).
- [ ] Implement flags in the InformationHeader. 
    - [ ] Allow any basic type to be used within a header. Due to the fact that the InformationHeader will not be compressed while the rest of the file may be, there must be a mechanism preventing an object being used within a flag.
    - [ ] Implement compression status as a flag.
