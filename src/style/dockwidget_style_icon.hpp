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
 */

#ifndef DOCKWIDGET_STYLE_ICON_HPP
#define DOCKWIDGET_STYLE_ICON_HPP

#include <QProxyStyle>
#include <QWidget>
#include <QStyleOption>
#include <QPainter>
#include <QDockWidget>

class DockWidgetStyleIcon: public QProxyStyle
{
    Q_OBJECT

public:
    DockWidgetStyleIcon(QDockWidget* target)
        : QProxyStyle(target->style())
    {}

    void drawControl(ControlElement element,
                     const QStyleOption* option,
                     QPainter* painter,
                     const QWidget* widget = 0) const override
    {
        if( element == QStyle::CE_DockWidgetTitle )
        {
            int width = pixelMetric(QStyle::PM_SmallIconSize);

            int margin = baseStyle()->pixelMetric(QStyle::PM_DockWidgetTitleMargin);

            QPoint icon_point(margin + option->rect.left(),
                              margin + option->rect.center().y()-width/2);

            painter->drawPixmap(icon_point, widget->windowIcon().pixmap(width, width));

            const_cast<QStyleOption*>(option)->rect = option->rect.adjusted(width+margin, 0, 0, 0);
        }

        baseStyle()->drawControl(element, option, painter, widget);
    }
};

#endif // DOCKWIDGET_STYLE_ICON_HPP
