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

#ifndef RTTR_TEST_TYPE_H_
#define RTTR_TEST_TYPE_H_

#include <rttr/type>

template<typename T>
struct custom_wrapper
{
    public:
        custom_wrapper(T& obj) : m_value(std::addressof(obj)) {}
        T& get_data() const { return *m_value; }
    private:
        T* m_value;
};

namespace rttr
{
template<typename T>
struct wrapper_mapper<custom_wrapper<T>>
{
    typedef decltype(std::declval<custom_wrapper<T>>().get_data()) wrapped_type;
    typedef custom_wrapper<T> type;
    inline static wrapped_type get(const type& obj)
    {
       return obj.get_data();
    }
    inline static type create(const wrapped_type& value)
    {
       return custom_wrapper<T>(value);
    }
};

}

#endif // RTTR_TEST_TYPE_H_
