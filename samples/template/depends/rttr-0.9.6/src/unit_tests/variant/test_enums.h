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

#ifndef TEST_ENUMS_H_
#define TEST_ENUMS_H_

#include <cstdint>

enum class variant_enum_test : uint8_t
{
    VALUE_0 = 0,
    VALUE_1 = 1,
    VALUE_2 = 2
};

/////////////////////////////////////////////////////////////////////////////////////////

enum class variant_enum_test_big : int32_t
{
    VALUE_0 = 500,
    VALUE_1 = 501,
    VALUE_2 = 502
};

/////////////////////////////////////////////////////////////////////////////////////////

enum class enum_bool : bool
{
    ON = true,
    OFF = false
};

/////////////////////////////////////////////////////////////////////////////////////////

enum class enum_int8_t : int8_t
{
    VALUE_1 = 100,
    VALUE_2,
    VALUE_3,
    VALUE_4,
    VALUE_NEG = -100
};

/////////////////////////////////////////////////////////////////////////////////////////

enum class enum_int16_t : int16_t
{
    VALUE_1 = 32760,
    VALUE_2,
    VALUE_3,
    VALUE_4,
    VALUE_NEG = -32760
};

/////////////////////////////////////////////////////////////////////////////////////////

enum class enum_int32_t : int32_t
{
    VALUE_1 = 2147483630,
    VALUE_2,
    VALUE_3,
    VALUE_4,
    VALUE_NEG = -2147483630
};

/////////////////////////////////////////////////////////////////////////////////////////

enum class enum_int64_t : int64_t
{
    VALUE_1 = 8223372036854775807L,
    VALUE_2,
    VALUE_3,
    VALUE_4,
    VALUE_NEG = -8223372036854775807L
};

/////////////////////////////////////////////////////////////////////////////////////////

enum class enum_uint8_t : uint8_t
{
    VALUE_1 = 200U,
    VALUE_2,
    VALUE_3,
    VALUE_4
};

/////////////////////////////////////////////////////////////////////////////////////////

enum class enum_uint16_t : uint16_t
{
    VALUE_1 = 65530,
    VALUE_2,
    VALUE_3,
    VALUE_4
};

/////////////////////////////////////////////////////////////////////////////////////////

enum class enum_uint32_t : uint32_t
{
    VALUE_1 = 4147483630U,
    VALUE_2,
    VALUE_3,
    VALUE_4
};

/////////////////////////////////////////////////////////////////////////////////////////

enum class enum_uint64_t : uint64_t
{
    VALUE_1 = 17446744073709551615U,
    VALUE_2,
    VALUE_3,
    VALUE_4
};

/////////////////////////////////////////////////////////////////////////////////////////

#endif // TEST_ENUMS_H_
