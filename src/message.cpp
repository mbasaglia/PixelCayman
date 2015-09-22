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

#include "message.hpp"

void MessageManager::show(const Message& msg)
{
    emit message(msg);
    if ( !msg.shown )
        const_cast<Message&>(msg).shown = true;
    if ( msg.hasBehaviour(Message::Stream) )
        outStream(msg);
    if ( msg.hasBehaviour(Message::Dialog) )
        showDialog(msg);
    if ( msg.hasBehaviour(Message::Critical) )
        QApplication::exit(1);
}

void MessageManager::outStream(const Message& msg)
{
    if ( msg.hasBehaviour(Message::Critical) )
        qCritical() << msg.message;
    else if ( msg.hasBehaviour(Message::Error) )
        qWarning() << msg.message;
    else
        qDebug() << msg.message;
}

void MessageManager::showDialog(const Message& msg)
{
    QMessageBox::Icon icon;
    QString title;
    if ( msg.hasBehaviour(Message::Critical) )
    {
        icon = QMessageBox::Critical;
        title = tr("Fatal error");
    }
    else if ( msg.hasBehaviour(Message::Error) )
    {
        icon = QMessageBox::Warning;
        title = tr("Warning");
    }
    else
    {
        icon = QMessageBox::Information;
        title = tr("Information");
    }

    QMessageBox(icon, title, msg.message, QMessageBox::Ok, dialog_parent).exec();
}
