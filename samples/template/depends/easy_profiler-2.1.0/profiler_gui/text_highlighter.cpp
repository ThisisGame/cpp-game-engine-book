/************************************************************************
* file name         : text_highlighter.cpp
* ----------------- :
* creation time     : 2019/10/12
* author            : Victor Zarubkin
* email             : v.s.zarubkin@gmail.com
* ----------------- :
* description       : The file contains implementation of TextHighlighter.
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

#include "text_highlighter.h"
#include <QColor>
#include <QRegExp>

#include <easy/details/profiler_colors.h>
#include "common_functions.h"

EASY_CONSTEXPR auto HighlightColor = profiler::colors::Yellow;
EASY_CONSTEXPR auto CurrentHighlightColor = profiler::colors::Magenta;

TextHighlighter::TextHighlighter(
    QTextDocument* doc,
    const QColor& normalTextColor,
    const QColor& lightTextColor,
    const QString& pattern,
    Qt::CaseSensitivity caseSensitivity,
    bool current
)
    : QSyntaxHighlighter(doc)
    , m_pattern(pattern)
    , m_caseSensitivity(caseSensitivity)
{
    auto color = current ? CurrentHighlightColor : HighlightColor;
    m_textCharFormat.setBackground(QColor::fromRgba(color));
    m_textCharFormat.setForeground(profiler_gui::isLightColor(color) ? normalTextColor : lightTextColor);
}

TextHighlighter::~TextHighlighter()
{

}

void TextHighlighter::highlightBlock(const QString& text)
{
    if (m_pattern.isEmpty())
    {
        return;
    }

    QRegExp expression(m_pattern, m_caseSensitivity);
    int index = text.indexOf(expression);
    while (index >= 0)
    {
        const auto length = expression.cap().length();
        setFormat(index, length, m_textCharFormat);

        auto prevIndex = index;
        index = text.indexOf(expression, index + length);
        if (index <= prevIndex)
        {
            break;
        }
    }
}
