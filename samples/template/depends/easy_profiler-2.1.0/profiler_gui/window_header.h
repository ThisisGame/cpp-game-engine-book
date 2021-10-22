/************************************************************************
* file name         : window_header.h
* ----------------- :
* creation time     : 2018/06/03
* author            : Victor Zarubkin
* email             : v.s.zarubkin@gmail.com
* ----------------- :
* description       : The file contains declaration of WindowHeader - a replacement
*                   : for standard system window header.
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

#ifndef EASY_PROFILER_WINDOW_HEADER_H
#define EASY_PROFILER_WINDOW_HEADER_H

#include <QFrame>

class QPushButton;
class QLabel;

class WindowHeader : public QFrame
{
    Q_OBJECT

    using This = WindowHeader;
    using Parent = QFrame;

    QPoint        m_mousePressPos;
    QPushButton* m_minimizeButton;
    QPushButton* m_maximizeButton;
    QPushButton*    m_closeButton;
    QLabel*              m_pixmap;
    QLabel*               m_title;
    bool             m_isDragging;

public:

    enum Button
    {
        NoButtons = 0,
        MinimizeButton = 0x01,
        MaximizeButton = 0x02,
        CloseButton = 0x04,
        AllButtons = MinimizeButton | MaximizeButton | CloseButton
    };

    Q_DECLARE_FLAGS(Buttons, Button)
    Q_FLAG(Buttons)

    explicit WindowHeader(const QString& title, Buttons buttons, QWidget& parentRef);
    ~WindowHeader() override;

    void setTitle(const QString& title);
    void setWindowIcon(const QIcon& icon);

    void setWindowTitle(const QString& title)
    {
        setTitle(title);
    }

private slots:

    void onWindowStateChanged();
    void onWindowHeaderChanged();
    void onWindowHeaderPositionChanged();

protected:

    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    bool eventFilter(QObject* obj, QEvent* event) override;
    void showEvent(QShowEvent* event) override;

private slots:

    void onMaximizeClicked(bool);
    void onMinimizeClicked(bool);

}; // end of class WindowHeader.

#endif // EASY_PROFILER_WINDOW_HEADER_H
