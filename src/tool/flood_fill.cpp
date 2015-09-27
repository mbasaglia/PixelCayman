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

#include "flood_fill.hpp"

#include <QCursor>
#include "misc/draw.hpp"

namespace tool {

FloodFill::Widget* FloodFill::options_widget = nullptr;
int FloodFill::options_widget_counter = 0;

FloodFill::FloodFill()
{
    options_widget_counter++;
}

FloodFill::~FloodFill()
{
    options_widget_counter--;

    /*if ( !options_widget_counter )
    {
        delete options_widget;
        options_widget = nullptr;
    }*/
}

QIcon FloodFill::icon() const
{
    return QIcon::fromTheme("fill-color");
}

QString FloodFill::name() const
{
    return tr("Flood Fill");
}

QString FloodFill::description() const
{
    return tr("Fill a region with the current color");
}

bool FloodFill::initialize(view::GraphicsWidget* widget)
{
    return true;
}

void FloodFill::finalize(view::GraphicsWidget* widget)
{
}

void FloodFill::mousePressEvent(const QMouseEvent* event, view::GraphicsWidget* widget)
{
    document::Image* image = activeImage(widget);
    if ( event->button() == Qt::LeftButton && image )
    {
        QPoint point = widget->mapToImage(event->pos());
        QRgb pixel = image->image().pixel(point);
        auto region = misc::draw::floodFill(image->image(), point,
            [pixel](QRgb rgb){ return rgb == pixel; });
        if ( !region.isEmpty() )
        {
            image->beginPainting(tr("Flood Fill"));
            QPainter painter(&image->image());
            painter.setPen(Qt::NoPen);
            painter.setBrush(widget->color());
            painter.drawRects(region.rects());
            painter.end();
            image->endPainting();
        }
    }
}

void FloodFill::mouseMoveEvent(const QMouseEvent* event, view::GraphicsWidget* widget)
{
}

void FloodFill::mouseReleaseEvent(const QMouseEvent* event, view::GraphicsWidget* widget)
{
}

void FloodFill::drawForeground(QPainter* painter, view::GraphicsWidget* widget)
{
}

QWidget* FloodFill::optionsWidget()
{
    /*if ( !options_widget )
    {
        options_widget = new Widget;
        // pretty much impossible to keep ownership of widgets
        // once they are inserted in a layout
        QObject::connect(options_widget, &QObject::destroyed, []{
            options_widget = nullptr;
        });
    }

    return options_widget;*/
    return nullptr;
}

QCursor FloodFill::cursor(const view::GraphicsWidget* widget) const
{
    return QCursor(Qt::CrossCursor);
}

} // namespace tool
