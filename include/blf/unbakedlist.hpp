#pragma once

#include <vector>
#include <initializer_list>

namespace blf
{
    // TODO: add rule of 5 everywhere.
    template<typename T>
    class UnbakedList
    {
        private:
            std::vector<T> m_vector;

        public:
            typedef typename std::vector<T>::iterator Iterator;
            typedef typename std::vector<T>::const_iterator ConstIterator;

            UnbakedList()
                : m_vector()
            {}

            UnbakedList(const UnbakedList& list)
                : UnbakedList(list.m_vector)
            {}

            UnbakedList(const std::vector<T>& vector)
                : m_vector(vector)
            {}

            UnbakedList(const std::initializer_list<T>& initializerList)
                : m_vector(initializerList)
            {}

            virtual ~UnbakedList() {}

            class BakedList
            {
                private:
                    size_t m_size;
                    T* m_list;

                    BakedList()
                        : m_size(0),
                          m_list(nullptr)
                    {}

                public:
                    static BakedList empty() { return BakedList(); }

                    BakedList(const UnbakedList<T>& unbakedList)
                        : m_size(unbakedList.size())
                    {
                        m_list = new T[unbakedList.size()];

                        T* m_head = m_list;

                        for(const T& obj : unbakedList)
                        {
                            m_head[0] = obj;
                            m_head++;
                        }
                    }

                    BakedList(const BakedList& list)
                        : m_size(list.m_size)
                    {
                        m_list = new T[list.m_size];

                        T* m_head = m_list;

                        for(const T& obj : list)
                        {
                            m_head[0] = obj;
                            m_head++;
                        }
                    }

                    virtual ~BakedList()
                    {
                        delete[] m_list;
                    }

                    const T& get(size_t index) const { return m_list[index]; }

                    size_t size() const { return m_size; }
                    T* begin() const { return m_list; }
                    T* end() const { return m_list + m_size; }
            };

            const T& get(size_t index) const { return m_vector[index]; }
            void add(const T& value) { m_vector.push_back(value); }

            BakedList bake() { return BakedList(*this); }

            template<typename... Args>
            void emplace(Args&&... args) { m_vector.emplace_back(args...); }

            size_t size() const { return m_vector.size(); }
            ConstIterator begin() const { return m_vector.begin(); }
            ConstIterator end() const { return m_vector.end(); }
    };
}