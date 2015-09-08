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
#include "math.hpp"

namespace tool {

class Paint : public Tool, protected document::visitor::FrameRenderer
{
public:
    Paint() : Tool(), FrameRenderer(nullptr)
    {
        ballBrush(1, 0);
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
        QPainter painter(&image.image());
        painter.setBrush(color);
        painter.setPen(QPen(Qt::transparent));
        ::draw::line(line, [this, &painter](const QPoint& point){
            painter.drawPath(brush_path.translated(point));
        });
    }

    void draw(view::GraphicsWidget* widget)
    {
        color = widget->color();
        widget->document()->apply(*this);
    }

private:

    /**
     * \brief Set the brush to use a p-norm ball with the given diameter
     */
    void ballBrush(int diameter, qreal p_norm)
    {
        QPoint center(diameter/2, diameter/2);
        brush_mask = QImage(QSize(diameter, diameter), QImage::Format_ARGB32_Premultiplied);
        brush_region = QRegion();

        // Simple square for inf-norm distance
        if ( p_norm <= 0 )
        {
            QRect rect = brush_mask.rect().translated(-center);
            brush_mask.fill(Qt::black);
            brush_region = QRegion(rect);
            brush_path = QPainterPath();
            brush_path.addRect(rect);
            return;
        }

        brush_mask.fill(Qt::transparent);
        for ( int y = 0; y <= brush_mask.height(); y++ )
            for ( int x = 0; x <= brush_mask.width(); x++ )
                if ( math::pdistance(QPoint(x,y), center, p_norm) * 2 <= diameter )
                {
                    brush_mask.setPixel(x, y, Qt::black);
                    brush_region |= QRect(x-center.x(), y-center.x(), 1, 1);
                }

        QPainterPath new_path;
        new_path.addRegion(brush_region);
        brush_path = new_path.simplified();
    }

    void drawForegroundImpl(QPainter* painter)
    {
        if ( draw_line && line.p1() != line.p2() )
        {
            painter->drawPath(brush_path.translated(line.p1()));
            painter->drawLine(QLineF(line).translated(0.5,0.5));
        }

        painter->drawPath(brush_path.translated(line.p2()));
    }

    QLine  line;
    QColor color;
    bool   draw_line = false;

    QImage       brush_mask;
    QRegion      brush_region;
    QPainterPath brush_path;
};

} // namespace tool
#endif // PIXEL_CAYMAN_TOOL_PAINT_HPP
