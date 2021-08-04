
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

#ifndef RTTR_ARRAY_RANGE_H_
#define RTTR_ARRAY_RANGE_H_

#include "rttr/detail/base/core_prerequisites.h"
#include <vector>
#include <cstddef>

namespace rttr
{
class property;
class method;
class constructor;
class enumeration;
class parameter_info;

namespace detail
{
template<typename T>
struct default_predicate;

} // end namespace detail


/*!
 * The \ref array_range class provides a view into an underlying data structure with lower and upper limits.
 * In order to iterate over the data structure use the returned forward iterators from begin() or rbegin().
 * To check whether the end of the range has reached use end() or rend().
 * Using the standard C++ range-based for loop will implicit do all this work.
 *
 * \remark When the underlying data structure gets modified while holding a range,
 *         its iterators and the range itself will be invalidated.
 */

template<typename T, typename Predicate = detail::default_predicate<T>>
class array_range
{
public:
    using value_type = T;
    using bounds_type = T*;
    using size_type = std::size_t;

    /*!
     * \brief Default constructor. Constructs an empty array_range.
     */
    array_range();

    /*!
     * \brief Constructs an array range starting from \p begin to \p end [begin, end).
     *
     * \param begin Marks the start of the range. Is included of the range.
     * \param size The number of elements to include in the range.
     * \param pred Determines whether an element in the range fulfills the condition of the predicate.
     */
    array_range(const T* begin, size_type size, const Predicate& pred = Predicate());

#ifndef DOXYGEN
    /*!
     * The base class for all item forward iterators.
     */
    template<typename DataType>
    class array_iterator_base
    {
        public:
            using self_type = array_iterator_base<DataType>;
            using value_type = DataType;
            using reference = DataType&;
            using pointer = DataType*;
            using iterator_category = std::forward_iterator_tag;
            using difference_type = std::ptrdiff_t;

            bool operator==(const self_type& rhs) const;
            bool operator!=(const self_type& rhs) const;

            array_iterator_base& operator=(const self_type& other);

        protected:
            friend class array_range<T, Predicate>;
            array_iterator_base();
            array_iterator_base(pointer ptr, const array_range<T, Predicate>* const range);

            pointer m_ptr;
            const array_range<T, Predicate>* m_range;
    };

    /*!
     * A forward iterator for items of type \p DataType.
     */
    template<typename DataType>
    class array_iterator : public array_iterator_base<DataType>
    {
        public:
            using self_type = array_iterator<DataType>;
            using reference = typename array_iterator_base<DataType>::reference;
            using pointer   = typename array_iterator_base<DataType>::pointer;

            array_iterator();
            array_iterator(const array_iterator<DataType>& other);


            reference operator*() const;
            pointer operator->();

            self_type& operator++();
            self_type operator++(int index);

        private:
            array_iterator(typename array_iterator_base<DataType>::pointer ptr,
                           const array_range<T, Predicate>* const range);
            friend class array_range<T, Predicate>;
    };

    /*!
     * A forward reverse iterator for items of type \p DataType.
     */
    template<typename DataType>
    class array_reverse_iterator : public array_iterator_base<DataType>
    {
        public:
            using self_type = array_reverse_iterator<DataType>;
            using reference = typename array_iterator_base<DataType>::reference;
            using pointer   = typename array_iterator_base<DataType>::pointer;

            array_reverse_iterator();
            array_reverse_iterator(const array_reverse_iterator<DataType>& other);

            reference operator*() const;
            pointer operator->();

            self_type& operator++();
            self_type operator++(int index);

        private:
            array_reverse_iterator(typename array_iterator_base<DataType>::pointer ptr,
                                   const array_range<T, Predicate>* const range);
            friend class array_range<T, Predicate>;
    };
#endif

    //! A forward iterator.
    using iterator = array_iterator<T>;
    //! A constant forward iterator.
    using const_iterator = array_iterator<const T>;

    //! A forward iterator that reverses the direction.
    using reverse_iterator = array_reverse_iterator<T>;
    //! A constant forward iterator that reverses the direction.
    using const_reverse_iterator = array_reverse_iterator<const T>;

    /*!
     * \brief Returns an iterator to the first element of the range.
     *
     * \remark If the range is empty, the returned iterator will be equal to \ref end().
     *
     * \return Iterator to the first element.
     */
    const_iterator begin();

    /*!
     * \brief Returns an iterator to the element following the last element of the range.
     *
     * \remark This element acts as placeholder, attempting to access it results in **undefined behavior**.
     *         If the range is empty, the returned iterator will be equal to \ref begin().
     *
     * \return Iterator to the element following the last element.
     */
    const_iterator end();

    /*!
     * \brief Returns a constant iterator to the first element of the range.
     *
     * \remark If the range is empty, the returned iterator will be equal to \ref end().
     *
     * \return Constant iterator to the first element.
     */
    const_iterator begin() const;

    /*!
     * \brief Returns a constant iterator to the element following the last element of the range.
     *
     * \remark This element acts as placeholder, attempting to access it results in **undefined behavior**.
     *         If the range is empty, the returned iterator will be equal to \ref begin().
     *
     * \return Constant iterator to the element following the last element.
     */
    const_iterator end() const;

     /*!
     * \brief Returns a constant iterator to the first element of the  range.
     *
     * \remark If the range is empty, the returned iterator will be equal to \ref end().
     *
     * \return Constant iterator to the first element.
     */
    const_iterator cbegin() const;

    /*!
     * \brief Returns a constant iterator to the element following the last element of the range.
     *
     * \remark This element acts as placeholder, attempting to access it results in **undefined behavior**.
     *         If the range is empty, the returned iterator will be equal to \ref begin().
     *
     * \return Constant iterator to the element following the last element.
     */
    const_iterator cend() const;

    /*!
     * \brief Returns a reverse iterator to the first element of the reversed range.
     *        It corresponds to the last element of the non-reversed range.
     *
     * \remark If the range is empty, the returned iterator will be equal to \ref rend().
     *
     * \return Reverse iterator to the first element.
     */
    const_reverse_iterator rbegin();

    /*!
     * \brief Returns a reverse iterator to the element following the last element of the reversed range.
     *        It corresponds to the element preceding the first element of the non-reversed range.
     *
     * \remark If the range is empty, the returned iterator will be equal to \ref rbegin().
     *
     * \return Reverse iterator to the element following the last element.
     */
    const_reverse_iterator rend();

    /*!
     * \brief Returns a constant reverse iterator to the first element of the reversed range.
     *        It corresponds to the last element of the non-reversed range.
     *
     * \remark If the range is empty, the returned iterator will be equal to \ref rend().
     *
     * \return Constant reverse iterator to the first element.
     */
    const_reverse_iterator rbegin() const;

    /*!
     * \brief Returns a constant reverse iterator to the element following the last element of the reversed range.
     *        It corresponds to the element preceding the first element of the non-reversed range.
     *
     * \remark If the range is empty, the returned iterator will be equal to \ref rbegin().
     *
     * \return Constant reverse iterator to the element following the last element.
     */
    const_reverse_iterator rend() const;

    /*!
     * \brief Returns a constant reverse iterator to the first element of the reversed range.
     *        It corresponds to the last element of the non-reversed range.
     *
     * \remark If the range is empty, the returned iterator will be equal to \ref rend().
     *
     * \return Constant reverse iterator to the first element.
     */
    const_reverse_iterator crbegin() const;

    /*!
     * \brief Returns a constant reverse iterator to the element following the last element of the reversed range.
     *        It corresponds to the element preceding the first element of the non-reversed range.
     *
     * \remark If the range is empty, the returned iterator will be equal to \ref rbegin().
     *
     * \return Constant reverse iterator to the element following the last element.
     */
    const_reverse_iterator crend() const;

    /*!
     * \brief Returns the number of elements in the range.
     *
     * \remark Every element will be checked against the condition of the used predicate.
     *         Only elements which fulfill the condition of predicate will be included in the counter.
     *         That means, in order to determine the size of the range, the underlying algorithm needs to iterate
     *         through the whole range. So don't call it to often. It's better to cache the result in a temporary variable.
     *
     * \return The number of elements in the range.
     */
    size_t size() const;

    /*!
     * \brief Checks if the range has no elements, i.e. whether `begin() == end()`.
     *
     * \remark Every element will be checked against the condition of the used predicate.
     *         Only when every element doe not fulfill the condition of predicate, the range is declared empty.
     *         That means, in order to check for emptiness, the underlying algorithm needs to iterate
     *         through the whole range. So don't call it to often. It's better to cache the result in a temporary variable.
     *
     * \return `True` if this range is empty, otherwise `false`.
     */
    bool empty() const;

private:
    template<typename DataType>
    void next(array_iterator<DataType>& itr) const;

    template<typename DataType>
    void prev(array_reverse_iterator<DataType>& itr) const;

    bool empty_() const;
    array_range<T, Predicate>& operator=(const array_range<T, Predicate>& other);

private:
    const T* const   m_begin;
    const T* const   m_end;
    const Predicate  m_pred;
};

/////////////////////////////////////////////////////////////////////////////////////////

} // end namespace rttr

#include "rttr/detail/impl/array_range_impl.h"

#endif // RTTR_ARRAY_RANGE_H_
