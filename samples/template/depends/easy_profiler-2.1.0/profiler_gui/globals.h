/************************************************************************
* file name         : globals.h
* ----------------- :
* creation time     : 2016/08/03
* author            : Victor Zarubkin
* email             : v.s.zarubkin@gmail.com
* ----------------- :
* description       : The file contains declaration of global constants and variables for profiler gui.
* ----------------- :
* change log        : * 2016/08/03 Victor Zarubkin: initial commit.
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

#ifndef EASY_PROFILER__GUI_GLOBALS_H
#define EASY_PROFILER__GUI_GLOBALS_H

#include <string>
#include <QObject>
#include <QColor>
#include <QTextCodec>
#include <QSize>
#include <QFont>
#include "common_functions.h"
#include "globals_qobjects.h"

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

namespace profiler_gui {

    const QString ORGANAZATION_NAME = "EasyProfiler";
    const QString DEFAULT_WINDOW_TITLE = "EasyProfiler";
    const QString APPLICATION_NAME = "Easy profiler gui application";

    const QColor RULER_COLOR = QColor::fromRgba(0x40000000 | (::profiler::colors::RichBlue & 0x00ffffff));// 0x402020c0);
    const QColor RULER_COLOR2 = QColor::fromRgba(0x40000000 | (::profiler::colors::Dark & 0x00ffffff));// 0x40408040);
    const QColor TEXT_COLOR = QColor::fromRgb(0xff504040);
    const QColor SYSTEM_BORDER_COLOR = QColor::fromRgb(0xffc4c4c4);

    EASY_CONSTEXPR QRgb BLOCK_BORDER_COLOR = profiler::colors::Grey600 & 0x00ffffff;
    EASY_CONSTEXPR QRgb SELECTED_THREAD_BACKGROUND = 0xffe0e060;
    EASY_CONSTEXPR QRgb SELECTED_THREAD_FOREGROUND = 0xffffffff - (SELECTED_THREAD_BACKGROUND & 0x00ffffff);

    EASY_CONSTEXPR qreal SCALING_COEFFICIENT = 1.25;
    EASY_CONSTEXPR qreal SCALING_COEFFICIENT_INV = 1.0 / SCALING_COEFFICIENT;

    EASY_CONSTEXPR uint32_t V130 = 0x01030000;

#ifdef _WIN32
    EASY_CONSTEXPR qreal FONT_METRICS_FACTOR = 1.05;
#else
    EASY_CONSTEXPR qreal FONT_METRICS_FACTOR = 1.;
#endif

    //////////////////////////////////////////////////////////////////////////

    template <class T>
    inline auto toUnicode(const T& _inputString) -> decltype(QTextCodec::codecForLocale()->toUnicode(_inputString))
    {
        return QTextCodec::codecForLocale()->toUnicode(_inputString);
    }

    //////////////////////////////////////////////////////////////////////////

    inline QString decoratedThreadName(bool _use_decorated_thread_name, const::profiler::BlocksTreeRoot& _root, const QString& _unicodeThreadWord, bool _hex = false)
    {
        if (_root.got_name())
        {
            QString rootname(toUnicode(_root.name()));
            if (!_use_decorated_thread_name || rootname.contains(_unicodeThreadWord, Qt::CaseInsensitive))
            {
                if (_hex)
                    return QString("%1 0x%2").arg(rootname).arg(_root.thread_id, 0, 16);
                return QString("%1 %2").arg(rootname).arg(_root.thread_id);
            }

            if (_hex)
                return QString("%1 Thread 0x%2").arg(rootname).arg(_root.thread_id, 0, 16);
            return QString("%1 Thread %2").arg(rootname).arg(_root.thread_id);
        }

        if (_hex)
            return QString("Thread 0x%1").arg(_root.thread_id, 0, 16);
        return QString("Thread %1").arg(_root.thread_id);
    }

    inline QString decoratedThreadName(bool _use_decorated_thread_name, const ::profiler::BlocksTreeRoot& _root, bool _hex = false)
    {
        if (_root.got_name())
        {
            QString rootname(toUnicode(_root.name()));
            if (!_use_decorated_thread_name || rootname.contains(toUnicode("thread"), Qt::CaseInsensitive))
            {
                if (_hex)
                    return QString("%1 0x%2").arg(rootname).arg(_root.thread_id, 0, 16);
                return QString("%1 %2").arg(rootname).arg(_root.thread_id);
            }

            if (_hex)
                return QString("%1 Thread 0x%2").arg(rootname).arg(_root.thread_id, 0, 16);
            return QString("%1 Thread %2").arg(rootname).arg(_root.thread_id);
        }

        if (_hex)
            return QString("Thread 0x%1").arg(_root.thread_id, 0, 16);
        return QString("Thread %1").arg(_root.thread_id);
    }

    //////////////////////////////////////////////////////////////////////////

    enum RulerTextPosition : int8_t
    {
        RulerTextPosition_Center = 0,
        RulerTextPosition_Top,
        RulerTextPosition_Bottom,

    }; // END of enum RulerTextPosition.

    //////////////////////////////////////////////////////////////////////////

    class Globals Q_DECL_FINAL
    {
        struct SceneData
        {
            qreal left = 0;
            qreal right = 100;
            qreal window = 100;
            qreal offset = 0;
            bool empty = true;
        };

        struct Fonts
        {
            QFont  default_font; ///< Default font
            QFont    background; ///< Font for blocks_graphics_view
            QFont         ruler; ///< Font for diagram rulers
            QFont          item; ///< Font for graphics items
            QFont selected_item; ///< Font for selected graphics item

            Fonts();
        };

        struct SizeGuide
        {
            qreal pixelRatio = 1;
            int font_height = 17;
            int font_line_spacing = 20;
            int graphics_row_height = 20;
            int graphics_row_spacing = 0;
            int graphics_row_full = 20;
            int threads_row_spacing = 10;
            int timeline_height = 26;
            int icon_size = 28;
        };

    public:

        GlobalSignals                             events; ///< Global signals
        ::profiler::thread_blocks_tree_t profiler_blocks; ///< Profiler blocks tree loaded from file
        ::profiler::descriptors_list_t       descriptors; ///< Profiler block descriptors list
        ::profiler::bookmarks_t                bookmarks; ///< User bookmarks
        EasyBlocks                            gui_blocks; ///< Profiler graphics blocks builded by GUI

        QString                                    theme; ///< Current UI theme name
        QString                              lastFileDir;
        Fonts                                       font; ///< Fonts

        SceneData                                  scene; ///< Diagram scene sizes and visible area position
        SizeGuide                                   size; ///< Various widgets and font sizes adapted to current device pixel ratio
        ::profiler::processid_t                      pid; ///< Profiled process ID
        ::profiler::timestamp_t               begin_time; ///< Timestamp of the most left diagram scene point (x=0)
        ::profiler::thread_id_t          selected_thread; ///< Current selected thread id
        ::profiler::block_index_t         selected_block; ///< Current selected profiler block index
        ::profiler::block_id_t         selected_block_id; ///< Current selected profiler block id
        uint32_t                                 version; ///< Opened file version (files may have different format)

        uint32_t                          max_rows_count; ///< Maximum blocks count for the StatsTree widget for the full call-stack mode

        float                                 frame_time; ///< Expected frame time value in microseconds to be displayed at minimap on graphics scrollbar
        int                               blocks_spacing; ///< Minimum blocks spacing on diagram
        int                              blocks_size_min; ///< Minimum blocks size on diagram
        int                   histogram_column_width_min; ///< Minimum column width on histogram when borders are enabled
        int                           blocks_narrow_size; ///< Width indicating narrow blocks
        int                              max_fps_history; ///< Max frames history displayed in FPS Monitor
        int                           fps_timer_interval; ///< Interval in milliseconds for sending network requests to the profiled application (used by FPS Monitor)
        int                        fps_widget_line_width; ///< Line width in pixels of FPS lines for FPS Monitor
        ::profiler::color_t       bookmark_default_color; ///<

        RulerTextPosition           chrono_text_position; ///< Selected interval text position
        TimeUnits                             time_units; ///< Units type for time (milliseconds, microseconds, nanoseconds or auto-definition)

        bool                                   connected; ///< Is connected to source (to be able to capture profiling information)
        bool                           has_local_changes; ///<

        bool                    use_custom_window_header; ///<
        bool             is_right_window_header_controls; ///<
        bool                                 fps_enabled; ///< Is FPS Monitor enabled
        bool                   use_decorated_thread_name; ///< Add "Thread" to the name of each thread (if there is no one)
        bool                               hex_thread_id; ///< Use hex view for thread-id instead of decimal
        bool                        enable_event_markers; ///< Enable event indicators painting (These are narrow rectangles at the bottom of each thread)
        bool                           enable_statistics; ///< Enable gathering and using statistics (Disable if you want to consume less memory)
        bool                          enable_zero_length; ///< Enable zero length blocks (if true, then such blocks will have width == 1 pixel on each scale)
        bool                add_zero_blocks_to_hierarchy; ///< Enable adding zero blocks into hierarchy tree
        bool                 draw_graphics_items_borders; ///< Draw borders for graphics blocks or not
        bool                      draw_histogram_borders; ///< Draw borders for histogram columns or not
        bool                        hide_narrow_children; ///< Hide children for narrow graphics blocks (See blocks_narrow_size)
        bool                         hide_minsize_blocks; ///< Hide blocks which screen size is less than blocks_size_min
        bool                hide_stats_for_single_blocks; ///< Hide min, max, avg, median durations in stats tree if there is only 1 call for a block
        bool                collapse_items_on_tree_close; ///< Collapse all items which were displayed in the hierarchy tree after tree close/reset
        bool               all_items_expanded_by_default; ///< Expand all items after file is opened
        bool               only_current_thread_hierarchy; ///< Build hierarchy tree for current thread only
        bool               highlight_blocks_with_same_id; ///< Highlight all blocks with same id on diagram
        bool              selecting_block_changes_thread; ///< If true then current selected thread will change every time you select block
        bool                auto_adjust_histogram_height; ///< Automatically adjust histogram height to the visible region
        bool                    auto_adjust_chart_height; ///< Automatically adjust arbitrary value chart height to the visible region
        bool            display_only_frames_on_histogram; ///< Display only top-level blocks on histogram when drawing histogram by block id
        bool           bind_scene_and_tree_expand_status; /** \brief If true then items on graphics scene and in the tree (blocks hierarchy) are binded on each other
                                                                so expanding/collapsing items on scene also expands/collapse items in the tree. */

        static Globals& instance();

    private:

        Globals();

    }; // END of struct Globals.

    //////////////////////////////////////////////////////////////////////////

} // END of namespace profiler_gui.

#define EASY_GLOBALS profiler_gui::Globals::instance()

inline profiler_gui::EasyBlock& easyBlock(profiler::block_index_t i) {
    return EASY_GLOBALS.gui_blocks[i];
}

inline profiler::SerializedBlockDescriptor& easyDescriptor(profiler::block_id_t i) {
    return *EASY_GLOBALS.descriptors[i];
}

inline profiler::SerializedBlockDescriptor& easyDescriptor(const profiler::BlocksTree& block) {
    return easyDescriptor(block.node->id());
}

EASY_FORCE_INLINE const profiler::BlocksTree& easyBlocksTree(profiler::block_index_t i) {
    return easyBlock(i).tree;
}

EASY_FORCE_INLINE const char* easyBlockName(const profiler::BlocksTree& block) {
    const char* name = block.node->name();
    return *name != 0 ? name : easyDescriptor(block.node->id()).name();
}

EASY_FORCE_INLINE const char* easyBlockName(const profiler::BlocksTree& block, const profiler::SerializedBlockDescriptor& desc) {
    const char* name = block.node->name();
    return *name != 0 ? name : desc.name();
}

EASY_FORCE_INLINE const char* easyBlockName(profiler::block_index_t i) {
    return easyBlockName(easyBlock(i).tree);
}

inline qreal sceneX(profiler::timestamp_t time) {
    return PROF_MICROSECONDS(qreal(time - EASY_GLOBALS.begin_time));
}

inline QString imagePath(const QString& resource_name) {
    return QString(":/images/%1/%2").arg(EASY_GLOBALS.theme).arg(resource_name);
}

inline QString imagePath(const char* resource_name) {
    return QString(":/images/%1/%2").arg(EASY_GLOBALS.theme).arg(resource_name);
}

inline QSize applicationIconsSize() {
    return QSize(EASY_GLOBALS.size.icon_size, EASY_GLOBALS.size.icon_size);
}

inline qreal pxf(int pixels) {
    return pixels * EASY_GLOBALS.size.pixelRatio;
}

inline int px(int pixels) {
    return static_cast<int>(pxf(pixels) + 0.5);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#endif // EASY_PROFILER__GUI_GLOBALS_H
