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

#ifndef RTTR_BASE_CLASSES_H_
#define RTTR_BASE_CLASSES_H_


namespace rttr
{
namespace detail
{

struct base_class_info
{
    base_class_info(type t, void*(*rttr_cast_func)(void*))
    :   m_base_type(t), m_rttr_cast_func(rttr_cast_func)
    {}
    type            m_base_type;
    void*           (*m_rttr_cast_func)(void*);
};

/*!
 * This trait checks if a given type T has a typedef named \a base_class_list.
 * has_base_class_list_impl::value is true, when it has this type, otherwise false.
 */
template <typename T>
class has_base_class_list_impl
{
    typedef char YesType[1];
    typedef char NoType[2];

    template <typename C>
    static YesType& test(typename C::base_class_list*);

    template <typename>
    static NoType& test(...);

public:
    static RTTR_CONSTEXPR_OR_CONST bool value = (sizeof(YesType) == sizeof(test<T>(0)));
};

/*!
 * If T has a type alias called \a 'base_class_list' then inherits from true_type, otherwise inherits from false_type.
 */
template<typename T>
using has_base_class_list = std::integral_constant<bool, has_base_class_list_impl<T>::value>;

using info_container = std::vector<detail::base_class_info>;

/*!
 * This class fills from a given type_list the corresponding type objects into a std::vector.
 */
template<typename DerivedClass, typename... T>
struct RTTR_LOCAL type_from_base_classes;

template<typename DerivedClass>
struct RTTR_LOCAL type_from_base_classes<DerivedClass>
{
    static RTTR_INLINE void fill(info_container&)
    {
    }
};

/*!
 * This is the trick to make the replacement of `dynamic_cast` possible.
 *
 * For every \p DerivedType is a function pointer stored, which performs a
 * static_cast operation to it's BaseType.
 * The given \p ptr is a void* ptr and has to be of type \p DerivedType,
 * otherwise we would get undefined behavior.
 * Therefore the \ref get_derived_info() function is used. It will return this information.
 */
template<typename DerivedType, typename BaseType>
static void* rttr_cast_impl(void* ptr)
{
    return static_cast<void*>(static_cast<BaseType*>(static_cast<DerivedType*>(ptr)));
}

template<typename DerivedClass, typename BaseClass, typename... U>
struct RTTR_LOCAL type_from_base_classes<DerivedClass, BaseClass, U...>
{
    static RTTR_INLINE void fill(info_container& vec)
    {
        static_assert(has_base_class_list<BaseClass>::value, "The parent class has no base class list defined - please use the macro RTTR_ENABLE");
        vec.emplace_back(type::get<BaseClass>(), &rttr_cast_impl<DerivedClass, BaseClass>);
        // retrieve also the types of all base classes of the base classes; you will get an compile error here,
        // when the base class has not defined the 'base_class_list' typedef
        type_from_base_classes<DerivedClass, typename BaseClass::base_class_list>::fill(vec);
        // continue with the rest
        type_from_base_classes<DerivedClass, U...>::fill(vec);
    }
};

template<typename... U> struct type_list;

template<typename DerivedClass, class... BaseClassList>
struct type_from_base_classes<DerivedClass, type_list<BaseClassList...>> : type_from_base_classes<DerivedClass, BaseClassList...> { };

/*!
 * This helper trait returns a vector with type object of all base classes.
 * When there is no type_list defined or the class has no base class, an empty vector is returned.
 */
template<typename T, typename Enable = void>
struct RTTR_LOCAL base_classes
{
    static RTTR_INLINE info_container get_types()
    {
        info_container result;
        return result;
    }
};

template<typename T>
struct RTTR_LOCAL base_classes<T, typename std::enable_if<has_base_class_list<T>::value>::type>
{
    static RTTR_INLINE info_container get_types()
    {
        info_container result;
        type_from_base_classes<T, typename T::base_class_list>::fill(result);
        return result;
    }
};

} // end namespace detail
} // end namespace rttr

#endif // RTTR_BASE_CLASSES_H_
