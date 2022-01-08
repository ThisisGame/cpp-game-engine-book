/************************************************************************
* file name         : arbitrary_value_tooltip.cpp
* ----------------- :
* creation time     : 2018/03/25
* author            : Victor Zarubkin
* email             : v.s.zarubkin@gmail.com
* ----------------- :
* description       : The file contains implementation of ArbitraryValueToolTip which is used
*                   : for displaying arbitrary value in Diagram and StatsTree widgets.
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

#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QMouseEvent>
#include <QTextEdit>
#include <QWheelEvent>
#include "arbitrary_value_tooltip.h"
#include "common_functions.h"
#include "globals.h"

ArbitraryValueToolTip::ArbitraryValueToolTip(const QString& _name
    , const profiler::BlocksTree& _block, QWidget* _parent)
    : QWidget(_parent, Qt::Tool | Qt::SubWindow | Qt::FramelessWindowHint)
{
    auto content = new QWidget();
    content->setObjectName("cnt");

    auto layout = new QVBoxLayout(content);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    auto pane = new QTextEdit();
    pane->setWordWrapMode(QTextOption::NoWrap);
    pane->setReadOnly(true);
    pane->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);

    auto label = new QLabel(QStringLiteral("Arbitrary Value"));
    QFont font = EASY_GLOBALS.font.default_font;
    font.setBold(true);
    label->setFont(font);

    layout->addWidget(label, 0, Qt::AlignCenter);
    layout->addWidget(pane);

    QString firstString;
    int rowsCount = 0;

    if (_block.value->isArray())
    {
        const auto size = profiler_gui::valueArraySize(*_block.value);
        firstString = QString("%1  %2[%3]  (0x%4)").arg(profiler_gui::valueTypeString(_block.value->type()))
            .arg(_name).arg(size).arg(_block.value->value_id(), 0, 16);
        pane->append(firstString);
        rowsCount += 1;

        if (_block.per_thread_stats != nullptr)
        {
            pane->append(QString("N calls/Thread: %1").arg(_block.per_thread_stats->calls_number));
            rowsCount += 1;
        }

        if (_block.value->type() == profiler::DataType::String)
        {
            pane->append(QString("value:\t%1").arg(profiler_gui::valueString(*_block.value)));
            rowsCount += 1;
        }
        else
        {
            rowsCount += size;
            for (int i = 0; i < size; ++i)
                pane->append(QString("[%1]\t%2").arg(i).arg(profiler_gui::valueString(*_block.value, i)));

            if (rowsCount > 9)
                rowsCount = 9;
        }
    }
    else
    {
        rowsCount += 2;

        firstString = QString("%1  %2  (0x%3)").arg(profiler_gui::valueTypeString(_block.value->type()))
            .arg(_name).arg(_block.value->value_id(), 0, 16);

        pane->append(firstString);

        if (_block.per_thread_stats != nullptr)
        {
            pane->append(QString("N calls/Thread: %1").arg(_block.per_thread_stats->calls_number));
            rowsCount += 1;
        }

        pane->append(QString("value:\t%1").arg(profiler_gui::valueString(*_block.value)));
    }

    ++rowsCount;

    QFontMetrics fm(EASY_GLOBALS.font.default_font);
    QFontMetrics fm2(font);
    pane->setMinimumWidth(fm.width(firstString) + 24);
    pane->setMaximumHeight((fm.height() + fm.leading() + 1) * rowsCount);

    setMaximumHeight(pane->maximumHeight() + fm2.height() + fm2.leading() + 10);

    auto l = new QVBoxLayout(this);
    l->setContentsMargins(0, 0, 0, 0);
    l->addWidget(content);
}

ArbitraryValueToolTip::~ArbitraryValueToolTip()
{

}
