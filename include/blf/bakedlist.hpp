#pragma once

#include "unbakedlist.hpp"

namespace blf
{
    template<typename T>
    class BakedList : public UnbakedList<T>::BakedList 
    {
        public:
            BakedList(const UnbakedList<T>& unbakedList)
                : UnbakedList<T>::BakedList(unbakedList)
            {}

            BakedList(const typename UnbakedList<T>::BakedList& unbakedList)
                : UnbakedList<T>::BakedList(unbakedList)
            {}

            BakedList(const BakedList& list)
                : UnbakedList<T>::BakedList(list)
            {}

            ~BakedList() = default;
    };

    //template<typename T>
    //using BakedList = typename UnbakedList<T>::BakedList;
}