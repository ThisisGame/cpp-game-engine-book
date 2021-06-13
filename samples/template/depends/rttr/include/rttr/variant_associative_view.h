/************************************************************************************
*                                                                                   *
*   Copyright (c) 2014 - 2018 Axel Menzel <info@rttr.org>                           *
*                                                                                   *
*   This file is part of RTTR (Run Time Type Reflection)                            *
*   License: MIT License                                                            *
*                                                                                   *
*   Permission is hereby granted, free of charge, to any person obtaining           *
*   a copy of this software and associated documentation files (the "Software"),    *
*   to deal in the Software without restriction, including without limitation       *
*   the rights to use, copy, modify, merge, publish, distribute, sublicense,        *
*   and/or sell copies of the Software, and to permit persons to whom the           *
*   Software is furnished to do so, subject to the following conditions:            *
*                                                                                   *
*   The above copyright notice and this permission notice shall be included in      *
*   all copies or substantial portions of the Software.                             *
*                                                                                   *
*   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR      *
*   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,        *
*   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE     *
*   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER          *
*   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,   *
*   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE   *
*   SOFTWARE.                                                                       *
*                                                                                   *
*************************************************************************************/

#ifndef RTTR_VARIANT_ASSOCIATIVE_VIEW_H_
#define RTTR_VARIANT_ASSOCIATIVE_VIEW_H_

#include "rttr/detail/base/core_prerequisites.h"
#include "rttr/detail/misc/misc_type_traits.h"
#include "rttr/variant.h"
#include "rttr/detail/variant_associative_view/variant_associative_view_private.h"

#include <cstddef>
#include <vector>
#include <memory>

namespace rttr
{
    class type;
    class instance;
    class argument;

/*!
 * The \ref variant_associative_view describes a class that refers to an
 * <a target="_blank" href=https://en.wikipedia.org/wiki/Associative_containers>associative container</a> (e.g: `std::map`)
 * inside a \ref variant.
 * With an instance of that class you can set/get values of such container,
 * without having access to the type declaration of the type or it's elements.
 *
 * A \ref variant_associative_view can be created directly from a \ref variant with its member function \ref variant::create_associative_view() "create_associative_view()".
 * \remark The instance of an variant_associative_view is always valid as long as the referenced \ref variant is valid, otherwise accessing a variant_associative_view
 *         is undefined behaviour.
 *
 * Meta Information
 * ----------------
 *
 * RTTR recognize whether a type is an associative container or not with the help of the \ref associative_container_mapper class template.
 * This call can access different container types via one common interface.
 * At the moment there exist specializations for following types:
 * `std::set<Key>`, `std::map<Key, T>`, `std::multiset<Key>`, `std::multimap<Key, T>`, `std::unordered_set<Key>`,
 * `std::unordered_map<Key, T>`, `std::unordered_multiset<Key>` and `std::unordered_multimap<Key, T>`.
 *
 * Copying and Assignment
 * ----------------------
 * A \ref variant_associative_view object can be copied and assigned,
 * however each copy will reference the data of same underlying \ref variant value.
 *
 * Typical Usage
 * ----------------------
 *
 * \code{.cpp}
 *  std::map<int, std::string> my_map = { { 1, "one" }, { 2, "two" }, { 3, "three" } };
 *  variant var = my_map;
 *  if (var.is_associative_container())
 *  {
 *      variant_associative_view view = var.create_associative_view();
 *      std::cout << view.get_size() << std::endl;      // prints: '3'
 *      for (const auto& item : view)
 *      {
 *          // remark that the key and value are stored inside a 'std::reference_wrapper'
 *          std::cout << "Key: " << item.first.extract_wrapped_value().to_string() << " ";
 *          std::cout << "Value: " << item.second.extract_wrapped_value().to_string() << std::endl;
 *      }
 *  }
 * \endcode
 *
 * \see variant
 */
class RTTR_API variant_associative_view
{
    public:
        class const_iterator;

        /*!
         * \brief Constructs an invalid variant_associative_view object.
         *
         * \see is_valid()
         */
        variant_associative_view();

        /*!
         * \brief Constructs a copy of the given variant_associative_view \p other.
         */
        variant_associative_view(const variant_associative_view& other);

        /*!
         * \brief Destroys the variant_associative_view.
         *
         * \remark The underlying data is not destroyed.
         */
        ~variant_associative_view() RTTR_NOEXCEPT;

        /*!
         * \brief Assigns the value of the \a other variant_associative_view to this variant_associative_view.
         *
         * \return A reference to the variant_associative_view with the new data.
         */
        variant_associative_view& operator=(const variant_associative_view& other) RTTR_NOEXCEPT;

        /*!
         * \brief Returns true if this variant_associative_view is valid, that means the object is holding some data.
         *        When the variant_associative_view doesn't hold any data it will return false.
         *
         * \return True if this \ref variant_associative_view is valid, otherwise false.
         */
        bool is_valid() const RTTR_NOEXCEPT;

        /*!
         * \brief Convenience function to check if this \ref variant_associative_view is valid or not.
         *
         * \see is_valid()
         *
         * \return True if this \ref variant_associative_view is valid, otherwise false.
         */
        explicit operator bool() const RTTR_NOEXCEPT;

        /*!
         * \brief Swaps this variant_associative_view with the \a other variant_associative_view.
         */
        void swap(variant_associative_view& other) RTTR_NOEXCEPT;


        /*!
         * \brief Returns the \ref type object of this associative container.
         *
         * \remark When the view is not valid, this function will return an invalid type object.
         *
         * \return \ref type "Type" of the associative container.
         */
        type get_type() const RTTR_NOEXCEPT;

        /*!
         * \brief Returns the \ref type from the key of this associative container.
         *
         * \remark When the view is not valid, this function will return an invalid type object.
         *
         * \return \ref type "Type" from the key of the associative container.
         */
        type get_key_type() const RTTR_NOEXCEPT;

        /*!
         * \brief Returns the \ref type object from the value of this associative container.
         *
         * \remark When the view is not valid, this function will return an invalid type object.
         *
         * \return \ref type "Type" from the value of the associative container.
         */
        type get_value_type() const RTTR_NOEXCEPT;

        /*!
         * \brief Returns `true`, when this associative container stores only keys.
         *        When also value are stored, it will return `false`.
         *
         * For example an `std::set` has no values, it's a key-only associative container. Then this method returns `true`.
         * An `std::map<K,V>` on the other hand contains keys and values. So this method will then return `false`.
         * A simple convenience method instead of calling: `get_value_type().is_valid() == false;`
         *
         * \return `true`, when this associative container stores only keys; otherwise `false`.
         */
        bool is_key_only_type() const RTTR_NOEXCEPT;

        /*!
         * \brief Checks if the container has no elements.
         *
         * \return `true` if container is empty, otherwise `false`.
         */
        bool is_empty() const RTTR_NOEXCEPT;

        /*!
         * \brief Returns the number of elements in the associative container.
         *
         * \return The number of elements in the associative container.
         */
        std::size_t get_size() const RTTR_NOEXCEPT;

        /*!
         * \brief Insert a key into the container.
         *
         * \return A pair consisting of an iterator to the inserted element (or to the element that prevented the insertion)
         *         and a bool denoting whether the insertion took place.
         */
        std::pair<const_iterator, bool> insert(argument key);

        /*!
         * \brief Insert a key-value pair into the container.
         *
         * \return A pair consisting of an iterator to the inserted element (or to the element that prevented the insertion)
         *         and a bool denoting whether the insertion took place.
         */
        std::pair<const_iterator, bool> insert(argument key, argument value);

        /*!
         * \brief Finds an element with specific key \p key .
         *
         * \return The element with key equivalent to \p key. If no element is found an invalid iterator is returned.
         */
        const_iterator find(argument key);

        /*!
         * \brief Removes the element (if one exists) with the key equivalent to \p key.
         *
         * \return The number of elements removed.
         */
        std::size_t erase(argument key);

        /*!
         * \brief Removes all elements from the container.
         *
         * \remark Invalidates all references, pointers, or iterators referring to contained elements.
         */
        void clear();

        /*!
         * \brief Returns a range containing all elements with the given \p key in the container.
         *
         * Example code:
         * \code{.cpp}
         *  auto multimap = std::multimap<int, std::string>{ { 1, "A" }, { 2, "B" },  { 2, "C"}, { 2, "D" },
         *                                                   { 3, "E" }, { 3, "F" } };
         *
         *  variant var = multimap;
         *
         *  auto view = var.create_associative_view();
         *  for (int i = 1; i <= 3; ++i)
         *  {
         *      std::cout << i << " =>";
         *      auto range = view.equal_range(i);
         *      for (auto itr = range.first; itr != range.second; ++itr)
         *      {
         *          std::cout << " " << itr.value().extract_wrapped_value().to_string();
         *      }
         *      std::cout << std::endl;
         *  }
         *\endcode
         *
         * Output:
         * \code
         *  1 => A
         *  2 => B C D
         *  3 => E, F
         * \endcode
         *
         * \return std::pair containing a pair of iterators defining the wanted range:
         *         the first pointing to the first element that is not less than \p key and
         *         the second pointing to the first element greater than \p key.
         */
        std::pair<const_iterator, const_iterator> equal_range(argument key);

        /*!
         * \brief Returns an iterator to the first element of the container.
         *
         * \see end()
         *
         * \return Iterator to the first element .
         */
        const_iterator begin() const;

        /*!
         * \brief Returns an iterator to the element following the last element of the container.
         *
         * \see begin()
         *
         * \return Iterator to the element following the last element.
         */
        const_iterator end() const;

        /*!
         * The \ref variant_associative_view::const_iterator allows iteration over an associative container in a variant.
         * An instance can only be created by an variant_associative_view.
         *
         * Typical Usage
         * ----------------------
         *
         * \code{.cpp}
         *  std::map<int, std::string> my_map = { { 1, "one" }, { 2, "two" }, { 3, "three" } };
         *  variant var = my_map;
         *  if (var.is_associative_container())
         *  {
         *      variant_associative_view view = var.create_associative_view();
         *      std::cout << view.get_size() << std::endl;      // prints: '3'
         *      for (const auto& item : view)
         *      {
         *          // remark that the key and value are stored inside a 'std::reference_wrapper'
         *          std::cout << "Key: " << item.first.extract_wrapped_value().to_string() << " ";
         *          std::cout << "Value: " << item.second.extract_wrapped_value().to_string() << std::endl;
         *      }
         *  }
         * \endcode
         *
         * \remark The iterator is valid as long as the variant_associative_view and it corresponding variant is valid and not modified.
         */
        class RTTR_API const_iterator
        {
            public:
                using self_type = const_iterator;
                using value_type = variant;

                /*!
                 * \brief Destroys the variant_associative_view::const_iterator
                 */
                ~const_iterator();

                /*!
                 * \brief Creates a copy of \p other
                 */
                const_iterator(const const_iterator& other);

                 /*!
                 * \brief Assigns \p other to `this`.
                 */
                const_iterator& operator=(const_iterator other);

                /*!
                 * Returns the underlying key and value stored in a `std::pair<key, value>`.
                 * The actual data in the variant is stored inside a `std::reference_wrapper<T>`
                 *
                 * \see variant::extract_wrapped_value(), variant::get_wrapped_value<T>()
                 */
                const std::pair<variant, variant> operator*() const;

                /*!
                 * \brief Returns the current key, stored inside a `std::reference_wrapper<T>`
                 *        and copied to a variant.
                 *
                 * \see variant::extract_wrapped_value(), variant::get_wrapped_value<T>()
                 */
                const variant get_key() const;

                /*!
                 * \brief Returns the current value, stored inside a `std::reference_wrapper<T>`
                 *        and copied to a variant.
                 *
                 * \see variant::extract_wrapped_value(), variant::get_wrapped_value<T>()
                 */
                const variant get_value() const;

                /*!
                 * \brief Pre-increment operator advances the iterator to the next item
                 *        in the container and returns an iterator to the new current item.
                 *
                 * \remark Calling this function on and iterator with value variant_associative_view::end()
                 *         leads to undefined behaviour.
                 */
                const_iterator &operator++();

                /*!
                 * \brief Post-increment operator advances the iterator to the next item
                 *        in the container and returns an iterator to the previously current item.
                 */
                const_iterator operator++(int);

                /*!
                 * \brief Pre-decrement operator makes the preceding item current and returns
                 *        an iterator to the new current item.
                 *
                 * \remark Calling this function on and iterator with value variant_associative_view::begin()
                 *         leads to undefined behaviour.
                 */
                const_iterator &operator--();

                /*!
                 * \brief Post-decrement operator makes the preceding item current
                 *        and returns an iterator to the previously current item.
                 */
                const_iterator operator--(int);

                /*!
                 * \brief Advances the iterator by i items.
                 */
                const_iterator &operator+=(int i);

                /*!
                 * \brief Returns an iterator to the item at i positions backward from this iterator.
                 */
                const_iterator &operator-=(int i);

                /*!
                 * \brief Returns an iterator to the item at i positions forward from this iterator.
                 */
                const_iterator operator+(int i) const;

                /*!
                 * \brief Returns an iterator to the item at i positions backward from this iterator.
                 */
                const_iterator operator-(int i) const;

                /*!
                 * \brief Returns `true` if \p other points to the same item
                 *        as this iterator; otherwise returns false.
                 *
                 * \see \ref const_iterator::operator!= "operator!="
                 */
                bool operator==(const const_iterator& other) const;

                /*!
                 * \brief Returns true if \p other points to a different item
                 *        than this iterator; otherwise returns false.
                 *
                 * \see \ref operator== "operator=="
                 */
                bool operator!=(const const_iterator& other) const;

            private:
                const_iterator(const detail::variant_associative_view_private* view) RTTR_NOEXCEPT;
                void swap(const_iterator& other);

                friend class variant_associative_view;

                const detail::variant_associative_view_private* m_view;
                detail::iterator_data m_itr;
        };
    private:
        friend class variant;
        friend class argument;

        detail::variant_associative_view_private m_view;
};

} // end namespace rttr

#endif // RTTR_VARIANT_ASSOCIATIVE_VIEW_H_
