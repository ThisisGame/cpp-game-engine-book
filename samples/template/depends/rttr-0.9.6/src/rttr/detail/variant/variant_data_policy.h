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

#ifndef RTTR_VARIANT_DATA_POLICY_H_
#define RTTR_VARIANT_DATA_POLICY_H_

#include "rttr/detail/misc/misc_type_traits.h"
#include "rttr/detail/variant/variant_data.h"
#include "rttr/detail/misc/argument_wrapper.h"
#include "rttr/detail/variant_associative_view/variant_associative_view_creator.h"
#include "rttr/detail/variant_sequential_view/variant_sequential_view_creator.h"
#include "rttr/detail/variant/variant_data_converter.h"
#include "rttr/detail/comparison/compare_equal.h"
#include "rttr/detail/comparison/compare_less.h"

#include <cstdint>

namespace rttr
{
namespace detail
{

template<typename T, typename Converter = empty_type_converter<T>>
struct variant_data_policy_big;
template<typename T, typename Converter = empty_type_converter<T>>
struct variant_data_policy_small;
template<typename T>
struct variant_data_policy_array_small;
template<typename T>
struct variant_data_policy_array_big;

struct void_variant_type;
struct variant_data_policy_void;

struct variant_data_policy_nullptr_t;

struct variant_data_policy_string;

template<typename T>
struct variant_data_policy_arithmetic;

/////////////////////////////////////////////////////////////////////////////////////////

template<typename T, bool Can_Place = (sizeof(T) <= sizeof(variant_data)) &&
                                      (std::alignment_of<T>::value <= std::alignment_of<variant_data>::value)>
using can_place_in_variant = std::integral_constant<bool, Can_Place>;

/*!
 * This template alias will determine the manager class for the given type \p T which is placed inside the variant.
 *
 * Small types will be placed inside the variant without explicit allocation of memory.
 * This will improve the performance a lot.
 *
 * \remark Raw char arrays with rank equal to one, will be copied to a std::string.
 *
 * \return The manager class for the type T.
 */
template<typename T>
using variant_policy = conditional_t<std::is_same<T, void_variant_type>::value,
                                     variant_data_policy_void,
                                     conditional_t<is_nullptr_t<T>::value,
                                                   variant_data_policy_nullptr_t,
                                                   conditional_t<std::is_same<T, std::string>::value || is_one_dim_char_array<T>::value,
                                                                 variant_data_policy_string,
                                                                 conditional_t<can_place_in_variant<T>::value,
                                                                               conditional_t<std::is_arithmetic<T>::value,
                                                                                             variant_data_policy_arithmetic<T>,
                                                                                             conditional_t<std::is_array<T>::value,
                                                                                                           variant_data_policy_array_small<T>,
                                                                                                           conditional_t<std::is_enum<T>::value,
                                                                                                                         variant_data_policy_small<T, default_type_converter<T, convert_from_enum<T>>>,
                                                                                                                         variant_data_policy_small<T>
                                                                                                                        >
                                                                                                          >
                                                                                            >,
                                                                                conditional_t<std::is_array<T>::value,
                                                                                              variant_data_policy_array_big<T>,
                                                                                              conditional_t<std::is_enum<T>::value,
                                                                                                            variant_data_policy_big<T, default_type_converter<T, convert_from_enum<T>>>,
                                                                                                            variant_data_policy_big<T>
                                                                                                           >
                                                                                             >
                                                                              >
                                                                >
                                                  >
                                    >;

/////////////////////////////////////////////////////////////////////////////////////////

/*!
 * \brief This enum is used in the invoke function to delegate
 *        the execution of certain operations.
 *
 */
enum class variant_policy_operation : uint8_t
{
    DESTROY,
    CLONE,
    SWAP,
    EXTRACT_WRAPPED_VALUE,
    CREATE_WRAPPED_VALUE,
    GET_VALUE,
    GET_TYPE,
    GET_PTR,
    GET_RAW_TYPE,
    GET_RAW_PTR,
    GET_ADDRESS_CONTAINER,
    IS_ASSOCIATIVE_CONTAINER,
    IS_SEQUENTIAL_CONTAINER,
    CREATE_ASSOCIATIV_VIEW,
    CREATE_SEQUENTIAL_VIEW,
    IS_VALID,
    IS_NULLPTR,
    CONVERT,
    COMPARE_EQUAL,
    COMPARE_LESS
};

/////////////////////////////////////////////////////////////////////////////////////////

using variant_policy_func = bool (*)(variant_policy_operation, const variant_data&, argument_wrapper);

/////////////////////////////////////////////////////////////////////////////////////////
// some ugly workaround for MSVC < v. 1800

#if RTTR_COMPILER == RTTR_COMPILER_MSVC  && RTTR_COMP_VER <= 1800
    #define COMPARE_EQUAL_PRE_PROC(lhs, rhs, ok)                                  \
        compare_equal(const_cast<typename remove_const<T>::type&>(Tp::get_value(lhs)), const_cast<typename remove_const<T>::type&>(rhs.get_value<T>()), ok)
#else
    #define COMPARE_EQUAL_PRE_PROC(lhs, rhs, ok)                                  \
        compare_equal(Tp::get_value(src_data), rhs.get_value<T>(), ok)
#endif

#if RTTR_COMPILER == RTTR_COMPILER_MSVC && RTTR_COMP_VER <= 1800
    #define COMPARE_LESS_PRE_PROC(lhs, rhs, result)                           \
        compare_less_than(const_cast<typename remove_const<T>::type&>(Tp::get_value(lhs)), const_cast<typename remove_const<T>::type&>(rhs.get_value<T>()), result)
#else
    #define COMPARE_LESS_PRE_PROC(lhs, rhs, result)                           \
        compare_less_than(Tp::get_value(src_data), rhs.get_value<T>(), result)
#endif

/////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
enable_if_t<std::is_pointer<T>::value, bool>
static RTTR_INLINE is_nullptr(T& val)
{
    return (val == nullptr);
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
enable_if_t<!std::is_pointer<T>::value, bool>
static RTTR_INLINE is_nullptr(T& to)
{
    return false;
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
#if RTTR_COMPILER == RTTR_COMPILER_MSVC && RTTR_COMP_VER <= 1800
    // MSVC 2013 has not a full working "std::is_copy_constructible", thats why
    // this workaround is used here
    template<typename T>
    using is_copyable = ::rttr::detail::is_copy_constructible<T>;
#else
    template<typename T>
    using is_copyable = std::is_copy_constructible<T>;
#endif

template<typename T, typename Tp = decay_except_array_t<wrapper_mapper_t<T>> >
enable_if_t<is_copyable<Tp>::value &&
            is_wrapper<T>::value, variant> get_wrapped_value(T& value)
{
    using raw_wrapper_type = remove_cv_t<remove_reference_t<T>>;
    return variant(wrapper_mapper<raw_wrapper_type>::get(value));
}

/////////////////////////////////////////////////////////////////////////////////////////

template<typename T, typename Tp = decay_except_array_t<wrapper_mapper_t<T>>>
enable_if_t<!is_copyable<Tp>::value ||
            !is_wrapper<T>::value, variant> get_wrapped_value(T& value)
{
    return variant();
}

/////////////////////////////////////////////////////////////////////////////////////////

/*!
 * This class represents the base implementation for variant_data policy.
 *
 * We use the C++ idiom CRTP (Curiously Recurring Template Pattern) to avoid rewriting the same code over an over again.
 * The template parameter \p Tp represents the derived class, which will be invoked.
 * Hopefully, the function call to derived class will be inlined.
 */
template<typename T, typename Tp, typename Converter>
struct variant_data_base_policy
{
    static bool invoke(variant_policy_operation op, const variant_data& src_data, argument_wrapper arg)
    {
        switch (op)
        {
            case variant_policy_operation::DESTROY:
            {
                Tp::destroy(const_cast<T&>(Tp::get_value(src_data)));
                break;
            }
            case variant_policy_operation::CLONE:
            {
                Tp::clone(Tp::get_value(src_data), arg.get_value<variant_data>());
                break;
            }
            case variant_policy_operation::SWAP:
            {
                Tp::swap(const_cast<T&>(Tp::get_value(src_data)), arg.get_value<variant_data>());
                break;
            }
            case variant_policy_operation::EXTRACT_WRAPPED_VALUE:
            {
                arg.get_value<variant>() = get_wrapped_value(Tp::get_value(src_data));
                break;
            }
            case variant_policy_operation::CREATE_WRAPPED_VALUE:
            {
                const auto& params          = arg.get_value<std::tuple<variant&, const type&>>();
                variant& var                = std::get<0>(params);
                const type& wrapper_type    = std::get<1>(params);

                wrapper_type.create_wrapped_value(Tp::get_value(src_data), var);
                break;
            }
            case variant_policy_operation::GET_VALUE:
            {
                arg.get_value<const void*>() = &Tp::get_value(src_data);
                break;
            }
            case variant_policy_operation::GET_TYPE:
            {
                arg.get_value<type>() = type::get<T>();
                break;
            }
            case variant_policy_operation::GET_PTR:
            {
                arg.get_value<void*>() = as_void_ptr(std::addressof(Tp::get_value(src_data)));
                break;
            }
            case variant_policy_operation::GET_RAW_TYPE:
            {
                arg.get_value<type>() = type::get<typename raw_type<T>::type>();
                break;
            }
            case variant_policy_operation::GET_RAW_PTR:
            {
                arg.get_value<void*>() = as_void_ptr(raw_addressof(Tp::get_value(src_data)));
                break;
            }
            case variant_policy_operation::GET_ADDRESS_CONTAINER:
            {
                data_address_container& data        = arg.get_value<data_address_container>();

                data.m_type                         = type::get< raw_addressof_return_type_t<T> >();
                data.m_wrapped_type                 = type::get< wrapper_address_return_type_t<T> >();
                data.m_data_address                 = as_void_ptr(raw_addressof(Tp::get_value(src_data)));
                data.m_data_address_wrapped_type    = as_void_ptr(wrapped_raw_addressof(Tp::get_value(src_data)));
                break;
            }
            case variant_policy_operation::IS_ASSOCIATIVE_CONTAINER:
            {
                return can_create_associative_view<T>::value;
            }
            case variant_policy_operation::IS_SEQUENTIAL_CONTAINER:
            {
                return can_create_sequential_view<T>::value;
            }
            case variant_policy_operation::CREATE_ASSOCIATIV_VIEW:
            {
                arg.get_value<variant_associative_view_private&>() = create_variant_associative_view(const_cast<T&>(Tp::get_value(src_data)));
                break;
            }
            case variant_policy_operation::CREATE_SEQUENTIAL_VIEW:
            {
                arg.get_value<variant_sequential_view_private&>() = create_variant_sequential_view(const_cast<T&>(Tp::get_value(src_data)));
                break;
            }
            case variant_policy_operation::CONVERT:
            {
                return Converter::convert_to(Tp::get_value(src_data), arg.get_value<argument>());
            }
            case variant_policy_operation::IS_VALID:
            {
                return true;
            }
            case variant_policy_operation::IS_NULLPTR:
            {
                return is_nullptr(Tp::get_value(src_data));
            }
            case variant_policy_operation::COMPARE_EQUAL:
            {
                const auto& param   = arg.get_value<std::tuple<const variant&, const variant&, bool&>>();
                const variant& lhs  = std::get<0>(param);
                const variant& rhs  = std::get<1>(param);
                bool& ok            = std::get<2>(param);
                const type rhs_type = rhs.get_type();
                const type lhs_type = type::get<T>();
                const auto is_lhs_arithmetic = std::is_arithmetic<T>::value;
                RTTR_BEGIN_DISABLE_CONDITIONAL_EXPR_WARNING

                if (lhs_type == rhs_type)
                {
                    return COMPARE_EQUAL_PRE_PROC(src_data, rhs, ok);
                }
                else
                {
                    if (is_lhs_arithmetic && rhs_type.is_arithmetic())
                    {
                        return variant_compare_equal(lhs, lhs_type, rhs, rhs_type, ok);
                    }
                    else
                    {
                        variant var_tmp;
                        if (rhs.convert(lhs_type, var_tmp))
                            return COMPARE_EQUAL_PRE_PROC(src_data, var_tmp, ok);
                        else if (lhs.convert(rhs_type, var_tmp))
                            return (var_tmp.compare_equal(rhs, ok));
                        else if (rhs.is_nullptr())
                            return is_nullptr(Tp::get_value(src_data));
                    }
                }

                RTTR_END_DISABLE_CONDITIONAL_EXPR_WARNING
                return false;
            }
            case variant_policy_operation::COMPARE_LESS:
            {
                const auto& param   = arg.get_value<std::tuple<const variant&, const variant&, bool&>>();
                const variant& lhs  = std::get<0>(param);
                const variant& rhs  = std::get<1>(param);
                bool& ok            = std::get<2>(param);
                const type rhs_type = rhs.get_type();
                const type lhs_type = type::get<T>();
                int result = 0;
                if (lhs_type == rhs_type)
                {
                    if ((ok = COMPARE_LESS_PRE_PROC(src_data, rhs, result)) == true)
                        return (result == -1 ? true : false);
                }
                else
                {
                    return variant_compare_less(lhs, lhs_type, rhs, rhs_type, ok);
                }

                bool ok1 = false;
                bool ok2 = false;
                auto ret = (lhs.to_string(&ok1) < rhs.to_string(&ok2));
                if (ok1 && ok2)
                {
                    ok = true;
                    return ret;
                }
                else
                {
                    return false;
                }
            }
        }

        return true;
    }
};

/////////////////////////////////////////////////////////////////////////////////////////

/*!
 * This policy is used for small custom data types.
 *
 * These are types which will fit into the storage of \ref variant_data.
 * The data will be placed with "placement new" inside the variant.
 * That also means the destructor will be called explicitly.
 */
template<typename T, typename Converter>
struct variant_data_policy_small : variant_data_base_policy<T, variant_data_policy_small<T>, Converter>
{
    static RTTR_INLINE const T& get_value(const variant_data& data)
    {
        return reinterpret_cast<const T&>(data);
    }

    static RTTR_INLINE void destroy(T& value)
    {
        value.~T();
    }

    static RTTR_INLINE void clone(const T& value, variant_data& dest)
    {
        new (&dest) T(value);
    }

    static RTTR_INLINE void swap(T& value, variant_data& dest)
    {
        new (&dest) T(value);
        destroy(value);
    }

    template<typename U>
    static RTTR_INLINE void create(U&& value, variant_data& dest)
    {
        new (&dest) T(std::forward<U>(value));
    }
};

/////////////////////////////////////////////////////////////////////////////////////////

/*!
 * This policy is used for custom types that size does NOT fit into variant_data.
 *
 * Then the type will be allocated on the heap and a pointer to the data is stored in variant_data.
 */
template<typename T, typename Converter>
struct variant_data_policy_big : variant_data_base_policy<T, variant_data_policy_big<T>, Converter>
{
    static RTTR_INLINE const T& get_value(const variant_data& data)
    {
        return *reinterpret_cast<T* const &>(data);
    }

    static RTTR_INLINE void destroy(T& value)
    {
        delete &value;
    }

    static RTTR_INLINE void clone(const T& value, variant_data& dest)
    {
        reinterpret_cast<T*&>(dest) = new T(value);
    }

    static RTTR_INLINE void swap(T& value, variant_data& dest)
    {
        reinterpret_cast<T*&>(dest) = &value;
    }

    template<typename U>
    static RTTR_INLINE void create(U&& value, variant_data& dest)
    {
        reinterpret_cast<T*&>(dest) = new T(std::forward<U>(value));
    }
};

/////////////////////////////////////////////////////////////////////////////////////////

#if RTTR_COMPILER == RTTR_COMPILER_MSVC && RTTR_COMP_VER <= 1800
    #define COPY_ARRAY_PRE_PROC(value, dest) \
                copy_array(const_cast<typename remove_const<T>::type&>(value), const_cast<typename remove_const<T>::type&>(get_value(dest)))
#else
    #define COPY_ARRAY_PRE_PROC(value, dest) \
                copy_array(value, const_cast<typename remove_const<T>::type&>(get_value(dest)));
#endif

/*!
 * This policy is used for small raw array types, which fit in \p variant_data.
 *
 * The array data will be copied into \p variant_data.
 */
template<typename T>
struct variant_data_policy_array_small : variant_data_base_policy<T, variant_data_policy_array_small<T>>
{
    static RTTR_INLINE const T& get_value(const variant_data& data)
    {
        return reinterpret_cast<const T&>(data);
    }

    static RTTR_INLINE void destroy(T& value)
    {
    }

    static RTTR_INLINE void clone(const T& value, variant_data& dest)
    {
        COPY_ARRAY_PRE_PROC(value, dest);
    }

    static RTTR_INLINE void swap(T& value, variant_data& dest)
    {
        clone(value, dest);
    }

    template<typename U>
    static RTTR_INLINE void create(U&& value, variant_data& dest)
    {
        COPY_ARRAY_PRE_PROC(value, dest);
    }
};

/////////////////////////////////////////////////////////////////////////////////////////

/*!
 * This policy is used for raw array types of arbitrary rank, which fit NOT into \p variant_data.
 *
 * A copy of the given array value will be allocated on the heap. The pointer to the array is stored in \p variant_data.
 */
template<typename T>
struct variant_data_policy_array_big : variant_data_base_policy<T, variant_data_policy_array_big<T>>
{
    using array_dest_type = decltype(new T);

    static RTTR_INLINE const T& get_value(const variant_data& data)
    {
        return reinterpret_cast<const T&>(*reinterpret_cast<const array_dest_type&>(data));
    }

    static RTTR_INLINE void destroy(T& value)
    {
        delete [] &value;
    }

    static RTTR_INLINE void clone(const T& value, variant_data& dest)
    {
        reinterpret_cast<array_dest_type&>(dest) = new T;

        COPY_ARRAY_PRE_PROC(value, dest);
    }

    static RTTR_INLINE void swap(T& value, variant_data& dest)
    {
        reinterpret_cast<array_dest_type&>(dest) = value;
    }

    template<typename U>
    static RTTR_INLINE void create(U&& value, variant_data& dest)
    {
        using array_dest_type = decltype(new T);
        reinterpret_cast<array_dest_type&>(dest) = new T;

        COPY_ARRAY_PRE_PROC(value, dest);
    }
};

/////////////////////////////////////////////////////////////////////////////////////////

/*!
 * This policy is used for all arithmetic types, which fit into the storage of variant_data.
 *
 * The data will be raw copied into the variant_data.
 */
template<typename T>
struct variant_data_policy_arithmetic : variant_data_base_policy<T, variant_data_policy_arithmetic<T>, default_type_converter<T>>
{
    static RTTR_INLINE const T& get_value(const variant_data& data)
    {
        return reinterpret_cast<const T&>(data);
    }

    static RTTR_INLINE void destroy(T& value)
    {
    }

    static RTTR_INLINE void clone(const T& value, variant_data& dest)
    {
        reinterpret_cast<T&>(dest) = value;
    }

    static RTTR_INLINE void swap(T& value, variant_data& dest)
    {
        clone(value, dest);
    }

    template<typename U>
    static RTTR_INLINE void create(U&& value, variant_data& dest)
    {
        reinterpret_cast<T&>(dest) = value;
    }
};

/////////////////////////////////////////////////////////////////////////////////////////

/*!
 * This policy will manage the type std::string.
 *
 * This type has build in converter for several other basic types, that why we have a specialization here for it.
 */
struct RTTR_API variant_data_policy_string : variant_data_policy_big<std::string, default_type_converter<std::string>>
{
    template<typename U>
    static RTTR_INLINE void create(U&& value, variant_data& dest)
    {
        reinterpret_cast<std::string*&>(dest) = new std::string(std::forward<U>(value));
    }

    template<std::size_t N>
    static RTTR_INLINE void create(const char (&value)[N], variant_data& dest)
    {
        reinterpret_cast<std::string*&>(dest) = new std::string(value, N - 1);
    }
};

/////////////////////////////////////////////////////////////////////////////////////////

/*!
 * This policy is used when the variant does not contain any data. So in fact an invalid variant.
 *
 * With this approach we avoid checking for an valid variant. E.g. during destruction.
 */
struct RTTR_API variant_data_policy_empty
{
    static bool invoke(variant_policy_operation op, const variant_data& src_data, argument_wrapper arg)
    {
        switch (op)
        {
            case variant_policy_operation::DESTROY:
            case variant_policy_operation::CLONE:
            case variant_policy_operation::SWAP:
            case variant_policy_operation::EXTRACT_WRAPPED_VALUE:
            case variant_policy_operation::CREATE_WRAPPED_VALUE:
            {
                break;
            }
            case variant_policy_operation::GET_VALUE:
            {
                arg.get_value<const void*>() = nullptr;
                break;
            }
            case variant_policy_operation::GET_TYPE:
            {
                arg.get_value<type>() = get_invalid_type();
                break;
            }
            case variant_policy_operation::GET_PTR:
            {
                arg.get_value<void*>() = nullptr;
                break;
            }
            case variant_policy_operation::GET_RAW_TYPE:
            {
                arg.get_value<type>() = get_invalid_type();
                break;
            }
            case variant_policy_operation::GET_RAW_PTR:
            {
                arg.get_value<void*>() = nullptr;
                break;
            }
            case variant_policy_operation::GET_ADDRESS_CONTAINER:
            {
                data_address_container& data        = arg.get_value<data_address_container>();

                data.m_type                         = get_invalid_type();
                data.m_wrapped_type                 = get_invalid_type();
                data.m_data_address                 = nullptr;
                data.m_data_address_wrapped_type    = nullptr;
                break;
            }
            case variant_policy_operation::IS_ASSOCIATIVE_CONTAINER:
            {
                return false;
            }
            case variant_policy_operation::IS_SEQUENTIAL_CONTAINER:
            {
                return false;
            }
            case variant_policy_operation::CREATE_ASSOCIATIV_VIEW:
            {
                break;
            }
            case variant_policy_operation::CREATE_SEQUENTIAL_VIEW:
            {
                break;
            }
            case variant_policy_operation::IS_VALID:
            {
                return false;
            }
            case variant_policy_operation::IS_NULLPTR:
            {
                return false;
            }
            case variant_policy_operation::CONVERT:
            {
                return false;
            }
            case variant_policy_operation::COMPARE_EQUAL:
            {
                const auto& param   = arg.get_value<std::tuple<const variant&, const variant&, bool&>>();
                const variant& rhs  = std::get<1>(param);
                bool& ok            = std::get<2>(param);
                if (!rhs.is_valid())
                    ok = true;
                return ok;
            }
            case variant_policy_operation::COMPARE_LESS:
            {
                return false;
            }
        }
        return true;
    }

    template<typename U>
    static RTTR_INLINE void create(U&&, variant_data&)
    {
    }
};

/////////////////////////////////////////////////////////////////////////////////////////

/*!
 * This policy is used when the variant does contain a `void`.
 *
 * A `void` variant is a special variant to indicate that a function call was successful.
 * So in fact it does not contain any data, but the returned type of \ref variant::get_type() is a `void`.
 */
struct RTTR_API variant_data_policy_void
{
    static bool invoke(variant_policy_operation op, const variant_data& src_data, argument_wrapper arg)
    {
        switch (op)
        {
            case variant_policy_operation::DESTROY:
            case variant_policy_operation::CLONE:
            case variant_policy_operation::SWAP:
            case variant_policy_operation::EXTRACT_WRAPPED_VALUE:
            {
                break;
            }
            case variant_policy_operation::GET_VALUE:
            {
                arg.get_value<const void*>() = nullptr;
                break;
            }
            case variant_policy_operation::GET_TYPE:
            {
                arg.get_value<type>() = type::get<void>();
                break;
            }
            case variant_policy_operation::GET_PTR:
            {
                arg.get_value<void*>() = nullptr;
            }
            case variant_policy_operation::GET_RAW_TYPE:
            {
                arg.get_value<type>() = type::get<void>();
                break;
            }
            case variant_policy_operation::GET_RAW_PTR:
            {
                arg.get_value<void*>() = nullptr;
                break;
            }
            case variant_policy_operation::GET_ADDRESS_CONTAINER:
            {
                data_address_container& data        = arg.get_value<data_address_container>();

                data.m_type                         = type::get<void>();
                data.m_wrapped_type                 = type::get<void>();
                data.m_data_address                 = nullptr;
                data.m_data_address_wrapped_type    = nullptr;
                break;
            }
            case variant_policy_operation::IS_ASSOCIATIVE_CONTAINER:
            {
                return false;
            }
            case variant_policy_operation::IS_SEQUENTIAL_CONTAINER:
            {
                return false;
            }
            case variant_policy_operation::CREATE_ASSOCIATIV_VIEW:
            {
                break;
            }
            case variant_policy_operation::CREATE_SEQUENTIAL_VIEW:
            {
                break;
            }
            case variant_policy_operation::CREATE_WRAPPED_VALUE:
            {
                return false;
            }
            case variant_policy_operation::IS_NULLPTR:
            {
                return false;
            }
            case variant_policy_operation::IS_VALID:
            {
                return true;
            }
            case variant_policy_operation::CONVERT:
            {
                return false;
            }
            case variant_policy_operation::COMPARE_EQUAL:
            {
                const auto& param   = arg.get_value<std::tuple<const variant&, const variant&, bool&>>();
                const variant& rhs  = std::get<1>(param);
                bool& ok            = std::get<2>(param);
                if (rhs.is_type<void>())
                    ok = true;
                return ok;
            }
            case variant_policy_operation::COMPARE_LESS:
            {
                return false;
            }
        }
        return true;
    }

    template<typename U>
    static RTTR_INLINE void create(U&&, variant_data&)
    {
    }
};

/////////////////////////////////////////////////////////////////////////////////////////


/*!
 * This policy is used when the variant does contain a `nullptr`.
 *
 * We need this special handling because MSVC 2013 cannot handle correct a const std::nullptr_t,
 * On the other hand we have some special handling for comparison.
 */
struct RTTR_API variant_data_policy_nullptr_t
{
    static RTTR_INLINE std::nullptr_t& get_value(const variant_data& data)
    {
        return reinterpret_cast<std::nullptr_t&>(const_cast<variant_data&>(data));
    }

    static RTTR_INLINE void destroy(std::nullptr_t& value)
    {
        // for unknown reason we have to fully qualify the dtor call here;
        // otherwise mingw has reports a problems here: "request for member 'nullptr_t' in non-class type 'std::nullptr_t'"
        value.std::nullptr_t::~nullptr_t();
    }

    static RTTR_INLINE void clone(const std::nullptr_t& value, variant_data& dest)
    {
        new (&dest) std::nullptr_t(value);
    }

    static RTTR_INLINE void swap(std::nullptr_t& value, variant_data& dest)
    {
        new (&dest) std::nullptr_t(value);
        destroy(value);
    }

    static bool invoke(variant_policy_operation op, const variant_data& src_data, argument_wrapper arg)
    {
        switch (op)
        {
            case variant_policy_operation::DESTROY:
            {
                destroy(get_value(src_data));
                break;
            }
            case variant_policy_operation::CLONE:
            {
                clone(get_value(src_data), arg.get_value<variant_data>());
                break;
            }
            case variant_policy_operation::SWAP:
            {
                swap(get_value(src_data), arg.get_value<variant_data>());
                break;
            }
            case variant_policy_operation::CREATE_WRAPPED_VALUE:
            {
                return false;
            }
            case variant_policy_operation::EXTRACT_WRAPPED_VALUE:
            {
                break;
            }
            case variant_policy_operation::GET_VALUE:
            {
                arg.get_value<const void*>() = &get_value(src_data);
                break;
            }
            case variant_policy_operation::GET_TYPE:
            {
                arg.get_value<type>() = type::get<std::nullptr_t>();
                break;
            }
            case variant_policy_operation::GET_PTR:
            {
                arg.get_value<void*>() = as_void_ptr(std::addressof(get_value(src_data)));
            }
            case variant_policy_operation::GET_RAW_TYPE:
            {
                arg.get_value<type>() = type::get<std::nullptr_t>();
                break;
            }
            case variant_policy_operation::GET_RAW_PTR:
            {
                arg.get_value<void*>() = as_void_ptr(raw_addressof(get_value(src_data)));
                break;
            }
            case variant_policy_operation::GET_ADDRESS_CONTAINER:
            {
                data_address_container& data        = arg.get_value<data_address_container>();

                data.m_type                         = type::get<std::nullptr_t>();
                data.m_wrapped_type                 = type::get<std::nullptr_t>();
                data.m_data_address                 = as_void_ptr(raw_addressof(get_value(src_data)));
                data.m_data_address_wrapped_type    = as_void_ptr(wrapped_raw_addressof(get_value(src_data)));
                break;
            }
            case variant_policy_operation::IS_ASSOCIATIVE_CONTAINER:
            {
                return false;
            }
            case variant_policy_operation::IS_SEQUENTIAL_CONTAINER:
            {
                return false;
            }
            case variant_policy_operation::CREATE_ASSOCIATIV_VIEW:
            {
                break;
            }
            case variant_policy_operation::CREATE_SEQUENTIAL_VIEW:
            {
                break;
            }
            case variant_policy_operation::IS_VALID:
            {
                return true;
            }
            case variant_policy_operation::IS_NULLPTR:
            {
                return true;
            }
            case variant_policy_operation::CONVERT:
            {
                return false;
            }
            case variant_policy_operation::COMPARE_EQUAL:
            {
                const auto& param   = arg.get_value<std::tuple<const variant&, const variant&, bool&>>();
                const variant& rhs  = std::get<1>(param);
                bool&           ok  = std::get<2>(param);
                ok = true;
                return rhs.is_nullptr();
            }
            case variant_policy_operation::COMPARE_LESS:
            {
                const auto& param   = arg.get_value<std::tuple<const variant&, const variant&, bool&>>();
                const variant& lhs  = std::get<0>(param);
                const variant& rhs  = std::get<1>(param);
                bool& ok            = std::get<2>(param);
                ok = true;
                return (lhs.is_nullptr() && !rhs.is_nullptr());
            }
        }
        return true;
    }

    template<typename U>
    static RTTR_INLINE void create(U&& value, variant_data& dest)
    {
        new (&dest) std::nullptr_t(std::forward<U>(value));
    }
};


/////////////////////////////////////////////////////////////////////////////////////////
// default export, to reduce compile time, and code bloat
template struct RTTR_API variant_data_policy_arithmetic<bool>;
template struct RTTR_API variant_data_policy_arithmetic<char>;
template struct RTTR_API variant_data_policy_arithmetic<float>;
template struct RTTR_API variant_data_policy_arithmetic<double>;

template struct RTTR_API variant_data_policy_arithmetic<std::uint8_t>;
template struct RTTR_API variant_data_policy_arithmetic<std::uint16_t>;
template struct RTTR_API variant_data_policy_arithmetic<std::uint32_t>;
template struct RTTR_API variant_data_policy_arithmetic<std::uint64_t>;

template struct RTTR_API variant_data_policy_arithmetic<std::int8_t>;
template struct RTTR_API variant_data_policy_arithmetic<std::int16_t>;
template struct RTTR_API variant_data_policy_arithmetic<std::int32_t>;
template struct RTTR_API variant_data_policy_arithmetic<std::int64_t>;


template struct RTTR_API variant_data_policy_small<bool*>;
template struct RTTR_API variant_data_policy_small<char*>;
template struct RTTR_API variant_data_policy_small<float*>;
template struct RTTR_API variant_data_policy_small<double*>;

template struct RTTR_API variant_data_policy_small<std::uint8_t*>;
template struct RTTR_API variant_data_policy_small<std::uint16_t*>;
template struct RTTR_API variant_data_policy_small<std::uint32_t*>;
template struct RTTR_API variant_data_policy_small<std::uint64_t*>;

template struct RTTR_API variant_data_policy_small<std::int8_t*>;
template struct RTTR_API variant_data_policy_small<std::int16_t*>;
template struct RTTR_API variant_data_policy_small<std::int32_t*>;
template struct RTTR_API variant_data_policy_small<std::int64_t*>;

/////////////////////////////////////////////////////////////////////////////////////////

} // end namespace detail
} // end namespace rttr

#endif // RTTR_VARIANT_DATA_POLICY_H_
