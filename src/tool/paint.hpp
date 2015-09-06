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
#ifndef PIXEL_CAYMAN_TOOL_PAINT_HPP
#define PIXEL_CAYMAN_TOOL_PAINT_HPP

#include "tool.hpp"

#include <QIcon>
#include <QPainter>
#include "view/graphics_widget.hpp"
#include "draw.hpp"

namespace tool {

class Paint : public Tool, protected document::visitor::FrameRenderer
{
public:
    Paint() : Tool(), FrameRenderer(nullptr)
    {
    }

    QIcon icon() const override
    {
        return QIcon::fromTheme("draw-brush");
    }

    QString name() const override
    {
        return QObject::tr("Paint");
    }

    QString description() const override
    {
        return {}; /// \todo
    }

    bool initialize(view::GraphicsWidget* widget) override
    {
        widget->setCursor(Qt::CrossCursor);
        return true;
    }

    void finalize(view::GraphicsWidget* widget) override
    {
    }

    void mousePressEvent(const QMouseEvent* event, view::GraphicsWidget* widget) override
    {
        draw_line = event->modifiers() & Qt::ShiftModifier;

        QPoint point = widget->mapToImage(event->pos());
        line.setP1(point);
        line.setP2(point);

        if ( event->button() == Qt::LeftButton )
            draw(widget);
    }

    void mouseMoveEvent(const QMouseEvent* event, view::GraphicsWidget* widget) override
    {
        /// \todo Allow lines click by click instead of only via draggin (?)
        draw_line = (event->buttons() & Qt::LeftButton) &&
                    (event->modifiers() & Qt::ShiftModifier);

        if ( !draw_line )
            line.setP1(line.p2());

        line.setP2(widget->mapToImage(event->pos()));

        if ( draw_line && (event->modifiers() & Qt::ControlModifier) )
        {
            QLineF linef(line);
            linef.setAngle(qRound(linef.angle()/15)*15);
            line = linef.toLine();
        }

        if ( (event->buttons() & Qt::LeftButton) && !draw_line )
            draw(widget);
    }

    void mouseReleaseEvent(const QMouseEvent* event, view::GraphicsWidget* widget) override
    {
        if ( !draw_line )
            line.setP1(line.p2());

        draw_line = false;

        line.setP2(widget->mapToImage(event->pos()));

        if ( event->button() == Qt::LeftButton )
            draw(widget);

        line.setP1(line.p2());
    }

    void drawForeground(QPainter* painter, view::GraphicsWidget* widget) override
    {
        QPen pen(Qt::white, 2);
        pen.setCosmetic(true);
        painter->setPen(pen);
        painter->setBrush(Qt::transparent);
        drawForegroundImpl(painter);

        pen.setColor(Qt::black);
        pen.setWidth(0);
        painter->setPen(pen);
        drawForegroundImpl(painter);
    }

    QWidget* optionsWidget() override
    {
        return nullptr;
    }

    QCursor cursor(const view::GraphicsWidget* widget) const override
    {
        return QCursor(Qt::CrossCursor);
    }

protected:
    void render(document::Image& image) override
    {
        ::draw::line(line, [this, &image](const QPoint& point){

            QRect rect = area(point).intersected(image.image().rect());

            if ( rect.isNull() )
                return;

            for ( int y = rect.top(); y <= rect.bottom(); y++ )
                for ( int x = rect.left(); x <= rect.right(); x++ )
                    image.image().setPixel(x, y, color.rgba());
        });
    }

    void draw(view::GraphicsWidget* widget)
    {
        color = widget->color();
        widget->document()->apply(*this);
    }

private:
    QRect area(const QPoint& center)
    {
        return QRect(
            center.x()-diameter/2,
            center.y()-diameter/2,
            diameter,
            diameter);
    }

    void drawForegroundImpl(QPainter* painter)
    {
        if ( draw_line && line.p1() != line.p2() )
        {
            painter->drawEllipse(area(line.p1()));
            painter->drawLine(QLineF(line).translated(diameter/2.0, diameter/2.0));
        }

        painter->drawEllipse(area(line.p2()));
    }

    QLine  line;
    QColor color;
    int    diameter = 1;
    bool   draw_line = false;
};

} // namespace tool
#endif // PIXEL_CAYMAN_TOOL_PAINT_HPP
