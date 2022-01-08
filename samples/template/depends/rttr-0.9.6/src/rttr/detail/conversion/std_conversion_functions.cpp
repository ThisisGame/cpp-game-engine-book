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

#include "rttr/detail/conversion/std_conversion_functions.h"

#include "rttr/detail/conversion/number_conversion.h"

#include <sstream>
#include <locale>
#include <limits>
#include <algorithm>
#include <climits>
#include <iomanip>

namespace rttr
{
namespace detail
{
/////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
std::string to_string_impl(T value, bool* ok)
{
    try
    {
        std::string text = std::to_string(value);
        if (ok)
            *ok = true;
        return text;
    }
    catch (...)
    {
        if (ok)
            *ok = false;
        return std::string();
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

std::string to_string(int value, bool* ok)
{
    return to_string_impl(value, ok);
}

/////////////////////////////////////////////////////////////////////////////////////////

std::string to_string(long value, bool* ok)
{
    return to_string_impl(value, ok);
}

/////////////////////////////////////////////////////////////////////////////////////////

std::string to_string(long long value, bool* ok)
{
    return to_string_impl(value, ok);
}

/////////////////////////////////////////////////////////////////////////////////////////

std::string to_string(unsigned value, bool* ok)
{
    return to_string_impl(value, ok);
}

/////////////////////////////////////////////////////////////////////////////////////////

std::string to_string(unsigned long value, bool* ok)
{
    return to_string_impl(value, ok);
}

/////////////////////////////////////////////////////////////////////////////////////////

std::string to_string(unsigned long long value, bool* ok)
{
    return to_string_impl(value, ok);
}

/////////////////////////////////////////////////////////////////////////////////////////

std::string to_string(float value, bool* ok)
{
    try
    {
        std::stringstream ss;
        ss << std::setprecision(std::numeric_limits<float>::digits10) << value;
        if (ok)
            *ok = true;
        return ss.str();
    }
    catch (...)
    {
        if (ok)
            *ok = false;
        return std::string();
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

std::string to_string(double value, bool* ok)
{
    try
    {
        std::stringstream ss;
        ss << std::setprecision(std::numeric_limits<double>::digits10) << value;
        if (ok)
            *ok = true;
        return ss.str();
    }
    catch (...)
    {
        if (ok)
            *ok = false;
        return std::string();
    }
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

bool string_to_bool(std::string text, bool* ok)
{
    std::transform(text.begin(), text.end(), text.begin(), [](char ch) { return std::tolower(ch, std::locale::classic()); });
    text.erase( std::remove_if( text.begin(), text.end(),  [](char ch) { return std::isspace(ch, std::locale::classic() ); } ), text.end() );

    if (text == "false" || text == "0" || text.empty())
    {
        if (ok)
            *ok = true;
        return false;
    }

    if (ok)
        *ok = true;

    return true;
}

/////////////////////////////////////////////////////////////////////////////////////////

int string_to_int(const std::string& source, bool* ok)
{
    try
    {
        std::size_t pos = 0;
        const int value = std::stoi(source, &pos);
        if (pos == source.length())
        {
            if (ok)
                *ok = true;
            return value;
        }
    }
    catch (...)
    {
        if (ok)
            *ok = false;
        return 0;
    }

    if (ok)
        *ok = false;
    return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////

unsigned long string_to_ulong(const std::string& source, bool* ok)
{
    try
    {
        std::size_t pos = 0;
        const long long value = std::stoll(source, &pos);
        unsigned long result = 0;
        if (pos == source.length() && convert_to(value, result))
        {
            if (ok)
                *ok = true;
            return result;
        }
    }
    catch (...)
    {
        if (ok)
            *ok = false;
        return 0;
    }

    if (ok)
        *ok = false;
    return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////

long long string_to_long_long(const std::string& source, bool* ok)
{
    try
    {
        std::size_t pos = 0;
        const long long value = std::stoll(source, &pos);
        if (pos == source.length())
        {
            if (ok)
                *ok = true;
            return value;
        }
    }
    catch (...)
    {
        if (ok)
            *ok = false;
        return 0;
    }

    if (ok)
        *ok = false;
    return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////

unsigned long long string_to_ulong_long(const std::string& source, bool* ok)
{
    try
    {
        std::size_t pos = 0;
        const auto itr = std::find_if(source.begin(), source.end(), [](char c){ return !std::isdigit(c, std::locale()); });
        if (itr == source.end())
        {
            const unsigned long long value = std::stoull(source, &pos);
            if (pos == source.length())
            {
                if (ok)
                    *ok = true;
                return value;
            }
        }
    }
    catch (...)
    {
        if (ok)
            *ok = false;
        return 0;
    }

    if (ok)
        *ok = false;
    return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////

float string_to_float(const std::string& source, bool* ok)
{
    try
    {
        std::size_t pos = 0;
        const float value = std::stof(source, &pos);
        if (pos == source.length())
        {
            if (ok)
                *ok = true;
            return value;
        }
    }
    catch (...)
    {
        if (ok)
            *ok = false;
        return 0;
    }

    if (ok)
        *ok = false;
    return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////

double string_to_double(const std::string& source, bool* ok)
{
    try
    {
        std::size_t pos = 0;
        const double value = std::stod(source, &pos);
        if (pos == source.length())
        {
            if (ok)
                *ok = true;
            return value;
        }
    }
    catch (...)
    {
        if (ok)
            *ok = false;
        return 0;
    }

    if (ok)
        *ok = false;

    return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////

} // end namespace detail
} // end namespace rttr
