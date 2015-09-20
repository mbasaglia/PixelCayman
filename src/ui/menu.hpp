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
#ifndef PIXEL_CAYMAN_MENU_HPP
#define PIXEL_CAYMAN_MENU_HPP

#include <QMap>
#include <QMenu>

class Menu : public QObject
{
    Q_OBJECT

public:
    static Menu& instance()
    {
        static Menu singleton;
        return singleton;
    }

    bool addMenu(const QString& id, QMenu* menu)
    {
        if ( menus_.contains(id) )
        {
            delete menu;
            return false;
        }

        menus_.insert(id, menu);
        emit addedMenu(menu);
        return true;
    }

    void removeMenu(const QString& id)
    {
        auto it = menus_.find(id);
        if ( it != menus_.end() )
        {
            emit removedMenu(*it);
            delete *it;
            menus_.erase(it);
        }
    }

    QList<QMenu*> menus() const
    {
        return menus_.values();
    }

    QMenu* menu(const QString& id) const
    {
        return menus_[id];
    }

    bool empty() const
    {
        return menus_.empty();
    }

signals:
    void addedMenu(QMenu* menu);
    void removedMenu(QMenu* menu);

private:
    Menu() {}
    ~Menu()
    {
        for ( auto* menu : menus_ )
            delete menu;
    }

    QMap<QString, QMenu*> menus_;
};

#endif // PIXEL_CAYMAN_MENU_HPP
