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


#include "rttr/variant_sequential_view.h"

#include "rttr/argument.h"
#include "rttr/instance.h"

using namespace std;

namespace rttr
{

/////////////////////////////////////////////////////////////////////////////////

variant_sequential_view::variant_sequential_view()
{
}

/////////////////////////////////////////////////////////////////////////////////

variant_sequential_view::variant_sequential_view(const variant_sequential_view& other)
:   m_view(other.m_view)
{

}

/////////////////////////////////////////////////////////////////////////////////

variant_sequential_view::~variant_sequential_view() RTTR_NOEXCEPT
{
}

/////////////////////////////////////////////////////////////////////////////////

void variant_sequential_view::swap(variant_sequential_view& other) RTTR_NOEXCEPT
{
    std::swap(m_view, other.m_view);
}

/////////////////////////////////////////////////////////////////////////////////

variant_sequential_view& variant_sequential_view::operator=(const variant_sequential_view& other) RTTR_NOEXCEPT
{
    variant_sequential_view(other).swap(*this);
    return *this;
}

/////////////////////////////////////////////////////////////////////////////////////////

bool variant_sequential_view::is_valid() const RTTR_NOEXCEPT
{
    return m_view.get_type().is_valid();
}

/////////////////////////////////////////////////////////////////////////////////////////

variant_sequential_view::operator bool() const RTTR_NOEXCEPT
{
    return m_view.get_type().is_valid();
}

/////////////////////////////////////////////////////////////////////////////////////////

type variant_sequential_view::get_type() const RTTR_NOEXCEPT
{
    return m_view.get_type();
}

/////////////////////////////////////////////////////////////////////////////////////////

type variant_sequential_view::get_value_type() const RTTR_NOEXCEPT
{
    return m_view.get_value_type();
}

/////////////////////////////////////////////////////////////////////////////////////////

bool variant_sequential_view::is_empty() const RTTR_NOEXCEPT
{
    return m_view.is_empty();
}

/////////////////////////////////////////////////////////////////////////////////////////

bool variant_sequential_view::is_dynamic() const RTTR_NOEXCEPT
{
    return m_view.is_dynamic();
}

/////////////////////////////////////////////////////////////////////////////////////////

std::size_t variant_sequential_view::get_rank() const RTTR_NOEXCEPT
{
    return m_view.get_rank();
}

/////////////////////////////////////////////////////////////////////////////////////////

type variant_sequential_view::get_rank_type(std::size_t index) const RTTR_NOEXCEPT
{
    return m_view.get_rank_type(index);
}

/////////////////////////////////////////////////////////////////////////////////////////

std::size_t variant_sequential_view::get_size() const RTTR_NOEXCEPT
{
    return m_view.get_size();
}

/////////////////////////////////////////////////////////////////////////////////////////

bool variant_sequential_view::set_size(std::size_t size) const RTTR_NOEXCEPT
{
    return m_view.set_size(size);
}

/////////////////////////////////////////////////////////////////////////////////////////

variant_sequential_view::const_iterator variant_sequential_view::insert(const const_iterator& pos, argument value)
{
    const_iterator itr(&m_view);

    m_view.insert(pos.m_itr, value, itr.m_itr);

    return itr;
}

/////////////////////////////////////////////////////////////////////////////////////////

variant_sequential_view::const_iterator variant_sequential_view::erase(const const_iterator& pos)
{
    const_iterator itr(&m_view);

    m_view.erase(pos.m_itr, itr.m_itr);

    return itr;
}

/////////////////////////////////////////////////////////////////////////////////////////

void variant_sequential_view::clear()
{
    m_view.clear();
}

/////////////////////////////////////////////////////////////////////////////////////////

bool variant_sequential_view::set_value(std::size_t index, argument arg)
{
    return m_view.set_value(index, arg);
}

/////////////////////////////////////////////////////////////////////////////////////////

variant variant_sequential_view::get_value(std::size_t index) const
{
    return m_view.get_value(index);
}

/////////////////////////////////////////////////////////////////////////////////////////

variant_sequential_view::const_iterator variant_sequential_view::begin() const
{
    const_iterator itr(&m_view);

    m_view.begin(itr.m_itr);

    return itr;
}

/////////////////////////////////////////////////////////////////////////////////////////

variant_sequential_view::const_iterator variant_sequential_view::end() const
{
    const_iterator itr(&m_view);

    m_view.end(itr.m_itr);

    return itr;
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

variant_sequential_view::const_iterator::const_iterator(const detail::variant_sequential_view_private* view) RTTR_NOEXCEPT
:   m_view(view)
{
}

/////////////////////////////////////////////////////////////////////////////////////////

variant_sequential_view::const_iterator::~const_iterator()
{
    m_view->destroy(m_itr);
}

/////////////////////////////////////////////////////////////////////////////////////////

variant_sequential_view::const_iterator::const_iterator(const const_iterator &other)
:   m_view(other.m_view),
    m_itr(other.m_itr)
{
    m_view->copy(m_itr, other.m_itr);
}

/////////////////////////////////////////////////////////////////////////////////////////

variant_sequential_view::const_iterator& variant_sequential_view::const_iterator::operator=(const_iterator other)
{
    swap(other);
    return *this;
}

/////////////////////////////////////////////////////////////////////////////////////////

void variant_sequential_view::const_iterator::swap(const_iterator& other)
{
    std::swap(m_itr, other.m_itr);
    std::swap(m_view, other.m_view);
}

/////////////////////////////////////////////////////////////////////////////////////////

const variant variant_sequential_view::const_iterator::operator*() const
{
    return m_view->get_data(m_itr);
}

/////////////////////////////////////////////////////////////////////////////////////////

const variant variant_sequential_view::const_iterator::get_data() const
{
    return m_view->get_data(m_itr);
}

/////////////////////////////////////////////////////////////////////////////////////////

variant_sequential_view::const_iterator& variant_sequential_view::const_iterator::operator++()
{
    m_view->advance(m_itr, 1);
    return *this;
}

/////////////////////////////////////////////////////////////////////////////////////////

variant_sequential_view::const_iterator variant_sequential_view::const_iterator::operator++(int)
{
    const_iterator result(m_view);

    m_view->copy(result.m_itr, m_itr);
    m_view->advance(m_itr, 1);

    return result;
}

/////////////////////////////////////////////////////////////////////////////////////////

variant_sequential_view::const_iterator& variant_sequential_view::const_iterator::operator--()
{
    m_view->advance(m_itr, -1);
    return *this;
}

/////////////////////////////////////////////////////////////////////////////////////////

variant_sequential_view::const_iterator variant_sequential_view::const_iterator::operator--(int)
{
    const_iterator result(m_view);

    m_view->copy(result.m_itr, m_itr);
    m_view->advance(m_itr, -1);

    return result;
}

/////////////////////////////////////////////////////////////////////////////////////////

variant_sequential_view::const_iterator& variant_sequential_view::const_iterator::operator+=(int i)
{
    m_view->advance(m_itr, i);
    return *this;
}

/////////////////////////////////////////////////////////////////////////////////////////

variant_sequential_view::const_iterator& variant_sequential_view::const_iterator::operator-=(int i)
{
    m_view->advance(m_itr, -i);
    return *this;
}

/////////////////////////////////////////////////////////////////////////////////////////

variant_sequential_view::const_iterator variant_sequential_view::const_iterator::operator+(int i) const
{
    const_iterator result(m_view);

    m_view->copy(result.m_itr, m_itr);
    result.m_view->advance(result.m_itr, i);

    return result;
}

/////////////////////////////////////////////////////////////////////////////////////////

variant_sequential_view::const_iterator variant_sequential_view::const_iterator::operator-(int i) const
{
    const_iterator result(m_view);

    m_view->copy(result.m_itr, m_itr);
    result.m_view->advance(result.m_itr, -i);

    return result;
}

/////////////////////////////////////////////////////////////////////////////////////////

bool variant_sequential_view::const_iterator::operator==(const const_iterator& other) const
{
    return m_view->equal(m_itr, other.m_itr);
}

/////////////////////////////////////////////////////////////////////////////////////////

bool variant_sequential_view::const_iterator::operator!=(const const_iterator& other) const
{
    return !m_view->equal(m_itr, other.m_itr);
}

/////////////////////////////////////////////////////////////////////////////////////////

} // end namespace rttr

