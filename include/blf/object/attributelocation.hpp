#pragma once

#include <string>
#include <type_traits>

namespace blf
{
    /**
     * What is an attribute location?
     * Simply, it's an additional template class which helps blf::SpecializedClassObjectAttribute to find
     * a given attribute within an object. It is a helper class with static methods to help access the data
     * for a given attribute.

     * What's the need for attribute locations?
     * It perhaps may be possible that a given object attribute cannot be accessed with your standard member pointer.
     * This may be because the member may be private. However, there may still be forms of access available to us.
     * Namely, thanks to attribute locations we are able to leverage the getters and setters of a class in order to
     * supply/extract the data needed for (de)serialization.

     * What kinds of attribute locations exist?
     * Currently there are three types:
     * 1. MemberAttributeLocation<T>
     * - This one relies on a member pointer to a public member to allow the attribute to pull from it/push data to it.
     * 2. AccessorAttributeLocation<T>
     * - This one relies on two methods. Namely, a getter and a setter in order to pull/push data.
     * 3. ConstAccessorAttributeLocation<T>
     * - This method is used when the getter is a const method.
     */
    template<typename Class, typename T>
    struct MemberAttributeLocation
    {
        inline static constexpr bool forceCopy = false;

        struct Store
        {
            T Class::* memberPointer;

            inline T get(Class* classPtr) const
            {
                return classPtr->*memberPointer;
            };

            inline T* ptr(Class* classPtr) const
            {
                return &(classPtr->*memberPointer);
            }

            inline const T* ptr(const Class* classPtr) const
            {
                return &(classPtr->*memberPointer);
            }

            inline void set(Class* classPtr, T value) const
            {
                classPtr->*memberPointer = value;
            }
        };
    };
    
    template<typename Class, typename T>
    struct AccessorAttributeLocation
    {
        inline static constexpr bool forceCopy = true;

        struct Store
        {
            T(Class::*getterPointer)();
            void(Class::*setterPointer)(T);

            inline constexpr T get(const Class* classPtr) const
            {
                return classPtr->*getterPointer();
            }

            inline void set(Class* classPtr, T value) const
            {
                classPtr->*setterPointer(value);
            }
        };
    };

    template<typename Class, typename Tg, typename Ts = Tg>
    struct ConstAccessorAttributeLocation
    {
        inline static constexpr bool forceCopy = true;

        struct Store
        {
            Tg(Class::*getterPointer)() const;
            void(Class::*setterPointer)(Ts);

            inline constexpr Tg get(const Class* classPtr) const
            {
                return (classPtr->*getterPointer)();
            }

            inline void set(Class* classPtr, Ts value) const
            {
                (classPtr->*setterPointer)(value);
            }
        };
    };

    template<typename T>
    struct is_attribute_location : std::false_type {};

    template<typename Class, typename T>
    struct is_attribute_location<MemberAttributeLocation<Class, T>> : std::true_type {};

    template<typename Class, typename T>
    struct is_attribute_location<AccessorAttributeLocation<Class, T>> : std::true_type {};

    template<typename Class, typename T>
    struct is_attribute_location<ConstAccessorAttributeLocation<Class, T>> : std::true_type {};

    template<typename T>
    constexpr inline bool is_attribute_location_v = is_attribute_location<T>::value;
}