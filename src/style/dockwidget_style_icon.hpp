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
#include <QMainWindow>

class DockWidgetStyleIcon: public QProxyStyle
{
    Q_OBJECT
public:
    DockWidgetStyleIcon(QDockWidget* target)
        : QProxyStyle(target->style()), target(target)
    {}

    void drawControl(ControlElement element,
                     const QStyleOption* option,
                     QPainter* painter,
                     const QWidget* widget = 0) const override
    {
        if ( element == CE_DockWidgetTitle )
        {
            if ( const QStyleOptionDockWidget* dock_option =
                    qstyleoption_cast<const QStyleOptionDockWidget *>(option) )
            {
                QStyleOptionDockWidget my_option = *dock_option;

                int width = pixelMetric(PM_SmallIconSize);
                int margin_base = pixelMetric(PM_DockWidgetTitleMargin);
                int margin_h = margin_base + pixelMetric(PM_TabBarTabHSpace) / 2;
                int margin_v = margin_base;

                QPoint icon_point(margin_h + my_option.rect.left(),
                                  margin_v + my_option.rect.center().y()-width/2);

                painter->drawPixmap(icon_point, widget->windowIcon().pixmap(width, width));

                my_option.rect.adjust(width+margin_h, 0, 0, 0);

                return QProxyStyle::drawControl(element, &my_option, painter, widget);;
            }
        }
        /*else if ( element == CE_TabBarTabLabel && qobject_cast<const QMainWindow*>(widget->parentWidget()) )
        {
            if ( const QStyleOptionTab* tab_option = qstyleoption_cast<const QStyleOptionTab *>(option) )
            {
                QStyleOptionTab my_option(*tab_option);
                my_option.icon = target->windowIcon();
                return QProxyStyle::drawControl(element, &my_option, painter, widget);
            }
        }*/

        QProxyStyle::drawControl(element, option, painter, widget);
    }

private:
    QDockWidget* target;

};

#endif // DOCKWIDGET_STYLE_ICON_HPP
