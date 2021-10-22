/************************************************************************
* file name         : window_header.cpp
* ----------------- :
* creation time     : 2018/06/03
* author            : Victor Zarubkin
* email             : v.s.zarubkin@gmail.com
* ----------------- :
* description       : The file contains implementation of WindowHeader.
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

#include "globals.h"
#include "window_header.h"
#include <QApplication>
#include <QHBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QPushButton>
#include <QStyle>

static void setButtonSize(QPushButton* button, int size)
{
    if (button != nullptr)
        button->setFixedSize(size * 191 / 100, size);
}

WindowHeader::WindowHeader(const QString& title, Buttons buttons, QWidget& parentRef)
    : Parent(&parentRef)
    , m_minimizeButton(nullptr)
    , m_maximizeButton(nullptr)
    , m_closeButton(nullptr)
    , m_pixmap(new QLabel())
    , m_title(new QLabel(title))
    , m_isDragging(false)
{
    auto parent = &parentRef;

    m_title->installEventFilter(this);
    m_pixmap->installEventFilter(this);

    const auto icon = QApplication::windowIcon();
    m_pixmap->setWindowIcon(icon);
    m_pixmap->setPixmap(icon.isNull() ? QPixmap() : icon.pixmap(16, 16));

    setMouseTracking(true);

    if (buttons.testFlag(WindowHeader::MinimizeButton))
    {
        m_minimizeButton = new QPushButton();
        m_minimizeButton->setObjectName("WindowHeader_MinButton");
        connect(m_minimizeButton, &QPushButton::clicked, this, &This::onMinimizeClicked, Qt::QueuedConnection);
    }

    if (buttons.testFlag(WindowHeader::MaximizeButton))
    {
        m_maximizeButton = new QPushButton();
        m_maximizeButton->setProperty("max", parent->isMaximized());
        m_maximizeButton->setObjectName("WindowHeader_MaxButton");
        connect(m_maximizeButton, &QPushButton::clicked, this, &This::onMaximizeClicked, Qt::QueuedConnection);
    }

    if (buttons.testFlag(WindowHeader::CloseButton))
    {
        m_closeButton = new QPushButton();
        m_closeButton->setObjectName("WindowHeader_CloseButton");
        connect(m_closeButton, &QPushButton::clicked, parent, &QWidget::close, Qt::QueuedConnection);
    }

#if !defined(_WIN32)
    if (m_maximizeButton != nullptr || m_minimizeButton != nullptr)
    {
        parent->setWindowFlags(parent->windowFlags() | Qt::SubWindow);
    }
#endif

    auto layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(1);

    if (EASY_GLOBALS.is_right_window_header_controls)
    {
        setProperty("left", false);

        layout->addWidget(m_pixmap, 0, Qt::AlignVCenter | Qt::AlignLeft);
        layout->addSpacing(3);
        layout->addWidget(m_title, 0, Qt::AlignVCenter | Qt::AlignLeft);
        layout->addStretch(1);

        if (m_minimizeButton != nullptr)
            layout->addWidget(m_minimizeButton);

        if (m_maximizeButton != nullptr)
            layout->addWidget(m_maximizeButton);

        if (m_closeButton != nullptr)
            layout->addWidget(m_closeButton);
    }
    else
    {
        setProperty("left", true);

        if (m_closeButton != nullptr)
            layout->addWidget(m_closeButton);

        if (m_maximizeButton != nullptr)
            layout->addWidget(m_maximizeButton);

        if (m_minimizeButton != nullptr)
            layout->addWidget(m_minimizeButton);

        layout->addSpacing(3);
        layout->addWidget(m_pixmap, 0, Qt::AlignVCenter | Qt::AlignLeft);
        layout->addSpacing(3);
        layout->addWidget(m_title, 0, Qt::AlignVCenter | Qt::AlignLeft);
        layout->addStretch(1);
    }

    if (m_maximizeButton != nullptr)
        parent->installEventFilter(this);

    m_pixmap->setVisible(!icon.isNull());

    connect(&EASY_GLOBALS.events, &profiler_gui::GlobalSignals::customWindowHeaderChanged,
            this, &This::onWindowHeaderChanged);

    connect(&EASY_GLOBALS.events, &profiler_gui::GlobalSignals::windowHeaderPositionChanged,
            this, &This::onWindowHeaderPositionChanged);

    if (EASY_GLOBALS.use_custom_window_header)
        parent->setWindowFlags(parentWidget()->windowFlags() | Qt::FramelessWindowHint);
    else
        parent->setWindowFlags(parentWidget()->windowFlags() & ~Qt::FramelessWindowHint);

    setVisible(EASY_GLOBALS.use_custom_window_header);
}

WindowHeader::~WindowHeader()
{

}

void WindowHeader::setTitle(const QString& title)
{
    m_title->setText(title);
}

void WindowHeader::setWindowIcon(const QIcon& icon)
{
    const int size = std::max(m_title->fontMetrics().height(), 16);
    m_pixmap->setWindowIcon(icon);
    m_pixmap->setPixmap(icon.isNull() ? QPixmap() : icon.pixmap(size, size));
    m_pixmap->setVisible(!icon.isNull());
}

void WindowHeader::onWindowStateChanged()
{
    if (m_maximizeButton != nullptr)
    {
        profiler_gui::updateProperty(m_maximizeButton, "max", parentWidget()->isMaximized());
        setButtonSize(m_maximizeButton, height());
    }
}

void WindowHeader::onWindowHeaderChanged()
{

}

void WindowHeader::onWindowHeaderPositionChanged()
{

}

void WindowHeader::mouseDoubleClickEvent(QMouseEvent* /*event*/)
{
    if (m_maximizeButton != nullptr)
        onMaximizeClicked(true);
}

void WindowHeader::mousePressEvent(QMouseEvent* event)
{
    m_mousePressPos = mapFromGlobal(event->globalPos());
}

void WindowHeader::mouseReleaseEvent(QMouseEvent* /*event*/)
{
    m_mousePressPos = QPoint();
    m_isDragging = false;
}

void WindowHeader::mouseMoveEvent(QMouseEvent* event)
{
    if (!event->buttons().testFlag(Qt::LeftButton))
        return;

    if (m_isDragging)
    {
        parentWidget()->move(event->globalPos() - m_mousePressPos);
        return;
    }

    if (m_maximizeButton != nullptr && m_maximizeButton->underMouse())
        return;

    if (m_minimizeButton != nullptr && m_minimizeButton->underMouse())
        return;

    if (m_closeButton != nullptr && m_closeButton->underMouse())
        return;

    const auto pos = mapFromGlobal(event->globalPos());
    const auto line = m_mousePressPos - pos;
    if (line.manhattanLength() > 5)
    {
        m_isDragging = true;

        auto parent = parentWidget();
        if (parent->isMaximized())
        {
            const int w = parent->width();

            parent->showNormal();

            const auto k = static_cast<qreal>(pos.x()) / static_cast<qreal>(w);
            const int xlocal = static_cast<int>(static_cast<qreal>(parent->width()) * k);
            const int xglobal = event->globalPos().x() - xlocal;
            parent->move(xglobal, parent->y());

            m_mousePressPos = QPoint(xlocal, static_cast<int>(pos.y() * k));
        }
        else
        {
            m_mousePressPos = pos;
        }
    }
}

bool WindowHeader::eventFilter(QObject* obj, QEvent* event)
{
    if (obj == m_title || obj == m_pixmap)
    {
        switch (event->type())
        {
            case QEvent::MouseMove:
            {
                mouseMoveEvent(static_cast<QMouseEvent*>(event));
                return false;
            }

            case QEvent::MouseButtonPress:
            {
                mousePressEvent(static_cast<QMouseEvent*>(event));
                return false;
            }

            case QEvent::MouseButtonRelease:
            {
                mouseReleaseEvent(static_cast<QMouseEvent*>(event));
                return false;
            }

            case QEvent::MouseButtonDblClick:
            {
                mouseDoubleClickEvent(static_cast<QMouseEvent*>(event));
                return false;
            }

            default:
                break;
        }
    }
    else if (obj == parentWidget())
    {
        if (event->type() == QEvent::WindowStateChange)
            onWindowStateChanged();
        return false;
    }

    return Parent::eventFilter(obj, event);
}

void WindowHeader::showEvent(QShowEvent* event)
{
    Parent::showEvent(event);

    if (!m_pixmap->windowIcon().isNull())
    {
        const int size = std::max(m_title->fontMetrics().height(), 16);
        if (m_pixmap->pixmap()->height() != size)
            m_pixmap->setPixmap(m_pixmap->windowIcon().pixmap(size, size));
    }

    setButtonSize(m_minimizeButton, height());
    setButtonSize(m_maximizeButton, height());
    setButtonSize(m_closeButton, height());
}

void WindowHeader::onMaximizeClicked(bool)
{
    auto parent = parentWidget();
    if (parent->isMaximized())
    {
        parent->showNormal();
    }
    else
    {
        parent->showMaximized();
    }
}

void WindowHeader::onMinimizeClicked(bool)
{
    parentWidget()->setWindowState(Qt::WindowMinimized);
}
