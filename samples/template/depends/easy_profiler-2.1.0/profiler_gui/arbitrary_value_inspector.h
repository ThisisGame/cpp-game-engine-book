/************************************************************************
* file name         : arbitrary_value_inspector.h
* ----------------- :
* creation time     : 2017/11/30
* author            : Victor Zarubkin
* email             : v.s.zarubkin@gmail.com
* ----------------- :
* description       : The file contains declaration of .
* ----------------- :
* change log        : * 2017/11/30 Victor Zarubkin: initial commit.
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

#ifndef EASY_PROFILER_GUI_ARBITRARY_VALUE_INSPECTOR_H
#define EASY_PROFILER_GUI_ARBITRARY_VALUE_INSPECTOR_H

#include <QWidget>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QTimer>
#include <QPointF>
#include <QList>
#include <easy/serialized_block.h>
#include <easy/reader.h>
#include <easy/utility.h>
#include <unordered_map>
#include <map>
#include <vector>
#include <string>
#include <functional>
#include <atomic>
#include <memory>
#include "graphics_slider_area.h"
#include "thread_pool_task.h"

//////////////////////////////////////////////////////////////////////////

using Points = std::vector<QPointF>;
using ArbitraryValues = std::vector<const profiler::ArbitraryValue*>;
using Durations = std::vector<profiler::timestamp_t>;
using ComplexityValuesMap = std::map<double, Durations>;

enum class ChartType : uint8_t
{
    Regular = 0, ///< Regular chart; X axis = time,  Y axis = value
    Complexity   ///< Complexity chart; X axis = value, Y axis = duration
};

enum class FilterType : uint8_t
{
    None = 0,
    Gauss,
    Median,
};

enum class ChartPenStyle : uint8_t
{
    Line = 0,
    Points
};

class ArbitraryValuesCollection EASY_FINAL
{
public:

    enum JobStatus : uint8_t { Idle = 0, Ready, InProgress };
    enum JobType : uint8_t { None = 0, ValuesJob = 1 << 0, PointsJob = 1 << 1 };

private:

    using This = ArbitraryValuesCollection;

    ArbitraryValues            m_values;
    ComplexityValuesMap m_complexityMap;
    Points                     m_points;
    ThreadPoolTask             m_worker;
    profiler::timestamp_t   m_beginTime;
    profiler::timestamp_t m_minDuration;
    profiler::timestamp_t m_maxDuration;
    qreal                    m_minValue;
    qreal                    m_maxValue;
    std::atomic<uint8_t>       m_status;
    std::atomic_bool       m_bInterrupt;
    ChartType               m_chartType;
    uint8_t                   m_jobType;

public:

    explicit ArbitraryValuesCollection();
    ~ArbitraryValuesCollection();

    ChartType chartType() const;
    const ArbitraryValues& values() const;
    const ComplexityValuesMap& complexityMap() const;
    const Points& points() const;
    JobStatus status() const;

    profiler::timestamp_t minDuration() const;
    profiler::timestamp_t maxDuration() const;

    qreal minValue() const;
    qreal maxValue() const;

    void collectValues(ChartType _chartType, profiler::thread_id_t _threadId, profiler::vin_t _valueId
        , const char* _valueName, profiler::block_id_t _parentBlockId, int _index = -1);

    bool calculatePoints(profiler::timestamp_t _beginTime);

    void collectValuesAndPoints(ChartType _chartType, profiler::thread_id_t _threadId, profiler::vin_t _valueId
        , const char* _valueName, profiler::timestamp_t _beginTime, profiler::block_id_t _parentBlockId
        , int _index = -1);

    void interrupt();

private:

    void setStatus(JobStatus _status);

    void collectById(profiler::thread_id_t _threadId, profiler::vin_t _valueId
        , profiler::block_id_t _parentBlockId, int _index);

    void collectByName(profiler::thread_id_t _threadId, const std::string _valueName
        , profiler::block_id_t _parentBlockId, int _index);

    bool collectByIdForThread(const profiler::BlocksTreeRoot& _threadRoot, profiler::vin_t _valueId
        , bool _calculatePoints, profiler::block_id_t _parentBlockId, int _index);

    bool collectByNameForThread(const profiler::BlocksTreeRoot& _threadRoot, const std::string& _valueName
        , bool _calculatePoints, profiler::block_id_t _parentBlockId, int _index);

    bool depthFirstSearch(const profiler::BlocksTreeRoot& _threadRoot, bool _calculatePoints
        , profiler::block_id_t _parentBlockId, int _index
        , std::function<bool(profiler::vin_t, const char*)> _isSuitableValue);

    double addPoint(const profiler::ArbitraryValue& _value, int _index);
    QPointF point(const profiler::ArbitraryValue& _value, int _index) const;

}; // end of class ArbitraryValuesCollection.

struct CollectionPaintData EASY_FINAL
{
    const ArbitraryValuesCollection* ptr;
    QRgb                           color;
    ChartPenStyle          chartPenStyle;
    bool                        selected;
};

using Collections = std::vector<CollectionPaintData>;

//////////////////////////////////////////////////////////////////////////

class ArbitraryValuesChartItem : public GraphicsImageItem
{
    using Parent = GraphicsImageItem;
    using This = ArbitraryValuesChartItem;

    Collections                 m_collections;
    qreal                    m_workerMaxValue;
    qreal                    m_workerMinValue;
    profiler::timestamp_t m_workerMaxDuration;
    profiler::timestamp_t m_workerMinDuration;
    profiler::timestamp_t       m_maxDuration;
    profiler::timestamp_t       m_minDuration;
    int                    m_filterWindowSize;
    ChartType                     m_chartType;
    FilterType                   m_filterType;

public:

    explicit ArbitraryValuesChartItem();
    ~ArbitraryValuesChartItem() override;

    void paint(QPainter* _painter, const QStyleOptionGraphicsItem* _option, QWidget* _widget) override;

    bool updateImage() override;

protected:

    void onImageUpdated() override;

public:

    void clear();
    void update(Collections _collections);
    void update(const ArbitraryValuesCollection* _selected);
    void setChartType(ChartType _chartType);
    void setFilterType(FilterType _filterType);
    void setFilterWindowSize(int _size);

    ChartType chartType() const;
    FilterType filterType() const;
    int filterWindowSize() const;

private:

    void paintMouseIndicator(QPainter* _painter, qreal _top, qreal _bottom, qreal _width, qreal _height, int _font_h,
                             qreal _visibleRegionLeft, qreal _visibleRegionWidth);

    void updateRegularImageAsync(QRectF _boundingRect, qreal _current_scale, qreal _minimum, qreal _maximum
        , qreal _range, qreal _value, qreal _width, bool _bindMode, profiler::timestamp_t _begin_time, bool _autoAdjust);

    void updateComplexityImageAsync(QRectF _boundingRect, qreal _current_scale, qreal _minimum, qreal _maximum
        , qreal _range, qreal _value, qreal _width, bool _bindMode, profiler::timestamp_t _begin_time, bool _autoAdjust);

    void drawGrid(QPainter& _painter, int _width, int _height) const;

}; // end of class ArbitraryValuesChartItem.

class GraphicsChart : public GraphicsSliderArea
{
    Q_OBJECT

private:

    using Parent = GraphicsSliderArea;
    using This = GraphicsChart;

    ArbitraryValuesChartItem* m_chartItem;

public:

    explicit GraphicsChart(QWidget* _parent = nullptr);
    ~GraphicsChart() override;

    void clear() override;

public:

    void cancelImageUpdate();
    void update(Collections _collections);
    void update(const ArbitraryValuesCollection* _selected);
    void setChartType(ChartType _chartType);
    void setFilterType(FilterType _filterType);
    void setFilterWindowSize(int _size);

    ChartType chartType() const;
    FilterType filterType() const;
    int filterWindowSize() const;

protected:

    bool canShowSlider() const override;

private slots:

    void onAutoAdjustChartChanged();

}; // end of class GraphicsChart.

//////////////////////////////////////////////////////////////////////////

class ArbitraryTreeWidgetItem : public QTreeWidgetItem
{
    using Parent = QTreeWidgetItem;
    using This = ArbitraryTreeWidgetItem;
    using CollectionPtr = std::unique_ptr<ArbitraryValuesCollection>;

    const profiler::ArbitraryValue& m_value;
    QFont                 m_font;
    CollectionPtr   m_collection;
    profiler::color_t    m_color;
    int              m_widthHint;

public:

    explicit ArbitraryTreeWidgetItem(QTreeWidgetItem* _parent, bool _checkable, profiler::color_t _color, const profiler::ArbitraryValue& _value);
    ~ArbitraryTreeWidgetItem() override;

    QVariant data(int _column, int _role) const override;

    const profiler::ArbitraryValue& value() const;

    void setWidthHint(int _width);
    void setBold(bool _isBold);

    const ArbitraryValuesCollection* collection() const;
    ArbitraryValuesCollection* collection();
    void collectValues(profiler::thread_id_t _threadId, ChartType _chartType);
    void interrupt();

    profiler::color_t color() const;

    bool isArrayItem() const;
    int getSelfIndexInArray() const;

private:

    profiler::block_id_t getParentBlockId(QTreeWidgetItem* _item) const;

}; // end of class ArbitraryTreeWidgetItem.

//////////////////////////////////////////////////////////////////////////

class ArbitraryValuesWidget : public QWidget
{
    Q_OBJECT

    using Parent = QWidget;
    using This = ArbitraryValuesWidget;

    QTimer                      m_collectionsTimer;
    QList<ArbitraryTreeWidgetItem*> m_checkedItems;
    std::vector<int>         m_columnsMinimumWidth;
    class QSplitter*                    m_splitter;
    QTreeWidget*                      m_treeWidget;
    GraphicsChart*                         m_chart;
    class QLabel*                 m_filterBoxLabel;
    class QComboBox*              m_filterComboBox;
    class QLabel*              m_filterWindowLabel;
    class QSpinBox*           m_filterWindowPicker;
    class QAction*             m_exportToCsvAction;
    ArbitraryTreeWidgetItem*            m_boldItem;
    profiler::thread_id_t               m_threadId;
    profiler::block_index_t           m_blockIndex;
    profiler::block_id_t                 m_blockId;
    bool                            m_bInitialized;
    const bool                       m_bMainWidget;

    explicit ArbitraryValuesWidget(bool _isMainWidget, profiler::thread_id_t _threadId
        , profiler::block_index_t _blockIndex, profiler::block_id_t _blockId, QWidget* _parent);
    explicit ArbitraryValuesWidget(const QList<ArbitraryTreeWidgetItem*>& _checkedItems
        , QTreeWidgetItem* _currentItem, profiler::thread_id_t _threadId
        , profiler::block_index_t _blockIndex, profiler::block_id_t _blockId, QWidget* _parent = nullptr);

public:

    explicit ArbitraryValuesWidget(QWidget* _parent = nullptr);
    ~ArbitraryValuesWidget() override;

    void showEvent(class QShowEvent* event) override;
    void contextMenuEvent(QContextMenuEvent*) override { /* ignore context menu event */ }

public slots:

    void clear();
    void rebuild();
    void rebuild(profiler::thread_id_t _threadId, profiler::block_index_t _blockIndex, profiler::block_id_t _blockId);
    void select(const profiler::ArbitraryValue& _value, bool _resetOthers = true);

private slots:

    void onHeaderSectionResized(int logicalIndex, int oldSize, int newSize);
    void onSelectedThreadChanged(profiler::thread_id_t);
    void onSelectedBlockChanged(uint32_t _block_index);
    void onSelectedBlockIdChanged(profiler::block_id_t _id);
    void onItemDoubleClicked(QTreeWidgetItem* _item, int _column);
    void onItemChanged(QTreeWidgetItem* _item, int _column);
    void onCurrentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*);
    void onCollectionsTimeout();
    void onRegularChartTypeChecked(bool _checked);
    void onComplexityChartTypeChecked(bool _checked);
    void onFilterComboBoxChanged(int _index);
    void onFilterWindowSizeChanged(int _size);
    void onExportToCsvClicked(bool);
    void onOpenInNewWindowClicked(bool);

private:

    void repaint();

    void buildTree(profiler::thread_id_t _threadId, profiler::block_index_t _blockIndex, profiler::block_id_t _blockId);
    QTreeWidgetItem* buildTreeForThread(const profiler::BlocksTreeRoot& _threadRoot, profiler::block_index_t _blockIndex, profiler::block_id_t _blockId);

    void loadSettings();
    void saveSettings();

}; // end of class ArbitraryValuesWidget.

//////////////////////////////////////////////////////////////////////////

#endif // EASY_PROFILER_GUI_ARBITRARY_VALUE_INSPECTOR_H
