#pragma once

#include "unbakedlist.hpp"

namespace blf
{
    template<typename T>
    using BakedList = typename UnbakedList<T>::BakedList;
}