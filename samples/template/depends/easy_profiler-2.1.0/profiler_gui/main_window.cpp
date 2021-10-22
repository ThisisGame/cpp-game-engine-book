/************************************************************************
* file name         : main_window.cpp
* ----------------- :
* creation time     : 2016/06/26
* author            : Victor Zarubkin
* email             : v.s.zarubkin@gmail.com
* ----------------- :
* description       : The file contains implementation of MainWindow for easy_profiler GUI.
* ----------------- :
* change log        : * 2016/06/26 Victor Zarubkin: Initial commit.
*                   :
*                   : * 2016/06/27 Victor Zarubkin: Passing blocks number to BlocksTreeWidget::setTree().
*                   :
*                   : * 2016/06/29 Victor Zarubkin: Added menu with tests.
*                   :
*                   : * 2016/06/30 Sergey Yagovtsev: Open file by command line argument
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

#include <fstream>

#include <QApplication>
#include <QCoreApplication>
#include <QDebug>

#include <QAction>
#include <QCloseEvent>
#include <QDateTime>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDragLeaveEvent>
#include <QDropEvent>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QFont>
#include <QFontMetrics>
#include <QFontMetricsF>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QMimeData>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTextCodec>
#include <QPlainTextEdit>
#include <QTextStream>
#include <QToolBar>
#include <QToolButton>
#include <QSet>
#include <QSettings>
#include <QSignalBlocker>
#include <QSpinBox>
#include <QStatusBar>
#include <QVBoxLayout>
#include <QWidgetAction>

#include <easy/easy_net.h>
#include <easy/profiler.h>

#include "arbitrary_value_inspector.h"
#include "blocks_graphics_view.h"
#include "blocks_tree_widget.h"
#include "descriptors_tree_widget.h"
#include "dialog.h"
#include "globals.h"
#include "fps_widget.h"
#include "round_progress_widget.h"

#include "main_window.h"

#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif

//////////////////////////////////////////////////////////////////////////

namespace {

const int LOADER_TIMER_INTERVAL = 40;

} // end of namespace <noname>.

//////////////////////////////////////////////////////////////////////////

static const QStringList& UI_themes()
{
    static const QStringList themes {
        "default"
    };

    return themes;
}

//////////////////////////////////////////////////////////////////////////

static void convertPointSizes(QString& style, QString from, QString to)
{
    // Find font family
    const auto fontMatch = QRegularExpression("font-family:\\s*\\\"(.*)\\\"\\s*;").match(style);
    const auto fontFamily = fontMatch.hasMatch() ? fontMatch.captured(fontMatch.lastCapturedIndex()) : QString("DejaVu Sans");
    //QMessageBox::information(nullptr, "Found font family", fontFamily);

    // Calculate point size using current font
    const auto pointSizeF = QFontMetricsF(QFont(fontFamily, 100)).height() * 1e-2;
    //QMessageBox::information(nullptr, "Point size", QString("100pt = %1\n1pt = %2").arg(pointSizeF * 1e2).arg(pointSizeF));

    // Find and convert all sizes from points to pixels
    QRegularExpression re(QString("(\\d+\\.?\\d*)%1").arg(from));
    auto it = re.globalMatch(style);

    std::vector<QStringList> matches;
    {
        QSet<QString> uniqueMatches;
        while (it.hasNext())
        {
            const auto match = it.next();
            if (!uniqueMatches.contains(match.captured()))
            {
                uniqueMatches.insert(match.captured());
                matches.emplace_back(match.capturedTexts());
            }
        }
    }

    for (const auto& capturedTexts : matches)
    {
        const auto pt = capturedTexts.back().toDouble();
        const int pixels = static_cast<int>(lround(pointSizeF * pt));
        auto converted = QString("%1%2").arg(pixels).arg(to);
        style.replace(QString(" %1").arg(capturedTexts.front()), QString(" %1").arg(converted));
        style.replace(QString(":%1").arg(capturedTexts.front()), QString(":%1").arg(converted));
    }
}

static void replaceOsDependentSettings(QString& style)
{
    // Find and convert all OS dependent options
    // Example: "/*{lin}font-weight: bold;*/" -> "font-weight: bold;"

#if defined(_WIN32)
    QRegularExpression re("/\\*\\{win\\}(.*)\\*/");
#elif defined(__APPLE__)
    QRegularExpression re("/\\*\\{osx\\}(.*)\\*/");
#else
    QRegularExpression re("/\\*\\{lin\\}(.*)\\*/");
#endif

    auto it = re.globalMatch(style);

    std::vector<QStringList> matches;
    {
        QSet<QString> uniqueMatches;
        while (it.hasNext())
        {
            const auto match = it.next();
            if (!uniqueMatches.contains(match.captured()))
            {
                uniqueMatches.insert(match.captured());
                matches.emplace_back(match.capturedTexts());
            }
        }
    }

    for (const auto& capturedTexts : matches)
    {
        style.replace(capturedTexts.front(), capturedTexts.back());
    }
}

static void loadTheme(const QString& _theme)
{
    QFile file(QStringLiteral(":/themes/") + _theme);
    if (file.open(QFile::ReadOnly | QFile::Text))
    {
        QTextStream in(&file);
        QString style = in.readAll();
        if (!style.isEmpty())
        {
            convertPointSizes(style, "ex", "px");
            convertPointSizes(style, "qex", "");
            replaceOsDependentSettings(style);
            qApp->setStyleSheet(style);
        }
    }
}

//////////////////////////////////////////////////////////////////////////

DockWidget::DockWidget(const QString& title, QWidget* parent)
    : QDockWidget(title, parent)
    , m_floatingButton(new QPushButton())
{
    m_floatingButton->setObjectName("EasyDockWidgetFloatButton");
    m_floatingButton->setProperty("floating", isFloating());
    connect(m_floatingButton, &QPushButton::clicked, this, &DockWidget::toggleState);
    connect(this, &QDockWidget::topLevelChanged, this, &DockWidget::onTopLevelChanged);

    auto closeButton = new QPushButton();
    closeButton->setObjectName("EasyDockWidgetCloseButton");
    connect(closeButton, &QPushButton::clicked, this, &DockWidget::close);

    auto caption = new QWidget(this);
    caption->setObjectName("EasyDockWidgetTitle");

    auto lay = new QHBoxLayout(caption);
    lay->setContentsMargins(0, 0, 0, 0);
    lay->setSpacing(2);
    lay->addWidget(new QLabel(title));
    lay->addStretch(100);
    lay->addWidget(m_floatingButton);
    lay->addWidget(closeButton);

    setTitleBarWidget(caption);
}

DockWidget::~DockWidget()
{
}

void DockWidget::toggleState()
{
    setFloating(!isFloating());
}

void DockWidget::onTopLevelChanged()
{
    profiler_gui::updateProperty(m_floatingButton, "floating", isFloating());
}

void MainWindow::configureSizes()
{
    QWidget w(this);
    w.show(); // All sizes (font() size for example) become valid only after show()

    EASY_GLOBALS.font.default_font = w.font();
    const QFontMetricsF fm(w.font());

#ifdef _WIN32
    EASY_CONSTEXPR qreal DefaultHeight = 16;
#else
    EASY_CONSTEXPR qreal DefaultHeight = 17;
#endif

    auto& size = EASY_GLOBALS.size;
    size.font_height = static_cast<int>(fm.height() + 0.5);
    size.pixelRatio = std::max(fm.height() / DefaultHeight, 1.0);
    size.font_line_spacing = static_cast<int>(fm.lineSpacing() + 0.5);
    size.graphics_row_height = size.font_height + px(4);
    size.graphics_row_spacing = 0;
    size.graphics_row_full = size.graphics_row_height;
    size.threads_row_spacing = size.graphics_row_full >> 1;
    size.timeline_height = size.font_height + px(10);
    size.icon_size = size.font_height + px(5);

    const auto fontFamily = w.font().family();
    const auto pixelSize = w.font().pixelSize();
    const auto pointSize = w.font().pointSize();
    const auto updateFont = [&] (QFont& font)
    {
        font.setFamily(fontFamily);
        if (pixelSize >= 0)
            font.setPixelSize(pixelSize);
        if (pointSize >= 0)
            font.setPointSize(pointSize);
    };

    auto& fonts = EASY_GLOBALS.font;
    updateFont(fonts.background);
    updateFont(fonts.item);
    updateFont(fonts.selected_item);
    fonts.ruler.setFamily(fontFamily);

    /*
    printf("Viewport info:\n");
    printf("- device pixel ratio = %f\n", size.pixelRatio);
    printf("- font height = %dpx\n", size.font_height);
    printf("- font line spacing = %dpx\n", size.font_line_spacing);
    printf("- diagram row = %dpx\n", size.graphics_row_height);
    printf("- diagram spacing = %dpx\n", size.threads_row_spacing);
    printf("- icon size = %dx%d px\n", size.icon_size, size.icon_size);
    */

    w.hide();
}

MainWindow::MainWindow() : Parent(), m_theme("default"), m_lastAddress("localhost"), m_lastPort(profiler::DEFAULT_PORT)
{
    { QIcon icon(":/images/logo"); if (!icon.isNull()) QApplication::setWindowIcon(icon); }

    setObjectName("ProfilerGUI_MainWindow");
    setWindowTitle(profiler_gui::DEFAULT_WINDOW_TITLE);
    setDockNestingEnabled(true);
    setAcceptDrops(true);
    setStatusBar(nullptr);

    loadSettings();
    loadTheme(m_theme);
    configureSizes();
    resize(px(800), px(600));

    m_graphicsView = new DockWidget("Diagram", this);
    m_graphicsView->setObjectName("ProfilerGUI_Diagram");
    m_graphicsView->setMinimumHeight(px(50));
    m_graphicsView->setAllowedAreas(Qt::AllDockWidgetAreas);

    auto graphicsView = new DiagramWidget(this);
    graphicsView->setObjectName("ProfilerGUI_Diagram_GraphicsView");
    connect(this, &MainWindow::activationChanged, graphicsView->view(), &BlocksGraphicsView::onWindowActivationChanged, Qt::QueuedConnection);
    connect(this, &MainWindow::activationChanged, graphicsView->threadsView(), &ThreadNamesWidget::onWindowActivationChanged, Qt::QueuedConnection);
    m_graphicsView->setWidget(graphicsView);

    m_treeWidget = new DockWidget("Stats", this);
    m_treeWidget->setObjectName("ProfilerGUI_Hierarchy");
    m_treeWidget->setMinimumHeight(px(50));
    m_treeWidget->setAllowedAreas(Qt::AllDockWidgetAreas);

    auto treeWidget = new StatsWidget(this);
    m_treeWidget->setWidget(treeWidget);

    m_fpsViewer = new DockWidget("FPS Monitor", this);
    m_fpsViewer->setObjectName("ProfilerGUI_FPS");
    m_fpsViewer->setWidget(new FpsWidget(this));
    m_fpsViewer->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);

    addDockWidget(Qt::TopDockWidgetArea, m_graphicsView);
    addDockWidget(Qt::BottomDockWidgetArea, m_treeWidget);
    addDockWidget(Qt::TopDockWidgetArea, m_fpsViewer);

#if EASY_GUI_USE_DESCRIPTORS_DOCK_WINDOW != 0
    auto descTree = new BlockDescriptorsWidget();
    m_descTreeWidget = new DockWidget("Blocks");
    m_descTreeWidget->setObjectName("ProfilerGUI_Blocks");
    m_descTreeWidget->setMinimumHeight(px(50));
    m_descTreeWidget->setAllowedAreas(Qt::AllDockWidgetAreas);
    m_descTreeWidget->setWidget(descTree);
    addDockWidget(Qt::BottomDockWidgetArea, m_descTreeWidget);
#endif


    auto toolbar = addToolBar("FileToolbar");
    toolbar->setIconSize(applicationIconsSize());
    toolbar->setObjectName("ProfilerGUI_FileToolbar");
    toolbar->setContentsMargins(1, 0, 1, 0);

    m_loadActionMenu = new QMenu(this);
    auto action = m_loadActionMenu->menuAction();
    action->setText("Open file");
    action->setIcon(QIcon(imagePath("open")));
    connect(action, &QAction::triggered, this, &This::onOpenFileClicked);
    toolbar->addAction(action);

    for (const auto& f : m_lastFiles)
    {
        action = new QAction(f, this);
        connect(action, &QAction::triggered, this, &This::onOpenFileClicked);
        m_loadActionMenu->addAction(action);
    }

    m_saveAction = toolbar->addAction(QIcon(imagePath("save")), tr("Save"), this, SLOT(onSaveFileClicked(bool)));
    m_deleteAction = toolbar->addAction(QIcon(imagePath("delete")), tr("Clear all"), this, SLOT(onDeleteClicked(bool)));

    m_saveAction->setEnabled(false);
    m_deleteAction->setEnabled(false);

    toolbar = addToolBar("ProfileToolbar");
    toolbar->setIconSize(applicationIconsSize());
    toolbar->setObjectName("ProfilerGUI_ProfileToolbar");
    toolbar->setContentsMargins(1, 0, 1, 0);

    toolbar->addAction(QIcon(imagePath("list")), tr("Blocks"), this, SLOT(onEditBlocksClicked(bool)));
    m_captureAction = toolbar->addAction(QIcon(imagePath("start")), tr("Capture"), this, SLOT(onCaptureClicked(bool)));
    m_captureAction->setEnabled(false);

    toolbar->addSeparator();
    m_connectAction = toolbar->addAction(QIcon(imagePath("connect")), tr("Connect"), this, SLOT(onConnectClicked(bool)));

    auto lbl = new QLabel("Address:", toolbar);
    lbl->setContentsMargins(5, 0, 2, 0);
    toolbar->addWidget(lbl);
    m_addressEdit = new QLineEdit();
    m_addressEdit->setToolTip("Enter IP-address or host name");
    //QRegExp rx("^0*(2(5[0-5]|[0-4]\\d)|1?\\d{1,2})(\\.0*(2(5[0-5]|[0-4]\\d)|1?\\d{1,2})){3}$");
    //m_addressEdit->setValidator(new QRegExpValidator(rx, m_addressEdit));
    m_addressEdit->setText(m_lastAddress);
    m_addressEdit->setFixedWidth((m_addressEdit->fontMetrics().width(QString("255.255.255.255")) * 3) / 2);
    toolbar->addWidget(m_addressEdit);

    lbl = new QLabel("Port:", toolbar);
    lbl->setContentsMargins(5, 0, 2, 0);
    toolbar->addWidget(lbl);
    m_portEdit = new QLineEdit();
    m_portEdit->setValidator(new QIntValidator(1, 65535, m_portEdit));
    m_portEdit->setText(QString::number(m_lastPort));
    m_portEdit->setFixedWidth(m_portEdit->fontMetrics().width(QString("000000")) + 10);
    toolbar->addWidget(m_portEdit);

    connect(m_addressEdit, &QLineEdit::returnPressed, [this] { onConnectClicked(true); });
    connect(m_portEdit, &QLineEdit::returnPressed, [this] { onConnectClicked(true); });



    toolbar = addToolBar("SetupToolbar");
    toolbar->setIconSize(applicationIconsSize());
    toolbar->setObjectName("ProfilerGUI_SetupToolbar");
    toolbar->setContentsMargins(1, 0, 1, 0);

    toolbar->addAction(QIcon(imagePath("expand")), "Expand all", this, SLOT(onExpandAllClicked(bool)));
    toolbar->addAction(QIcon(imagePath("collapse")), "Collapse all", this, SLOT(onCollapseAllClicked(bool)));

    action = toolbar->addAction(QIcon(imagePath("binoculars")), "Build stats tree");
    action->setToolTip("Build stats tree\nfor current visible area\nor zoom to current selected area.");
    connect(action, &QAction::triggered, [this] (bool) {
        static_cast<DiagramWidget*>(m_graphicsView->widget())->view()->inspectCurrentView(true);
    });

    action = toolbar->addAction(QIcon(imagePath("crop")), "Snapshot");
    action->setToolTip("Take a snapshot.\nSave selected area to\nseparate .prof file.");
    action->setEnabled(false);
    connect(action, &QAction::triggered, this, &This::onSnapshotClicked);
    connect(&EASY_GLOBALS.events, &profiler_gui::GlobalSignals::rulerVisible, action, &QAction::setEnabled);

    toolbar->addSeparator();
    auto menu = new QMenu("Settings", this);
    menu->setToolTipsVisible(true);

    auto toolButton = new QToolButton(toolbar);
    toolButton->setIcon(QIcon(imagePath("settings")));
    toolButton->setMenu(menu);
    toolButton->setPopupMode(QToolButton::InstantPopup);
    toolbar->addWidget(toolButton);


    auto submenu = menu->addMenu("Diagram");
    submenu->setToolTipsVisible(true);

    action = submenu->addAction("Draw borders");
    action->setToolTip("Draw borders for blocks on diagram.\nThis slightly reduces performance.");
    action->setCheckable(true);
    action->setChecked(EASY_GLOBALS.draw_graphics_items_borders);
    connect(action, &QAction::triggered, [this] (bool _checked) {
        EASY_GLOBALS.draw_graphics_items_borders = _checked;
        refreshDiagram();
    });

    action = submenu->addAction("Overlap narrow children");
    action->setToolTip("Children blocks will be overlaped by narrow\nparent blocks. See also \'Blocks narrow size\'.\nThis improves performance.");
    action->setCheckable(true);
    action->setChecked(EASY_GLOBALS.hide_narrow_children);
    connect(action, &QAction::triggered, [this] (bool _checked) { EASY_GLOBALS.hide_narrow_children = _checked; refreshDiagram(); });

    action = submenu->addAction("Hide min-size blocks");
    action->setToolTip("Hides blocks which screen size\nis less than \'Min blocks size\'.");
    action->setCheckable(true);
    action->setChecked(EASY_GLOBALS.hide_minsize_blocks);
    connect(action, &QAction::triggered, [this] (bool _checked) { EASY_GLOBALS.hide_minsize_blocks = _checked; refreshDiagram(); });

    action = submenu->addAction("Enable zero duration blocks on diagram");
    action->setToolTip("If checked then allows diagram to paint zero duration blocks\nwith 1px width on each scale. Otherwise, such blocks will be resized\nto 250ns duration.");
    action->setCheckable(true);
    action->setChecked(EASY_GLOBALS.enable_zero_length);
    connect(action, &QAction::triggered, [this] (bool _checked) { EASY_GLOBALS.enable_zero_length = _checked; refreshDiagram(); });

    action = submenu->addAction("Highlight similar blocks");
    action->setToolTip("Highlight all visible blocks which are similar\nto the current selected block.\nThis reduces performance.");
    action->setCheckable(true);
    action->setChecked(EASY_GLOBALS.highlight_blocks_with_same_id);
    connect(action, &QAction::triggered, [this] (bool _checked) { EASY_GLOBALS.highlight_blocks_with_same_id = _checked; refreshDiagram(); });

    action = submenu->addAction("Collapse blocks on tree reset");
    action->setToolTip("This collapses all blocks on diagram\nafter stats tree reset.");
    action->setCheckable(true);
    action->setChecked(EASY_GLOBALS.collapse_items_on_tree_close);
    connect(action, &QAction::triggered, this, &This::onCollapseItemsAfterCloseChanged);

    action = submenu->addAction("Expand all on file open");
    action->setToolTip("If checked then all blocks on diagram\nwill be initially expanded.");
    action->setCheckable(true);
    action->setChecked(EASY_GLOBALS.all_items_expanded_by_default);
    connect(action, &QAction::triggered, this, &This::onAllItemsExpandedByDefaultChange);

    action = submenu->addAction("Bind diagram and tree expand");
    action->setToolTip("Expanding/collapsing blocks at diagram expands/collapses\nblocks at stats tree and wise versa.");
    action->setCheckable(true);
    action->setChecked(EASY_GLOBALS.bind_scene_and_tree_expand_status);
    connect(action, &QAction::triggered, this, &This::onBindExpandStatusChange);

    action = submenu->addAction("Selecting block changes current thread");
    action->setToolTip("Automatically select thread while selecting a block.\nIf not checked then you will have to select current thread\nmanually double clicking on thread name on a diagram.");
    action->setCheckable(true);
    action->setChecked(EASY_GLOBALS.selecting_block_changes_thread);
    connect(action, &QAction::triggered, [this] (bool _checked) { EASY_GLOBALS.selecting_block_changes_thread = _checked; });

    action = submenu->addAction("Draw event markers");
    action->setToolTip("Display event markers under the blocks\n(even if event-blocks are not visible).\nThis slightly reduces performance.");
    action->setCheckable(true);
    action->setChecked(EASY_GLOBALS.enable_event_markers);
    connect(action, &QAction::triggered, [this] (bool _checked)
    {
        EASY_GLOBALS.enable_event_markers = _checked;
        refreshDiagram();
    });

    submenu->addSeparator();
    auto actionGroup = new QActionGroup(this);
    actionGroup->setExclusive(true);

    action = new QAction("Ruler text at top", actionGroup);
    action->setToolTip("Draw duration of selected interval\nat the top of the diagram.");
    action->setCheckable(true);
    action->setData(static_cast<int>(profiler_gui::RulerTextPosition_Top));
    if (EASY_GLOBALS.chrono_text_position == profiler_gui::RulerTextPosition_Top)
        action->setChecked(true);
    submenu->addAction(action);
    connect(action, &QAction::triggered, this, &This::onRulerTextPosChanged);

    action = new QAction("Ruler text at center", actionGroup);
    action->setToolTip("Draw duration of selected interval\nat the center of the diagram.");
    action->setCheckable(true);
    action->setData(static_cast<int>(profiler_gui::RulerTextPosition_Center));
    if (EASY_GLOBALS.chrono_text_position == profiler_gui::RulerTextPosition_Center)
        action->setChecked(true);
    submenu->addAction(action);
    connect(action, &QAction::triggered, this, &This::onRulerTextPosChanged);

    action = new QAction("Ruler text at bottom", actionGroup);
    action->setToolTip("Draw duration of selected interval\nat the bottom of the diagram.");
    action->setCheckable(true);
    action->setData(static_cast<int>(profiler_gui::RulerTextPosition_Bottom));
    if (EASY_GLOBALS.chrono_text_position == profiler_gui::RulerTextPosition_Bottom)
        action->setChecked(true);
    submenu->addAction(action);
    connect(action, &QAction::triggered, this, &This::onRulerTextPosChanged);

    submenu->addSeparator();
    auto w = new QWidget(submenu);
    auto l = new QHBoxLayout(w);
    l->setContentsMargins(26, 1, 16, 1);
    l->addWidget(new QLabel("Min blocks spacing, px", w), 0, Qt::AlignLeft);
    auto spinbox = new QSpinBox(w);
    spinbox->setRange(0, 400);
    spinbox->setValue(EASY_GLOBALS.blocks_spacing);
    spinbox->setFixedWidth(px(70));
    connect(spinbox, Overload<int>::of(&QSpinBox::valueChanged), this, &This::onSpacingChange);
    l->addWidget(spinbox);
    w->setLayout(l);
    auto waction = new QWidgetAction(submenu);
    waction->setDefaultWidget(w);
    submenu->addAction(waction);

    w = new QWidget(submenu);
    l = new QHBoxLayout(w);
    l->setContentsMargins(26, 1, 16, 1);
    l->addWidget(new QLabel("Min blocks size, px", w), 0, Qt::AlignLeft);
    spinbox = new QSpinBox(w);
    spinbox->setRange(1, 400);
    spinbox->setValue(EASY_GLOBALS.blocks_size_min);
    spinbox->setFixedWidth(px(70));
    connect(spinbox, Overload<int>::of(&QSpinBox::valueChanged), this, &This::onMinSizeChange);
    l->addWidget(spinbox);
    w->setLayout(l);
    waction = new QWidgetAction(submenu);
    waction->setDefaultWidget(w);
    submenu->addAction(waction);

    w = new QWidget(submenu);
    l = new QHBoxLayout(w);
    l->setContentsMargins(26, 1, 16, 1);
    l->addWidget(new QLabel("Blocks narrow size, px", w), 0, Qt::AlignLeft);
    spinbox = new QSpinBox(w);
    spinbox->setRange(1, 400);
    spinbox->setValue(EASY_GLOBALS.blocks_narrow_size);
    spinbox->setFixedWidth(px(70));
    connect(spinbox, Overload<int>::of(&QSpinBox::valueChanged), this, &This::onNarrowSizeChange);
    l->addWidget(spinbox);
    w->setLayout(l);
    waction = new QWidgetAction(submenu);
    waction->setDefaultWidget(w);
    submenu->addAction(waction);


    submenu = menu->addMenu("Histogram");
    submenu->setToolTipsVisible(true);

    action = submenu->addAction("Draw borders");
    action->setToolTip("Draw borders for histogram columns.");
    action->setCheckable(true);
    action->setChecked(EASY_GLOBALS.draw_histogram_borders);
    connect(action, &QAction::triggered, [this] (bool _checked) {
        EASY_GLOBALS.draw_histogram_borders = _checked;
        refreshHistogramImage();
    });

    action = submenu->addAction("Automatically adjust histogram height");
    action->setToolTip("You do not need to adjust boundaries manually,\n"
                       "but this restricts you from adjusting boundaries at all (zoom mode).\n"
                       "You can still adjust boundaries in overview mode though.");
    action->setCheckable(true);
    action->setChecked(EASY_GLOBALS.auto_adjust_histogram_height);
    connect(action, &QAction::triggered, [] (bool _checked)
    {
        EASY_GLOBALS.auto_adjust_histogram_height = _checked;
        emit EASY_GLOBALS.events.autoAdjustHistogramChanged();
    });

    action = submenu->addAction("Only frames on histogram");
    action->setToolTip("Display only top-level blocks on histogram.");
    action->setCheckable(true);
    action->setChecked(EASY_GLOBALS.display_only_frames_on_histogram);
    connect(action, &QAction::triggered, [this] (bool _checked)
    {
        EASY_GLOBALS.display_only_frames_on_histogram = _checked;
        emit EASY_GLOBALS.events.displayOnlyFramesOnHistogramChanged();
    });

    submenu->addSeparator();
    w = new QWidget(submenu);
    l = new QHBoxLayout(w);
    l->setContentsMargins(26, 1, 16, 1);
    l->addWidget(new QLabel("Min column width, px", w), 0, Qt::AlignLeft);
    spinbox = new QSpinBox(w);
    spinbox->setRange(1, 400);
    spinbox->setValue(EASY_GLOBALS.histogram_column_width_min);
    spinbox->setFixedWidth(px(70));
    connect(spinbox, Overload<int>::of(&QSpinBox::valueChanged), this, &This::onHistogramMinSizeChange);
    l->addWidget(spinbox);
    w->setLayout(l);
    waction = new QWidgetAction(submenu);
    waction->setDefaultWidget(w);
    waction->setToolTip("Minimum column width when borders are enabled.");
    submenu->addAction(waction);


    submenu = menu->addMenu("Stats");
    submenu->setToolTipsVisible(true);

    action = submenu->addAction("Statistics enabled");
    action->setCheckable(true);
    action->setChecked(EASY_GLOBALS.enable_statistics);
    connect(action, &QAction::triggered, this, &This::onEnableDisableStatistics);
    if (EASY_GLOBALS.enable_statistics)
    {
        auto f = action->font();
        f.setBold(true);
        action->setFont(f);
        action->setIcon(QIcon(imagePath("stats")));
    }
    else
    {
        action->setText("Statistics disabled");
        action->setIcon(QIcon(imagePath("stats-off")));
    }

    action = submenu->addAction("Build tree only for current thread");
    action->setToolTip("Stats tree will be built\nfor blocks from current thread only.\nThis improves performance\nand saves a lot of memory for Call-stack mode.");
    action->setCheckable(true);
    action->setChecked(EASY_GLOBALS.only_current_thread_hierarchy);
    connect(action, &QAction::triggered, this, &This::onHierarchyFlagChange);

    action = submenu->addAction("Add zero blocks to the tree");
    action->setToolTip("Zero duration blocks will be added into stats tree.\nThis reduces performance and increases memory consumption.");
    action->setCheckable(true);
    action->setChecked(EASY_GLOBALS.add_zero_blocks_to_hierarchy);
    connect(action, &QAction::triggered, [this](bool _checked)
    {
        EASY_GLOBALS.add_zero_blocks_to_hierarchy = _checked;
        emit EASY_GLOBALS.events.hierarchyFlagChanged(_checked);
    });

    action = submenu->addAction("Hide stats for single blocks in tree");
    action->setToolTip("If checked then such stats like Min,Max,Avg etc.\nwill not be displayed in stats tree for blocks\nwith number of calls == 1");
    action->setCheckable(true);
    action->setChecked(EASY_GLOBALS.hide_stats_for_single_blocks);
    connect(action, &QAction::triggered, this, &This::onDisplayRelevantStatsChange);

    w = new QWidget(submenu);
    l = new QHBoxLayout(w);
    l->setContentsMargins(26, 1, 16, 1);
    l->addWidget(new QLabel("Max rows in tree", w), 0, Qt::AlignLeft);
    spinbox = new QSpinBox(w);
    spinbox->setRange(0, std::numeric_limits<int>::max());
    spinbox->setValue(static_cast<int>(EASY_GLOBALS.max_rows_count));
    spinbox->setFixedWidth(px(100));
    connect(spinbox, Overload<int>::of(&QSpinBox::valueChanged), this, &This::onMaxBlocksCountChange);
    l->addWidget(spinbox);
    w->setLayout(l);
    waction = new QWidgetAction(submenu);
    waction->setDefaultWidget(w);
    submenu->addAction(waction);


    submenu = menu->addMenu("General");
    submenu->setToolTipsVisible(true);

    action = submenu->addAction("Automatically adjust values chart height");
    action->setToolTip("Automatically adjusts arbitrary values chart height\nfor currently visible region.");
    action->setCheckable(true);
    action->setChecked(EASY_GLOBALS.auto_adjust_chart_height);
    connect(action, &QAction::triggered, [](bool _checked)
    {
        EASY_GLOBALS.auto_adjust_chart_height = _checked;
        emit EASY_GLOBALS.events.autoAdjustChartChanged();
    });

    action = submenu->addAction("Use decorated thread names");
    action->setToolTip("Add \'Thread\' word into thread name if there is no one already.\nExamples: \'Render\' will change to \'Render Thread\'\n\'WorkerThread\' will not change.");
    action->setCheckable(true);
    action->setChecked(EASY_GLOBALS.use_decorated_thread_name);
    connect(action, &QAction::triggered, [this](bool _checked)
    {
        EASY_GLOBALS.use_decorated_thread_name = _checked;
        emit EASY_GLOBALS.events.threadNameDecorationChanged();
    });

    action = submenu->addAction("Display hex thread id");
    action->setToolTip("Display hex thread id instead of decimal.");
    action->setCheckable(true);
    action->setChecked(EASY_GLOBALS.hex_thread_id);
    connect(action, &QAction::triggered, [this](bool _checked)
    {
        EASY_GLOBALS.hex_thread_id = _checked;
        emit EASY_GLOBALS.events.hexThreadIdChanged();
    });


    submenu = menu->addMenu("FPS Monitor");
    w = new QWidget(submenu);
    l = new QHBoxLayout(w);
    l->setContentsMargins(26, 1, 16, 1);
    l->addWidget(new QLabel("Request interval, ms", w), 0, Qt::AlignLeft);
    spinbox = new QSpinBox(w);
    spinbox->setRange(1, 600000);
    spinbox->setValue(EASY_GLOBALS.fps_timer_interval);
    spinbox->setFixedWidth(px(70));
    connect(spinbox, Overload<int>::of(&QSpinBox::valueChanged), this, &This::onFpsIntervalChange);
    l->addWidget(spinbox);
    w->setLayout(l);
    waction = new QWidgetAction(submenu);
    waction->setDefaultWidget(w);
    submenu->addAction(waction);

    w = new QWidget(submenu);
    l = new QHBoxLayout(w);
    l->setContentsMargins(26, 1, 16, 1);
    l->addWidget(new QLabel("Max history size", w), 0, Qt::AlignLeft);
    spinbox = new QSpinBox(w);
    spinbox->setRange(2, 200);
    spinbox->setValue(EASY_GLOBALS.max_fps_history);
    spinbox->setFixedWidth(px(70));
    connect(spinbox, Overload<int>::of(&QSpinBox::valueChanged), this, &This::onFpsHistoryChange);
    l->addWidget(spinbox);
    w->setLayout(l);
    waction = new QWidgetAction(submenu);
    waction->setDefaultWidget(w);
    submenu->addAction(waction);

    w = new QWidget(submenu);
    l = new QHBoxLayout(w);
    l->setContentsMargins(26, 1, 16, 1);
    l->addWidget(new QLabel("Line width, px", w), 0, Qt::AlignLeft);
    spinbox = new QSpinBox(w);
    spinbox->setRange(1, 6);
    spinbox->setValue(EASY_GLOBALS.fps_widget_line_width);
    spinbox->setFixedWidth(px(70));
    connect(spinbox, Overload<int>::of(&QSpinBox::valueChanged), this, &This::onFpsMonitorLineWidthChange);
    l->addWidget(spinbox);
    w->setLayout(l);
    waction = new QWidgetAction(submenu);
    waction->setDefaultWidget(w);
    submenu->addAction(waction);




    submenu = menu->addMenu("Units");
    actionGroup = new QActionGroup(this);
    actionGroup->setExclusive(true);
    action = new QAction("Auto", actionGroup);
    action->setCheckable(true);
    action->setData(static_cast<int>(profiler_gui::TimeUnits_auto));
    if (EASY_GLOBALS.time_units == profiler_gui::TimeUnits_auto)
        action->setChecked(true);
    submenu->addAction(action);
    connect(action, &QAction::triggered, this, &This::onUnitsChanged);

    action = new QAction("Milliseconds", actionGroup);
    action->setCheckable(true);
    action->setData(static_cast<int>(profiler_gui::TimeUnits_ms));
    if (EASY_GLOBALS.time_units == profiler_gui::TimeUnits_ms)
        action->setChecked(true);
    submenu->addAction(action);
    connect(action, &QAction::triggered, this, &This::onUnitsChanged);

    action = new QAction("Microseconds", actionGroup);
    action->setCheckable(true);
    action->setData(static_cast<int>(profiler_gui::TimeUnits_us));
    if (EASY_GLOBALS.time_units == profiler_gui::TimeUnits_us)
        action->setChecked(true);
    submenu->addAction(action);
    connect(action, &QAction::triggered, this, &This::onUnitsChanged);

    action = new QAction("Nanoseconds", actionGroup);
    action->setCheckable(true);
    action->setData(static_cast<int>(profiler_gui::TimeUnits_ns));
    if (EASY_GLOBALS.time_units == profiler_gui::TimeUnits_ns)
        action->setChecked(true);
    submenu->addAction(action);
    connect(action, &QAction::triggered, this, &This::onUnitsChanged);


    submenu = menu->addMenu("Remote");
    m_eventTracingEnableAction = submenu->addAction("Event tracing enabled");
    m_eventTracingEnableAction->setCheckable(true);
    m_eventTracingEnableAction->setEnabled(false);
    connect(m_eventTracingEnableAction, &QAction::triggered, this, &This::onEventTracingEnableChange);

    m_eventTracingPriorityAction = submenu->addAction("Low priority event tracing");
    m_eventTracingPriorityAction->setCheckable(true);
    m_eventTracingPriorityAction->setChecked(EASY_OPTION_LOW_PRIORITY_EVENT_TRACING);
    m_eventTracingPriorityAction->setEnabled(false);
    connect(m_eventTracingPriorityAction, &QAction::triggered, this, &This::onEventTracingPriorityChange);


    submenu = menu->addMenu("Encoding");
    actionGroup = new QActionGroup(this);
    actionGroup->setExclusive(true);

    auto default_codec_mib = QTextCodec::codecForLocale()->mibEnum();
    {
        QList<QAction*> actions;

        for (int mib : QTextCodec::availableMibs())
        {
            auto codec = QTextCodec::codecForMib(mib)->name();

            action = new QAction(codec, actionGroup);
            action->setData(mib);
            action->setCheckable(true);
            if (mib == default_codec_mib)
                action->setChecked(true);

            actions.push_back(action);
            connect(action, &QAction::triggered, this, &This::onEncodingChanged);
        }

        qSort(actions.begin(), actions.end(), [](QAction* lhs, QAction* rhs) {
            return lhs->text().compare(rhs->text(), Qt::CaseInsensitive) < 0;
        });

        submenu->addActions(actions);
    }



    submenu = menu->addMenu("Appearance");

    actionGroup = new QActionGroup(this);
    actionGroup->setExclusive(true);

    action = submenu->addAction("Custom window headers");
    action->setCheckable(true);
    action->setChecked(EASY_GLOBALS.use_custom_window_header);
    connect(action, &QAction::triggered, [=] (bool checked)
    {
        actionGroup->setEnabled(checked);
        onCustomWindowHeaderTriggered(checked);
    });

    action = new QAction("Position: right", actionGroup);
    action->setCheckable(true);
    action->setChecked(EASY_GLOBALS.is_right_window_header_controls);
    connect(action, &QAction::triggered, this, &This::onRightWindowHeaderPosition);
    submenu->addAction(action);

    action = new QAction("Position: left", actionGroup);
    action->setCheckable(true);
    action->setChecked(!EASY_GLOBALS.is_right_window_header_controls);
    connect(action, &QAction::triggered, this, &This::onLeftWindowHeaderPosition);
    submenu->addAction(action);

    actionGroup->setEnabled(EASY_GLOBALS.use_custom_window_header);

    submenu->addSeparator();

    action = submenu->addAction("See viewport info");
    connect(action, &QAction::triggered, this, &This::onViewportInfoClicked);

    auto submenu2 = submenu->addMenu("Theme");
    actionGroup = new QActionGroup(this);
    actionGroup->setExclusive(true);

    for (const auto& theme : UI_themes())
    {
        action = new QAction(theme, actionGroup);
        action->setCheckable(true);
        action->setChecked(action->text() == EASY_GLOBALS.theme);
        connect(action, &QAction::triggered, this, &MainWindow::onThemeChange);
        submenu2->addAction(action);
    }



    auto tb_height = toolbar->height() + 4;
    toolbar = addToolBar("FrameToolbar");
    toolbar->setIconSize(applicationIconsSize());
    toolbar->setObjectName("ProfilerGUI_FrameToolbar");
    toolbar->setContentsMargins(1, 0, 1, 0);
    toolbar->setMinimumHeight(tb_height);

    lbl = new QLabel("Expected frame time:", toolbar);
    lbl->setContentsMargins(5, 2, 2, 2);
    toolbar->addWidget(lbl);

    m_frameTimeEdit = new QLineEdit();
    m_frameTimeEdit->setFixedWidth(px(80));
    auto val = new QDoubleValidator(m_frameTimeEdit);
    val->setLocale(QLocale::c());
    val->setBottom(0);
    val->setDecimals(3);
    m_frameTimeEdit->setValidator(val);
    m_frameTimeEdit->setText(QString::number(EASY_GLOBALS.frame_time * 1e-3, 'f', 3));
    connect(m_frameTimeEdit, &QLineEdit::editingFinished, this, &This::onFrameTimeEditFinish);
    connect(&EASY_GLOBALS.events, &profiler_gui::GlobalSignals::expectedFrameTimeChanged, this, &This::onFrameTimeChanged);
    toolbar->addWidget(m_frameTimeEdit);

    lbl = new QLabel("ms", toolbar);
    lbl->setContentsMargins(5, 2, 1, 1);
    toolbar->addWidget(lbl);

    m_readerTimer.setInterval(LOADER_TIMER_INTERVAL);

    connect(graphicsView->view(), &BlocksGraphicsView::intervalChanged, treeWidget->tree(), &BlocksTreeWidget::setTreeBlocks);
    connect(&m_readerTimer, &QTimer::timeout, this, &This::onFileReaderTimeout);
    connect(&m_listenerTimer, &QTimer::timeout, this, &This::onListenerTimerTimeout);
    connect(&m_fpsRequestTimer, &QTimer::timeout, this, &This::onFrameTimeRequestTimeout);
    

    loadGeometry();

    if(QCoreApplication::arguments().size() > 1)
    {
        auto opened_filename = QCoreApplication::arguments().at(1);
        loadFile(opened_filename);
    }

    using profiler_gui::GlobalSignals;
    connect(&EASY_GLOBALS.events, &GlobalSignals::blockStatusChanged, this, &This::onBlockStatusChange);
    connect(&EASY_GLOBALS.events, &GlobalSignals::blocksRefreshRequired, this, &This::onGetBlockDescriptionsClicked);
    connect(&EASY_GLOBALS.events, &GlobalSignals::selectValue, this, &This::onSelectValue);
}

MainWindow::~MainWindow()
{
}

void MainWindow::validateLastDir()
{
    if (m_lastFiles.empty())
        EASY_GLOBALS.lastFileDir = QString();
    else
        EASY_GLOBALS.lastFileDir = QFileInfo(m_lastFiles.front()).absoluteDir().canonicalPath();
}

//////////////////////////////////////////////////////////////////////////

void MainWindow::dragEnterEvent(QDragEnterEvent* drag_event)
{
    if (drag_event->mimeData()->hasUrls())
        drag_event->acceptProposedAction();
}

void MainWindow::dragMoveEvent(QDragMoveEvent* drag_event)
{
    if (drag_event->mimeData()->hasUrls())
        drag_event->acceptProposedAction();
}

void MainWindow::dragLeaveEvent(QDragLeaveEvent* drag_event)
{
    drag_event->accept();
}

void MainWindow::dropEvent(QDropEvent* drop_event)
{
    const auto& urls = drop_event->mimeData()->urls();
    if (!urls.empty())
    {
        if (m_bNetworkFileRegime)
        {
            // Warn user about unsaved network information and suggest to save
            auto result = Dialog::question(this, "Unsaved session"
                , "You have unsaved data!\nSave before opening new file?"
                , QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);

            if (result == QMessageBox::Yes)
            {
                onSaveFileClicked(true);
            }
            else if (result != QMessageBox::No)
            {
                // User cancelled opening new file
                return;
            }
        }

        loadFile(urls.front().toLocalFile());
    }
}

//////////////////////////////////////////////////////////////////////////

void MainWindow::onThemeChange(bool)
{
    auto action = qobject_cast<QAction*>(sender());
    if (action == nullptr)
        return;

    auto newTheme = action->text();
    if (m_theme != newTheme)
    {
        m_theme = std::move(newTheme);

        Dialog::information(this, "UI theme changed"
            , "You may need to restart the application\nto apply the theme correctly.");

        loadTheme(m_theme);
        validateLineEdits();
        configureSizes();
    }
}

//////////////////////////////////////////////////////////////////////////

void MainWindow::onOpenFileClicked(bool)
{
    auto action = qobject_cast<QAction*>(sender());
    if (action == nullptr)
        return;

    QString filename;

    if (action == m_loadActionMenu->menuAction())
        filename = QFileDialog::getOpenFileName(this, "Open EasyProfiler File", m_lastFiles.empty() ? QString() : m_lastFiles.front(), "EasyProfiler File (*.prof);;All Files (*.*)");
    else
        filename = action->text();

    if (!filename.isEmpty())
    {
        if (m_bNetworkFileRegime)
        {
            // Warn user about unsaved network information and suggest to save
            auto result = Dialog::question(this, "Unsaved session"
                , "You have unsaved data!\nSave before opening new file?"
                , QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);

            if (result == QMessageBox::Yes)
            {
                onSaveFileClicked(true);
            }
            else if (result != QMessageBox::No)
            {
                // User cancelled opening new file
                return;
            }
        }

        loadFile(filename);
    }
}

//////////////////////////////////////////////////////////////////////////

void MainWindow::addFileToList(const QString& filename, bool changeWindowTitle)
{
    auto index = m_lastFiles.indexOf(filename, 0);
    if (index >= 0)
    {
        if (index != 0)
        {
            // This file has been already loaded. Move it to the front.
            m_lastFiles.move(index, 0);
            auto fileActions = m_loadActionMenu->actions();
            auto action = fileActions.at(index);
            m_loadActionMenu->removeAction(action);
            m_loadActionMenu->insertAction(fileActions.front(), action);
            validateLastDir();
        }

        m_bOpenedCacheFile = filename.contains(NETWORK_CACHE_FILE);

        if (changeWindowTitle)
        {
            if (m_bOpenedCacheFile)
                setWindowTitle(QString("%1 - [%2] - UNSAVED network cache file").arg(profiler_gui::DEFAULT_WINDOW_TITLE).arg(filename));
            else
                setWindowTitle(QString("%1 - [%2]").arg(profiler_gui::DEFAULT_WINDOW_TITLE).arg(filename));
        }

        return;
    }

    m_lastFiles.push_front(filename);
    validateLastDir();

    auto action = new QAction(filename, this);
    connect(action, &QAction::triggered, this, &This::onOpenFileClicked);
    auto fileActions = m_loadActionMenu->actions();
    if (fileActions.empty())
        m_loadActionMenu->addAction(action);
    else
        m_loadActionMenu->insertAction(fileActions.front(), action);

    if (m_lastFiles.size() > 10)
    {
        // Keep 10 files at the list
        m_lastFiles.pop_back();
        m_loadActionMenu->removeAction(fileActions.back());
        delete fileActions.back();
    }

    m_bOpenedCacheFile = filename.contains(NETWORK_CACHE_FILE);

    if (changeWindowTitle)
    {
        if (m_bOpenedCacheFile)
            setWindowTitle(QString("%1 - [%2] - UNSAVED network cache file").arg(profiler_gui::DEFAULT_WINDOW_TITLE).arg(m_lastFiles.front()));
        else
            setWindowTitle(QString("%1 - [%2]").arg(profiler_gui::DEFAULT_WINDOW_TITLE).arg(m_lastFiles.front()));
    }
}

void MainWindow::loadFile(const QString& filename)
{
    const auto i = filename.lastIndexOf(QChar('/'));
    const auto j = filename.lastIndexOf(QChar('\\'));

    createProgressDialog(QString("Loading %1...").arg(filename.mid(std::max(i, j) + 1)));

    m_readerTimer.start();
    m_reader.load(filename);
}

void MainWindow::readStream(std::stringstream& _data)
{
    createProgressDialog(tr("Reading from stream..."));
    m_readerTimer.start();
    m_reader.load(_data);
}

//////////////////////////////////////////////////////////////////////////

void MainWindow::onSaveFileClicked(bool)
{
    if (m_serializedBlocks.empty())
        return;

    QString lastFile = m_lastFiles.empty() ? QString() : m_lastFiles.front();

    const auto i = lastFile.lastIndexOf(QChar('/'));
    const auto j = lastFile.lastIndexOf(QChar('\\'));
    auto k = std::max(i, j);

    QString dir;
    if (k > 0)
        dir = lastFile.mid(0, ++k);

    if (m_bNetworkFileRegime)
    {
        // Current file is network cache file, use current system time as output file name

        if (!dir.isEmpty())
            dir += QDateTime::currentDateTime().toString("/yyyy-MM-dd_HH-mm-ss.prof");
        else
            dir = QDateTime::currentDateTime().toString("yyyy-MM-dd_HH-mm-ss.prof");
    }
    else if (m_bOpenedCacheFile)
    {
        // Opened old network cache file, use it's last modification time as output file name

        QFileInfo fileInfo(lastFile);
        if (!fileInfo.exists())
        {
            // Can not open the file!

            Dialog::warning(this, "Warning"
                , "Cannot open source file.\nSaving incomplete.", QMessageBox::Close);

            m_lastFiles.pop_front();
            auto action = m_loadActionMenu->actions().front();
            m_loadActionMenu->removeAction(action);
            delete action;
            validateLastDir();
            return;
        }

        if (!dir.isEmpty())
            dir += fileInfo.lastModified().toString("/yyyy-MM-dd_HH-mm-ss.prof");
        else
            dir = fileInfo.lastModified().toString("yyyy-MM-dd_HH-mm-ss.prof");
    }
    else
    {
        dir = lastFile;
    }

    auto filename = QFileDialog::getSaveFileName(this, "Save EasyProfiler File", dir, "EasyProfiler File (*.prof);;All Files (*.*)");
    if (filename.isEmpty())
        return;

    if (EASY_GLOBALS.has_local_changes)
    {
        createProgressDialog(tr("Saving file..."));
        m_readerTimer.start();
        m_reader.save(filename, m_beginEndTime.beginTime, m_beginEndTime.endTime, m_serializedDescriptors,
                      EASY_GLOBALS.descriptors, m_descriptorsNumberInFile, EASY_GLOBALS.profiler_blocks,
                      EASY_GLOBALS.bookmarks, easyBlocksTree, EASY_GLOBALS.pid, false);
        return;
    }

    // Check if the same file has been selected
    {
        QFileInfo fileInfo1(m_bNetworkFileRegime ? QString(NETWORK_CACHE_FILE) : lastFile), fileInfo2(filename);
        if (fileInfo1.exists() && fileInfo2.exists() && fileInfo1 == fileInfo2)
        {
            // Selected the same file - do nothing
            return;
        }
    }

    bool inOk = false, outOk = false;
    int8_t retry1 = -1;
    while (++retry1 < 4)
    {
        std::ifstream inFile(m_bNetworkFileRegime ? NETWORK_CACHE_FILE : lastFile.toStdString().c_str(), std::fstream::binary);
        if (!inFile.is_open())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            continue;
        }

        inOk = true;

        int8_t retry2 = -1;
        while (++retry2 < 4)
        {
            std::ofstream outFile(filename.toStdString(), std::fstream::binary);
            if (!outFile.is_open())
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
                continue;
            }

            outFile << inFile.rdbuf();
            outOk = true;
            break;
        }

        break;
    }

    if (outOk)
    {
        if (m_bNetworkFileRegime)
        {
            // Remove temporary network cahche file
            QFile::remove(QString(NETWORK_CACHE_FILE));
        }
        else if (m_bOpenedCacheFile)
        {
            // Remove old temporary network cahche file

            QFile::remove(lastFile.toStdString().c_str());

            m_lastFiles.pop_front();
            auto action = m_loadActionMenu->actions().front();
            m_loadActionMenu->removeAction(action);
            delete action;
            validateLastDir();
        }

        addFileToList(filename);

        m_bNetworkFileRegime = false;
    }
    else if (inOk)
    {
        Dialog::warning(this, "Warning"
            , "Cannot open destination file.\nSaving incomplete.", QMessageBox::Close);
    }
    else
    {
        if (m_bNetworkFileRegime)
            Dialog::warning(this, "Warning"
                , "Cannot open network cache file.\nSaving incomplete.", QMessageBox::Close);
        else
            Dialog::warning(this, "Warning"
                , "Cannot open source file.\nSaving incomplete.", QMessageBox::Close);
    }
}

//////////////////////////////////////////////////////////////////////////

void MainWindow::clear()
{
    emit EASY_GLOBALS.events.allDataGoingToBeDeleted();

    EASY_GLOBALS.selected_thread = 0;
    profiler_gui::set_max(EASY_GLOBALS.selected_block);
    profiler_gui::set_max(EASY_GLOBALS.selected_block_id);
    EASY_GLOBALS.profiler_blocks.clear();
    EASY_GLOBALS.descriptors.clear();
    EASY_GLOBALS.gui_blocks.clear();

    m_serializedBlocks.clear();
    m_serializedDescriptors.clear();

    m_saveAction->setEnabled(false);
    m_deleteAction->setEnabled(false);

    if (m_bNetworkFileRegime)
        QFile::remove(QString(NETWORK_CACHE_FILE));

    m_bNetworkFileRegime = false;
    m_bOpenedCacheFile = false;

    setWindowTitle(profiler_gui::DEFAULT_WINDOW_TITLE);
}

//////////////////////////////////////////////////////////////////////////

void MainWindow::refreshDiagram()
{
    static_cast<DiagramWidget*>(m_graphicsView->widget())->view()->scene()->update();
}

void MainWindow::refreshHistogramImage()
{
    static_cast<DiagramWidget*>(m_graphicsView->widget())->view()->repaintHistogramImage();
}

//////////////////////////////////////////////////////////////////////////

void MainWindow::onDeleteClicked(bool)
{
    int button = QMessageBox::Yes;
    if (m_bNetworkFileRegime)
        button = Dialog::question(this, "Clear all profiled data"
            , "All profiled data and network cache file\nare going to be deleted!\nContinue?"
            , QMessageBox::Yes | QMessageBox::No);

    if (button == QMessageBox::Yes)
        clear();
}

//////////////////////////////////////////////////////////////////////////

void MainWindow::onExitClicked(bool)
{
    close();
}

//////////////////////////////////////////////////////////////////////////

void MainWindow::onEncodingChanged(bool)
{
    auto action = qobject_cast<QAction*>(sender());
    if (action == nullptr)
        return;

    const int mib = action->data().toInt();
    auto codec = QTextCodec::codecForMib(mib);
    if (codec != nullptr)
        QTextCodec::setCodecForLocale(codec);
}

void MainWindow::onRulerTextPosChanged(bool)
{
    auto _sender = qobject_cast<QAction*>(sender());
    EASY_GLOBALS.chrono_text_position = static_cast<profiler_gui::RulerTextPosition>(_sender->data().toInt());
    refreshDiagram();
}

void MainWindow::onUnitsChanged(bool)
{
    auto _sender = qobject_cast<QAction*>(sender());
    EASY_GLOBALS.time_units = static_cast<profiler_gui::TimeUnits>(_sender->data().toInt());
}

void MainWindow::onEnableDisableStatistics(bool _checked)
{
    EASY_GLOBALS.enable_statistics = _checked;

    auto action = qobject_cast<QAction*>(sender());
    if (action != nullptr)
    {
        auto f = action->font();
        f.setBold(_checked);
        action->setFont(f);

        if (_checked)
        {
            action->setText("Statistics enabled");
            action->setIcon(QIcon(imagePath("stats")));
        }
        else
        {
            action->setText("Statistics disabled");
            action->setIcon(QIcon(imagePath("stats-off")));
        }
    }
}

void MainWindow::onCollapseItemsAfterCloseChanged(bool _checked)
{
    EASY_GLOBALS.collapse_items_on_tree_close = _checked;
}

void MainWindow::onAllItemsExpandedByDefaultChange(bool _checked)
{
    EASY_GLOBALS.all_items_expanded_by_default = _checked;
}

void MainWindow::onBindExpandStatusChange(bool _checked)
{
    EASY_GLOBALS.bind_scene_and_tree_expand_status = _checked;
}

void MainWindow::onDisplayRelevantStatsChange(bool _checked)
{
    EASY_GLOBALS.hide_stats_for_single_blocks = _checked;
}

void MainWindow::onHierarchyFlagChange(bool _checked)
{
    EASY_GLOBALS.only_current_thread_hierarchy = _checked;
    emit EASY_GLOBALS.events.hierarchyFlagChanged(_checked);
}

//////////////////////////////////////////////////////////////////////////

void MainWindow::onExpandAllClicked(bool)
{
    for (auto& block : EASY_GLOBALS.gui_blocks)
        block.expanded = true;

    emit EASY_GLOBALS.events.itemsExpandStateChanged();

    auto tree = static_cast<StatsWidget*>(m_treeWidget->widget())->tree();
    const QSignalBlocker b(tree);
    tree->expandAll();
}

void MainWindow::onCollapseAllClicked(bool)
{
    for (auto& block : EASY_GLOBALS.gui_blocks)
        block.expanded = false;

    emit EASY_GLOBALS.events.itemsExpandStateChanged();

    auto tree = static_cast<StatsWidget*>(m_treeWidget->widget())->tree();
    const QSignalBlocker b(tree);
    tree->collapseAll();
}

void MainWindow::onViewportInfoClicked(bool)
{
    const auto& size = EASY_GLOBALS.size;

    auto contents = QString("Device pixel ratio = %1\nFont height = %2px\nFont line spacing = %3px\nDiagram row = %4px\nDiagram spacing = %5px\nIcon size = %6x%6 px")
        .arg(size.pixelRatio).arg(size.font_height).arg(size.font_line_spacing)
        .arg(size.graphics_row_height).arg(size.threads_row_spacing).arg(size.icon_size);

    Dialog::information(this, "Viewport info", contents);
}

//////////////////////////////////////////////////////////////////////////

void MainWindow::onMaxBlocksCountChange(int _value)
{
    if (_value >= 0)
    {
        EASY_GLOBALS.max_rows_count = static_cast<uint32_t>(_value);
    }
}

//////////////////////////////////////////////////////////////////////////

void MainWindow::onSpacingChange(int _value)
{
    EASY_GLOBALS.blocks_spacing = _value;
    refreshDiagram();
}

void MainWindow::onMinSizeChange(int _value)
{
    EASY_GLOBALS.blocks_size_min = _value;
    refreshDiagram();
}

void MainWindow::onHistogramMinSizeChange(int _value)
{
    EASY_GLOBALS.histogram_column_width_min = _value;
    refreshHistogramImage();
}

void MainWindow::onNarrowSizeChange(int _value)
{
    EASY_GLOBALS.blocks_narrow_size = _value;
    refreshDiagram();
}

//////////////////////////////////////////////////////////////////////////

void MainWindow::onFpsIntervalChange(int _value)
{
    EASY_GLOBALS.fps_timer_interval = _value;

    if (m_fpsRequestTimer.isActive())
        m_fpsRequestTimer.stop();

    if (EASY_GLOBALS.connected)
        m_fpsRequestTimer.start(_value);
}

void MainWindow::onFpsHistoryChange(int _value)
{
    EASY_GLOBALS.max_fps_history = _value;
}

void MainWindow::onFpsMonitorLineWidthChange(int _value)
{
    EASY_GLOBALS.fps_widget_line_width = _value;
}

//////////////////////////////////////////////////////////////////////////

void MainWindow::onEditBlocksClicked(bool)
{
    if (m_descTreeDialog.ptr != nullptr)
    {
        if (m_descTreeDialog.ptr->isMinimized())
        {
            m_descTreeDialog.ptr->setWindowState((m_descTreeDialog.ptr->windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
        }

        m_descTreeDialog.ptr->raise();
        m_descTreeDialog.ptr->setFocus();

        return;
    }

    m_dialogDescTree = new BlockDescriptorsWidget();
    m_descTreeDialog.create(m_dialogDescTree);
    connect(m_descTreeDialog.ptr, &QDialog::finished, this, &This::onDescTreeDialogClose);

    m_dialogDescTree->build();

    m_descTreeDialog.restoreGeometry();
    m_descTreeDialog.ptr->show();
    m_descTreeDialog.ptr->setFocus();
}

void MainWindow::onDescTreeDialogClose(int)
{
    disconnect(m_descTreeDialog.ptr, &QDialog::finished, this, &This::onDescTreeDialogClose);
    m_descTreeDialog.saveGeometry();
    m_dialogDescTree = nullptr;
    m_descTreeDialog.ptr = nullptr;
}

//////////////////////////////////////////////////////////////////////////

void MainWindow::validateLineEdits()
{
    m_addressEdit->setFixedWidth((m_addressEdit->fontMetrics().width(QString("255.255.255.255")) * 3) / 2);
    m_portEdit->setFixedWidth(m_portEdit->fontMetrics().width(QString("000000")) + 10);
    m_frameTimeEdit->setFixedWidth(m_frameTimeEdit->fontMetrics().width(QString("000.000")) + 5);
}

//////////////////////////////////////////////////////////////////////////

void MainWindow::showEvent(QShowEvent* show_event)
{
    Parent::showEvent(show_event);
    validateLineEdits();
    configureSizes();
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
    Parent::resizeEvent(event);
    centerDialogs();
}

void MainWindow::moveEvent(QMoveEvent* _event)
{
    Parent::moveEvent(_event);
    centerDialogs();
}

void MainWindow::centerDialogs()
{
    const auto center = rect().center();

    if (m_progress != nullptr)
    {
        m_progress->move(center.x() - (m_progress->width() >> 1), center.y() - (m_progress->height() >> 1));
    }

    if (m_listenerDialog != nullptr)
    {
        m_listenerDialog->move(center.x() - (m_listenerDialog->width() >> 1), center.y() - (m_listenerDialog->height() >> 1));
    }
}

void MainWindow::closeEvent(QCloseEvent* close_event)
{
    if (!m_bCloseAfterSave && (m_bNetworkFileRegime || EASY_GLOBALS.has_local_changes))
    {
        // Warn user about unsaved network information and suggest to save
        const auto result = Dialog::question(this, "Unsaved session"
            , "You have unsaved data!\nSave before exit?", QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        
        if (result == QMessageBox::Yes)
        {
            onSaveFileClicked(true);
            if (m_reader.isSaving() && m_progress != nullptr)
            {
                // Wait until finish and close
                m_bCloseAfterSave = true;
                close_event->ignore();
                return;
            }
        }
        else if (result == QMessageBox::Cancel)
        {
            close_event->ignore();
            return;
        }
    }

    emit EASY_GLOBALS.events.closeEvent();

    if (m_descTreeDialog.ptr != nullptr)
    {
        m_descTreeDialog.ptr->reject();
        m_descTreeDialog.ptr = nullptr;
        m_dialogDescTree = nullptr;
    }

    saveSettingsAndGeometry();

    Parent::closeEvent(close_event);
}

void MainWindow::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::ActivationChange)
    {
        emit activationChanged();
    }
}

//////////////////////////////////////////////////////////////////////////

void MainWindow::loadSettings()
{
    QSettings settings(profiler_gui::ORGANAZATION_NAME, profiler_gui::APPLICATION_NAME);
    settings.beginGroup("main");

    auto last_files = settings.value("last_files");
    if (!last_files.isNull())
    {
        m_lastFiles = last_files.toStringList();
        validateLastDir();
    }

    auto last_addr = settings.value("ip_address");
    if (!last_addr.isNull())
        m_lastAddress = last_addr.toString();

    auto last_port = settings.value("port");
    if (!last_port.isNull())
        m_lastPort = (uint16_t)last_port.toUInt();


    auto val = settings.value("chrono_text_position");
    if (!val.isNull())
        EASY_GLOBALS.chrono_text_position = static_cast<profiler_gui::RulerTextPosition>(val.toInt());

    val = settings.value("time_units");
    if (!val.isNull())
        EASY_GLOBALS.time_units = static_cast<profiler_gui::TimeUnits>(val.toInt());


    val = settings.value("frame_time");
    if (!val.isNull())
        EASY_GLOBALS.frame_time = val.toFloat();

    val = settings.value("max_rows_count");
    if (!val.isNull())
        EASY_GLOBALS.max_rows_count = val.toUInt();

    val = settings.value("blocks_spacing");
    if (!val.isNull())
        EASY_GLOBALS.blocks_spacing = val.toInt();

    val = settings.value("blocks_size_min");
    if (!val.isNull())
        EASY_GLOBALS.blocks_size_min = val.toInt();

    val = settings.value("histogram_column_width_min");
    if (!val.isNull())
        EASY_GLOBALS.histogram_column_width_min = val.toInt();

    val = settings.value("blocks_narrow_size");
    if (!val.isNull())
        EASY_GLOBALS.blocks_narrow_size = val.toInt();


    auto flag = settings.value("draw_graphics_items_borders");
    if (!flag.isNull())
        EASY_GLOBALS.draw_graphics_items_borders = flag.toBool();

    flag = settings.value("draw_histogram_borders");
    if (!flag.isNull())
        EASY_GLOBALS.draw_histogram_borders = flag.toBool();

    flag = settings.value("hide_narrow_children");
    if (!flag.isNull())
        EASY_GLOBALS.hide_narrow_children = flag.toBool();

    flag = settings.value("hide_minsize_blocks");
    if (!flag.isNull())
        EASY_GLOBALS.hide_minsize_blocks = flag.toBool();

    flag = settings.value("collapse_items_on_tree_close");
    if (!flag.isNull())
        EASY_GLOBALS.collapse_items_on_tree_close = flag.toBool();

    flag = settings.value("all_items_expanded_by_default");
    if (!flag.isNull())
        EASY_GLOBALS.all_items_expanded_by_default = flag.toBool();

    flag = settings.value("only_current_thread_hierarchy");
    if (!flag.isNull())
        EASY_GLOBALS.only_current_thread_hierarchy = flag.toBool();

    flag = settings.value("enable_zero_length");
    if (!flag.isNull())
        EASY_GLOBALS.enable_zero_length = flag.toBool();

    flag = settings.value("add_zero_blocks_to_hierarchy");
    if (!flag.isNull())
        EASY_GLOBALS.add_zero_blocks_to_hierarchy = flag.toBool();


    flag = settings.value("highlight_blocks_with_same_id");
    if (!flag.isNull())
        EASY_GLOBALS.highlight_blocks_with_same_id = flag.toBool();

    flag = settings.value("bind_scene_and_tree_expand_status");
    if (!flag.isNull())
        EASY_GLOBALS.bind_scene_and_tree_expand_status = flag.toBool();

    flag = settings.value("hide_stats_for_single_blocks");
    if (!flag.isNull())
        EASY_GLOBALS.hide_stats_for_single_blocks = flag.toBool();

    flag = settings.value("selecting_block_changes_thread");
    if (!flag.isNull())
        EASY_GLOBALS.selecting_block_changes_thread = flag.toBool();

    flag = settings.value("enable_event_indicators");
    if (!flag.isNull())
        EASY_GLOBALS.enable_event_markers = flag.toBool();

    flag = settings.value("auto_adjust_histogram_height");
    if (!flag.isNull())
        EASY_GLOBALS.auto_adjust_histogram_height = flag.toBool();

    flag = settings.value("auto_adjust_chart_height");
    if (!flag.isNull())
        EASY_GLOBALS.auto_adjust_chart_height = flag.toBool();

    flag = settings.value("display_only_frames_on_histogram");
    if (!flag.isNull())
        EASY_GLOBALS.display_only_frames_on_histogram = flag.toBool();

    flag = settings.value("use_decorated_thread_name");
    if (!flag.isNull())
        EASY_GLOBALS.use_decorated_thread_name = flag.toBool();

    flag = settings.value("hex_thread_id");
    if (!flag.isNull())
        EASY_GLOBALS.hex_thread_id = flag.toBool();

    flag = settings.value("fps_timer_interval");
    if (!flag.isNull())
        EASY_GLOBALS.fps_timer_interval = flag.toInt();

    flag = settings.value("max_fps_history");
    if (!flag.isNull())
        EASY_GLOBALS.max_fps_history = flag.toInt();

    flag = settings.value("fps_widget_line_width");
    if (!flag.isNull())
        EASY_GLOBALS.fps_widget_line_width = flag.toInt();

    flag = settings.value("enable_statistics");
    if (!flag.isNull())
        EASY_GLOBALS.enable_statistics = flag.toBool();

    QString encoding = settings.value("encoding", "UTF-8").toString();
    auto default_codec_mib = QTextCodec::codecForName(encoding.toStdString().c_str())->mibEnum();
    auto default_codec = QTextCodec::codecForMib(default_codec_mib);
    QTextCodec::setCodecForLocale(default_codec);

    auto theme = settings.value("theme");
    if (theme.isValid())
    {
        EASY_GLOBALS.theme = m_theme = theme.toString();
    }
    else
    {
        m_theme = EASY_GLOBALS.theme;
    }

    flag = settings.value("use_custom_window_header");
    if (!flag.isNull())
        EASY_GLOBALS.use_custom_window_header = flag.toBool();

    flag = settings.value("is_right_window_header_controls");
    if (!flag.isNull())
        EASY_GLOBALS.is_right_window_header_controls = flag.toBool();

    settings.endGroup();
}

void MainWindow::loadGeometry()
{
    QSettings settings(profiler_gui::ORGANAZATION_NAME, profiler_gui::APPLICATION_NAME);
    settings.beginGroup("main");

    auto geometry = settings.value("geometry").toByteArray();
    if (!geometry.isEmpty())
        restoreGeometry(geometry);

    geometry = settings.value("fpsGeometry").toByteArray();
    if (!geometry.isEmpty())
        m_fpsViewer->restoreGeometry(geometry);

    geometry = settings.value("descTreeDialogGeometry").toByteArray();
    if (!geometry.isEmpty())
        m_descTreeDialog.geometry.swap(geometry);

    geometry = settings.value("diagramGeometry").toByteArray();
    if (!geometry.isEmpty())
        m_graphicsView->restoreGeometry(geometry);

    static_cast<DiagramWidget*>(m_graphicsView->widget())->restore(settings);

    auto state = settings.value("windowState").toByteArray();
    if (!state.isEmpty())
        restoreState(state);

    settings.endGroup();
}

void MainWindow::saveSettingsAndGeometry()
{
    QSettings settings(profiler_gui::ORGANAZATION_NAME, profiler_gui::APPLICATION_NAME);
    settings.beginGroup("main");

    settings.setValue("geometry", this->saveGeometry());
    settings.setValue("fpsGeometry", m_fpsViewer->saveGeometry());
    settings.setValue("descTreeDialogGeometry", m_descTreeDialog.geometry);
    settings.setValue("diagramGeometry", m_graphicsView->saveGeometry());
    static_cast<DiagramWidget*>(m_graphicsView->widget())->save(settings);

    settings.setValue("windowState", this->saveState());

    settings.setValue("last_files", m_lastFiles);
    settings.setValue("ip_address", m_lastAddress);
    settings.setValue("port", (quint32)m_lastPort);
    settings.setValue("chrono_text_position", static_cast<int>(EASY_GLOBALS.chrono_text_position));
    settings.setValue("time_units", static_cast<int>(EASY_GLOBALS.time_units));
    settings.setValue("frame_time", EASY_GLOBALS.frame_time);
    settings.setValue("max_rows_count", EASY_GLOBALS.max_rows_count);
    settings.setValue("blocks_spacing", EASY_GLOBALS.blocks_spacing);
    settings.setValue("blocks_size_min", EASY_GLOBALS.blocks_size_min);
    settings.setValue("histogram_column_width_min", EASY_GLOBALS.histogram_column_width_min);
    settings.setValue("blocks_narrow_size", EASY_GLOBALS.blocks_narrow_size);
    settings.setValue("draw_graphics_items_borders", EASY_GLOBALS.draw_graphics_items_borders);
    settings.setValue("draw_histogram_borders", EASY_GLOBALS.draw_histogram_borders);
    settings.setValue("hide_narrow_children", EASY_GLOBALS.hide_narrow_children);
    settings.setValue("hide_minsize_blocks", EASY_GLOBALS.hide_minsize_blocks);
    settings.setValue("collapse_items_on_tree_close", EASY_GLOBALS.collapse_items_on_tree_close);
    settings.setValue("all_items_expanded_by_default", EASY_GLOBALS.all_items_expanded_by_default);
    settings.setValue("only_current_thread_hierarchy", EASY_GLOBALS.only_current_thread_hierarchy);
    settings.setValue("enable_zero_length", EASY_GLOBALS.enable_zero_length);
    settings.setValue("add_zero_blocks_to_hierarchy", EASY_GLOBALS.add_zero_blocks_to_hierarchy);
    settings.setValue("highlight_blocks_with_same_id", EASY_GLOBALS.highlight_blocks_with_same_id);
    settings.setValue("bind_scene_and_tree_expand_status", EASY_GLOBALS.bind_scene_and_tree_expand_status);
    settings.setValue("hide_stats_for_single_blocks", EASY_GLOBALS.hide_stats_for_single_blocks);
    settings.setValue("selecting_block_changes_thread", EASY_GLOBALS.selecting_block_changes_thread);
    settings.setValue("enable_event_indicators", EASY_GLOBALS.enable_event_markers);
    settings.setValue("auto_adjust_histogram_height", EASY_GLOBALS.auto_adjust_histogram_height);
    settings.setValue("auto_adjust_chart_height", EASY_GLOBALS.auto_adjust_chart_height);
    settings.setValue("display_only_frames_on_histogram", EASY_GLOBALS.display_only_frames_on_histogram);
    settings.setValue("use_decorated_thread_name", EASY_GLOBALS.use_decorated_thread_name);
    settings.setValue("hex_thread_id", EASY_GLOBALS.hex_thread_id);
    settings.setValue("enable_statistics", EASY_GLOBALS.enable_statistics);
    settings.setValue("fps_timer_interval", EASY_GLOBALS.fps_timer_interval);
    settings.setValue("max_fps_history", EASY_GLOBALS.max_fps_history);
    settings.setValue("fps_widget_line_width", EASY_GLOBALS.fps_widget_line_width);
    settings.setValue("use_custom_window_header", EASY_GLOBALS.use_custom_window_header);
    settings.setValue("is_right_window_header_controls", EASY_GLOBALS.is_right_window_header_controls);
    settings.setValue("encoding", QTextCodec::codecForLocale()->name());
    settings.setValue("theme", m_theme);

    settings.endGroup();
}

void MainWindow::destroyProgressDialog()
{
    if (m_progress != nullptr)
    {
        m_progress->setValue(100);
        m_progress->deleteLater();
        m_progress = nullptr;
    }
}

void MainWindow::createProgressDialog(const QString& text)
{
    destroyProgressDialog();

    m_progress = new RoundProgressDialog(text, this);
    m_progress->setObjectName(QStringLiteral("LoadProgress"));
    connect(m_progress, &RoundProgressDialog::rejected, this, &This::onFileReaderCancel);

    m_progress->setModal(true);
    m_progress->setValue(0);
    m_progress->show();

    const auto pos = rect().center();
    m_progress->move(pos.x() - (m_progress->width() >> 1), pos.y() - (m_progress->height() >> 1));
}

void MainWindow::setDisconnected(bool _showMessage)
{
    if (m_fpsRequestTimer.isActive())
        m_fpsRequestTimer.stop();

    if (_showMessage)
        Dialog::warning(this, "Warning", "Connection was lost", QMessageBox::Close);

    EASY_GLOBALS.connected = false;
    m_captureAction->setEnabled(false);
    m_connectAction->setIcon(QIcon(imagePath("connect")));
    m_connectAction->setText(tr("Connect"));

    m_eventTracingEnableAction->setEnabled(false);
    m_eventTracingPriorityAction->setEnabled(false);

    m_addressEdit->setEnabled(true);
    m_portEdit->setEnabled(true);

    emit EASY_GLOBALS.events.connectionChanged(false);

}

//////////////////////////////////////////////////////////////////////////

void MainWindow::onFrameTimeRequestTimeout()
{
    if (EASY_GLOBALS.fps_enabled && EASY_GLOBALS.connected &&
        (m_listener.regime() == ListenerRegime::Idle || m_listener.regime() == ListenerRegime::Capture))
    {
        if (m_listener.requestFrameTime())
        {
            QTimer::singleShot(100, this, &This::checkFrameTimeReady);
        }
        else if (!m_listener.connected())
        {
            m_listener.closeSocket();
            setDisconnected();
        }
    }
}

void MainWindow::checkFrameTimeReady()
{
    if (EASY_GLOBALS.fps_enabled && EASY_GLOBALS.connected &&
        (m_listener.regime() == ListenerRegime::Idle || m_listener.regime() == ListenerRegime::Capture))
    {
        uint32_t maxTime = 0, avgTime = 0;
        if (m_listener.frameTime(maxTime, avgTime))
        {
            static_cast<FpsWidget*>(m_fpsViewer->widget())->addPoint(maxTime, avgTime);
        }
        else if (m_fpsRequestTimer.isActive())
        {
            QTimer::singleShot(100, this, &This::checkFrameTimeReady);
        }
    }
}

//////////////////////////////////////////////////////////////////////////

void MainWindow::onListenerTimerTimeout()
{
    const auto now = std::chrono::system_clock::now();
    const auto passedTime = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_listenStartTime).count();
    const auto seconds = static_cast<double>(passedTime) * 1e-3;

    switch (m_listener.regime())
    {
        case ListenerRegime::Capture:
        {
            m_listenerDialog->setTitle(QString("Capturing frames... %1s").arg(seconds, 0, 'f', 1));
            break;
        }

        case ListenerRegime::Capture_Receive:
        {
            m_listenerDialog->setTitle(QString("Receiving data... %1s").arg(seconds, 0, 'f', 1));
            break;
        }

        default:
        {
            break;
        }
    }

    if (!m_listener.connected())
    {
        if (m_listener.regime() == ListenerRegime::Capture_Receive)
            m_listener.finalizeCapture();
        if (m_listenerDialog)
            m_listenerDialog->reject();
    }
    else if (m_listener.regime() == ListenerRegime::Capture_Receive)
    {
        if (m_listener.captured())
        {
            if (m_listenerTimer.isActive())
                m_listenerTimer.stop();

            m_listener.finalizeCapture();

            m_listenerDialog->accept();
            m_listenerDialog = nullptr;

            if (m_listener.size() != 0)
            {
                readStream(m_listener.data());
                m_listener.clearData();
            }
        }
    }
}

void MainWindow::onListenerDialogClose(int _result)
{
    if (m_listener.regime() != ListenerRegime::Capture_Receive || !m_listener.connected())
    {
        if (m_listenerTimer.isActive())
            m_listenerTimer.stop();
    }

    disconnect(m_listenerDialog, &QDialog::finished, this, &This::onListenerDialogClose);
    m_listenerDialog = nullptr;

    switch (m_listener.regime())
    {
        case ListenerRegime::Capture:
        {
            m_listenStartTime = std::chrono::system_clock::now();
            m_listenerDialog = new RoundProgressDialog(
                QStringLiteral("Receiving data..."),
                RoundProgressIndicator::Cross,
                QDialog::Rejected,
                this
            );
            m_listenerDialog->setObjectName("ReceiveProgress");
            m_listenerDialog->setAttribute(Qt::WA_DeleteOnClose, true);
            m_listenerDialog->show();
            centerDialogs();

            m_listener.stopCapture();

            if (m_listener.regime() != ListenerRegime::Capture_Receive)
            {
                m_listenerDialog->reject();
                m_listenerDialog = nullptr;
            }
            else
            {
                connect(m_listenerDialog, &QDialog::finished, this, &This::onListenerDialogClose);
                m_listenerTimer.start(250);
            }

            break;
        }

        case ListenerRegime::Capture_Receive:
        {
            if (!m_listener.captured())
            {
                if (_result == QDialog::Accepted)
                {
                    m_listenStartTime = std::chrono::system_clock::now();
                    m_listenerDialog = new RoundProgressDialog(
                        QStringLiteral("Receiving data..."),
                        RoundProgressIndicator::Cross,
                        QDialog::Rejected,
                        this
                    );
                    m_listenerDialog->setObjectName("ReceiveProgress");
                    connect(m_listenerDialog, &QDialog::finished, this, &This::onListenerDialogClose);
                    m_listenerDialog->setAttribute(Qt::WA_DeleteOnClose, true);
                    m_listenerDialog->show();
                    centerDialogs();
                }
                else
                {
                    m_listener.finalizeCapture();
                    m_listener.clearData();

                    if (m_listener.connected())
                    {
                        // make reconnect to clear socket buffers
                        const std::string address = m_listener.address();
                        const auto port = m_listener.port();

                        profiler::net::EasyProfilerStatus reply(false, false, false);
                        if (m_listener.reconnect(address.c_str(), port, reply))
                        {
                            disconnect(m_eventTracingEnableAction, &QAction::triggered, this, &This::onEventTracingEnableChange);
                            disconnect(m_eventTracingPriorityAction, &QAction::triggered, this, &This::onEventTracingPriorityChange);

                            m_eventTracingEnableAction->setChecked(reply.isEventTracingEnabled);
                            m_eventTracingPriorityAction->setChecked(reply.isLowPriorityEventTracing);

                            connect(m_eventTracingEnableAction, &QAction::triggered, this, &This::onEventTracingEnableChange);
                            connect(m_eventTracingPriorityAction, &QAction::triggered, this, &This::onEventTracingPriorityChange);

                            if (reply.isProfilerEnabled)
                            {
                                // Connected application is already profiling.
                                // Show capture dialog immediately
                                onCaptureClicked(true);
                            }
                        }
                    }
                }

                break;
            }

            if (m_listenerTimer.isActive())
                m_listenerTimer.stop();

            m_listener.finalizeCapture();

            if (m_listener.size() != 0)
            {
                readStream(m_listener.data());
                m_listener.clearData();
            }

            break;
        }

        case ListenerRegime::Descriptors:
        {
            break;
        }

        default:
            return;
    }

    if (!m_listener.connected())
    {
        m_listener.closeSocket();
        setDisconnected();
    }
}


//////////////////////////////////////////////////////////////////////////

void MainWindow::closeProgressDialogAndClearReader()
{
    m_reader.interrupt();
    m_readerTimer.stop();
    destroyProgressDialog();
}

void MainWindow::onLoadingFinish(profiler::block_index_t& _nblocks)
{
    _nblocks = m_reader.size();
    if (_nblocks != 0)
    {
        emit EASY_GLOBALS.events.allDataGoingToBeDeleted();
        EASY_GLOBALS.has_local_changes = false;

        profiler::SerializedData serialized_blocks;
        profiler::SerializedData serialized_descriptors;
        profiler::descriptors_list_t descriptors;
        profiler::blocks_t blocks;
        profiler::thread_blocks_tree_t threads_map;
        profiler::bookmarks_t bookmarks;
        profiler::BeginEndTime beginEndTime;
        QString filename;
        uint32_t descriptorsNumberInFile = 0;
        uint32_t version = 0;
        profiler::processid_t pid = 0;

        m_reader.get(serialized_blocks, serialized_descriptors, descriptors, blocks, threads_map,
                     bookmarks, beginEndTime, descriptorsNumberInFile, version, pid, filename);

        if (threads_map.size() > 0xff)
        {
            QString message;

            if (m_reader.isFile())
                message = QString("File %1 contains %2 threads!").arg(filename).arg(threads_map.size());
            else
                message = QString("Input stream contains %1 threads!").arg(threads_map.size());

            Dialog::warning(this, "Warning",
                            QString("%1\nCurrently, maximum number of displayed threads is 255!"
                                    "\nSome threads will not be displayed.").arg(message), QMessageBox::Close);
        }

        m_bNetworkFileRegime = !m_reader.isFile();
        if (!m_bNetworkFileRegime)
        {
            addFileToList(filename);
        }
        else
        {
            m_bOpenedCacheFile = false;
            setWindowTitle(QString("%1 - UNSAVED network cache").arg(profiler_gui::DEFAULT_WINDOW_TITLE));
        }

        m_serializedBlocks = std::move(serialized_blocks);
        m_serializedDescriptors = std::move(serialized_descriptors);
        m_descriptorsNumberInFile = descriptorsNumberInFile;
        m_beginEndTime = beginEndTime;
        EASY_GLOBALS.selected_thread = 0;
        EASY_GLOBALS.version = version;
        EASY_GLOBALS.pid = pid;
        profiler_gui::set_max(EASY_GLOBALS.selected_block);
        profiler_gui::set_max(EASY_GLOBALS.selected_block_id);
        EASY_GLOBALS.profiler_blocks.swap(threads_map);
        EASY_GLOBALS.descriptors.swap(descriptors);
        EASY_GLOBALS.bookmarks.swap(bookmarks);

        EASY_GLOBALS.gui_blocks.clear();
        EASY_GLOBALS.gui_blocks.resize(_nblocks);
        memset(EASY_GLOBALS.gui_blocks.data(), 0, sizeof(profiler_gui::EasyBlock) * _nblocks);

        for (std::remove_reference<decltype(_nblocks)>::type i = 0; i < _nblocks; ++i)
        {
            auto& guiblock = EASY_GLOBALS.gui_blocks[i];
            guiblock.tree = std::move(blocks[i]);
        }

        m_saveAction->setEnabled(true);
        m_deleteAction->setEnabled(true);
    }
    else
    {
        Dialog::warning(this, "Warning", QString("Cannot read profiled blocks.\n\nReason:\n%1")
            .arg(m_reader.getError()), QMessageBox::Close);

        if (m_reader.isFile())
        {
            auto index = m_lastFiles.indexOf(m_reader.filename(), 0);
            if (index >= 0)
            {
                // Remove unexisting file from list
                m_lastFiles.removeAt(index);
                auto action = m_loadActionMenu->actions().at(index);
                m_loadActionMenu->removeAction(action);
                delete action;
                validateLastDir();
            }
        }
    }
}

void MainWindow::onSavingFinish()
{
    const auto errorMessage = m_reader.getError();
    if (!errorMessage.isEmpty())
    {
        Dialog::warning(this, "Warning", QString("Cannot save profiled blocks.\n\nReason:\n%1")
            .arg(errorMessage), QMessageBox::Close);
    }
    else
    {
        if (!m_reader.isSnapshot())
            EASY_GLOBALS.has_local_changes = false;
        addFileToList(m_reader.filename(), !m_reader.isSnapshot());
    }
}

void MainWindow::onFileReaderTimeout()
{
    if (m_reader.done())
    {
        if (m_reader.isLoading())
        {
            profiler::block_index_t nblocks = 0;

            onLoadingFinish(nblocks);
            closeProgressDialogAndClearReader();

            if (nblocks != 0)
            {
                emit EASY_GLOBALS.events.fileOpened();
                if (EASY_GLOBALS.all_items_expanded_by_default)
                    onExpandAllClicked(true);
            }
        }
        else if (m_reader.isSaving())
        {
            onSavingFinish();
            closeProgressDialogAndClearReader();
            if (m_bCloseAfterSave)
            {
                setEnabled(false);
                QTimer::singleShot(1500, this, &This::close);
            }
        }
        else
        {
            closeProgressDialogAndClearReader();
        }
    }
    else if (m_progress != nullptr)
    {
        m_progress->setValue(m_reader.progress());
    }
}

void MainWindow::onFileReaderCancel()
{
    m_readerTimer.stop();
    m_reader.interrupt();
    destroyProgressDialog();
}

//////////////////////////////////////////////////////////////////////////

void MainWindow::onEventTracingPriorityChange(bool _checked)
{
    if (EASY_GLOBALS.connected)
        m_listener.send(profiler::net::BoolMessage(profiler::net::MessageType::Change_Event_Tracing_Priority, _checked));
}

void MainWindow::onEventTracingEnableChange(bool _checked)
{
    if (EASY_GLOBALS.connected)
        m_listener.send(profiler::net::BoolMessage(profiler::net::MessageType::Change_Event_Tracing_Status, _checked));
}

//////////////////////////////////////////////////////////////////////////

void MainWindow::onFrameTimeEditFinish()
{
    auto text = m_frameTimeEdit->text();
    if (text.contains(QChar(',')))
    {
        text.remove(QChar('.')).replace(QChar(','), QChar('.'));
        m_frameTimeEdit->setText(text);
    }

    EASY_GLOBALS.frame_time = text.toFloat() * 1e3f;
    emit EASY_GLOBALS.events.expectedFrameTimeChanged();
}

void MainWindow::onFrameTimeChanged()
{
    m_frameTimeEdit->setText(QString::number(EASY_GLOBALS.frame_time * 1e-3, 'f', 3));
}

//////////////////////////////////////////////////////////////////////////

void MainWindow::onSnapshotClicked(bool)
{
    profiler::timestamp_t beginTime = 0, endTime = 0;
    const bool hasSelection = static_cast<DiagramWidget*>(m_graphicsView->widget())->view()->getSelectionRegionForSaving(beginTime, endTime);
    if (!hasSelection)
        return;

    QString lastFile = m_lastFiles.empty() ? QString() : m_lastFiles.front();

    const auto i = lastFile.lastIndexOf(QChar('/'));
    const auto j = lastFile.lastIndexOf(QChar('\\'));
    auto k = std::max(i, j);

    QString dir;
    if (k > 0)
        dir = lastFile.mid(0, ++k);

    auto filename = QFileDialog::getSaveFileName(this, "Save cropped area to EasyProfiler File", dir,
                                                 "EasyProfiler File (*.prof);;All Files (*.*)");
    if (filename.isEmpty())
        return;

    createProgressDialog(tr("Saving selected region..."));
    m_readerTimer.start();

    m_reader.save(filename, beginTime, endTime, m_serializedDescriptors, EASY_GLOBALS.descriptors,
                  m_descriptorsNumberInFile, EASY_GLOBALS.profiler_blocks, EASY_GLOBALS.bookmarks,
                  easyBlocksTree, EASY_GLOBALS.pid, true);
}

//////////////////////////////////////////////////////////////////////////

void MainWindow::onCustomWindowHeaderTriggered(bool _checked)
{
    EASY_GLOBALS.use_custom_window_header = _checked;
    emit EASY_GLOBALS.events.customWindowHeaderChanged();
}

void MainWindow::onRightWindowHeaderPosition(bool _checked)
{
    if (!_checked)
        return;

    EASY_GLOBALS.is_right_window_header_controls = true;
    emit EASY_GLOBALS.events.windowHeaderPositionChanged();
}

void MainWindow::onLeftWindowHeaderPosition(bool _checked)
{
    if (!_checked)
        return;

    EASY_GLOBALS.is_right_window_header_controls = false;
    emit EASY_GLOBALS.events.windowHeaderPositionChanged();
}

//////////////////////////////////////////////////////////////////////////

void MainWindow::onConnectClicked(bool)
{
    if (EASY_GLOBALS.connected)
    {
        // Disconnect if already connected
        m_listener.disconnect();
        setDisconnected(false);
        return;
    }

    QString address = m_addressEdit->text();
    const decltype(m_lastPort) port = m_portEdit->text().toUShort();

    const bool isSameAddress = (EASY_GLOBALS.connected && m_listener.port() == port &&
        address.toStdString() == m_listener.address());

    profiler::net::EasyProfilerStatus reply(false, false, false);
    if (!m_listener.connect(address.toStdString().c_str(), port, reply))
    {
        Dialog::warning(this, "Warning", QString("Cannot connect to %1").arg(address), QMessageBox::Close);
        // we need to close socket everytime on failed connection
        m_listener.closeSocket();
        setDisconnected(false);

        if (!isSameAddress)
        {
            m_lastAddress = std::move(address);
            m_lastPort = port;
        }

        return;
    }

    m_lastAddress = std::move(address);
    m_lastPort = port;

    qInfo() << "Connected successfully";
    EASY_GLOBALS.connected = true;
    m_captureAction->setEnabled(true);
    m_connectAction->setIcon(QIcon(imagePath("connected")));
    m_connectAction->setText(tr("Disconnect"));

    if (m_fpsViewer->isVisible())
        static_cast<FpsWidget*>(m_fpsViewer->widget())->clear();

    if (!m_fpsRequestTimer.isActive())
        m_fpsRequestTimer.start(EASY_GLOBALS.fps_timer_interval);

    disconnect(m_eventTracingEnableAction, &QAction::triggered, this, &This::onEventTracingEnableChange);
    disconnect(m_eventTracingPriorityAction, &QAction::triggered, this, &This::onEventTracingPriorityChange);

    m_eventTracingEnableAction->setEnabled(true);
    m_eventTracingPriorityAction->setEnabled(true);

    m_eventTracingEnableAction->setChecked(reply.isEventTracingEnabled);
    m_eventTracingPriorityAction->setChecked(reply.isLowPriorityEventTracing);

    connect(m_eventTracingEnableAction, &QAction::triggered, this, &This::onEventTracingEnableChange);
    connect(m_eventTracingPriorityAction, &QAction::triggered, this, &This::onEventTracingPriorityChange);

    m_addressEdit->setEnabled(false);
    m_portEdit->setEnabled(false);

    emit EASY_GLOBALS.events.connectionChanged(true);

    if (reply.isProfilerEnabled)
    {
        // Connected application is already profiling.
        // Show capture dialog immediately
        onCaptureClicked(true);
    }
}

void MainWindow::onCaptureClicked(bool)
{
    if (!EASY_GLOBALS.connected)
    {
        Dialog::warning(this, "Warning", "No connection with profiling app", QMessageBox::Close);
        return;
    }

    if (m_listener.regime() != ListenerRegime::Idle)
    {
        if (m_listener.regime() == ListenerRegime::Capture || m_listener.regime() == ListenerRegime::Capture_Receive)
        {
            Dialog::warning(this, "Warning",
                "Already capturing frames.\nFinish old capturing session first.", QMessageBox::Close);
        }
        else
        {
            Dialog::warning(this, "Warning",
                "Capturing blocks description.\nFinish old capturing session first.", QMessageBox::Close);
        }

        return;
    }

    if (!m_listener.startCapture())
    {
        // Connection lost. Try to restore connection.

        profiler::net::EasyProfilerStatus reply(false, false, false);
        if (!m_listener.connect(m_lastAddress.toStdString().c_str(), m_lastPort, reply))
        {
            m_listener.closeSocket();
            setDisconnected();
            return;
        }

        if (!m_listener.startCapture())
        {
            m_listener.closeSocket();
            setDisconnected();
            return;
        }
    }

    m_listenerTimer.start(250);

    m_listenStartTime = std::chrono::system_clock::now();
    m_listenerDialog = new RoundProgressDialog(
        QStringLiteral("Capturing frames..."),
        RoundProgressIndicator::Stop,
        QDialog::Accepted,
        this
    );
    m_listenerDialog->setObjectName("CaptureProgress");

    m_listenerDialog->setAttribute(Qt::WA_DeleteOnClose, true);
    connect(m_listenerDialog, &QDialog::finished, this, &This::onListenerDialogClose);
    m_listenerDialog->show();
    centerDialogs();
}

void MainWindow::onGetBlockDescriptionsClicked(bool)
{
    if (!EASY_GLOBALS.connected)
    {
        Dialog::warning(this, "Warning", "No connection with profiling app", QMessageBox::Close);
        return;
    }

    if (m_listener.regime() != ListenerRegime::Idle)
    {
        if (m_listener.regime() == ListenerRegime::Descriptors)
        {
            Dialog::warning(this, "Warning",
                "Already capturing blocks description.\nFinish old capturing session first.", QMessageBox::Close);
        }
        else
        {
            Dialog::warning(this, "Warning",
                "Already capturing frames.\nFinish old capturing session first.", QMessageBox::Close);
        }

        return;
    }

    m_listenStartTime = std::chrono::system_clock::now();
    m_listenerDialog = new RoundProgressDialog(QStringLiteral("Waiting for blocks..."), this);
    m_listenerDialog->setObjectName("GetDescriptorsProgress");
    m_listenerDialog->setAttribute(Qt::WA_DeleteOnClose, true);
    m_listenerDialog->show();
    centerDialogs();

    m_listener.requestBlocksDescription();

    m_listenerDialog->reject();
    m_listenerDialog = nullptr;

    if (m_listener.size() != 0)
    {
        // Read descriptions from stream

        decltype(EASY_GLOBALS.descriptors) descriptors;
        decltype(m_serializedDescriptors) serializedDescriptors;
        std::stringstream errorMessage;
        if (readDescriptionsFromStream(m_listener.data(), serializedDescriptors, descriptors, errorMessage))
        {
            // Merge old and new descriptions

            bool cancel = false;
            const bool doFlush = m_descriptorsNumberInFile > descriptors.size();
            if (doFlush && !m_serializedBlocks.empty())
            {
                auto button = Dialog::question(this, "Information",
                    QString("New blocks description number = %1\nis less than the old one = %2.\nTo avoid possible conflicts\nall profiled data will be deleted.\nContinue?")
                    .arg(descriptors.size())
                    .arg(m_descriptorsNumberInFile),
                    QMessageBox::Yes | QMessageBox::No);

                if (button == QMessageBox::Yes)
                    clear(); // Clear all contents because new descriptors list conflicts with old one
                else
                    cancel = true;
            }

            if (!cancel)
            {
                if (!doFlush && m_descriptorsNumberInFile < EASY_GLOBALS.descriptors.size())
                {
                    // There are dynamically added descriptors, add them to the new list too

                    auto newnumber = static_cast<decltype(m_descriptorsNumberInFile)>(descriptors.size());
                    auto size = static_cast<decltype(m_descriptorsNumberInFile)>(EASY_GLOBALS.descriptors.size());
                    auto diff = newnumber - size;
                    decltype(newnumber) failnumber = 0;

                    descriptors.reserve(descriptors.size() + EASY_GLOBALS.descriptors.size() - m_descriptorsNumberInFile);
                    for (auto i = m_descriptorsNumberInFile; i < size; ++i)
                    {
                        auto id = EASY_GLOBALS.descriptors[i]->id();
                        if (id < newnumber)
                            descriptors.push_back(descriptors[id]);
                        else
                            ++failnumber;
                    }

                    if (failnumber != 0)
                    {
                        // There are some errors...

                        // revert changes
                        descriptors.resize(newnumber);

                        // clear all profiled data to avoid conflicts
                        auto button = Dialog::question(this, "Information",
                            "There are errors while merging block descriptions lists.\nTo avoid possible conflicts\nall profiled data will be deleted.\nContinue?",
                            QMessageBox::Yes | QMessageBox::No);

                        if (button == QMessageBox::Yes)
                            clear(); // Clear all contents because new descriptors list conflicts with old one
                        else
                            cancel = true;
                    }

                    if (!cancel && diff != 0)
                    {
                        for (auto& b : EASY_GLOBALS.gui_blocks)
                        {
                            if (b.tree.node->id() >= m_descriptorsNumberInFile)
                                b.tree.node->setId(b.tree.node->id() + diff);
                        }

                        m_descriptorsNumberInFile = newnumber;
                    }
                }

                if (!cancel)
                {
                    EASY_GLOBALS.descriptors.swap(descriptors);
                    m_serializedDescriptors.swap(serializedDescriptors);
                    m_descriptorsNumberInFile = static_cast<uint32_t>(EASY_GLOBALS.descriptors.size());

                    if (m_descTreeDialog.ptr != nullptr)
                    {
#if EASY_GUI_USE_DESCRIPTORS_DOCK_WINDOW != 0
                        static_cast<BlockDescriptorsWidget*>(m_descTreeWidget->widget())->build();
#endif
                        m_dialogDescTree->build();
                        m_descTreeDialog.ptr->raise();
                        m_descTreeDialog.ptr->setFocus();
                    }
                    else
                    {
                        onEditBlocksClicked(true);
                    }
                }
            }
        }
        else
        {
            Dialog::warning(this, "Warning",
                            QString("Cannot read blocks description from stream.\n\nReason:\n%1")
                                .arg(errorMessage.str().c_str()), QMessageBox::Close);
        }

        m_listener.clearData();
    }

    if (!m_listener.connected())
    {
        m_listener.closeSocket();
        setDisconnected();
    }
}

//////////////////////////////////////////////////////////////////////////

void MainWindow::onBlockStatusChange(profiler::block_id_t _id, profiler::EasyBlockStatus _status)
{
    if (EASY_GLOBALS.connected)
        m_listener.send(profiler::net::BlockStatusMessage(_id, static_cast<uint8_t>(_status)));
}

void MainWindow::onSelectValue(profiler::thread_id_t _thread_id, uint32_t _value_index, const profiler::ArbitraryValue& _value)
{
    onEditBlocksClicked(true);
    m_dialogDescTree->dataViewer()->rebuild(_thread_id, _value_index, _value.id());
}

void DialogWithGeometry::create(QWidget* content, QWidget* parent)
{
    ptr = new Dialog(parent, profiler_gui::DEFAULT_WINDOW_TITLE, content, WindowHeader::AllButtons, QMessageBox::NoButton);
    ptr->setProperty("stayVisible", true);
    ptr->setAttribute(Qt::WA_DeleteOnClose, true);
}

void DialogWithGeometry::saveGeometry()
{
    geometry = ptr->saveGeometry();
}

void DialogWithGeometry::restoreGeometry()
{
    if (!geometry.isEmpty())
        ptr->restoreGeometry(geometry);
}

//////////////////////////////////////////////////////////////////////////

