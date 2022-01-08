/************************************************************************
* file name         : main_window.h
* ----------------- :
* creation time     : 2016/06/26
* author            : Victor Zarubkin
* email             : v.s.zarubkin@gmail.com
* ----------------- :
* description       : The file contains declaration of MainWindow for easy_profiler GUI.
* ----------------- :
* change log        : * 2016/06/26 Victor Zarubkin: initial commit.
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

#ifndef EASY_PROFILER_GUI__MAIN_WINDOW__H
#define EASY_PROFILER_GUI__MAIN_WINDOW__H

#include <atomic>
#include <chrono>
#include <sstream>
#include <string>
#include <thread>

#include <QMainWindow>
#include <QDockWidget>
#include <QTimer>
#include <QStringList>

#include "file_reader.h"
#include "socket_listener.h"

#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif

//////////////////////////////////////////////////////////////////////////

#define EASY_GUI_USE_DESCRIPTORS_DOCK_WINDOW 0

class DockWidget : public QDockWidget
{
    Q_OBJECT

    class QPushButton* m_floatingButton;

public:

    explicit DockWidget(const QString& title, QWidget* parent = nullptr);
    ~DockWidget() override;

private slots:

    void toggleState();
    void onTopLevelChanged();

}; // end of class DockWidget.

struct DialogWithGeometry EASY_FINAL
{
    QByteArray geometry;
    class Dialog* ptr = nullptr;

    void create(QWidget* content, QWidget* parent = nullptr);
    void saveGeometry();
    void restoreGeometry();

}; // end of struct DialogWithGeometry.

class MainWindow : public QMainWindow
{
    Q_OBJECT

protected:

    using This = MainWindow;
    using Parent = QMainWindow;

    DialogWithGeometry m_descTreeDialog;

    QStringList                            m_lastFiles;
    QString                                    m_theme;
    QString                              m_lastAddress;

    QDockWidget*                m_treeWidget = nullptr;
    QDockWidget*              m_graphicsView = nullptr;
    QDockWidget*                 m_fpsViewer = nullptr;

#if EASY_GUI_USE_DESCRIPTORS_DOCK_WINDOW != 0
    QDockWidget*                      m_descTreeWidget = nullptr;
#endif

    class RoundProgressDialog*          m_progress = nullptr;
    class BlockDescriptorsWidget* m_dialogDescTree = nullptr;
    class RoundProgressDialog*    m_listenerDialog = nullptr;
    QTimer                               m_readerTimer;
    QTimer                             m_listenerTimer;
    QTimer                           m_fpsRequestTimer;
    profiler::SerializedData        m_serializedBlocks;
    profiler::SerializedData   m_serializedDescriptors;
    profiler::BeginEndTime              m_beginEndTime;
    FileReader                                m_reader;
    SocketListener                          m_listener;
    std::chrono::system_clock::time_point m_listenStartTime;

    class QLineEdit*   m_addressEdit = nullptr;
    class QLineEdit*      m_portEdit = nullptr;
    class QLineEdit* m_frameTimeEdit = nullptr;

    class QMenu*   m_loadActionMenu = nullptr;
    class QAction*     m_saveAction = nullptr;
    class QAction*   m_deleteAction = nullptr;

    class QAction*              m_captureAction = nullptr;
    class QAction*              m_connectAction = nullptr;
    class QAction*   m_eventTracingEnableAction = nullptr;
    class QAction* m_eventTracingPriorityAction = nullptr;

    uint32_t m_descriptorsNumberInFile = 0;
    uint16_t                m_lastPort = 0;
    bool      m_bNetworkFileRegime = false;
    bool        m_bOpenedCacheFile = false;
    bool         m_bCloseAfterSave = false;

public:

    explicit MainWindow();
    ~MainWindow() override;

    // Public virtual methods

    void showEvent(QShowEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void moveEvent(QMoveEvent* _event) override;
    void closeEvent(QCloseEvent* close_event) override;
    void changeEvent(QEvent* event) override;
    void dragEnterEvent(QDragEnterEvent* drag_event) override;
    void dragMoveEvent(QDragMoveEvent* drag_event) override;
    void dragLeaveEvent(QDragLeaveEvent* drag_event) override;
    void dropEvent(QDropEvent* drop_event) override;

signals:
    void activationChanged();

protected slots:

    void onThemeChange(bool);
    void onOpenFileClicked(bool);
    void onSaveFileClicked(bool);
    void onDeleteClicked(bool);
    void onExitClicked(bool);
    void onEncodingChanged(bool);
    void onRulerTextPosChanged(bool);
    void onUnitsChanged(bool);
    void onEnableDisableStatistics(bool);
    void onCollapseItemsAfterCloseChanged(bool);
    void onAllItemsExpandedByDefaultChange(bool);
    void onBindExpandStatusChange(bool);
    void onHierarchyFlagChange(bool);
    void onDisplayRelevantStatsChange(bool);
    void onExpandAllClicked(bool);
    void onCollapseAllClicked(bool);
    void onViewportInfoClicked(bool);
    void onMaxBlocksCountChange(int _value);
    void onSpacingChange(int _value);
    void onMinSizeChange(int _value);
    void onHistogramMinSizeChange(int _value);
    void onNarrowSizeChange(int _value);
    void onFpsIntervalChange(int _value);
    void onFpsHistoryChange(int _value);
    void onFpsMonitorLineWidthChange(int _value);
    void onFileReaderTimeout();
    void onFrameTimeRequestTimeout();
    void onListenerTimerTimeout();
    void onFileReaderCancel();
    void onEditBlocksClicked(bool);
    void onDescTreeDialogClose(int);
    void onListenerDialogClose(int);
    void onCaptureClicked(bool);
    void onGetBlockDescriptionsClicked(bool);
    void onConnectClicked(bool);
    void onEventTracingPriorityChange(bool _checked);
    void onEventTracingEnableChange(bool _checked);
    void onFrameTimeEditFinish();
    void onFrameTimeChanged();
    void onSnapshotClicked(bool);
    void onCustomWindowHeaderTriggered(bool _checked);
    void onRightWindowHeaderPosition(bool _checked);
    void onLeftWindowHeaderPosition(bool _checked);

    void onBlockStatusChange(profiler::block_id_t _id, profiler::EasyBlockStatus _status);

    void onSelectValue(profiler::thread_id_t _thread_id, uint32_t _value_index, const profiler::ArbitraryValue& _value);

    void checkFrameTimeReady();

    void validateLastDir();

private:

    // Private non-virtual methods

    void centerDialogs();

    void closeProgressDialogAndClearReader();
    void onLoadingFinish(profiler::block_index_t& _nblocks);
    void onSavingFinish();

    void configureSizes();

    void clear();

    void refreshDiagram();
    void refreshHistogramImage();

    void addFileToList(const QString& filename, bool changeWindowTitle = true);
    void loadFile(const QString& filename);
    void readStream(std::stringstream& data);

    void loadSettings();
    void loadGeometry();
    void saveSettingsAndGeometry();

    void setDisconnected(bool _showMessage = true);

    void destroyProgressDialog();
    void createProgressDialog(const QString& text);

    void validateLineEdits();

}; // END of class MainWindow.

//////////////////////////////////////////////////////////////////////////

#endif // EASY_PROFILER_GUI__MAIN_WINDOW__H
