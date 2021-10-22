/************************************************************************
* file name         : dialog.h
* ----------------- :
* creation time     : 2018/06/03
* author            : Victor Zarubkin
* email             : v.s.zarubkin@gmail.com
* ----------------- :
* description       : The file contains declaration of Dialog - a preferred base class
*                   : for all dialogs in the application.
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

#ifndef EASY_PROFILER_DIALOG_H
#define EASY_PROFILER_DIALOG_H

#include "window_header.h"
#include <QDialog>
#include <QDialogButtonBox>
#include <QMessageBox>

class Dialog : public QDialog
{
    Q_OBJECT;

    QWidget*                           m_buttonBox;
    WindowHeader*                         m_header;
    QMessageBox::StandardButton m_rejectRoleButton;
    QMessageBox::StandardButton m_acceptRoleButton;

public:

    explicit Dialog(QWidget* parent,
        const QString& title,
        QWidget* content,
        QMessageBox::StandardButtons buttons = QMessageBox::Close);

    explicit Dialog(QWidget* parent,
        const QString& title,
        QWidget* content,
        WindowHeader::Buttons headerButtons,
        QMessageBox::StandardButtons buttons = QMessageBox::Close);

    explicit Dialog(QWidget* parent,
        QMessageBox::Icon icon,
        const QString& title,
        const QString& text,
        QMessageBox::StandardButtons buttons = QMessageBox::Ok);

    ~Dialog() override;

    static QMessageBox::StandardButton question(QWidget* parent, const QString& title, const QString& text, QMessageBox::StandardButtons buttons = QMessageBox::Yes | QMessageBox::No);
    static QMessageBox::StandardButton information(QWidget* parent, const QString& title, const QString& text, QMessageBox::StandardButtons buttons = QMessageBox::Ok);
    static QMessageBox::StandardButton warning(QWidget* parent, const QString& title, const QString& text, QMessageBox::StandardButtons buttons = QMessageBox::Close);
    static QMessageBox::StandardButton critical(QWidget* parent, const QString& title, const QString& text, QMessageBox::StandardButtons buttons = QMessageBox::Close);

    void addButton(class QAbstractButton* button, QDialogButtonBox::ButtonRole role);
    void addButton(class QAbstractButton* button, QMessageBox::ButtonRole role);

    void setWindowTitle(const QString& title);
    void setWindowIcon(const QIcon& icon);

    void done(int result) override;

private slots:

    void onButtonClicked(bool);

private:

    bool isPositiveRole(QDialogButtonBox::ButtonRole role) const;
    bool isNegativeRole(QDialogButtonBox::ButtonRole role) const;

    void createButtons(class QHBoxLayout* buttonBoxLayout, QMessageBox::StandardButtons buttons);
    class QPushButton* createStandardButton(QMessageBox::StandardButton id);

    static QWidget* messageBoxContent(QMessageBox::Icon icon, const QString& text);

}; // end of class Dialog.

#endif // EASY_PROFILER_DIALOG_H
