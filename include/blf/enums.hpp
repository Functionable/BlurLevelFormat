#pragma once

#include "string.hpp"
#include "bakedlist.hpp"

#include <vector>
#include <string>
#include <cstdint>
#include <type_traits>

namespace blf
{
    const int8_t DYNAMIC_TYPE = -1;
    enum BLF_TYPE : uint8_t
	{
		TYPE_NULL, // THIS TYPE SHOULD NEVER BE USED.
		TYPE_UNKNOWN, // TODO: ERROR IF ENCOUNTERED.
        TYPE_BYTE,
        TYPE_SHORT,
		TYPE_INT, /** Represents an int32_t */
		TYPE_LONG, /**Represents an int64_t*/
		TYPE_FLOAT,
		TYPE_DOUBLE,
		TYPE_STRING,
		TYPE_OBJECT,
		TYPE_COMPAT_OBJECTREFERENCE, // used to mean just object. now means object pointer or reference.

		// Aliases for other types!
		TYPE_BOOL = TYPE_BYTE,

		TYPE_LIST
	};

	template<int size>
	struct blf_integral_type
	{
		static constexpr BLF_TYPE type = TYPE_UNKNOWN;
	};

	template<>
	struct blf_integral_type<1>
	{
		static constexpr BLF_TYPE type = TYPE_BYTE;
	};

	template<>
	struct blf_integral_type<2>
	{
		static constexpr BLF_TYPE type = TYPE_SHORT;
	};

	template<>
	struct blf_integral_type<4>
	{
		static constexpr BLF_TYPE type = TYPE_INT;
	};

	template<>
	struct blf_integral_type<8>
	{
		static constexpr BLF_TYPE type = TYPE_LONG;
	};

	template<int size>
	struct blf_floating_point_type
	{
		static constexpr BLF_TYPE type = TYPE_UNKNOWN;
	};

	template<>
	struct blf_floating_point_type<4>
	{
		static constexpr BLF_TYPE type = TYPE_FLOAT;
	};

	template<>
	struct blf_floating_point_type<8>
	{
		static constexpr BLF_TYPE type = TYPE_DOUBLE;
	};

	template<typename T>
	struct list_traits 
	{
		typedef void not_a_list;
	};

	template<typename T>
	struct list_traits<std::vector<T>>
	{
		//using item_type = T;
		typedef T item_type;
	};

	template<typename T>
	struct list_traits<BakedList<T>>
	{
		//using item_type = T;
		typedef T item_type;
	};

	template<typename T>
	struct is_string : public std::false_type {};

	template<>
	struct is_string<std::string> : public std::true_type {};

	template<>
	struct is_string<String> : public std::true_type {};

	template<typename T, typename Enable = void>
	struct is_list : std::false_type {};

	template<typename T>
	struct is_list<T, std::void_t<typename list_traits<T>::item_type>> : std::true_type {};

	template<typename T>
	inline constexpr bool is_list_v = is_list<T>::value;

	template<typename T>
	inline constexpr bool is_string_v = is_string<T>::value;

	template<typename T>
	inline constexpr bool is_class_v = std::is_class_v<T> && !is_string_v<T> && !is_list_v<T>;

	template<typename T>
	inline constexpr bool is_indirect_type_v = std::is_pointer_v<T> || std::is_reference_v<T>;

	template<typename T>
	inline constexpr bool is_indirect_class_v = is_indirect_type_v<T> && is_class_v<std::decay_t<T>>;

	template<typename T, typename Enable = void>
	struct infer
	{
		static constexpr BLF_TYPE type = TYPE_UNKNOWN;
	};

	template<typename T>
	struct infer<T, std::enable_if_t<is_string_v<T>>>
	{
		static constexpr BLF_TYPE type = TYPE_STRING;
	};

	template<typename T>
	struct infer<T, std::enable_if_t<is_class_v<T>>>
	{
		static constexpr BLF_TYPE type = TYPE_OBJECT;
	};

	template<typename T>
	struct infer<T, std::enable_if_t<is_list_v<T>>>
	{
		static constexpr BLF_TYPE type = TYPE_LIST;
	};

	template<typename T>
	struct infer<T, 
		std::enable_if_t<std::is_integral_v<T>||std::is_enum_v<T>>>
	{
		static constexpr BLF_TYPE type = blf_integral_type<sizeof(T)>::type;
	};

	template<typename T>
	struct infer<T, std::enable_if_t<std::is_floating_point_v<T>>>
	{
		static constexpr BLF_TYPE type = blf_floating_point_type<sizeof(T)>::type;
	};

	template <typename T>
	inline constexpr BLF_TYPE infer_v = infer<T>::type;

    enum BLF_TYPECATEGORY
    {
        CATEGORY_NONE,
        CATEGORY_OBJECTREFERENCE,
        CATEGORY_NUMERICFLOAT,
        CATEGORY_NUMERICINTEGER,
        CATEGORY_STRING,
    };
    
	// TODO: replace with new flags?
	enum BLF_FLAG
	{
		FLAG_EMPTY		    = 0, /** No flag */
		FLAG_VERBOSE 		= 1, /** Should it print output logs */
		FLAG_TYPESTRICT 	= 2, /** Aborts loading as soon as the file has conflicting object definition types. */
		FLAG_COMPRESSED		= 4, /** GZIP Compressed */
	};

    int8_t typeSize(const BLF_TYPE attributeType);
}