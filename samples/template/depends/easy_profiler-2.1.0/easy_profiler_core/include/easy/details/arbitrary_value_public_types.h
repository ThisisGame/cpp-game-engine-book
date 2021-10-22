/**
Lightweight profiler library for c++
Copyright(C) 2016-2019  Sergey Yagovtsev, Victor Zarubkin

Licensed under either of
    * MIT license (LICENSE.MIT or http://opensource.org/licenses/MIT)
    * Apache License, Version 2.0, (LICENSE.APACHE or http://www.apache.org/licenses/LICENSE-2.0)
at your option.

The MIT License
    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
    of the Software, and to permit persons to whom the Software is furnished
    to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
    INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
    PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
    LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
    TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
    USE OR OTHER DEALINGS IN THE SOFTWARE.


The Apache License, Version 2.0 (the "License");
    You may not use this file except in compliance with the License.
    You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

**/

#ifndef EASY_PROFILER_ARBITRARY_VALUE_PUBLIC_TYPES_H
#define EASY_PROFILER_ARBITRARY_VALUE_PUBLIC_TYPES_H

#include <easy/details/arbitrary_value_aux.h>
#include <easy/details/profiler_public_types.h>
#include <string.h>

namespace profiler
{

    enum class DataType : uint8_t
    {
        Bool = 0,
        Char,
        Int8,
        Uint8,
        Int16,
        Uint16,
        Int32,
        Uint32,
        Int64,
        Uint64,
        Float,
        Double,
        String,

        TypesCount
    }; // end of enum class DataType.

    template <DataType dataType> struct StdType;

    template <class T> struct StdToDataType EASY_FINAL {
        EASY_STATIC_CONSTEXPR auto data_type = DataType::TypesCount;
    };

# define EASY_DATATYPE_CONVERSION(DataTypeName, StdTypeName)\
    template <> struct StdType<DataTypeName> EASY_FINAL { using value_type = StdTypeName; };\
    template <> struct StdToDataType<StdTypeName> EASY_FINAL { EASY_STATIC_CONSTEXPR auto data_type = DataTypeName; }

    EASY_DATATYPE_CONVERSION(DataType::Bool  , bool    );
    EASY_DATATYPE_CONVERSION(DataType::Char  , char    );
    EASY_DATATYPE_CONVERSION(DataType::Int8  , int8_t  );
    EASY_DATATYPE_CONVERSION(DataType::Uint8 , uint8_t );
    EASY_DATATYPE_CONVERSION(DataType::Int16 , int16_t );
    EASY_DATATYPE_CONVERSION(DataType::Uint16, uint16_t);
    EASY_DATATYPE_CONVERSION(DataType::Int32 , int32_t );
    EASY_DATATYPE_CONVERSION(DataType::Uint32, uint32_t);
    EASY_DATATYPE_CONVERSION(DataType::Int64 , int64_t );
    EASY_DATATYPE_CONVERSION(DataType::Uint64, uint64_t);
    EASY_DATATYPE_CONVERSION(DataType::Float , float   );
    EASY_DATATYPE_CONVERSION(DataType::Double, double  );

# undef EASY_DATATYPE_CONVERSION

    template <> struct StdType<DataType::String> EASY_FINAL { using value_type = char; };
    template <> struct StdToDataType<const char*> EASY_FINAL { EASY_STATIC_CONSTEXPR auto data_type = DataType::String; };

} // end of namespace profiler.

#endif //EASY_PROFILER_ARBITRARY_VALUE_PUBLIC_TYPES_H
