/**
 * \file
 *
 * \author Mattia Basaglia
 *
 * \section License
 *
 * Copyright (C) 2015 Mattia Basaglia
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

#include "paint.hpp"

#include "view/graphics_widget.hpp"
#include "draw.hpp"
#include "math.hpp"
#include "ui/tool_paint_widget.hpp"

namespace tool {

Paint::Paint()
{
    options_widget = new Widget(this);

    ballBrush(1, 0);
}

Paint::~Paint()
{
    delete options_widget;
}

QIcon Paint::icon() const
{
    return QIcon::fromTheme("draw-brush");
}

QString Paint::name() const
{
    return QObject::tr("Paint");
}

QString Paint::description() const
{
    return {}; /// \todo
}

bool Paint::initialize(view::GraphicsWidget* widget)
{
    return true;
}

void Paint::finalize(view::GraphicsWidget* widget)
{
    end_draw(widget);
}

void Paint::mousePressEvent(const QMouseEvent* event, view::GraphicsWidget* widget)
{
    draw_line = event->modifiers() & Qt::ShiftModifier;

    QPoint point = widget->mapToImage(event->pos());
    line.setP1(point);
    line.setP2(point);

    if ( event->button() == Qt::LeftButton )
    {
        begin_draw(widget);
        draw(widget);
    }
}

void Paint::mouseMoveEvent(const QMouseEvent* event, view::GraphicsWidget* widget)
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

void Paint::mouseReleaseEvent(const QMouseEvent* event, view::GraphicsWidget* widget)
{
    if ( !draw_line )
        line.setP1(line.p2());

    draw_line = false;

    line.setP2(widget->mapToImage(event->pos()));

    if ( event->button() == Qt::LeftButton )
    {
        draw(widget);
        end_draw(widget);
    }

    line.setP1(line.p2());
}

void Paint::drawForeground(QPainter* painter, view::GraphicsWidget* widget)
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

QWidget* Paint::optionsWidget()
{
    return options_widget;
}

QCursor Paint::cursor(const view::GraphicsWidget* widget) const
{
    return QCursor(Qt::CrossCursor);
}

void Paint::draw(view::GraphicsWidget* widget)
{
    document::Image* image = activeImage(widget);
    if ( !image )
        return;

    QPainter painter(&image->image());
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.setBrush(widget->color());
    painter.setPen(Qt::NoPen);
    ::draw::line(line, [this, &painter](const QPoint& point){
        painter.drawPath(brush_path.translated(point));
    });
}

void Paint::begin_draw(view::GraphicsWidget* widget)
{
    if ( document::Image* image = activeImage(widget) )
        image->beginPainting(document::Image::tr("Paint"));
}

void Paint::end_draw(view::GraphicsWidget* widget)
{
    if ( document::Image* image = activeImage(widget) )
        image->endPainting();
}

document::Image* Paint::activeImage(view::GraphicsWidget* widget)
{
    if ( !widget )
        return nullptr;

    if ( !widget->activeLayer() || widget->activeLayer()->locked() )
        return nullptr;

    /// \todo Select the active frame
    return widget->activeLayer()->frameImage(nullptr);
}


void Paint::drawForegroundImpl(QPainter* painter)
{
    if ( draw_line && line.p1() != line.p2() )
    {
        painter->drawPath(brush_path.translated(line.p1()));
        painter->drawLine(QLineF(line).translated(0.5,0.5));
    }

    painter->drawPath(brush_path.translated(line.p2()));
}

void Paint::ballBrush(int diameter, qreal p_norm)
{
    QPoint center(diameter/2, diameter/2);


    // Simple square for inf-norm distance
    if ( p_norm <= 0 )
    {
        rectangleBrush({diameter, diameter});
        return;
    }

    int img_size = diameter;
    if ( diameter % 2 == 0 )
        img_size += 1;
    brush_mask = QImage(QSize(img_size, img_size), QImage::Format_ARGB32_Premultiplied);
    brush_mask.setOffset(-center);

    QRegion brush_region;
    brush_mask.fill(Qt::transparent);
    for ( int y = 0; y < brush_mask.height(); y++ )
        for ( int x = 0; x < brush_mask.width(); x++ )
            if ( math::pdistance(QPointF(x,y), center, p_norm) * 2 <= diameter )
            {
                brush_mask.setPixel(x, y, QColor(Qt::black).rgba());
                brush_region |= QRect(x-center.x(), y-center.x(), 1, 1);
            }

    QPainterPath new_path;
    new_path.addRegion(brush_region);
    brush_path = new_path.simplified();
    options_widget->updatePreview();
}

void Paint::rectangleBrush(const QSize& size)
{
    QRect rect(QPoint(0,0), size);
    brush_path = QPainterPath();
    brush_path.addRect(rect.translated(-rect.center()));

    brush_mask = QImage(size, QImage::Format_ARGB32_Premultiplied);
    brush_mask.fill(Qt::black);
    brush_mask.setOffset(-rect.center());
    options_widget->updatePreview();
}

} // namespace tool
