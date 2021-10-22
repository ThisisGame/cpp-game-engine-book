/************************************************************************
* file name         : hashed_str.h
* ----------------- :
* creation time     : 2016/09/11
* author            : Victor Zarubkin
* email             : v.s.zarubkin@gmail.com
* ----------------- :
* description       : The file contains definition of C-strings with calculated hash-code.
*                   : These strings may be used as optimized keys for std::unordered_map.
* ----------------- :
* change log        : * 2016/09/11 Victor Zarubkin: Initial commit. Moved sources from reader.cpp
*                   : 
*                   : *
* ----------------- :
* license           : Lightweight profiler library for c++
*                   : Copyright(C) 2016-2019  Sergey Yagovtsev, Victor Zarubkin
*                   :
*                   : Licensed under either of
*                   :     * MIT license (LICENSE.MIT or http://opensource.org/licenses/MIT)
*                   :     * Apache License, Version 2.0, (LICENSE.APACHE or http://www.apache.org/licenses/LICENSE-2.0)
*                   : at your option.
*                   :
*                   : The MIT License
*                   :
*                   : Permission is hereby granted, free of charge, to any person obtaining a copy
*                   : of this software and associated documentation files (the "Software"), to deal
*                   : in the Software without restriction, including without limitation the rights 
*                   : to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies 
*                   : of the Software, and to permit persons to whom the Software is furnished 
*                   : to do so, subject to the following conditions:
*                   : 
*                   : The above copyright notice and this permission notice shall be included in all 
*                   : copies or substantial portions of the Software.
*                   : 
*                   : THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
*                   : INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR 
*                   : PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE 
*                   : LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, 
*                   : TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE 
*                   : USE OR OTHER DEALINGS IN THE SOFTWARE.
*                   : 
*                   : The Apache License, Version 2.0 (the "License")
*                   :
*                   : You may not use this file except in compliance with the License.
*                   : You may obtain a copy of the License at
*                   :
*                   : http://www.apache.org/licenses/LICENSE-2.0
*                   :
*                   : Unless required by applicable law or agreed to in writing, software
*                   : distributed under the License is distributed on an "AS IS" BASIS,
*                   : WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*                   : See the License for the specific language governing permissions and
*                   : limitations under the License.
************************************************************************/

#ifndef EASY_PROFILER__HASHED_CSTR__H_
#define EASY_PROFILER__HASHED_CSTR__H_

#include <functional>
#include <string.h>
#include <string>

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

namespace profiler {

    class hashed_stdstring
    {
        ::std::string m_str;
        size_t       m_hash;

    public:

        hashed_stdstring(const char* _str) : m_str(_str), m_hash(::std::hash<::std::string>()(m_str))
        {
        }

        hashed_stdstring(const ::std::string& _str) : m_str(_str), m_hash(::std::hash<::std::string>()(m_str))
        {
        }

        hashed_stdstring(::std::string&& _str) : m_str(::std::forward<::std::string&&>(_str)), m_hash(::std::hash<::std::string>()(m_str))
        {
        }

        hashed_stdstring(hashed_stdstring&& _other) : m_str(::std::move(_other.m_str)), m_hash(_other.m_hash)
        {
        }

        hashed_stdstring(const char* _str, size_t _hash_code) : m_str(_str), m_hash(_hash_code)
        {
        }

        hashed_stdstring(const ::std::string& _str, size_t _hash_code) : m_str(_str), m_hash(_hash_code)
        {
        }

        hashed_stdstring(::std::string&& _str, size_t _hash_code) : m_str(::std::forward<::std::string&&>(_str)), m_hash(_hash_code)
        {
        }

        hashed_stdstring(const hashed_stdstring&) = default;
        hashed_stdstring& operator = (const hashed_stdstring&) = default;

        hashed_stdstring& operator = (hashed_stdstring&& _other)
        {
            m_str = ::std::move(_other.m_str);
            m_hash = _other.m_hash;
            return *this;
        }

        inline bool operator == (const hashed_stdstring& _other) const
        {
            return m_hash == _other.m_hash && m_str == _other.m_str;
        }

        inline bool operator != (const hashed_stdstring& _other) const
        {
            return !operator == (_other);
        }

        inline size_t hcode() const
        {
            return m_hash;
        }

        inline const char* c_str() const
        {
            return m_str.c_str();
        }

        inline size_t size() const
        {
            return m_str.size();
        }

    }; // END of class hashed_stdstring.

} // END of namespace profiler.

namespace std {

    /** \brief Simply returns precalculated hash of a std::string. */
    template <> struct hash<::profiler::hashed_stdstring> {
        typedef ::profiler::hashed_stdstring argument_type;
        typedef size_t                         result_type;
        inline size_t operator () (const ::profiler::hashed_stdstring& _str) const {
            return _str.hcode();
        }
    };

} // END of namespace std.

#if 0 == 1 //defined(_MSC_VER)// && _MSC_VER >= 1800
# define EASY_PROFILER_HASHED_CSTR_DEFINED

namespace profiler {

    /** \brief Simple C-string pointer with length.

    It is used as base class for a key in std::unordered_map.
    It is used to get better performance than std::string.
    It simply stores a pointer and a length, there is no
    any memory allocation and copy.

    \warning Make sure you know what you are doing. You have to be sure that
    pointed C-string will exist until you finish using this cstring.

    \ingroup profiler
    */
    class cstring
    {
    protected:

        const char* m_str;
        size_t      m_len;

    public:

        cstring(const char* _str) : m_str(_str), m_len(strlen(_str))
        {
        }

        cstring(const char* _str, size_t _len) : m_str(_str), m_len(_len)
        {
        }

        cstring(const cstring&) = default;
        cstring& operator = (const cstring&) = default;

        inline bool operator == (const cstring& _other) const
        {
            return m_len == _other.m_len && !strncmp(m_str, _other.m_str, m_len);
        }

        inline bool operator != (const cstring& _other) const
        {
            return !operator == (_other);
        }

        inline bool operator < (const cstring& _other) const
        {
            if (m_len == _other.m_len)
            {
                return strncmp(m_str, _other.m_str, m_len) < 0;
            }

            return m_len < _other.m_len;
        }

        inline const char* c_str() const
        {
            return m_str;
        }

        inline size_t size() const
        {
            return m_len;
        }

    }; // END of class cstring.

    /** \brief cstring with precalculated hash.

    This is used to calculate hash for C-string and to cache it
    to be used in the future without recurring hash calculatoin.

    \note This class is used as a key in std::unordered_map.

    \ingroup profiler
    */
    class hashed_cstr : public cstring
    {
        using Parent = cstring;

        size_t m_hash;

    public:

        hashed_cstr(const char* _str) : Parent(_str), m_hash(0)
        {
            m_hash = ::std::_Hash_seq((const unsigned char *)m_str, m_len);
        }

        hashed_cstr(const char* _str, size_t _hash_code) : Parent(_str), m_hash(_hash_code)
        {
        }

        hashed_cstr(const char* _str, size_t _len, size_t _hash_code) : Parent(_str, _len), m_hash(_hash_code)
        {
        }

        hashed_cstr(const hashed_cstr&) = default;
        hashed_cstr& operator = (const hashed_cstr&) = default;

        inline bool operator == (const hashed_cstr& _other) const
        {
            return m_hash == _other.m_hash && Parent::operator == (_other);
        }

        inline bool operator != (const hashed_cstr& _other) const
        {
            return !operator == (_other);
        }

        inline size_t hcode() const
        {
            return m_hash;
        }

    }; // END of class hashed_cstr.

    using string_with_hash = hashed_cstr;

} // END of namespace profiler.

namespace std {

    /** \brief Simply returns precalculated hash of a C-string. */
    template <> struct hash<::profiler::hashed_cstr>
    {
        using argument_type = ::profiler::hashed_cstr;
        using result_type = size_t;

        inline size_t operator () (const ::profiler::hashed_cstr& _str) const
        {
            return _str.hcode();
        }
    };

} // END of namespace std.

#else ////////////////////////////////////////////////////////////////////

// TODO: Create hashed_cstr for Linux (need to use Linux version of std::_Hash_seq)

namespace profiler {
    using string_with_hash = hashed_stdstring;
}

#endif

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#endif // EASY_PROFILER__HASHED_CSTR__H_
