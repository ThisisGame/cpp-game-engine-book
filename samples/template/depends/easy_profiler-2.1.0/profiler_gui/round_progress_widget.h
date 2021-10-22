/************************************************************************
* file name         : round_progress_widget.h
* ----------------- :
* creation time     : 2018/05/17
* author            : Victor Zarubkin
* email             : v.s.zarubkin@gmail.com
* ----------------- :
* description       : The file contains declaration of RoundProgressWidget.
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

#ifndef ROUND_PROGRESS_WIDGET_H
#define ROUND_PROGRESS_WIDGET_H

#include <stdint.h>
#include <QColor>
#include <QDialog>
#include <QTimer>
#include <QWidget>

class RoundProgressIndicator : public QWidget
{
    Q_OBJECT

public:

    enum ButtonStyle { NoButton = 0, Cross, Stop };
    enum Style { Percent = 0, Infinite };

private:

    using Parent = QWidget;
    using This = RoundProgressIndicator;

    QTimer             m_animationTimer;
    QString                      m_text;
    QColor                 m_background;
    QColor                      m_color;
    QColor                m_buttonColor;
    qreal                  m_buttonSize;
    Style                       m_style;
    ButtonStyle           m_buttonStyle;
    QDialog::DialogCode    m_buttonRole;
    int                         m_angle;
    int                m_indicatorWidth;
    int                    m_crossWidth;
    int8_t                      m_value;
    bool                      m_pressed;

public:

    Q_PROPERTY(QColor color READ color WRITE setColor);
    Q_PROPERTY(QColor background READ background WRITE setBackground);
    Q_PROPERTY(QColor buttonColor READ buttonColor WRITE setButtonColor);
    Q_PROPERTY(qreal buttonSize READ buttonSize WRITE setButtonSize);
    Q_PROPERTY(int indicatorWidth READ indicatorWidth WRITE setIndicatorWidth);
    Q_PROPERTY(int crossWidth READ crossWidth WRITE setCrossWidth);
    Q_PROPERTY(QString buttonStyle READ buttonStyleStr WRITE setButtonStyle);
    Q_PROPERTY(QString buttonRole READ buttonRoleStr WRITE setButtonRole);
    Q_PROPERTY(QString style READ styleStr WRITE setStyle);

    explicit RoundProgressIndicator(QWidget* parent = nullptr);
    ~RoundProgressIndicator() override;

    int value() const;
    void setValue(int value);
    void reset();

    QColor background() const;
    QColor color() const;
    QColor buttonColor() const;

    qreal buttonSize() const;
    int indicatorWidth() const;
    int crossWidth() const;

    ButtonStyle buttonStyle() const;
    QString buttonStyleStr() const;
    void setButtonStyle(ButtonStyle style);
    void setButtonStyle(QString style);

    QDialog::DialogCode buttonRole() const;
    QString buttonRoleStr() const;
    void setButtonRole(QDialog::DialogCode role);
    void setButtonRole(QString role);

    Style style() const;
    QString styleStr() const;
    void setStyle(Style style);
    void setStyle(QString style);

signals:

    void buttonClicked(int role);
    void sizeChanged();

public slots:

    void setBackground(QColor color);
    void setBackground(QString color);
    void setColor(QColor color);
    void setColor(QString color);
    void setButtonColor(QColor color);
    void setButtonColor(QString color);
    void setButtonSize(qreal size);
    void setIndicatorWidth(int width);
    void setCrossWidth(int width);

private slots:

    void onTimeout();

protected:

    void showEvent(QShowEvent* event) override;
    void paintEvent(QPaintEvent* event) override;
    void enterEvent(QEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;

private:

    void updateSize();
    void paintCrossButton(QPainter& painter, QRect& r);
    void paintStopButton(QPainter& painter, QRect& r);

}; // end of class RoundProgressIndicator.

using RoundProgressButtonStyle = RoundProgressIndicator::ButtonStyle;
using RoundProgressStyle = RoundProgressIndicator::Style;

class RoundProgressWidget : public QWidget
{
    Q_OBJECT

    using Parent = QWidget;
    using This = RoundProgressWidget;

public:

    Q_PROPERTY(bool topTitlePosition READ isTopTitlePosition WRITE setTopTitlePosition NOTIFY titlePositionChanged);

    enum TitlePosition : int8_t
    {
        Top = 0,
        Bottom,
    };

private:

    class QLabel*                   m_title;
    QWidget*             m_indicatorWrapper;
    RoundProgressIndicator*     m_indicator;
    TitlePosition           m_titlePosition;

public:

    explicit RoundProgressWidget(QWidget* parent = nullptr);
    explicit RoundProgressWidget(const QString& title, QWidget* parent = nullptr);
    ~RoundProgressWidget() override;

    void setTitle(const QString& title);
    int value() const;
    TitlePosition titlePosition() const;
    bool isTopTitlePosition() const;

    RoundProgressButtonStyle buttonStyle() const;
    void setButtonStyle(RoundProgressButtonStyle style);

    QDialog::DialogCode buttonRole() const;
    void setButtonRole(QDialog::DialogCode role);

    RoundProgressStyle style() const;
    void setStyle(RoundProgressStyle style);

public slots:

    void setValue(int value);
    void reset();
    void setTitlePosition(TitlePosition pos);
    void setTopTitlePosition(bool isTop);

signals:

    void valueChanged(int value);
    void finished(int role);
    void titlePositionChanged();

}; // end of class RoundProgressWidget.

class RoundProgressDialog : public QDialog
{
    Q_OBJECT

    using Parent = QDialog;
    using This = RoundProgressDialog;

    RoundProgressWidget* m_progress;
    QColor             m_background;
    int              m_borderRadius;

public:

    Q_PROPERTY(QColor background READ background WRITE setBackground);
    Q_PROPERTY(int borderRadius READ borderRadius WRITE setBorderRadius);

    explicit RoundProgressDialog(const QString& title, QWidget* parent = nullptr);
    RoundProgressDialog(
        const QString& title,
        RoundProgressIndicator::ButtonStyle button,
        QDialog::DialogCode buttonRole,
        QWidget* parent = nullptr
    );
    ~RoundProgressDialog() override;

    QColor background() const;
    int borderRadius() const;

    RoundProgressButtonStyle buttonStyle() const;
    void setButtonStyle(RoundProgressButtonStyle style);

    QDialog::DialogCode buttonRole() const;
    void setButtonRole(QDialog::DialogCode role);

    RoundProgressStyle style() const;
    void setStyle(RoundProgressStyle style);

    void setTitle(const QString& title);

protected:

    void showEvent(QShowEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

public slots:

    void setBackground(QColor color);
    void setBackground(QString color);
    void setBorderRadius(int radius);
    void setValue(int value);

signals:

    void valueChanged(int value);

private slots:

    void onFinished(int role);

}; // end of RoundProgressDialog.

#endif // ROUND_PROGRESS_WIDGET_H
