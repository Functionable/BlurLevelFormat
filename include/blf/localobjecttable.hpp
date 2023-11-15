#pragma once

#include "enums.hpp"
#include "object/attributeprocedure.hpp"
#include "objecttable.hpp"

#include "object/objectattribute.hpp"
#include "object/objectdefinition.hpp"
#include "object/attributelocation.hpp"
#include "object/localobjectdefinition.hpp"
#include "object/specializedclassobjectattribute.hpp"

#include <map>
#include <string>
#include <stdexcept>

namespace blf
{
    template<typename Class, typename T, typename AttribLoc, typename AttribProc>
    struct NamedClassArgument
    {
        std::string name;
        BLF_TYPE type;
        typename AttribLoc::Store locationStore;
        typename AttribProc::Store procedureStore;

        //const LocalObjectDefinition<T>* definition;

        //NamedClassArgument(std::string name,)
    };

    template<typename T>
    struct DefaultConstructor
    {
        static void* function()
        {
            return new T();
        }
    };

    /*template<typename Class, typename T, typename = std::enable_if_t<!is_class_v<T>>>
    NamedClassArgument<Class, T> arg(const std::string& name, 
        T Class::* ptr, BLF_TYPE type = infer_v<std::decay_t<T>>)
    {
        //static_assert(infer_v<std::decay<T>> != TYPE_OBJECT, "A local class definition must be passed as the third argument to blf::arg.");
        return {name, ptr, type, nullptr};
    }

    template<typename Class, typename T, typename = std::enable_if_t<is_class_v<T>>>
    NamedClassArgument<Class, T> arg(const std::string& name, 
        T Class::* ptr, const LocalObjectDefinition<T>& definition)
    {
        return {name, ptr, TYPE_OBJECT, &definition};
    }*/

    // OVERLOADS OF ARG FOR MEMBER POINTERS
    template<typename C, typename T, typename = std::enable_if_t<!is_class_v<T>>>
    NamedClassArgument<C, T, MemberAttributeLocation<C,T>, GenericAttributeProcedure<C, T>> arg(
        const std::string& name, T C::* ptr)
    {
        const BLF_TYPE type = infer_v<std::decay_t<T>>;

        return {name, type, ptr};
    }

    template<typename C, typename T, typename = std::enable_if_t<is_class_v<T>>>
    NamedClassArgument<C, T, MemberAttributeLocation<C, T>, ObjectAttributeProcedure<C, T>> arg(const std::string& name, 
        T C::* ptr, const LocalObjectDefinition<T>& definition)
    {
        return {name, TYPE_OBJECT, ptr, &definition};
    }

    // OVERLOADS OF ARG FOR ACCESSOR FUNCTIONS.
    template<typename C, typename T, typename = std::enable_if_t<!is_class_v<T>>>
    NamedClassArgument<C, T, ConstAccessorAttributeLocation<C,T>, GenericAttributeProcedure<C, T>> arg(
        const std::string& name, T (C::*getter)() const, void (C::*setter)(T) )
    {
        const BLF_TYPE type = infer_v<std::decay_t<T>>;

        return {name, type, {getter, setter}};
    }

        template<typename C, typename T, typename = std::enable_if_t<is_class_v<T>>>
    NamedClassArgument<C, T, ConstAccessorAttributeLocation<C, T>, ObjectAttributeProcedure<C, T>> arg(
        const std::string& name,  T (C::*getter)() const, void (C::*setter)(T), 
        const LocalObjectDefinition<T>& definition)
    {
        return {name, TYPE_OBJECT, {getter, setter}, &definition};
    }

    class LocalObjectTable
    {
        private:
            std::vector<ObjectAttribute*> m_attributeStore;
            std::vector<ObjectDefinition*> m_definitionStore;
            std::map<std::string, void*(*)()> m_constructors;
            std::vector<void*> m_createdObjects;

            template<typename Class, typename T, typename A, typename P>
            inline void addArgumentToUnbakedList(UnbakedList<ClassObjectAttribute<Class>*>& list, 
                const NamedClassArgument<Class, T, A, P>& argument)
            {
                SpecializedClassObjectAttribute<Class, T, A, P>* specializedAttribute = new SpecializedClassObjectAttribute<Class, T, A, P>(
                    argument.name, argument.type, argument.locationStore, argument.procedureStore
                );

                m_attributeStore.push_back(specializedAttribute);

                list.add(specializedAttribute);
            }

            template<typename Class, typename T, typename... Args>
            void addArgumentsToUnbakedList(UnbakedList<ClassObjectAttribute<Class>*>& list, 
                const T& argument, Args&&... args)
            {
                addArgumentToUnbakedList(list, argument);
                addArgumentsToUnbakedList(list, std::forward<Args>(args)...);
            }

            template<typename Class>
            void addArgumentsToUnbakedList(UnbakedList<ClassObjectAttribute<Class>*>& list)
            {}

            template<typename Class, typename... Args>
            BakedList<ClassObjectAttribute<Class>*> createBakedAttributeList(
                Args&&... args)
            {
                UnbakedList<ClassObjectAttribute<Class>*> unbakedList;
                
                addArgumentsToUnbakedList(unbakedList, std::forward<Args>(args)...);

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
                for( ObjectAttribute* attributePtr : m_attributeStore )
                {
                    delete attributePtr;
                }
                for( ObjectDefinition* definitionPtr : m_definitionStore )
                {
                    delete definitionPtr;
                }
                for( void* object : m_createdObjects )
                {
                    delete (char*)object;
                }
            }

            template<typename Class, typename... Args>
            LocalObjectDefinition<Class>& define(const std::string& name, Args&&... args)
            {
                BakedList<ClassObjectAttribute<Class>*> classAttributes = createBakedAttributeList<Class, Args...>(std::forward<Args>(args)...);
                
                //LocalObjectDefinition<Class> objectDefinition(name, classAttributes);
                LocalObjectDefinition<Class>* objectDefinition = new LocalObjectDefinition<Class>(name, classAttributes);
                m_definitionStore.push_back(objectDefinition);

                m_constructors.insert({name, &DefaultConstructor<Class>::function});

                return *objectDefinition;
            }

            template<typename Class>
            void* fromData(const LocalObjectDefinition<Class>& definition, const char* data)
            {
                void* (*constructor)() = m_constructors.at(definition.getName());

                void* initd = constructor();

                m_createdObjects.push_back(initd);

                definition.deserialize(initd, data);

                return initd;
            }

            void* fromData(const std::string& name, const char* data)
            {
                void* (*constructor)() = m_constructors.at(name);

                void* initd = constructor();

                m_createdObjects.push_back(initd);

                findDefinition(name)->deserialize(initd, data);

                return initd;
            }
    };
}