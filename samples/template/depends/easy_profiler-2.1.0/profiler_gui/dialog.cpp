/************************************************************************
* file name         : dialog.cpp
* ----------------- :
* creation time     : 2018/06/03
* author            : Victor Zarubkin
* email             : v.s.zarubkin@gmail.com
* ----------------- :
* description       : The file contains implementation of Dialog.
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

#include "dialog.h"
#include "globals.h"
#include "window_header.h"

#include <QApplication>
#include <QDebug>
#include <QHBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QPixmap>
#include <QPushButton>
#include <QSizePolicy>
#include <QStyle>
#include <QVBoxLayout>

#include <easy/utility.h>

Dialog::Dialog(QWidget* parent, const QString& title, QWidget* content, WindowHeader::Buttons headerButtons,
               QMessageBox::StandardButtons buttons)
    : QDialog(parent)
    , m_buttonBox(new QWidget())
    , m_header(nullptr)
    , m_rejectRoleButton(QMessageBox::NoButton)
    , m_acceptRoleButton(QMessageBox::NoButton)
{
    setSizeGripEnabled(EASY_GLOBALS.use_custom_window_header);

    m_header = new WindowHeader(title, headerButtons, *this);

    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(1, 1, 1, 1);
    mainLayout->setSpacing(0);
    mainLayout->addWidget(m_header, 0, Qt::AlignTop);
    mainLayout->addWidget(content, 1);

    auto buttonsLayout = new QHBoxLayout(m_buttonBox);
    if (buttons != QMessageBox::NoButton)
    {
        buttonsLayout->addStretch(1);
        createButtons(buttonsLayout, buttons);
        mainLayout->addWidget(m_buttonBox, 0, Qt::AlignBottom);
    }
}

Dialog::Dialog(QWidget* parent, const QString& title, QWidget* content, QMessageBox::StandardButtons buttons)
    : Dialog(parent, title, content, WindowHeader::CloseButton, buttons)
{

}

Dialog::Dialog(QWidget* parent, QMessageBox::Icon icon, const QString& title, const QString& text,
               QMessageBox::StandardButtons buttons)
    : Dialog(parent, title, Dialog::messageBoxContent(icon, text), WindowHeader::CloseButton, buttons)
{
    setSizeGripEnabled(false);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setFixedSize(sizeHint());
    setMouseTracking(false);
}

Dialog::~Dialog()
{

}

void Dialog::setWindowTitle(const QString& title)
{
    m_header->setTitle(title);
    QDialog::setWindowTitle(title);
}

void Dialog::setWindowIcon(const QIcon& icon)
{
    m_header->setWindowIcon(icon);
    QDialog::setWindowIcon(icon);
}

void Dialog::addButton(class QAbstractButton* button, QDialogButtonBox::ButtonRole role)
{
    button->setProperty("id", 0);
    button->setProperty("role", static_cast<int>(role));
    connect(button, &QAbstractButton::clicked, this, &Dialog::onButtonClicked);
    m_buttonBox->layout()->addWidget(button);
}

void Dialog::addButton(class QAbstractButton* button, QMessageBox::ButtonRole role)
{
    addButton(button, static_cast<QDialogButtonBox::ButtonRole>(role));
}

bool Dialog::isPositiveRole(QDialogButtonBox::ButtonRole role) const
{
    return role == QDialogButtonBox::AcceptRole ||
        role == QDialogButtonBox::ApplyRole ||
        role == QDialogButtonBox::YesRole ||
        role == QDialogButtonBox::NoRole;
}

bool Dialog::isNegativeRole(QDialogButtonBox::ButtonRole role) const
{
    return role == QDialogButtonBox::RejectRole ||
        role == QDialogButtonBox::DestructiveRole ||
        role == QDialogButtonBox::ResetRole;
}

void Dialog::createButtons(class QHBoxLayout* buttonBoxLayout, QMessageBox::StandardButtons buttons)
{
    for (int i = QMessageBox::FirstButton; i <= QMessageBox::LastButton; ++i)
    {
        const auto id = static_cast<QMessageBox::Button>(i);
        if (buttons.testFlag(id))
        {
            auto button = createStandardButton(id);
            if (button != nullptr)
                buttonBoxLayout->addWidget(button);
        }
    }
}

QPushButton* Dialog::createStandardButton(QMessageBox::StandardButton id)
{
    QString text;
    auto role = QDialogButtonBox::RejectRole;
    
    switch (id)
    {
        case QMessageBox::Ok:
            text = "OK"; role = QDialogButtonBox::AcceptRole;
            break;

        case QMessageBox::Open:
            text = "Open"; role = QDialogButtonBox::AcceptRole;
            break;

        case QMessageBox::Save:
            text = "Save"; role = QDialogButtonBox::AcceptRole;
            break;

        case QMessageBox::Cancel:
            text = "Cancel"; role = QDialogButtonBox::RejectRole;
            break;

        case QMessageBox::Close:
            text = "Close"; role = QDialogButtonBox::RejectRole;
            break;

        case QMessageBox::Discard:
            text = "Discard"; role = QDialogButtonBox::DestructiveRole;
            break;

        case QMessageBox::Apply:
            text = "Apply"; role = QDialogButtonBox::ApplyRole;
            break;

        case QMessageBox::Reset:
            text = "Reset"; role = QDialogButtonBox::ResetRole;
            break;

        case QMessageBox::RestoreDefaults:
            text = "Restore defaults"; role = QDialogButtonBox::ResetRole;
            break;

        case QMessageBox::Help:
            text = "Help"; role = QDialogButtonBox::HelpRole;
            break;

        case QMessageBox::SaveAll:
            text = "Save all"; role = QDialogButtonBox::AcceptRole;
            break;

        case QMessageBox::Yes:
            text = "Yes"; role = QDialogButtonBox::YesRole;
            break;

        case QMessageBox::YesToAll:
            text = "Yes to all"; role = QDialogButtonBox::YesRole;
            break;

        case QMessageBox::No:
            text = "No"; role = QDialogButtonBox::NoRole;
            break;

        case QMessageBox::NoToAll:
            text = "No to all"; role = QDialogButtonBox::NoRole;
            break;

        case QMessageBox::Abort:
            text = "Abort"; role = QDialogButtonBox::RejectRole;
            break;

        case QMessageBox::Retry:
            text = "Retry"; role = QDialogButtonBox::AcceptRole;
            break;

        case QMessageBox::Ignore:
            text = "Ignore"; role = QDialogButtonBox::AcceptRole;
            break;

        default:
            return nullptr;
    }

    if (isPositiveRole(role) && m_acceptRoleButton == QMessageBox::NoButton)
        m_acceptRoleButton = id;
    else if (isNegativeRole(role) && m_rejectRoleButton == QMessageBox::NoButton)
        m_rejectRoleButton = id;
    
    auto button = new QPushButton(text);
    button->setProperty("id", static_cast<int>(id));
    button->setProperty("role", static_cast<int>(role));
    connect(button, &QPushButton::clicked, this, &Dialog::onButtonClicked);

    return button;
}

QMessageBox::StandardButton Dialog::question(QWidget* parent, const QString& title, const QString& text, QMessageBox::StandardButtons buttons)
{
    Dialog dialog(parent, QMessageBox::Question, title, text, buttons);
    return static_cast<QMessageBox::StandardButton>(dialog.exec());
}

QMessageBox::StandardButton Dialog::information(QWidget* parent, const QString& title, const QString& text, QMessageBox::StandardButtons buttons)
{
    Dialog dialog(parent, QMessageBox::Information, title, text, buttons);
    return static_cast<QMessageBox::StandardButton>(dialog.exec());
}

QMessageBox::StandardButton Dialog::warning(QWidget* parent, const QString& title, const QString& text, QMessageBox::StandardButtons buttons)
{
    Dialog dialog(parent, QMessageBox::Warning, title, text, buttons);
    return static_cast<QMessageBox::StandardButton>(dialog.exec());
}

QMessageBox::StandardButton Dialog::critical(QWidget* parent, const QString& title, const QString& text, QMessageBox::StandardButtons buttons)
{
    Dialog dialog(parent, QMessageBox::Critical, title, text, buttons);
    return static_cast<QMessageBox::StandardButton>(dialog.exec());
}

QWidget* Dialog::messageBoxContent(QMessageBox::Icon icon, const QString& text)
{
    auto iconLabel = new QLabel();
    auto style = QApplication::style();
    const int size = style->pixelMetric(QStyle::PM_MessageBoxIconSize);

    switch (icon)
    {
        case QMessageBox::Question:
            iconLabel->setPixmap(style->standardIcon(QStyle::SP_MessageBoxQuestion).pixmap(size, size));
            break;

        case QMessageBox::Information:
            iconLabel->setPixmap(style->standardIcon(QStyle::SP_MessageBoxInformation).pixmap(size, size));
            break;

        case QMessageBox::Warning:
            iconLabel->setPixmap(style->standardIcon(QStyle::SP_MessageBoxWarning).pixmap(size, size));
            break;

        case QMessageBox::Critical:
            iconLabel->setPixmap(style->standardIcon(QStyle::SP_MessageBoxCritical).pixmap(size, size));
            break;

        default:
        {
            delete iconLabel;
            iconLabel = nullptr;
            break;
        }
    }

    auto content = new QWidget();
    auto layout = new QHBoxLayout(content);

    if (iconLabel != nullptr)
    {
        layout->addWidget(iconLabel, 0, Qt::AlignTop);
        layout->addSpacing(size / 4);
    }

    layout->addWidget(new QLabel(text), 1, Qt::AlignTop);

    return content;
}

void Dialog::onButtonClicked(bool)
{
    auto button = sender();

    const auto id = static_cast<QMessageBox::Button>(button->property("id").toInt());
    const auto role = static_cast<QMessageBox::ButtonRole>(button->property("role").toInt());

    if (id == QMessageBox::NoButton)
        QDialog::done(role);
    else
        QDialog::done(id);
}

void Dialog::done(int result)
{
    if (result == QDialog::Rejected)
        QDialog::done(m_rejectRoleButton);
    else if (result == QDialog::Accepted)
        QDialog::done(m_acceptRoleButton);
    else
        QDialog::done(result);
}

