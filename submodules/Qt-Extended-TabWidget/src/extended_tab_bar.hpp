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
#ifndef EXTENTED_TAB_BAR_HPP
#define EXTENTED_TAB_BAR_HPP

#include <QTabWidget>
#include <QTabBar>

class ExtendedTabBar : public QTabBar
{
    Q_OBJECT

public:
    explicit ExtendedTabBar(QWidget * parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent *qme) override;
    void mouseMoveEvent(QMouseEvent *qme) override;
    void mouseReleaseEvent(QMouseEvent *qme) override;

signals:
    /// emitted when the user wants to detach tab
    void dragAway(int tab);

private:
    QPoint drag_start;
    int    drag_index;
};

#endif // EXTENTED_TAB_BAR_HPP

