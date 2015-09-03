/**
 * \file
 *
 * \author Mattia Basaglia
 *
 * \copyright Copyright (C) 2012-2015 Mattia Basaglia
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
#include "extended_tab_bar.hpp"
#include <QMouseEvent>

ExtendedTabBar::ExtendedTabBar(QWidget * parent)
    : QTabBar(parent), drag_index(-1)
{
}

void ExtendedTabBar::mousePressEvent(QMouseEvent *qme)
{
    if ( qme->button() == Qt::LeftButton )
    {
        drag_start = qme->pos();
        drag_index = tabAt(drag_start);
    }
    QTabBar::mousePressEvent(qme);
}

void ExtendedTabBar::mouseMoveEvent(QMouseEvent *qme)
{
    if ( drag_index != -1  &&
         abs ( (qme->pos() - drag_start).y() ) >= height() )
    {
        emit dragAway(drag_index);
        drag_index = -1;
    }
    QTabBar::mouseMoveEvent(qme);
}

void ExtendedTabBar::mouseReleaseEvent(QMouseEvent *qme)
{
    if ( qme->button() == Qt::MiddleButton )
    {
        int tab = tabAt(qme->pos());
        if ( tab != -1 )
            emit tabCloseRequested(tab);
    }
    QTabBar::mouseReleaseEvent(qme);
}





