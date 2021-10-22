/************************************************************************
* file name         : common_types.h
* ----------------- :
* creation time     : 2016/07/31
* author            : Victor Zarubkin
* email             : v.s.zarubkin@gmail.com
* ----------------- :
* description       : The file contains declaration of common types for both GraphicsView
*                   : and TreeWidget.
* ----------------- :
* change log        : * 2016/07/31 Victor Zarubkin: initial commit.
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

#ifndef EASY_PROFILER__GUI_COMMON_TYPES_H
#define EASY_PROFILER__GUI_COMMON_TYPES_H

#include <map>
#include <vector>
#include <easy/reader.h>
#include <QObject>

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

namespace profiler_gui {
    
#define EASY_GRAPHICS_ITEM_RECURSIVE_PAINT
//#undef  EASY_GRAPHICS_ITEM_RECURSIVE_PAINT

#pragma pack(push, 1)
struct EasyBlockItem Q_DECL_FINAL
{
    qreal                              x; ///< x coordinate of the item (this is made qreal=double to avoid mistakes on very wide scene)
    float                              w; ///< Width of the item
    ::profiler::block_index_t      block; ///< Index of profiler block

#ifndef EASY_GRAPHICS_ITEM_RECURSIVE_PAINT
    ::profiler::block_index_t neighbours; ///< Number of neighbours (parent.children.size())
    uint32_t              children_begin; ///< Index of first child item on the next sublevel
    int8_t                         state; ///< 0 = no change, 1 = paint, -1 = do not paint
#else
    ::profiler::block_index_t max_depth_child; ///< Index of child with maximum tree depth
    uint32_t              children_begin; ///< Index of first child item on the next sublevel
#endif

    // Possible optimizations:
    // 1) We can save 1 more byte per block if we will use char instead of short + real time calculations for "totalHeight" var;
    // 2) We can save 12 bytes per block if "x" and "w" vars will be removed (all this information exist inside BlocksTree),
    //      but this requires runtime x-coodinate calculation because BlocksTree has x value in nanoseconds.

    inline void setPos(qreal _x, float _w) { x = _x; w = _w; }
    inline qreal left() const { return x; }
    inline qreal right() const { return x + w; }
    inline float width() const { return w; }

}; // END of struct EasyBlockItem.

struct EasyBlock Q_DECL_FINAL
{
    ::profiler::BlocksTree       tree;
    uint32_t      graphics_item_index;
    uint8_t       graphics_item_level;
    uint8_t             graphics_item;
    bool                     expanded;

    EasyBlock() = default;

    EasyBlock(EasyBlock&& that) EASY_NOEXCEPT
        : tree(::std::move(that.tree))
        , graphics_item_index(that.graphics_item_index)
        , graphics_item_level(that.graphics_item_level)
        , graphics_item(that.graphics_item)
        , expanded(that.expanded)
    {
    }

    EasyBlock(const EasyBlock&) = delete;
};
#pragma pack(pop)

using EasyItems = ::std::vector<EasyBlockItem>;
using EasyBlocks = ::std::vector<EasyBlock>;

//////////////////////////////////////////////////////////////////////////

struct EasySelectedBlock Q_DECL_FINAL
{
    const ::profiler::BlocksTreeRoot* root = nullptr;
    ::profiler::block_index_t         tree = 0xffffffff;

    EasySelectedBlock() = default;

    EasySelectedBlock(const ::profiler::BlocksTreeRoot* _root, const ::profiler::block_index_t _tree) EASY_NOEXCEPT
        : root(_root)
        , tree(_tree)
    {
    }

}; // END of struct EasySelectedBlock.

typedef ::std::vector<EasySelectedBlock> TreeBlocks;

//////////////////////////////////////////////////////////////////////////

enum TimeUnits : int8_t
{
    TimeUnits_ms = 0,
    TimeUnits_us,
    TimeUnits_ns,
    TimeUnits_auto

}; // END of enum TimeUnits.

//////////////////////////////////////////////////////////////////////////

class BoolFlagGuard EASY_FINAL
{
    bool&    m_ref;
    bool m_restore;

public:

    explicit BoolFlagGuard(bool& flag) : m_ref(flag), m_restore(!flag) {}
    explicit BoolFlagGuard(bool& flag, bool value) : m_ref(flag), m_restore(!value) { m_ref = value; }
    ~BoolFlagGuard() { restore(); }

    void restore() { m_ref = m_restore; }
};

//////////////////////////////////////////////////////////////////////////

template <class T>
struct Counter
{
    T count = 0;
};

using DurationsCountMap = std::map<profiler::timestamp_t, Counter<uint32_t> >;

} // END of namespace profiler_gui.

template <typename ... Args>
struct Overload
{
    template <typename TClass, typename TReturn>
    static EASY_CONSTEXPR_FCN auto of(TReturn (TClass::*method)(Args...)) -> decltype(method)
    {
        return method;
    }

    template <typename TReturn>
    static EASY_CONSTEXPR_FCN auto of(TReturn (*func)(Args...)) -> decltype(func)
    {
        return func;
    }
};

template <>
struct Overload<void>
{
    template <typename TClass, typename TReturn>
    static EASY_CONSTEXPR_FCN auto of(TReturn (TClass::*method)()) -> decltype(method)
    {
        return method;
    }

    template <typename TReturn>
    static EASY_CONSTEXPR_FCN auto of(TReturn (*func)()) -> decltype(func)
    {
        return func;
    }
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#endif // EASY_PROFILER__GUI_COMMON_TYPES_H
