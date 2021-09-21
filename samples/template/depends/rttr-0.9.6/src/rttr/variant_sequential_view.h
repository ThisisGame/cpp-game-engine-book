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

#ifndef RTTR_VARIANT_SEQUENTIAL_VIEW_H_
#define RTTR_VARIANT_SEQUENTIAL_VIEW_H_

#include "rttr/detail/base/core_prerequisites.h"
#include "rttr/detail/misc/misc_type_traits.h"
#include "rttr/variant.h"
#include "rttr/detail/variant_sequential_view/variant_sequential_view_private.h"

#include <cstddef>
#include <vector>
#include <memory>

namespace rttr
{
    class type;
    class instance;
    class argument;

/*!
 * The \ref variant_sequential_view describes a class that refers to an
 * <a target="_blank" href=https://en.wikipedia.org/wiki/Sequence_container_(C%2B%2B)>sequence container</a> (e.g: `std::vector`)
 * inside a \ref variant.
 * With an instance of that class you can set/get values of such container,
 * without having access to the type declaration of the type or it's elements.
 *
 * A \ref variant_sequential_view can be created directly from a \ref variant with its member function \ref variant::create_sequential_view() "create_sequential_view()".
 * \remark The instance of an variant_sequential_view is always valid as long as the referenced \ref variant is valid, otherwise accessing a variant_sequential_view
 *         is undefined behaviour.
 *
 * Meta Information
 * ----------------
 *
 * RTTR recognize whether a type is an sequential container or not with the help of the \ref sequential_container_mapper class template.
 * This call can access different container types via one common interface.
 * At the moment there exist specializations for following types:
 * `std::vector<T>`, `std::array<T, std::size_t>`, `std::list<T>`, `std::deque<T>`, `std::initializer_list<T>` and raw arrays
 *
 * Copying and Assignment
 * ----------------------
 * A \ref variant_sequential_view object can be copied and assigned,
 * however each copy will reference the data of same underlying \ref variant value.
 *
 * Typical Usage
 * ----------------------
 *
 * \code{.cpp}
 *  std::vector<int> my_vec = { 1, 2, 3, 4, 5};
 *  variant var = my_vec; // copies data into variant
 *  if (var.is_sequential_container())
 *  {
 *      variant_sequential_view view = var.create_sequential_view();  // performs no copy of the underlying vector
 *      std::cout << view.get_size() << std::endl;  // prints: '5'
 *      for (const auto& item : view)
 *      {
 *          // remark that the value is stored inside a 'std::reference_wrapper', however there is an automatic conversion for wrapper classes implemented.
 *          std::cout << "data: " << item.to_string() << " ";
 *      }
 *  }
 * \endcode
 *
 * Output:
 * \code
 *  1 2 3 4 5
 * \endcode
 *
 * \see variant, type::is_sequential_container()
 */
class RTTR_API variant_sequential_view
{
    public:
        class const_iterator;

        /*!
         * \brief Constructs an invalid variant_sequential_view object.
         *
         * \see is_valid()
         */
        variant_sequential_view();

        /*!
         * \brief Constructs a copy of the given variant_sequential_view \p other.
         */
        variant_sequential_view(const variant_sequential_view& other);

        /*!
         * \brief Destroys the variant_sequential_view.
         *
         * \remark The underlying data is not destroyed.
         */
        ~variant_sequential_view() RTTR_NOEXCEPT;

        /*!
         * \brief Assigns the value of the \a other variant_sequential_view to this variant_sequential_view.
         *
         * \return A reference to the variant_sequential_view with the new data.
         */
        variant_sequential_view& operator=(const variant_sequential_view& other) RTTR_NOEXCEPT;

        /*!
         * \brief Returns true if this variant_sequential_view is valid, that means the object is holding some data.
         *        When the variant_sequential_view doesn't hold any data it will return false.
         *
         * \return True if this \ref variant_sequential_view is valid, otherwise false.
         */
        bool is_valid() const RTTR_NOEXCEPT;

        /*!
         * \brief Convenience function to check if this \ref variant_sequential_view is valid or not.
         *
         * \see is_valid()
         *
         * \return True if this \ref variant_sequential_view is valid, otherwise false.
         */
        explicit operator bool() const RTTR_NOEXCEPT;

        /*!
         * \brief Swaps this variant_sequential_view with the \a other variant_sequential_view.
         */
        void swap(variant_sequential_view& other) RTTR_NOEXCEPT;


        /*!
         * \brief Returns the \ref type object of this sequential container.
         *
         * \remark When the view is not valid, this function will return an invalid type object.
         *
         * \return \ref type "Type" of the sequential container.
         */
        type get_type() const RTTR_NOEXCEPT;

        /*!
         * \brief Returns the \ref type object from the value of this sequential container.
         *
         * \remark When the view is not valid, this function will return an invalid type object.
         *
         * \return \ref type "Type" from the value of the sequential container.
         */
        type get_value_type() const RTTR_NOEXCEPT;

        /*!
         * \brief Checks if the container has no elements.
         *
         * \return `true` if container is empty, otherwise `false`.
         */
        bool is_empty() const RTTR_NOEXCEPT;

        /*!
         * \brief Returns `true` if this sequential view is dynamic, otherwise `false`.
         *        When an sequential view is dynamic, it is possible to change its \ref set_size "size",
         *        \ref clear its content or \ref insert and \ref erase values from it.
         *
         * \see set_size(), insert(), erase(), clear()
         *
         * \return A boolean flag which indicates whether this sequential container is dynamic or not.
         */
        bool is_dynamic() const RTTR_NOEXCEPT;

        /*!
         * \brief Gets the rank (number of dimensions) of this sequential container.
         *
         * Take a look at following return values:
         *  - \p `int[4]` => `1`
         *  - \p `int[4][4]` => `2`
         *  - \p `int[4][4][4]` => `3`
         *  - ...
         *
         * \return Returns the rank of the sequential container.
         */
        std::size_t get_rank() const RTTR_NOEXCEPT;

        /*!
         * \brief Gets the type of the given rank index.
         *
         * Take a look at following return value for an array of type: `int[2][10]`
         *  - \p `get_rank_type(0)` => `int[2][10]`
         *  - \p `get_rank_type(1)` => `int[10]`
         *  - \p `get_rank_type(2)` => `int`
         *  - \p `get_rank_type(3)` => **INVALID**
         *
         * \return The rank type at the given dimension \p index.
         */
        type get_rank_type(std::size_t index) const RTTR_NOEXCEPT;

        /*!
         * \brief Returns the number of elements in the sequential container.
         *
         * \return The number of elements in the sequential container.
         */
        std::size_t get_size() const RTTR_NOEXCEPT;

        /*!
         * \brief Sets the size of the sequential container.
         *
         * \return `true`, when the size of the container could be changed, otherwise `false`.
         *
         * \see is_dynamic()
         */
        bool set_size(std::size_t size) const RTTR_NOEXCEPT;

        /*!
         * \brief Insert a value into the container.
         *
         * \return An iterator to the inserted element, otherwise an invalid iterator, when the insertion was not possible.
         */
        const_iterator insert(const const_iterator& pos, argument value);

        /*!
         * \brief Removes the element (if one exists) at the position \p pos.
         *
         * \return Iterator following the last removed element.
         */
        const_iterator erase(const const_iterator& pos);

        /*!
         * \brief Removes all elements from the container.
         *
         * \remark Invalidates all references, pointers, or iterators referring to contained elements.
         */
        void clear();

        /*!
         * \brief Set the content of the the argument \p arg at the specified index \p index
         *        into the underlying sequential container.
         *
         * \return `true` if the value could be set, otherwise `false`.
         */
        bool set_value(std::size_t index, argument arg);

        /*!
         * \brief Returns the current value at index \p index.
         *
         * \return The data at the specified index \p index, wrapped inside a `std::reference_wrapper<T>`.
         *
         * \remark Make sure the index is in a valid range, otherwise undefined behaviour may occurr.
         *         For MSVC 2013 and raw arrays, the address of the data is returned (because of a bug).
         *
         * \see get_size()
         */
        variant get_value(std::size_t index) const;

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
         * The \ref variant_sequential_view::const_iterator allows iteration over an sequential container in a variant.
         * An instance can only be created by an variant_sequential_view.
         *
         * Typical Usage
         * ----------------------
         *
         * \code{.cpp}
         *  std::vector<int> my_vec = { 1, 2, 3, 4, 5};
         *  variant var = my_vec; // copies data into variant
         *  if (var.is_sequential_container())
         *  {
         *      variant_sequential_view view = var.create_sequential_view();  // performs no copy of the underlying vector
         *      std::cout << view.get_size() << std::endl;  // prints: '5'
         *      for (const auto& item : view)
         *      {
         *          // remark that the value is stored inside a 'std::reference_wrapper', however there is an automatic conversion for wrapper classes implemented.
         *          std::cout << "data: " << item.to_string() << " ";
         *      }
         *  }
         * \endcode
         *
         * \remark The iterator is valid as long as the variant_sequential_view and it corresponding variant is valid and not modified.
         */
        class RTTR_API const_iterator
        {
            public:
                using self_type = const_iterator;
                using value_type = variant;

                /*!
                 * \brief Destroys the variant_sequential_view::const_iterator
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
                 * Returns the underlying value in a variant stored in a `std::reference_wrapper<T>`.
                 * When the data cannot be returns as reference from the container, the data is stored directly inside the variant.
                 * E.g. for `std::vector<bool>` no reference can be returned.
                 *
                 * \remark For MSVC 2013, the return of `std::reference_wrapper` for arrays doesnt work, thats why we return a ptr instead.
                 *
                 * \see variant::extract_wrapped_value(), variant::get_wrapped_value<T>()
                 */
                const variant operator*() const;

                /*!
                 * \brief Returns the current value, stored inside a `std::reference_wrapper<T>`
                 *        and copied to a variant.
                 *
                 * \see operator*(), variant::extract_wrapped_value(), variant::get_wrapped_value<T>()
                 */
                const variant get_data() const;

                /*!
                 * \brief Pre-increment operator advances the iterator to the next item
                 *        in the container and returns an iterator to the new current item.
                 *
                 * \remark Calling this function on and iterator with value variant_sequential_view::end()
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
                 * \remark Calling this function on and iterator with value variant_sequential_view::begin()
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
                const_iterator(const detail::variant_sequential_view_private* view) RTTR_NOEXCEPT;
                void swap(const_iterator& other);

                friend class variant_sequential_view;

                const detail::variant_sequential_view_private* m_view;
                detail::iterator_data m_itr;
        };
    private:
        friend class variant;
        friend class argument;

        detail::variant_sequential_view_private m_view;
};

} // end namespace rttr

#endif // RTTR_VARIANT_SEQUENTIAL_VIEW_H_
