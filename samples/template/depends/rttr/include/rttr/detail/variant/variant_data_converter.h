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

#ifndef RTTR_VARIANT_DATA_CONVERTER_H_
#define RTTR_VARIANT_DATA_CONVERTER_H_

#include "rttr/detail/conversion/std_conversion_functions.h"
#include "rttr/detail/conversion/number_conversion.h"
#include "rttr/detail/enumeration/enumeration_helper.h"

namespace rttr
{
class argument;

namespace detail
{

template<typename T>
struct convert_from;

/////////////////////////////////////////////////////////////////////////////////////////

/*!
 * The default converter manager class.
 *
 * It will check at runtime the incoming argument type and will forward its value to the corresponding conversion function.
 * All basic fixed integer and floating-point (float, double) types are handled.
 *
 * \remark Custom types will not be handled here,
 *         therefore a converter function has to be registered explicitly. See \ref type::register_converter_func.
 *         With this class we can avoid this step.
 */
template<typename T, typename Type_Converter = convert_from<T>>
struct default_type_converter
{
    static bool convert_to(const T& value, argument& arg)
    {
        const type target_type = arg.get_type();
        if (target_type == type::get<bool>())
            return Type_Converter::to(value, arg.get_value<bool>());
        else if (target_type == type::get<char>())
            return Type_Converter::to(value, arg.get_value<char>());
        else if (target_type == type::get<int8_t>())
            return Type_Converter::to(value, arg.get_value<int8_t>());
        else if (target_type == type::get<int16_t>())
            return Type_Converter::to(value, arg.get_value<int16_t>());
        else if (target_type == type::get<int32_t>())
            return Type_Converter::to(value, arg.get_value<int32_t>());
        else if (target_type == type::get<int64_t>())
            return Type_Converter::to(value, arg.get_value<int64_t>());
        else if (target_type == type::get<uint8_t>())
            return Type_Converter::to(value, arg.get_value<uint8_t>());
        else if (target_type == type::get<uint16_t>())
            return Type_Converter::to(value, arg.get_value<uint16_t>());
        else if (target_type == type::get<uint32_t>())
            return Type_Converter::to(value, arg.get_value<uint32_t>());
        else if (target_type == type::get<uint64_t>())
            return Type_Converter::to(value, arg.get_value<uint64_t>());
        else if (target_type == type::get<float>())
            return Type_Converter::to(value, arg.get_value<float>());
        else if (target_type == type::get<double>())
            return Type_Converter::to(value, arg.get_value<double>());
        else if (target_type == type::get<std::string>())
            return Type_Converter::to(value, arg.get_value<std::string>());
        else if (is_variant_with_enum(arg))
            return Type_Converter::to_enum(value, arg);
        else
            return false;
    }
};

/////////////////////////////////////////////////////////////////////////////////////////

/*!
 * The empty converter class, does nothing. The only purpose is to avoid a compile time error.
 * The conversion of custom types will be handled via registered conversion functions. See \ref type::register_converter_func.
 */
template<typename T>
struct empty_type_converter
{
    static RTTR_INLINE bool convert_to(const T& value, argument& arg)
    {
        return false;
    }
};

/////////////////////////////////////////////////////////////////////////////////////////

/*!
 * The default implementation of the converter class for all basic types.
 *
 * That are:
 *  - all integral fixed types: int8_t till uint64_t
 *  - all floating point types
 *  - std::string
 */
template<typename T>
struct convert_from
{
    static RTTR_INLINE bool to(const T& from, bool& to)
    {
        return false;
    }

    static RTTR_INLINE bool to(const T& from, char& to)
    {
        return false;
    }

    static RTTR_INLINE bool to(const T& from, int8_t& to)
    {
        return false;
    }

    static RTTR_INLINE bool to(const T& from, int16_t& to)
    {
        return false;
    }

    static RTTR_INLINE bool to(const T& from, int32_t& to)
    {
        return false;
    }

    static RTTR_INLINE bool to(const T& from, int64_t& to)
    {
        return false;
    }

    static RTTR_INLINE bool to(const T& from, uint8_t& to)
    {
        return false;
    }

    static RTTR_INLINE bool to(const T& from, uint16_t& to)
    {
        return false;
    }

    static RTTR_INLINE bool to(const T& from, uint32_t& to)
    {
        return false;
    }

    static RTTR_INLINE bool to(const T& from, uint64_t& to)
    {
        return false;
    }

    static RTTR_INLINE bool to(const T& from, float& to)
    {
        return false;
    }

    static RTTR_INLINE bool to(const T& from, double& to)
    {
        return false;
    }

    static RTTR_INLINE bool to(const T& from, std::string& to)
    {
        return false;
    }

    static RTTR_INLINE bool to_enum(const T& from, argument& to)
    {
        return false;
    }
};

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
// here come the specialization for several atomic types

template<>
struct RTTR_API convert_from<bool>
{
    static RTTR_INLINE bool to(const bool& from, bool& to)
    {
        to = from;
        return true;
    }

    static RTTR_INLINE bool to(const bool& from, char& to)
    {
        to = static_cast<char>(from ? 1 : 0);
        return true;
    }

    static RTTR_INLINE bool to(const bool& from, int8_t& to)
    {
        to = static_cast<int8_t>(from ? 1 : 0);
        return true;
    }

    static RTTR_INLINE bool to(const bool& from, int16_t& to)
    {
        to = static_cast<int16_t>(from ? 1 : 0);
        return true;
    }

    static RTTR_INLINE bool to(const bool& from, int32_t& to)
    {
        to = static_cast<int32_t>(from ? 1 : 0);
        return true;
    }

    static RTTR_INLINE bool to(const bool& from, int64_t& to)
    {
        to = static_cast<int64_t>(from ? 1 : 0);
        return true;
    }

    static RTTR_INLINE bool to(const bool& from, uint8_t& to)
    {
        to = static_cast<uint8_t>(from ? 1 : 0);
        return true;
    }

    static RTTR_INLINE bool to(const bool& from, uint16_t& to)
    {
        to = static_cast<uint16_t>(from ? 1 : 0);
        return true;
    }

    static RTTR_INLINE bool to(const bool& from, uint32_t& to)
    {
        to = static_cast<uint32_t>(from ? 1 : 0);
        return true;
    }

    static RTTR_INLINE bool to(const bool& from, uint64_t& to)
    {
        to = static_cast<uint64_t>(from ? 1 : 0);
        return true;
    }

    static RTTR_INLINE bool to(const bool& from, float& to)
    {
        to = static_cast<float>(from ? 1.0f : 0.0f);
        return true;
    }

    static RTTR_INLINE bool to(const bool& from, double& to)
    {
        to = static_cast<double>(from ? 1.0 : 0.0);
        return true;
    }

    static RTTR_INLINE bool to(const bool& from, std::string& to)
    {
        to = (from ? "true" : "false");
        return true;
    }

    static RTTR_INLINE bool to_enum(const bool& from, argument& to)
    {
        return to_enumeration(from, to);
    }
};

/////////////////////////////////////////////////////////////////////////////////////////

template<>
struct RTTR_API convert_from<char>
{
    static RTTR_INLINE bool to(const char& from, bool& to)
    {
        to = (from != '\0');
        return true;
    }

    static RTTR_INLINE bool to(const char& from, char& to)
    {
        to = from;
        return true;
    }

    static RTTR_INLINE bool to(const char& from, int8_t& to)
    {
        to = static_cast<int8_t>(from);
        return true;
    }

    static RTTR_INLINE bool to(const char& from, int16_t& to)
    {
        to = static_cast<int16_t>(from);
        return true;
    }

    static RTTR_INLINE bool to(const char& from, int32_t& to)
    {
        to = static_cast<int32_t>(from);
        return true;
    }

    static RTTR_INLINE bool to(const char& from, int64_t& to)
    {
        to = static_cast<int64_t>(from);
        return true;
    }

    static RTTR_INLINE bool to(const char& from, uint8_t& to)
    {
        return convert_to(from, to);
    }

    static RTTR_INLINE bool to(const char& from, uint16_t& to)
    {
        return convert_to(from, to);
    }

    static RTTR_INLINE bool to(const char& from, uint32_t& to)
    {
        return convert_to(from, to);
    }

    static RTTR_INLINE bool to(const char& from, uint64_t& to)
    {
        return convert_to(from, to);
    }

    static RTTR_INLINE bool to(const char& from, float& to)
    {
        to = static_cast<float>(from);
        return true;
    }

    static RTTR_INLINE bool to(const char& from, double& to)
    {
        to = static_cast<double>(from);
        return true;
    }

    static RTTR_INLINE bool to(const char& from, std::string& to)
    {
        to = std::string(1, from);
        return true;
    }

    static RTTR_INLINE bool to_enum(const char& from, argument& to)
    {
        return to_enumeration(from, to);
    }
};

/////////////////////////////////////////////////////////////////////////////////////////

template<>
struct RTTR_API convert_from<int8_t>
{
    static RTTR_INLINE bool to(const int8_t& from, bool& to)
    {
        to = (from != 0);
        return true;
    }

    static RTTR_INLINE bool to(const int8_t& from, char& to)
    {
        to = static_cast<char>(from);
        return true;
    }

    static RTTR_INLINE bool to(const int8_t& from, int8_t& to)
    {
        to = from;
        return true;
    }

    static RTTR_INLINE bool to(const int8_t& from, int16_t& to)
    {
        to = from;
        return true;
    }

    static RTTR_INLINE bool to(const int8_t& from, int32_t& to)
    {
        to = from;
        return true;
    }

    static RTTR_INLINE bool to(const int8_t& from, int64_t& to)
    {
        to = from;
        return true;
    }

    static RTTR_INLINE bool to(const int8_t& from, uint8_t& to)
    {
        return convert_to(from, to);
    }

    static RTTR_INLINE bool to(const int8_t& from, uint16_t& to)
    {
        return convert_to(from, to);
    }

    static RTTR_INLINE bool to(const int8_t& from, uint32_t& to)
    {
        return convert_to(from, to);
    }

    static RTTR_INLINE bool to(const int8_t& from, uint64_t& to)
    {
        return convert_to(from, to);
    }

    static RTTR_INLINE bool to(const int8_t& from, float& to)
    {
        to = static_cast<float>(from);
        return true;
    }

    static RTTR_INLINE bool to(const int8_t& from, double& to)
    {
        to = static_cast<double>(from);
        return true;
    }

    static RTTR_INLINE bool to(const int8_t& from, std::string& to)
    {
        return convert_to(from, to);
    }

    static RTTR_INLINE bool to_enum(const int8_t& from, argument& to)
    {
        return to_enumeration(from, to);
    }
};

/////////////////////////////////////////////////////////////////////////////////////////

template<>
struct RTTR_API convert_from<int16_t>
{
    static RTTR_INLINE bool to(const int16_t& from, bool& to)
    {
        to = (from != 0);
        return true;
    }

    static RTTR_INLINE bool to(const int16_t& from, char& to)
    {
        return convert_to(from, to);
    }

    static RTTR_INLINE bool to(const int16_t& from, int8_t& to)
    {
        return convert_to(from, to);
    }

    static RTTR_INLINE bool to(const int16_t& from, int16_t& to)
    {
        to = from;
        return true;
    }

    static RTTR_INLINE bool to(const int16_t& from, int32_t& to)
    {
        to = from;
        return true;
    }

    static RTTR_INLINE bool to(const int16_t& from, int64_t& to)
    {
        to = from;
        return true;
    }

    static RTTR_INLINE bool to(const int16_t& from, uint8_t& to)
    {
        return convert_to(from, to);
    }

    static RTTR_INLINE bool to(const int16_t& from, uint16_t& to)
    {
        return convert_to(from, to);
    }

    static RTTR_INLINE bool to(const int16_t& from, uint32_t& to)
    {
        return convert_to(from, to);
    }

    static RTTR_INLINE bool to(const int16_t& from, uint64_t& to)
    {
        return convert_to(from, to);
    }

    static RTTR_INLINE bool to(const int16_t& from, float& to)
    {
        to = static_cast<float>(from);
        return true;
    }

    static RTTR_INLINE bool to(const int16_t& from, double& to)
    {
        to = static_cast<double>(from);
        return true;
    }

    static RTTR_INLINE bool to(const int16_t& from, std::string& to)
    {
        return convert_to(from, to);
    }

    static RTTR_INLINE bool to_enum(const int16_t& from, argument& to)
    {
        return to_enumeration(from, to);
    }
};

/////////////////////////////////////////////////////////////////////////////////////////

template<>
struct RTTR_API convert_from<int32_t>
{
    static RTTR_INLINE bool to(const int32_t& from, bool& to)
    {
        to = (from != 0);
        return true;
    }

    static RTTR_INLINE bool to(const int32_t& from, char& to)
    {
        return convert_to(from, to);
    }

    static RTTR_INLINE bool to(const int32_t& from, int8_t& to)
    {
        return convert_to(from, to);
    }

    static RTTR_INLINE bool to(const int32_t& from, int16_t& to)
    {
        return convert_to(from, to);
    }

    static RTTR_INLINE bool to(const int32_t& from, int32_t& to)
    {
        to = from;
        return true;
    }

    static RTTR_INLINE bool to(const int32_t& from, int64_t& to)
    {
        to = from;
        return true;
    }

    static RTTR_INLINE bool to(const int32_t& from, uint8_t& to)
    {
        return convert_to(from, to);
    }

    static RTTR_INLINE bool to(const int32_t& from, uint16_t& to)
    {
        return convert_to(from, to);
    }

    static RTTR_INLINE bool to(const int32_t& from, uint32_t& to)
    {
        return convert_to(from, to);
    }

    static RTTR_INLINE bool to(const int32_t& from, uint64_t& to)
    {
        return convert_to(from, to);
    }

    static RTTR_INLINE bool to(const int32_t& from, float& to)
    {
        to = static_cast<float>(from);
        return true;
    }

    static RTTR_INLINE bool to(const int32_t& from, double& to)
    {
        to = static_cast<double>(from);
        return true;
    }

    static RTTR_INLINE bool to(const int32_t& from, std::string& to)
    {
        return convert_to(from, to);
    }

    static RTTR_INLINE bool to_enum(const int32_t& from, argument& to)
    {
        return to_enumeration(from, to);
    }
};

/////////////////////////////////////////////////////////////////////////////////////////

template<>
struct RTTR_API convert_from<int64_t>
{
    static RTTR_INLINE bool to(const int64_t& from, bool& to)
    {
        to = (from != 0);
        return true;
    }

    static RTTR_INLINE bool to(const int64_t& from, char& to)
    {
        return convert_to(from, to);
    }

    static RTTR_INLINE bool to(const int64_t& from, int8_t& to)
    {
        return convert_to(from, to);
    }

    static RTTR_INLINE bool to(const int64_t& from, int16_t& to)
    {
        return convert_to(from, to);
    }

    static RTTR_INLINE bool to(const int64_t& from, int32_t& to)
    {
        return convert_to(from, to);
    }

    static RTTR_INLINE bool to(const int64_t& from, int64_t& to)
    {
        to = from;
        return true;
    }

    static RTTR_INLINE bool to(const int64_t& from, uint8_t& to)
    {
        return convert_to(from, to);
    }

    static RTTR_INLINE bool to(const int64_t& from, uint16_t& to)
    {
        return convert_to(from, to);
    }

    static RTTR_INLINE bool to(const int64_t& from, uint32_t& to)
    {
        return convert_to(from, to);
    }

    static RTTR_INLINE bool to(const int64_t& from, uint64_t& to)
    {
        return convert_to(from, to);
    }

    static RTTR_INLINE bool to(const int64_t& from, float& to)
    {
        to = static_cast<float>(from);
        return true;
    }

    static RTTR_INLINE bool to(const int64_t& from, double& to)
    {
        to = static_cast<double>(from);
        return true;
    }

    static RTTR_INLINE bool to(const int64_t& from, std::string& to)
    {
        return convert_to(from, to);
    }

    static RTTR_INLINE bool to_enum(const int64_t& from, argument& to)
    {
        return to_enumeration(from, to);
    }
};

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

template<>
struct RTTR_API convert_from<uint8_t>
{
    static RTTR_INLINE bool to(const uint8_t& from, bool& to)
    {
        to = (from != 0);
        return true;
    }

    static RTTR_INLINE bool to(const uint8_t& from, char& to)
    {
        return convert_to(from, to);
    }

    static RTTR_INLINE bool to(const uint8_t& from, int8_t& to)
    {
        return convert_to(from, to);
    }

    static RTTR_INLINE bool to(const uint8_t& from, int16_t& to)
    {
        return convert_to(from, to);
    }

    static RTTR_INLINE bool to(const uint8_t& from, int32_t& to)
    {
        to = static_cast<int32_t>(from);
        return true;
    }

    static RTTR_INLINE bool to(const uint8_t& from, int64_t& to)
    {
        to = static_cast<int64_t>(from);
        return true;
    }

    static RTTR_INLINE bool to(const uint8_t& from, uint8_t& to)
    {
        to = from;
        return true;
    }

    static RTTR_INLINE bool to(const uint8_t& from, uint16_t& to)
    {
        to = from;
        return true;
    }

    static RTTR_INLINE bool to(const uint8_t& from, uint32_t& to)
    {
        to = from;
        return true;
    }

    static RTTR_INLINE bool to(const uint8_t& from, uint64_t& to)
    {
        to = from;
        return true;
    }

    static RTTR_INLINE bool to(const uint8_t& from, float& to)
    {
        to = static_cast<float>(from);
        return true;
    }

    static RTTR_INLINE bool to(const uint8_t& from, double& to)
    {
        to = static_cast<double>(from);
        return true;
    }

    static RTTR_INLINE bool to(const uint8_t& from, std::string& to)
    {
        return convert_to(from, to);
    }

    static RTTR_INLINE bool to_enum(const uint8_t& from, argument& to)
    {
        return to_enumeration(from, to);
    }
};

/////////////////////////////////////////////////////////////////////////////////////////

template<>
struct RTTR_API convert_from<uint16_t>
{
    static RTTR_INLINE bool to(const uint16_t& from, bool& to)
    {
        to = (from != 0);
        return true;
    }

    static RTTR_INLINE bool to(const uint16_t& from, char& to)
    {
        return convert_to(from, to);
    }

    static RTTR_INLINE bool to(const uint16_t& from, int8_t& to)
    {
        return convert_to(from, to);
    }

    static RTTR_INLINE bool to(const uint16_t& from, int16_t& to)
    {
        return convert_to(from, to);
    }

    static RTTR_INLINE bool to(const uint16_t& from, int32_t& to)
    {
        return convert_to(from, to);
    }

    static RTTR_INLINE bool to(const uint16_t& from, int64_t& to)
    {
        to = static_cast<int64_t>(from);
        return true;
    }

    static RTTR_INLINE bool to(const uint16_t& from, uint8_t& to)
    {
        return convert_to(from, to);
    }

    static RTTR_INLINE bool to(const uint16_t& from, uint16_t& to)
    {
        to = from;
        return true;
    }

    static RTTR_INLINE bool to(const uint16_t& from, uint32_t& to)
    {
        to = from;
        return true;
    }

    static RTTR_INLINE bool to(const uint16_t& from, uint64_t& to)
    {
        to = from;
        return true;
    }

    static RTTR_INLINE bool to(const uint16_t& from, float& to)
    {
        to = static_cast<float>(from);
        return true;
    }

    static RTTR_INLINE bool to(const uint16_t& from, double& to)
    {
        to = static_cast<double>(from);
        return true;
    }

    static RTTR_INLINE bool to(const uint16_t& from, std::string& to)
    {
        return convert_to(from, to);
    }

    static RTTR_INLINE bool to_enum(const uint16_t& from, argument& to)
    {
        return to_enumeration(from, to);
    }
};

/////////////////////////////////////////////////////////////////////////////////////////

template<>
struct RTTR_API convert_from<uint32_t>
{
    static RTTR_INLINE bool to(const uint32_t& from, bool& to)
    {
        to = (from != 0);
        return true;
    }

    static RTTR_INLINE bool to(const uint32_t& from, char& to)
    {
        return convert_to(from, to);
    }

    static RTTR_INLINE bool to(const uint32_t& from, int8_t& to)
    {
        return convert_to(from, to);
    }

    static RTTR_INLINE bool to(const uint32_t& from, int16_t& to)
    {
        return convert_to(from, to);
    }

    static RTTR_INLINE bool to(const uint32_t& from, int32_t& to)
    {
        return convert_to(from, to);
    }

    static RTTR_INLINE bool to(const uint32_t& from, int64_t& to)
    {
        return convert_to(from, to);
    }

    static RTTR_INLINE bool to(const uint32_t& from, uint8_t& to)
    {
        return convert_to(from, to);
    }

    static RTTR_INLINE bool to(const uint32_t& from, uint16_t& to)
    {
        return convert_to(from, to);
    }

    static RTTR_INLINE bool to(const uint32_t& from, uint32_t& to)
    {
        to = from;
        return true;
    }

    static RTTR_INLINE bool to(const uint32_t& from, uint64_t& to)
    {
        to = from;
        return true;
    }

    static RTTR_INLINE bool to(const uint32_t& from, float& to)
    {
        to = static_cast<float>(from);
        return true;
    }

    static RTTR_INLINE bool to(const uint32_t& from, double& to)
    {
        to = static_cast<double>(from);
        return true;
    }

    static RTTR_INLINE bool to(const uint32_t& from, std::string& to)
    {
        return convert_to(from, to);
    }

    static RTTR_INLINE bool to_enum(const uint32_t& from, argument& to)
    {
        return to_enumeration(from, to);
    }
};

/////////////////////////////////////////////////////////////////////////////////////////

template<>
struct RTTR_API convert_from<uint64_t>
{
    static RTTR_INLINE bool to(const uint64_t& from, bool& to)
    {
        to = (from != 0);
        return true;
    }

    static RTTR_INLINE bool to(const uint64_t& from, char& to)
    {
        return convert_to(from, to);
    }

    static RTTR_INLINE bool to(const uint64_t& from, int8_t& to)
    {
        return convert_to(from, to);
    }

    static RTTR_INLINE bool to(const uint64_t& from, int16_t& to)
    {
        return convert_to(from, to);
    }

    static RTTR_INLINE bool to(const uint64_t& from, int32_t& to)
    {
        return convert_to(from, to);
    }

    static RTTR_INLINE bool to(const uint64_t& from, int64_t& to)
    {
        return convert_to(from, to);
    }

    static RTTR_INLINE bool to(const uint64_t& from, uint8_t& to)
    {
        return convert_to(from, to);
    }

    static RTTR_INLINE bool to(const uint64_t& from, uint16_t& to)
    {
        return convert_to(from, to);
    }

    static RTTR_INLINE bool to(const uint64_t& from, uint32_t& to)
    {
        return convert_to(from, to);
    }

    static RTTR_INLINE bool to(const uint64_t& from, uint64_t& to)
    {
        to = from;
        return true;
    }

    static RTTR_INLINE bool to(const uint64_t& from, float& to)
    {
        to = static_cast<float>(from);
        return true;
    }

    static RTTR_INLINE bool to(const uint64_t& from, double& to)
    {
        to = static_cast<double>(from);
        return true;
    }

    static RTTR_INLINE bool to(const uint64_t& from, std::string& to)
    {
        return convert_to(from, to);
    }

    static RTTR_INLINE bool to_enum(const uint64_t& from, argument& to)
    {
        return to_enumeration(from, to);
    }
};

/////////////////////////////////////////////////////////////////////////////////////////

template<>
struct RTTR_API convert_from<float>
{
    static RTTR_INLINE bool to(const float& from, bool& to)
    {
        to = !(from <= std::numeric_limits<float>::min() &&
               from >= -1 * std::numeric_limits<float>::min());

        return true;
    }

    static RTTR_INLINE bool to(const float& from, char& to)
    {
        return convert_to(from, to);
    }

    static RTTR_INLINE bool to(const float& from, int8_t& to)
    {
        return convert_to(from, to);
    }

    static RTTR_INLINE bool to(const float& from, int16_t& to)
    {
        return convert_to(from, to);
    }

    static RTTR_INLINE bool to(const float& from, int32_t& to)
    {
        return convert_to(from, to);
    }

    static RTTR_INLINE bool to(const float& from, int64_t& to)
    {
        return convert_to(from, to);
    }

    static RTTR_INLINE bool to(const float& from, uint8_t& to)
    {
        return convert_to(from, to);
    }

    static RTTR_INLINE bool to(const float& from, uint16_t& to)
    {
        return convert_to(from, to);
    }

    static RTTR_INLINE bool to(const float& from, uint32_t& to)
    {
        return convert_to(from, to);
    }

    static RTTR_INLINE bool to(const float& from, uint64_t& to)
    {
        return convert_to(from, to);
    }

    static RTTR_INLINE bool to(const float& from, float& to)
    {
        to = from;
        return true;
    }

    static RTTR_INLINE bool to(const float& from, double& to)
    {
        to = from;
        return true;
    }

    static RTTR_INLINE bool to(const float& from, std::string& to)
    {
        return convert_to(from, to);
    }

    static RTTR_INLINE bool to_enum(const float& from, argument& to)
    {
        return to_enumeration(from, to);
    }
};

/////////////////////////////////////////////////////////////////////////////////////////

template<>
struct RTTR_API convert_from<double>
{
    static RTTR_INLINE bool to(const double& from, bool& to)
    {
        to = !(from <= std::numeric_limits<double>::min() &&
               from >= -1 * std::numeric_limits<double>::min());

        return true;
    }

    static RTTR_INLINE bool to(const double& from, char& to)
    {
        return convert_to(from, to);
    }

    static RTTR_INLINE bool to(const double& from, int8_t& to)
    {
        return convert_to(from, to);
    }

    static RTTR_INLINE bool to(const double& from, int16_t& to)
    {
        return convert_to(from, to);
    }

    static RTTR_INLINE bool to(const double& from, int32_t& to)
    {
        return convert_to(from, to);
    }

    static RTTR_INLINE bool to(const double& from, int64_t& to)
    {
        return convert_to(from, to);
    }

    static RTTR_INLINE bool to(const double& from, uint8_t& to)
    {
        return convert_to(from, to);
    }

    static RTTR_INLINE bool to(const double& from, uint16_t& to)
    {
        return convert_to(from, to);
    }

    static RTTR_INLINE bool to(const double& from, uint32_t& to)
    {
        return convert_to(from, to);
    }

    static RTTR_INLINE bool to(const double& from, uint64_t& to)
    {
        return convert_to(from, to);
    }

    static RTTR_INLINE bool to(const double& from, float& to)
    {
        RTTR_CONSTEXPR_OR_CONST double float_min = -1 * std::numeric_limits<float>::max();
        RTTR_CONSTEXPR_OR_CONST double float_max = std::numeric_limits<float>::max();
        if (from < float_min || from > float_max)
            return false;

        to = static_cast<float>(from);
        return true;
    }

    static RTTR_INLINE bool to(const double& from, double& to)
    {
        to = from;
        return true;
    }

    static RTTR_INLINE bool to(const double& from, std::string& to)
    {
        return convert_to(from, to);
    }

    static RTTR_INLINE bool to_enum(const double& from, argument& to)
    {
        return to_enumeration(from, to);
    }
};

/////////////////////////////////////////////////////////////////////////////////////////

template<>
struct RTTR_API convert_from<std::string>
{
    static RTTR_INLINE bool to(const std::string& from, bool& to)
    {
        bool ok;
        to = string_to_bool(from, &ok);
        return ok;
    }

    static RTTR_INLINE bool to(const std::string& from, char& to)
    {
        const auto& val = from;
        if (val.empty())
            to ='\0';
        else
            to = val[0];

        return true;
    }

    static RTTR_INLINE bool to(const std::string& from, int8_t& to)
    {
        bool ok;
        int val = string_to_int(from, &ok);
        if (!ok)
            return false;

        return convert_to(val, to);
    }

    static RTTR_INLINE bool to(const std::string& from, int16_t& to)
    {
        bool ok;
        int val = string_to_int(from, &ok);
        if (!ok)
            return false;

        return convert_to(val, to);
    }

    static RTTR_INLINE bool to(const std::string& from, int32_t& to)
    {
        bool ok;
        to = string_to_int(from, &ok);
        return ok;
    }

    static RTTR_INLINE bool to(const std::string& from, int64_t& to)
    {
        bool ok;
        to = string_to_long_long(from, &ok);
        return ok;
    }

    static RTTR_INLINE bool to(const std::string& from, uint8_t& to)
    {
        bool ok;
        unsigned int val = string_to_int(from, &ok);
        if (!ok)
            return false;

        return convert_to(val, to);
    }

    static RTTR_INLINE bool to(const std::string& from, uint16_t& to)
    {
        bool ok;
        unsigned int val = string_to_int(from, &ok);
        if (!ok)
            return false;

        return convert_to(val, to);
    }

    static RTTR_INLINE bool to(const std::string& from, uint32_t& to)
    {
        bool ok;
        const auto val = string_to_ulong(from, &ok);
        if (!ok)
            return false;

        return convert_to(val, to);
    }

    static RTTR_INLINE bool to(const std::string& from, uint64_t& to)
    {
        bool ok;
        const auto val = string_to_ulong_long(from, &ok);
        if (!ok)
            return false;

        return convert_to(val, to);
    }

    static RTTR_INLINE bool to(const std::string& from, float& to)
    {
        bool ok;
        to = string_to_float(from, &ok);
        return ok;
    }

    static RTTR_INLINE bool to(const std::string& from, double& to)
    {
        bool ok;
        to = string_to_double(from, &ok);
        return ok;
    }

    static RTTR_INLINE bool to(const std::string& from, std::string& to)
    {
        to = from;
        return true;
    }

    static RTTR_INLINE bool to_enum(const std::string& from, argument& to)
    {
        return to_enumeration(string_view(from), to);
    }

};

/////////////////////////////////////////////////////////////////////////////////////////

// MSVC generates following warning: 'warning C4800: 'const enum_bool' : forcing value to bool 'true' or 'false' (performance warning)'
// For unknown reason the MSVC compiler is too dump to recognize that I can safely convert an enumeration
// with underlying type bool, to type bool (thats no int to bool conversion!)
// so we disable the warning for enum conversions:
#if  RTTR_COMPILER == RTTR_COMPILER_MSVC

#pragma warning(push)
#pragma warning(disable:4800)

#endif

template<typename T>
struct convert_from_enum
{
    template<typename T_>
    using enum_type_t = typename std::underlying_type<T_>::type;

    static RTTR_INLINE enum_type_t<T> get_underlying_value(const T& from)
    {
        return static_cast<enum_type_t<T>>(from);
    }

    template<typename T1>
    static RTTR_INLINE
    enable_if_t<!std::is_same<bool, enum_type_t<T1> >::value, bool>
    to(const T1& from, bool& to)
    {
        const auto value = get_underlying_value(from);
        if (value == 0)
        {
            to = false;
            return true;
        }
        else if (value == 1)
        {
            to = true;
            return true;
        }
        else
        {
            return false;
        }
    }

    template<typename T1>
    static RTTR_INLINE
    enable_if_t<std::is_same<bool, enum_type_t<T1> >::value, bool>
    to(const T1& from, bool& to)
    {
        // for unknown reason MSVC will here scream a warning 'C4800'...
        to = static_cast<bool>(from);
        return true;
    }

    static RTTR_INLINE bool to(const T& from, char& to)
    {
        return convert_from<enum_type_t<T>>::to(get_underlying_value(from), to);
    }

    static RTTR_INLINE bool to(const T& from, int8_t& to)
    {
        return convert_from<enum_type_t<T>>::to(get_underlying_value(from), to);
    }

    static RTTR_INLINE bool to(const T& from, int16_t& to)
    {
        return convert_from<enum_type_t<T>>::to(get_underlying_value(from), to);
    }

    static RTTR_INLINE bool to(const T& from, int32_t& to)
    {
        return convert_from<enum_type_t<T>>::to(get_underlying_value(from), to);
    }

    static RTTR_INLINE bool to(const T& from, int64_t& to)
    {
        return convert_from<enum_type_t<T>>::to(get_underlying_value(from), to);
    }

    static RTTR_INLINE bool to(const T& from, uint8_t& to)
    {
        return convert_from<enum_type_t<T>>::to(get_underlying_value(from), to);
    }

    static RTTR_INLINE bool to(const T& from, uint16_t& to)
    {
        return convert_from<enum_type_t<T>>::to(get_underlying_value(from), to);
    }

    static RTTR_INLINE bool to(const T& from, uint32_t& to)
    {
        return convert_from<enum_type_t<T>>::to(get_underlying_value(from), to);
    }

    static RTTR_INLINE bool to(const T& from, uint64_t& to)
    {
        return convert_from<enum_type_t<T>>::to(get_underlying_value(from), to);
    }

    static RTTR_INLINE bool to(const T& from, float& to)
    {
        return convert_from<enum_type_t<T>>::to(get_underlying_value(from), to);
    }

    static RTTR_INLINE bool to(const T& from, double& to)
    {
        return convert_from<enum_type_t<T>>::to(get_underlying_value(from), to);
    }

    static RTTR_INLINE bool to(const T& from, std::string& to)
    {
        to = get_enumeration_name(from).to_string();
        return (to.empty() == false);
    }

    static RTTR_INLINE bool to_enum(const T& from, argument& to)
    {
        return false;
    }
};

/////////////////////////////////////////////////////////////////////////////////////////

#if  RTTR_COMPILER == RTTR_COMPILER_MSVC
// restore warning level
#pragma warning(pop)

#endif

/////////////////////////////////////////////////////////////////////////////////////////

} // end namespace detail
} // end namespace rttr

#endif // RTTR_VARIANT_DATA_CONVERTER_H_
