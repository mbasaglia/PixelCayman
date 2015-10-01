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

#include "brush.hpp"

#include "eraser.hpp"
#include "view/graphics_widget.hpp"
#include "misc/draw.hpp"
#include "misc/math.hpp"
#include "ui/widgets/tool_paint_widget.hpp"
#include "registry.hpp"
#include <QApplication>

namespace tool {

Brush::Widget* Brush::options_widget = nullptr;
int Brush::options_widget_counter = 0;

Brush::Brush()
{
    options_widget_counter++;

    ballBrush(1, 0);
}

Brush::~Brush()
{
    options_widget_counter--;

    if ( !options_widget_counter )
    {
        delete options_widget;
        options_widget = nullptr;
    }
}

QIcon Brush::icon() const
{
    return QIcon::fromTheme("draw-brush");
}

QString Brush::name() const
{
    return tr("Brush");
}

QString Brush::description() const
{
    return tr("Draw with the current color");
}

bool Brush::initialize(view::GraphicsWidget* widget)
{
    return true;
}

void Brush::finalize(view::GraphicsWidget* widget)
{
    endDraw(widget);
}

void Brush::mousePressEvent(const QMouseEvent* event, view::GraphicsWidget* widget)
{
    draw_line = event->modifiers() & Qt::ShiftModifier;

    QPoint point = widget->mapToImage(event->pos());
    line.setP1(point);
    line.setP2(point);

    if ( event->button() == Qt::LeftButton )
    {
        beginDraw(widget);
        draw(widget);
    }
}

void Brush::mouseMoveEvent(const QMouseEvent* event, view::GraphicsWidget* widget)
{
    /// \todo Allow lines click by click instead of only via dragging (?)
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

void Brush::mouseReleaseEvent(const QMouseEvent* event, view::GraphicsWidget* widget)
{
    if ( !draw_line )
        line.setP1(line.p2());

    draw_line = false;

    line.setP2(widget->mapToImage(event->pos()));

    if ( event->button() == Qt::LeftButton )
    {
        draw(widget);
        endDraw(widget);
    }

    line.setP1(line.p2());
}

void Brush::drawForeground(QPainter* painter, view::GraphicsWidget* widget)
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

QWidget* Brush::optionsWidget()
{
    if ( !options_widget )
    {
        options_widget = new Widget;
        // pretty much impossible to keep ownership of widgets
        // once they are inserted in a layout
        QObject::connect(options_widget, &QObject::destroyed, []{
            options_widget = nullptr;
        });
    }

    options_widget->setTool(this);
    return options_widget;
}

QCursor Brush::cursor(const view::GraphicsWidget* widget) const
{
    return QCursor(Qt::CrossCursor);
}

QColor Brush::color(view::GraphicsWidget* widget) const
{
    return widget->color();
}

QPainter::CompositionMode Brush::blend(view::GraphicsWidget*) const
{
    return QPainter::CompositionMode_Source;
}

void Brush::draw(view::GraphicsWidget* widget)
{
    document::Image* image = activeImage(widget);
    if ( !image )
        return;

    QPainter painter(&image->image());
    painter.setCompositionMode(blend(widget));
    painter.setBrush(color(widget));
    painter.setPen(Qt::NoPen);
    misc::draw::line(line, [this, &painter](const QPoint& point){
        painter.drawPath(brush_path.translated(point));
    });
}

QString Brush::actionName(view::GraphicsWidget*) const
{
    return tr("Paint");
}

void Brush::beginDraw(view::GraphicsWidget* widget)
{
    if ( document::Image* image = activeImage(widget) )
        image->beginPainting(actionName(widget));
}

void Brush::endDraw(view::GraphicsWidget* widget)
{
    if ( document::Image* image = activeImage(widget) )
        image->endPainting();
}

void Brush::drawForegroundImpl(QPainter* painter)
{
    if ( draw_line && line.p1() != line.p2() )
    {
        painter->drawPath(brush_path.translated(line.p1()));
        painter->drawLine(QLineF(line).translated(0.5,0.5));
    }

    painter->drawPath(brush_path.translated(line.p2()));
}

void Brush::ballBrush(int diameter, qreal p_norm)
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

    if ( options_widget && options_widget->tool() == this )
        options_widget->updatePreview();
}

void Brush::rectangleBrush(const QSize& size)
{
    QRect rect(QPoint(0,0), size);
    brush_path = QPainterPath();
    brush_path.addRect(rect.translated(-rect.center()));

    brush_mask = QImage(size, QImage::Format_ARGB32_Premultiplied);
    brush_mask.fill(Qt::black);
    brush_mask.setOffset(-rect.center());

    if ( options_widget && options_widget->tool() == this )
        options_widget->updatePreview();
}

} // namespace tool
