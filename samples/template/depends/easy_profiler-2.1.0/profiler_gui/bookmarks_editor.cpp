/************************************************************************
* file name         : bookmarks_editor.cpp
* ----------------- :
* creation time     : 2018/06/03
* author            : Victor Zarubkin
* email             : v.s.zarubkin@gmail.com
* ----------------- :
* description       : The file contains implementation of BookmarkEditor.
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

#include "bookmarks_editor.h"
#include "globals.h"
#include "window_header.h"

#include <easy/reader.h>
#include <QColorDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSettings>
#include <QStyle>
#include <QTextEdit>

BookmarkEditor::BookmarkEditor(size_t bookmarkIndex, bool isNew, QWidget* parent)
    : Parent(parent)
    , m_textEdit(nullptr)
    , m_colorButton(nullptr)
    , m_bookmarkIndex(bookmarkIndex)
    , m_isNewBookmark(isNew)
{
    setAttribute(Qt::WA_DeleteOnClose, true);
    setSizeGripEnabled(EASY_GLOBALS.use_custom_window_header);

    const auto& bookmark = EASY_GLOBALS.bookmarks[m_bookmarkIndex];

    auto saveButton = new QPushButton("Save");
    connect(saveButton, &QPushButton::clicked, this, &This::onSaveClicked);

    auto deleteButton = new QPushButton("Delete");
    connect(deleteButton, &QPushButton::clicked, this, &This::onDeleteClicked);
    deleteButton->setVisible(!isNew);
    deleteButton->setEnabled(!isNew);

    auto cancelButton = new QPushButton("Cancel");
    connect(cancelButton, &QPushButton::clicked, this, &Parent::reject);

    auto buttonBox = new QWidget();
    buttonBox->setObjectName(QStringLiteral("BookmarkEditor_ButtonBox"));

    auto buttonBoxLayout = new QHBoxLayout(buttonBox);
    buttonBoxLayout->addStretch(1);
    buttonBoxLayout->addWidget(saveButton);
    buttonBoxLayout->addWidget(deleteButton);
    buttonBoxLayout->addWidget(cancelButton);

    m_textEdit = new QTextEdit();
    m_textEdit->setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    m_textEdit->setPlaceholderText(QStringLiteral("Add a description for the bookmark..."));
    m_textEdit->setText(QString::fromStdString(bookmark.text));

    m_colorButton = new QPushButton();
    m_colorButton->setObjectName(QStringLiteral("BookmarkEditor_ColorButton"));
    m_colorButton->setAutoFillBackground(true);
    connect(m_colorButton, &QPushButton::clicked, this, &This::onColorButtonClicked);

    auto palette = m_colorButton->palette();
    palette.setBrush(QPalette::Background, QBrush(QColor::fromRgb(bookmark.color)));
    m_colorButton->setPalette(palette);
    m_colorButton->setStyleSheet(QString("background-color: %1;").arg(QColor::fromRgb(bookmark.color).name()));

    auto colorBox = new QWidget();
    colorBox->setObjectName(QStringLiteral("BookmarkEditor_ColorBox"));

    auto colorBoxLayout = new QHBoxLayout(colorBox);
    colorBoxLayout->setContentsMargins(1, 0, 0, 2);
    colorBoxLayout->addWidget(new QLabel("Description"), 0, Qt::AlignLeft | Qt::AlignVCenter);
    colorBoxLayout->addStretch(1);
    colorBoxLayout->addWidget(new QLabel("Color"), 0, Qt::AlignRight | Qt::AlignVCenter);
    colorBoxLayout->addWidget(m_colorButton, 0, Qt::AlignRight | Qt::AlignVCenter);

    auto content = new QWidget();
    content->setObjectName(QStringLiteral("BookmarkEditor_Content"));

    auto contentLayout = new QVBoxLayout(content);
    contentLayout->setSpacing(0);
    contentLayout->addWidget(colorBox);
    contentLayout->addWidget(m_textEdit, 1);

    const WindowHeader::Buttons buttons {WindowHeader::MaximizeButton | WindowHeader::CloseButton};
    auto header = new WindowHeader(isNew ? "New bookmark" : "Edit bookmark", buttons, *this);

    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(1, 1, 1, 1);
    mainLayout->setSpacing(0);
    mainLayout->addWidget(header, 0, Qt::AlignTop);
    mainLayout->addWidget(content, 1);
    mainLayout->addWidget(buttonBox, 0, Qt::AlignBottom);

    connect(this, &QDialog::rejected, this, &This::onReject);

    loadSettings();
}

BookmarkEditor::~BookmarkEditor()
{
    saveSettings();
}

void BookmarkEditor::onSaveClicked(bool)
{
    auto& bookmark = EASY_GLOBALS.bookmarks[m_bookmarkIndex];

    bookmark.text = m_textEdit->toPlainText().trimmed().toStdString();
    bookmark.color = m_colorButton->palette().brush(QPalette::Background).color().rgb();
    EASY_GLOBALS.bookmark_default_color = bookmark.color;
    EASY_GLOBALS.has_local_changes = true;

    accept();
}

void BookmarkEditor::onDeleteClicked(bool)
{
    EASY_GLOBALS.has_local_changes = true;
    EASY_GLOBALS.bookmarks.erase(EASY_GLOBALS.bookmarks.begin() + m_bookmarkIndex);
    emit bookmarkRemoved(m_bookmarkIndex);
    reject();
}

void BookmarkEditor::onColorButtonClicked(bool)
{
    auto palette = m_colorButton->palette();

    QColorDialog colorDialog(palette.brush(QPalette::Background).color(), this);
    colorDialog.exec();

    auto color = colorDialog.currentColor();
    palette.setBrush(QPalette::Background, QBrush(color));
    m_colorButton->setPalette(palette);

    m_colorButton->setStyleSheet(QString("background-color: %1;").arg(color.name()));
    m_colorButton->style()->unpolish(m_colorButton);
    m_colorButton->style()->polish(m_colorButton);
    m_colorButton->update();
}

void BookmarkEditor::onReject()
{
    if (m_isNewBookmark)
    {
        EASY_GLOBALS.bookmarks.erase(EASY_GLOBALS.bookmarks.begin() + m_bookmarkIndex);
        emit bookmarkRemoved(m_bookmarkIndex);
    }
}

void BookmarkEditor::loadSettings()
{
    QSettings settings(::profiler_gui::ORGANAZATION_NAME, ::profiler_gui::APPLICATION_NAME);
    settings.beginGroup("BookmarkEditor");

    auto geometry = settings.value("geometry").toByteArray();
    if (!geometry.isEmpty())
        restoreGeometry(geometry);

    settings.endGroup();
}

void BookmarkEditor::saveSettings()
{
    QSettings settings(::profiler_gui::ORGANAZATION_NAME, ::profiler_gui::APPLICATION_NAME);
    settings.beginGroup("BookmarkEditor");
    settings.setValue("geometry", saveGeometry());
    settings.endGroup();
}
