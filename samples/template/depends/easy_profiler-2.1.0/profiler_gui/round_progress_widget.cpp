/************************************************************************
* file name         : round_progress_widget.cpp
* ----------------- :
* creation time     : 2018/05/17
* author            : Victor Zarubkin
* email             : v.s.zarubkin@gmail.com
* ----------------- :
* description       : The file contains implementation of RoundProgressWidget.
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

#include <math.h>

#include <QFontMetrics>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QLabel>
#include <QPainter>
#include <QStyle>
#include <QVariant>
#include <QVBoxLayout>

#include <easy/utility.h>

#include "common_functions.h"
#include "globals.h"
#include "round_progress_widget.h"

#ifdef max
# undef max
#endif

namespace
{

// According to the Qt documentation on QPainter::drawArc: an angle must be specified in 1/16th of a degree
EASY_CONSTEXPR int Deg90 = 90 * 16;
EASY_CONSTEXPR int FullCircle = 360 * 16;

} // end of namespace <noname>.

RoundProgressIndicator::RoundProgressIndicator(QWidget* parent)
    : Parent(parent)
    , m_text("0%")
    , m_background(Qt::transparent)
    , m_color(Qt::green)
    , m_buttonColor(QColor::fromRgb(profiler::colors::Red500))
    , m_buttonSize(0.33)
    , m_style(Percent)
    , m_buttonStyle(NoButton)
    , m_buttonRole(QDialog::Rejected)
    , m_angle(Deg90)
    , m_indicatorWidth(px(2))
    , m_crossWidth(px(1))
    , m_value(0)
    , m_pressed(false)
{
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setAutoFillBackground(false);
    setProperty("hover", false);
    setProperty("pressed", false);
    m_animationTimer.setInterval(16);
}

RoundProgressIndicator::~RoundProgressIndicator()
{

}

RoundProgressIndicator::ButtonStyle RoundProgressIndicator::buttonStyle() const
{
    return m_buttonStyle;
}

void RoundProgressIndicator::setButtonStyle(ButtonStyle style)
{
    const auto prev = m_buttonStyle;
    m_buttonStyle = style;

    if (prev == m_buttonStyle)
    {
        return;
    }

    const bool hover = property("hover").toBool();
    if (hover)
    {
        update();
    }
}

QString RoundProgressIndicator::buttonStyleStr() const
{
    switch (m_buttonStyle)
    {
        case NoButton:
        {
            return QStringLiteral("none");
        }

        case Cross:
        {
            return QStringLiteral("cross");
        }

        case Stop:
        {
            return QStringLiteral("stop");
        }

        default:
        {
            return QStringLiteral("unknown");
        }
    }
}

void RoundProgressIndicator::setButtonStyle(QString style)
{
    style = style.toLower();
    if (style == QStringLiteral("cross"))
    {
        setButtonStyle(Cross);
    }
    else if (style == QStringLiteral("stop"))
    {
        setButtonStyle(Stop);
    }
    else if (style == QStringLiteral("none") || style.isEmpty())
    {
        setButtonStyle(NoButton);
    }
}

RoundProgressIndicator::Style RoundProgressIndicator::style() const
{
    return m_style;
}

void RoundProgressIndicator::setStyle(Style style)
{
    const auto prev = m_style;
    m_style = style;

    if (prev != m_style)
    {
        if (m_animationTimer.isActive())
        {
            m_animationTimer.stop();
            disconnect(&m_animationTimer, &QTimer::timeout, this, &RoundProgressIndicator::onTimeout);
        }

        if (m_style == Infinite)
        {
            m_angle = Deg90;
            connect(&m_animationTimer, &QTimer::timeout, this, &RoundProgressIndicator::onTimeout);
            m_animationTimer.start();
        }

        update();
    }
}

QString RoundProgressIndicator::styleStr() const
{
    switch (m_style)
    {
        case Percent:
        {
            return QStringLiteral("percent");
        }

        case Infinite:
        {
            return QStringLiteral("infinite");
        }

        default:
        {
            return QStringLiteral("unknown");
        }
    }
}

void RoundProgressIndicator::setStyle(QString style)
{
    style = style.toLower();
    if (style == QStringLiteral("percent"))
    {
        setStyle(Percent);
    }
    else if (style == QStringLiteral("inf") || style == QStringLiteral("infinite"))
    {
        setStyle(Infinite);
    }
}

QDialog::DialogCode RoundProgressIndicator::buttonRole() const
{
    return m_buttonRole;
}

void RoundProgressIndicator::setButtonRole(QDialog::DialogCode role)
{
    m_buttonRole = role;
}

QString RoundProgressIndicator::buttonRoleStr() const
{
    switch (m_buttonRole)
    {
        case QDialog::Accepted:
        {
            return QStringLiteral("accept");
        }

        case QDialog::Rejected:
        {
            return QStringLiteral("reject");
        }

        default:
        {
            return QStringLiteral("unknown");
        }
    }
}

void RoundProgressIndicator::setButtonRole(QString style)
{
    style = style.toLower();
    if (style == QStringLiteral("accept"))
    {
        setButtonRole(QDialog::Accepted);
    }
    else if (style == QStringLiteral("reject"))
    {
        setButtonRole(QDialog::Rejected);
    }
}

int RoundProgressIndicator::value() const
{
    return m_value;
}

void RoundProgressIndicator::setValue(int value)
{
    m_value = static_cast<int8_t>(estd::clamp(0, value, 100));
    m_text = QString("%1%").arg(m_value);
    update();
}

void RoundProgressIndicator::reset()
{
    m_angle = Deg90;
    m_value = 0;
    m_text = "0%";
    update();
}

QColor RoundProgressIndicator::background() const
{
    return m_background;
}

void RoundProgressIndicator::setBackground(QColor color)
{
    m_background = std::move(color);
    update();
}

void RoundProgressIndicator::setBackground(QString color)
{
    m_background.setNamedColor(color);
    update();
}

QColor RoundProgressIndicator::color() const
{
    return m_color;
}

void RoundProgressIndicator::setColor(QColor color)
{
    m_color = std::move(color);
    update();
}

void RoundProgressIndicator::setColor(QString color)
{
    m_color.setNamedColor(color);
    update();
}

QColor RoundProgressIndicator::buttonColor() const
{
    return m_buttonColor;
}

void RoundProgressIndicator::setButtonColor(QColor color)
{
    m_buttonColor = std::move(color);

    if (m_buttonStyle == NoButton)
    {
        return;
    }

    const bool hover = property("hover").toBool();
    if (hover)
    {
        update();
    }
}

void RoundProgressIndicator::setButtonColor(QString color)
{
    m_buttonColor.setNamedColor(color);

    if (m_buttonStyle == NoButton)
    {
        return;
    }

    const bool hover = property("hover").toBool();
    if (hover)
    {
        update();
    }
}

qreal RoundProgressIndicator::buttonSize() const
{
    return m_buttonSize;
}

void RoundProgressIndicator::setButtonSize(qreal size)
{
    m_buttonSize = size;

    if (m_buttonStyle == NoButton)
    {
        return;
    }

    const bool hover = property("hover").toBool();
    if (hover)
    {
        update();
    }
}

int RoundProgressIndicator::crossWidth() const
{
    return m_crossWidth;
}

void RoundProgressIndicator::setCrossWidth(int width)
{
    m_crossWidth = width;

    if (m_buttonStyle != Cross)
    {
        return;
    }

    const bool hover = property("hover").toBool();
    if (hover)
    {
        update();
    }
}

int RoundProgressIndicator::indicatorWidth() const
{
    return m_indicatorWidth;
}

void RoundProgressIndicator::setIndicatorWidth(int width)
{
    m_indicatorWidth = width;
    update();
}

void RoundProgressIndicator::showEvent(QShowEvent* event)
{
    Parent::showEvent(event);
    updateSize();
}

void RoundProgressIndicator::updateSize()
{
    const QFontMetrics fm(font());
    const QString text = QStringLiteral("100%");
    const int margins = m_indicatorWidth * 4 + px(2);
    const int size = static_cast<int>(std::max(fm.width(text), fm.height()) * profiler_gui::FONT_METRICS_FACTOR) + margins;

    setFixedSize(size, size);

    emit sizeChanged();
}

void RoundProgressIndicator::onTimeout()
{
    EASY_CONSTEXPR int Step = -8 * 16;

    m_angle += Step;
    if (m_angle > FullCircle)
    {
        m_angle -= FullCircle;
    }
    else if (m_angle < 0)
    {
        m_angle += FullCircle;
    }

    update();
}

void RoundProgressIndicator::paintEvent(QPaintEvent* /*event*/)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(Qt::NoBrush);

    auto r = rect().adjusted(m_indicatorWidth, m_indicatorWidth, -m_indicatorWidth, -m_indicatorWidth);
    auto p = painter.pen();

    // Draw background circle
    p.setWidth(m_indicatorWidth);
    p.setColor(m_background);
    painter.setPen(p);
    painter.drawEllipse(r);

    // Draw current progress arc
    p.setColor(m_color);
    painter.setPen(p);

    if (m_style == Percent)
    {
        painter.drawArc(r, Deg90, -1 * static_cast<int>(m_value) * FullCircle / 100);
    }
    else
    {
        painter.drawArc(r, m_angle, -1 * FullCircle / 5);
    }

    const bool hover = property("hover").toBool();

    if (hover && m_buttonStyle != NoButton)
    {
        switch (m_buttonStyle)
        {
            case Cross:
            {
                paintCrossButton(painter, r);
                break;
            }

            case Stop:
            {
                paintStopButton(painter, r);
                break;
            }

            default:
            {
                break;
            }
        }
    }
    else if (m_style == Percent)
    {
        // Draw text
        p.setWidth(px(1));
        p.setColor(palette().foreground().color());
        painter.setPen(p);
        painter.setFont(font());
        painter.drawText(r, Qt::AlignCenter, m_text);
    }
}

void RoundProgressIndicator::paintCrossButton(QPainter& painter, QRect& r)
{
    // Draw cancel button (red cross)

    const auto margin = (1. - m_buttonSize) * 0.5;
    const auto dh = m_indicatorWidth + static_cast<int>(r.width() * margin);
    const auto dv = m_indicatorWidth + static_cast<int>(r.height() * margin);
    r.adjust(dh, dv, -dh, -dv);

    auto p = painter.pen();
    p.setWidth(m_crossWidth);
    p.setColor(m_buttonColor);
    p.setCapStyle(Qt::SquareCap);

    painter.setPen(p);
    painter.setBrush(Qt::NoBrush);
    painter.drawLine(r.topLeft(), r.bottomRight());
    painter.drawLine(r.bottomLeft(), r.topRight());
}

void RoundProgressIndicator::paintStopButton(QPainter& painter, QRect& r)
{
    // Draw cancel button (red cross)

    const auto margin = (1. - m_buttonSize) * 0.5;
    const auto dh = m_indicatorWidth + static_cast<int>(r.width() * margin);
    const auto dv = m_indicatorWidth + static_cast<int>(r.height() * margin);
    r.adjust(dh, dv, -dh, -dv);

    painter.setPen(Qt::NoPen);
    painter.setBrush(m_buttonColor);
    painter.drawRect(r);
}

void RoundProgressIndicator::enterEvent(QEvent* event) {
    Parent::enterEvent(event);
    profiler_gui::updateProperty(this, "hover", true);
}

void RoundProgressIndicator::leaveEvent(QEvent* event) {
    Parent::leaveEvent(event);
    profiler_gui::updateProperty(this, "hover", false);
}

void RoundProgressIndicator::mousePressEvent(QMouseEvent* event)
{
    Parent::mousePressEvent(event);
    m_pressed = true;
    profiler_gui::updateProperty(this, "pressed", true);
}

void RoundProgressIndicator::mouseReleaseEvent(QMouseEvent* event)
{
    Parent::mouseReleaseEvent(event);

    const bool hover = property("hover").toBool();
    const bool pressed = m_pressed;

    m_pressed = false;
    profiler_gui::updateProperty(this, "pressed", false);

    if (pressed && hover && m_buttonStyle != NoButton)
    {
        emit buttonClicked(m_buttonRole);
    }
}

void RoundProgressIndicator::mouseMoveEvent(QMouseEvent* event)
{
    if (m_pressed)
    {
        const bool hover = property("hover").toBool();
        if (rect().contains(event->pos()))
        {
            if (!hover)
            {
                profiler_gui::updateProperty(this, "hover", true);
            }
        }
        else if (hover)
        {
            profiler_gui::updateProperty(this, "hover", false);
        }
    }

    Parent::mouseMoveEvent(event);
}

RoundProgressWidget::RoundProgressWidget(QWidget* parent)
    : RoundProgressWidget(QString(), parent)
{
}

RoundProgressWidget::RoundProgressWidget(const QString& title, QWidget* parent)
    : Parent(parent)
    , m_title(new QLabel(title, this))
    , m_indicatorWrapper(new QWidget(this))
    , m_indicator(new RoundProgressIndicator(this))
    , m_titlePosition(RoundProgressWidget::Top)
{
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setAutoFillBackground(false);

    m_title->setAlignment(Qt::AlignCenter);

    auto wlay = new QHBoxLayout(m_indicatorWrapper);
    wlay->setContentsMargins(0, 0, 0, 0);
    wlay->addWidget(m_indicator, 0, Qt::AlignCenter);

    auto lay = new QVBoxLayout(this);
    lay->addWidget(m_title);
    lay->addWidget(m_indicatorWrapper);

    connect(m_indicator, &RoundProgressIndicator::buttonClicked, this, &RoundProgressWidget::finished);
    connect(m_indicator, &RoundProgressIndicator::sizeChanged, [this] {
        adjustSize();
    });
}

RoundProgressWidget::~RoundProgressWidget()
{

}

void RoundProgressWidget::setTitle(const QString& title)
{
    m_title->setText(title);
    adjustSize();
}

int RoundProgressWidget::value() const
{
    return m_indicator->value();
}

void RoundProgressWidget::setValue(int value)
{
    const auto prev = m_indicator->value();
    const auto newValue = static_cast<int8_t>(estd::clamp(0, value, 100));
    if (prev == newValue)
        return;

    m_indicator->setValue(newValue);
    const auto v = m_indicator->value();

    emit valueChanged(v);

    if (v == 100)
    {
        emit finished(QDialog::Accepted);
    }
}

void RoundProgressWidget::reset()
{
    m_indicator->reset();
}

RoundProgressWidget::TitlePosition RoundProgressWidget::titlePosition() const
{
    return m_titlePosition;
}

void RoundProgressWidget::setTitlePosition(TitlePosition pos)
{
    const auto prev = m_titlePosition;
    if (prev == pos)
        return;

    m_titlePosition = pos;

    auto lay = static_cast<QVBoxLayout*>(layout());
    if (pos == RoundProgressWidget::Top)
    {
        lay->removeWidget(m_indicatorWrapper);
        lay->removeWidget(m_title);

        lay->addWidget(m_title);
        lay->addWidget(m_indicatorWrapper);
    }
    else
    {
        lay->removeWidget(m_title);
        lay->removeWidget(m_indicator);

        lay->addWidget(m_indicator);
        lay->addWidget(m_title);
    }

    emit titlePositionChanged();

    update();
}

bool RoundProgressWidget::isTopTitlePosition() const
{
    return m_titlePosition == RoundProgressWidget::Top;
}

void RoundProgressWidget::setTopTitlePosition(bool isTop)
{
    setTitlePosition(isTop ? RoundProgressWidget::Top : RoundProgressWidget::Bottom);
}

RoundProgressButtonStyle RoundProgressWidget::buttonStyle() const
{
    return m_indicator->buttonStyle();
}

void RoundProgressWidget::setButtonStyle(RoundProgressButtonStyle style)
{
    m_indicator->setButtonStyle(style);
}

QDialog::DialogCode RoundProgressWidget::buttonRole() const
{
    return m_indicator->buttonRole();
}

void RoundProgressWidget::setButtonRole(QDialog::DialogCode role)
{
    m_indicator->setButtonRole(role);
}

RoundProgressStyle RoundProgressWidget::style() const
{
    return m_indicator->style();
}

void RoundProgressWidget::setStyle(RoundProgressStyle style)
{
    m_indicator->setStyle(style);
}


RoundProgressDialog::RoundProgressDialog(const QString& title, QWidget* parent)
    : RoundProgressDialog(title, RoundProgressIndicator::NoButton, QDialog::Rejected, parent)
{
}

RoundProgressDialog::RoundProgressDialog(
    const QString& title,
    RoundProgressButtonStyle button,
    QDialog::DialogCode buttonRole,
    QWidget* parent
)
    : Parent(parent)
    , m_progress(new RoundProgressWidget(title, this))
    , m_background(Qt::transparent)
    , m_borderRadius(px(15))
{
    setWindowTitle(profiler_gui::DEFAULT_WINDOW_TITLE);

    // do not merge with existing windowFlags() to let the dialog be always binded to it's parent
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setAutoFillBackground(false);

    auto lay = new QVBoxLayout(this);
    lay->addWidget(m_progress);

    connect(m_progress, &RoundProgressWidget::valueChanged, this, &RoundProgressDialog::valueChanged);
    connect(m_progress, &RoundProgressWidget::finished, this, &RoundProgressDialog::onFinished);

    m_progress->setButtonStyle(button);
    m_progress->setButtonRole(buttonRole);
}

RoundProgressDialog::~RoundProgressDialog()
{

}

void RoundProgressDialog::onFinished(int role)
{
    if (role == QDialog::Accepted)
    {
        accept();
    }
    else
    {
        reject();
    }
}

QColor RoundProgressDialog::background() const
{
    return m_background;
}

void RoundProgressDialog::setBackground(QColor color)
{
    m_background = std::move(color);
    update();
}

int RoundProgressDialog::borderRadius() const
{
    return m_borderRadius;
}

void RoundProgressDialog::setBorderRadius(int radius)
{
    m_borderRadius = radius;
    update();
}

void RoundProgressDialog::setBackground(QString color)
{
    m_background.setNamedColor(color);
    update();
}

RoundProgressButtonStyle RoundProgressDialog::buttonStyle() const
{
    return m_progress->buttonStyle();
}

void RoundProgressDialog::setButtonStyle(RoundProgressButtonStyle style)
{
    m_progress->setButtonStyle(style);
}

RoundProgressStyle RoundProgressDialog::style() const
{
    return m_progress->style();
}

void RoundProgressDialog::setStyle(RoundProgressStyle style)
{
    m_progress->setStyle(style);
}

void RoundProgressDialog::setTitle(const QString& title)
{
    m_progress->setTitle(title);
    adjustSize();
    update();
}

QDialog::DialogCode RoundProgressDialog::buttonRole() const
{
    return m_progress->buttonRole();
}

void RoundProgressDialog::setButtonRole(QDialog::DialogCode role)
{
    m_progress->setButtonRole(role);
}

void RoundProgressDialog::showEvent(QShowEvent* event)
{
    Parent::showEvent(event);
    adjustSize();
}

void RoundProgressDialog::setValue(int value)
{
    m_progress->setValue(value);
    if (value == 100)
        hide();
}

void RoundProgressDialog::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::NoPen);
    painter.setBrush(m_background);
    painter.drawRoundedRect(rect(), m_borderRadius, m_borderRadius);
}
