#pragma once

#include "enums.hpp"
#include "object/attributeprocedure.hpp"
#include "object/foreignobjectdefinition.hpp"
#include "objecttable.hpp"

#include "object/objectattribute.hpp"
#include "object/objectdefinition.hpp"
#include "object/attributelocation.hpp"
#include "object/localobjectdefinition.hpp"
#include "object/specializedclassobjectattribute.hpp"
#include "serialization/serializationcontext.hpp"

#include <map>
#include <string>
#include <stdexcept>
#include <type_traits>

namespace blf
{
    template<typename Class, typename T, typename AttribLoc, typename AttribProc>
    struct NamedClassArgument
    {
        std::string name;
        BLF_TYPE type;
        typename AttribLoc::Store locationStore;
        typename AttribProc::Store procedureStore;
    };

    template<typename T>
    struct DefaultConstructor
    {
        static void* function()
        {
            return new T();
        }
    };

    template<typename T>
    struct DefaultDestructor
    {
        static void function(void* ptr)
        {
            delete ((T*)ptr);
        }
    };

    /*template<typename SpecifiedClass, typename Tg, typename Ts>
        ConstAccessorAttributeLocation<SpecifiedClass, Tg, Ts> loc(
            Tg (SpecifiedClass::*getter)() const, void (SpecifiedClass::*setter)(Ts)
        )
    {
        return {getter, setter};
    }

    template<typename SpecifiedClass, typename ParentClass, 
             typename Tg, typename Ts,
                typename = std::enable_if_t<std::is_base_of_v<ParentClass, SpecifiedClass>>>
        ConstAccessorAttributeLocation<SpecifiedClass, Tg, Ts> loc(
            Tg (ParentClass::*getter)() const, void (ParentClass::*setter)(Ts)
        )
    {
        return {getter, setter};
    }*/

    template<typename C, typename T, 
        typename AttributeProcedure = infer_attribute_procedure_t<T>,
        typename... Args>
    NamedClassArgument<C, T, MemberAttributeLocation<C,T>, AttributeProcedure> arg(
        const std::string& name, T C::* ptr, Args&&... args)
    {
        const BLF_TYPE type = infer_v<std::decay_t<T>>;

        return {name, type, ptr, 
            typename AttributeProcedure::Store(std::forward<Args>(args)...)
        };
    }

    template<typename C, typename T, 
        typename AttributeLocation,
        typename AttributeProcedure = infer_attribute_procedure_t<T>,
        typename... Args>
    NamedClassArgument<C, T, AttributeLocation, AttributeProcedure> arg(
        const std::string& name, const AttributeLocation& location, Args&&... args)
    {
        const BLF_TYPE type = infer_v<std::decay_t<T>>;

        return {name, type, location, 
            typename AttributeProcedure::Store(std::forward<Args>(args)...)
        };
    }

    template<typename C,
        typename Tg, typename Ts, typename T=std::decay_t<Ts>,
        typename AttributeProcedure = infer_attribute_procedure_t<T>,
        typename... Args>
    NamedClassArgument<C, T, ConstAccessorAttributeLocation<C,Tg, Ts>, AttributeProcedure> arg(
        const std::string& name, Tg (C::*getter)() const, void (C::*setter)(Ts), Args&&... args)
    {
        // TODO: throw legible  error when const on getter but not on setter.
        // TODO: throw legible error when class mismatch on function
        const BLF_TYPE type = infer_v<std::decay_t<T>>;

        return {name, type, 
            {getter, setter}, 
            typename AttributeProcedure::Store(std::forward<Args>(args)...)
        };
    }

    template<typename C, typename C2,
        typename Tg, typename Ts, typename T=std::decay_t<Ts>,
        typename AttributeProcedure = infer_attribute_procedure_t<T>,
        typename... Args>
    NamedClassArgument<C, T, ConstAccessorAttributeLocation<C,Tg, Ts>, AttributeProcedure> arg(
        const std::string& name, Tg (C2::*getter)() const, void (C2::*setter)(Ts), Args&&... args)
        {
            return arg<C, Tg, Ts, T, AttributeProcedure, Args...>(
                name, (Tg (C::*)() const)getter, 
                    (void (C::*)(Ts))setter, std::forward<Args>(args)...
            );
        }

    class LocalObjectTable
    {
        private:
            typedef std::pair<ObjectAttribute*, std::string> NamedAttributePair;

            std::vector<NamedAttributePair> m_attributeStore;
            std::vector<ObjectDefinition*> m_definitionStore;
            std::map<std::string, void*(*)()> m_constructors;
            std::map<std::string, void(*)(void*)> m_destructors;
            std::vector<std::pair<void*, void(*)(void*)>> m_destructionPairs;
            std::vector<void*> m_createdObjects;

            template<typename Class, typename T, typename A, typename P, typename ArgumentClass>
            inline void addArgumentToUnbakedList(const std::string& name,
                UnbakedList<ClassObjectAttribute<Class>*>& list, 
                const NamedClassArgument<ArgumentClass, T, A, P>& argument)
            {
                static_assert(std::is_same_v<Class, ArgumentClass>, "Argument type is mismatched with definition type. It is possible that this is due to accessor methods from a parent class being used. In that case the parent class will automatically be inferred as the argument type. In order to prevent this, please use loc(getter, setter) to wrap your getters and setters.");
                SpecializedClassObjectAttribute<Class, T, A, P>* specializedAttribute = new SpecializedClassObjectAttribute<Class, T, A, P>(
                    argument.name, argument.type, argument.locationStore, argument.procedureStore
                );

                m_attributeStore.push_back({specializedAttribute, name});

                list.add(specializedAttribute);
            }

            template<typename Class, typename T, typename... Args>
            void addArgumentsToUnbakedList(const std::string& name,
                UnbakedList<ClassObjectAttribute<Class>*>& list, 
                const T& argument, Args&&... args)
            {
                addArgumentToUnbakedList(name, list, argument);
                addArgumentsToUnbakedList(name, list, std::forward<Args>(args)...);
            }

            template<typename Class>
            void addArgumentsToUnbakedList(const std::string& name,
                UnbakedList<ClassObjectAttribute<Class>*>& list)
            {}

            template<typename Class, typename... Args>
            BakedList<ClassObjectAttribute<Class>*> createBakedAttributeList(
                const std::string& name, Args&&... args)
            {
                UnbakedList<ClassObjectAttribute<Class>*> unbakedList;
                
                addArgumentsToUnbakedList(name, unbakedList, std::forward<Args>(args)...);

                return unbakedList.bake();
            }

            ObjectDefinition* findDefinition(const std::string& name)
            {
                for( ObjectDefinition* definition : m_definitionStore )
                {
                    std::string definitionName = definition->getName();
                    if( definitionName == name )
                    {
                        return definition;
                    }
                }

                throw std::out_of_range("Definition not found in LocalTable.");

                return nullptr;
            }

        public:
            ~LocalObjectTable()
            {
                for( const NamedAttributePair& attrPair : m_attributeStore )
                {
                    delete attrPair.first;
                }
                for( ObjectDefinition* definitionPtr : m_definitionStore )
                {
                    delete definitionPtr;
                }
                for( std::pair<void*, void(*)(void*)> destructionPair : m_destructionPairs)
                {
                    //delete (char*)object;
                    destructionPair.second(destructionPair.first);
                }
            }

            template<typename Class, typename... Args>
            LocalObjectDefinition<Class>& define(const std::string& name, Args&&... args)
            {
                BakedList<ClassObjectAttribute<Class>*> classAttributes = createBakedAttributeList<Class, Args...>(name, std::forward<Args>(args)...);
                
                //LocalObjectDefinition<Class> objectDefinition(name, classAttributes);
                LocalObjectDefinition<Class>* objectDefinition = new LocalObjectDefinition<Class>(name, classAttributes);
                m_definitionStore.push_back(objectDefinition);

                m_constructors.insert({name, &DefaultConstructor<Class>::function});
                m_destructors.insert({name, &DefaultDestructor<Class>::function});

                return *objectDefinition;
            }

            template<typename Class>
            void* fromData(SerializationContext& ctx, const LocalObjectDefinition<Class>& definition, const char* data)
            {
                void* (*constructor)() = m_constructors.at(definition.getName());
                void (*destructor)(void*) = m_destructors.at(definition.getName());

                void* initd = constructor();

                m_createdObjects.push_back(initd);
                m_destructionPairs.push_back({initd, destructor});

                definition.deserialize(ctx, (char*)initd, data);

                return initd;
            }

            void* fromData(SerializationContext& ctx, const std::string& name, const char* data)
            {
                void* (*constructor)() = m_constructors.at(name);
                void (*destructor)(void*) = m_destructors.at(name);

                void* initd = constructor();

                m_createdObjects.push_back(initd);
                m_destructionPairs.push_back({initd, destructor});

                findDefinition(name)->deserialize(ctx, (char*)initd, data);

                return initd;
            }

            ObjectDefinition* getDefinition(const std::string& name) const
            {
                for( ObjectDefinition* definition : m_definitionStore)
                {
                    if( definition->getName() == name )
                    {
                        return definition;
                    }
                }

                throw std::out_of_range("Given definition name not found in blf::LocalObjectTable");
            }

            /**
             * Takes all object definitions and their attributes
             * and turns them into a normal ObjectTable.
             * This is useful for serialization of the ObjectTable.
             */
            ObjectTable flatten()
            {
                UnbakedObjectTable table;

                for(ObjectDefinition* definition : m_definitionStore)
                {
                    const std::string& name = definition->getName();
                    UnbakedList<ObjectAttribute> attributes;

                    for( const NamedAttributePair& pair : m_attributeStore )
                    {
                        if( pair.second == name )
                        {
                            ObjectAttribute* attribute = pair.first;
                            attributes.add(ObjectAttribute(
                                attribute->getName(), attribute->getType(),
                                attribute->getObject()
                            ));
                        }
                    }

                    ForeignObjectDefinition foreignDefinition(name, attributes.bake());

                    table.add(foreignDefinition);
                }

                return table.bake();
            }
    };
}