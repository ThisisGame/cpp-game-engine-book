/************************************************************************
* file name         : globals_qobjects.h
* ----------------- :
* creation time     : 2016/08/08
* authors           : Victor Zarubkin, Sergey Yagovtsev
* email             : v.s.zarubkin@gmail.com, yse.sey@gmail.com
* ----------------- :
* description       : The file contains declaration of GlobalSignals QObject class.
* ----------------- :
* change log        : * 2016/08/08 Sergey Yagovtsev: moved sources from globals.h
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

#ifndef EASY_GLOBALS_QOBJECTS_H
#define EASY_GLOBALS_QOBJECTS_H

#include <QObject>
#include <easy/details/profiler_public_types.h>

namespace profiler { class ArbitraryValue; }

namespace profiler_gui {

    class GlobalSignals Q_DECL_FINAL : public QObject
    {
        Q_OBJECT

    public:

        GlobalSignals();
        ~GlobalSignals() Q_DECL_OVERRIDE;

    signals:

        void closeEvent();
        void allDataGoingToBeDeleted();
        void fileOpened();

        void selectedThreadChanged(::profiler::thread_id_t _id);
        void selectedBlockChanged(uint32_t _block_index);
        void selectedBlockIdChanged(::profiler::block_id_t _id);
        void itemsExpandStateChanged();
        void blockStatusChanged(::profiler::block_id_t _id, ::profiler::EasyBlockStatus _status);
        void connectionChanged(bool _connected);
        void blocksRefreshRequired(bool);
        void expectedFrameTimeChanged();
        void autoAdjustHistogramChanged();
        void autoAdjustChartChanged();
        void displayOnlyFramesOnHistogramChanged();
        void hierarchyFlagChanged(bool);
        void threadNameDecorationChanged();
        void hexThreadIdChanged();
        void refreshRequired();
        void blocksTreeModeChanged();
        void rulerVisible(bool);

        void sceneCleared();
        void sceneSizeChanged(qreal left, qreal right);
        void sceneVisibleRegionSizeChanged(qreal width);
        void sceneVisibleRegionPosChanged(qreal pos);
        void lockCharts();
        void unlockCharts();

        void chartWheeled(qreal pos, int delta);
        void chartSliderChanged(qreal pos);

        void selectValue(::profiler::thread_id_t _thread_id, uint32_t _value_index, const ::profiler::ArbitraryValue& _value);

        void customWindowHeaderChanged();
        void windowHeaderPositionChanged();

    }; // END of class GlobalSignals.

} // END of namespace profiler_gui.

#endif // EASY_GLOBALS_QOBJECTS_H
