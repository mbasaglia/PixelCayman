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
 */
#ifndef PIXEL_CAYMAN_MESSAGE_HPP
#define PIXEL_CAYMAN_MESSAGE_HPP

#include <QDebug>
#include <QMessageBox>
#include <QApplication>

class Message;
/**
 * \brief Manages and shows messages
 * \todo Dock in mainwindow that shows the log
 */
class MessageManager : public QObject
{
    Q_OBJECT

public:
    void show(const Message& msg);

    QWidget* dialogParent() const
    {
        return dialog_parent;
    }

    void setDialogParent(QWidget* widget)
    {
        dialog_parent = widget;
    }

signals:
    void message(const Message& msg);

private:
    MessageManager(){}
    ~MessageManager(){}

    void outStream(const Message& msg);
    void showDialog(const Message& msg);

    QWidget* dialog_parent = nullptr;

    friend class Message;
};

/**
 * \brief Class that shows a message
 */
class Message
{
public:

    static MessageManager& manager()
    {
        static MessageManager instance;
        return instance;
    }

    /**
     * \brief Message behaviour
     */
    enum BehaviourEnum
    {
        Info        = 0x00,         ///< Normal message (not an error)
        Error       = 0x01,         ///< Error message
        Critical    = 0x02,         ///< Critical error, causes the application to quit
        Stream      = 0x04,         ///< Prints in an output stream (stderr or stdout)
        Dialog      = 0x08,         ///< Shows a dialog
        AllOutput   = Stream|Dialog,///< Both Stream and Dialog
    };

    explicit Message(int behaviour)
        : stream(&message), behaviour(behaviour)
    {}

    ~Message()
    {
        if ( !shown )
            manager().show(*this);
    }

    template<class T>
        Message& operator<< (T&& msg)
        {
            stream << std::forward<T>(msg);
            return *this;
        }

    bool hasBehaviour(BehaviourEnum beh) const
    {
        return (behaviour & beh) == beh;
    }

    QString text() const
    {
        return message;
    }

private:

    QString message;
    QTextStream stream;
    int behaviour;
    bool shown = false;

    friend class MessageManager;
};


#endif // PIXEL_CAYMAN_MESSAGE_HPP
