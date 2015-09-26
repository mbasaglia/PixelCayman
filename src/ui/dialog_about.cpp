/**
 * \file
 *
 * \author Mattia Basaglia
 *
 * \copyright Copyright (C) 2015 Mattia Basaglia
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "dialog_about.hpp"

#include <QEvent>
#include <QDesktopServices>
#include <QApplication>
#include "cayman/data.hpp"

DialogAbout::DialogAbout(QWidget* parent)
    : QDialog(parent)
{
    setupUi(this);

    label_icon->setPixmap(cayman::data().caymanIcon("pixel-cayman").pixmap(64));
    label_name->setText(qApp->applicationDisplayName());
    label_version->setText(qApp->applicationVersion());

    text_license->setHtml(tr(
R"(<!DOCTYPE html">
<html><head><meta name="qrichtext" content="1" /></head><body>
<p>Copyright (C) 2015 Mattia Basaglia</p>
<p>This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.</p>
<p>This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.</p>
<p>You should have received a copy of the GNU General Public License
along with this program.  If not, see
 <a href="http://www.gnu.org/licenses/">http://www.gnu.org/licenses/</a>.</p>
</body></html>)"));

    connect(text_license, &QTextBrowser::anchorClicked, [](const QUrl& url){
        QDesktopServices::openUrl(url);
    });
}

void DialogAbout::changeEvent(QEvent* event)
{
    if ( event->type() == QEvent::LanguageChange )
    {
        retranslateUi(this);
    }

    QWidget::changeEvent(event);
}
