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

#include "graphics_widget.hpp"

#include <QMouseEvent>

namespace view {

namespace {
enum MouseMode {
    Resting,
    Panning,
};
} // namespace

class GraphicsWidget::Private
{
public:
    GraphicsItem* document_item;
    QPoint drag_point;
    MouseMode mouse_mode = Resting;
};

GraphicsWidget::GraphicsWidget(::document::Document* document)
    : p(new Private)
{
    QGraphicsScene* scene = new QGraphicsScene(this);
    scene->setSceneRect(QRectF(QPointF(),document->imageSize()));
    p->document_item = new GraphicsItem(document);
    scene->addItem(p->document_item);
    setScene(scene);

    setFrameStyle(QFrame::NoFrame);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
}

GraphicsWidget::~GraphicsWidget()
{
    delete p;
}

document::Document* GraphicsWidget::document() const
{
    return p->document_item->document();
}

qreal GraphicsWidget::zoomFactor() const
{
    return transform().m11();
}

void GraphicsWidget::setZoomFactor(qreal factor)
{
    if ( factor < 0.01 )
        return;
    QTransform new_transform (
        factor,            transform().m12(), transform().m13(),
        transform().m21(), factor,            transform().m23(),
        transform().m31(), transform().m32(), transform().m33()
    );
    setTransform(new_transform);
    emit zoomFactorChanged(factor);
}

void GraphicsWidget::zoom(qreal factor)
{
    if ( zoomFactor() * factor < 0.01 )
        return;

    QPoint mouse_pos = mapFromGlobal(QCursor::pos());
    QPointF scene_pos = mapToScene(mouse_pos);

    scale(factor,factor);

    // Anchor on mouse
    if ( rect().contains(mouse_pos) )
    {
        QPointF new_scene_pos = mapToScene(mouse_pos);
        translate(new_scene_pos-scene_pos);
    }

    emit zoomFactorChanged(zoomFactor());
}

void GraphicsWidget::expandSceneRect()
{
    QRectF viewport( mapToScene(0, 0), mapToScene(width(), height()));
    QRectF scene_rect = sceneRect();
    if ( !scene_rect.contains(viewport) )
    {
        setSceneRect(scene_rect.united(viewport));
    }
}

void GraphicsWidget::translate(const QPointF& delta)
{
    p->document_item->setPos(p->document_item->pos()+delta);
    expandSceneRect();
}

void GraphicsWidget::drawBackground(QPainter * painter, const QRectF & rect)
{
    /// \todo Make it available as an object in the color_widgets library
    static QBrush transparency(QPixmap(QLatin1String(":/color_widgets/alphaback.png")));

    QGraphicsView::drawBackground(painter, rect);

    painter->setBrush(transparency);
    painter->setPen(QPen(Qt::transparent));
    painter->drawRect(p->document_item->sceneBoundingRect());
}

void GraphicsWidget::drawForeground(QPainter * painter, const QRectF & rect)
{
    QPen outline(Qt::black, 1, Qt::DotLine);
    outline.setCosmetic(true);

    QGraphicsView::drawForeground(painter, rect);

    painter->setBrush(Qt::transparent);
    painter->setPen(outline);
    painter->drawRect(p->document_item->sceneBoundingRect());
}

void GraphicsWidget::mousePressEvent(QMouseEvent *event)
{
    // Only accept new modes if the old one has been resolved
    if ( p->mouse_mode != Resting )
        return;

    p->drag_point = event->pos();

    if ( event->button() == Qt::MiddleButton )
    {
        // drag view
        setCursor(Qt::ClosedHandCursor);
        p->mouse_mode = Panning;
    }
}

void GraphicsWidget::mouseMoveEvent(QMouseEvent *event)
{
    QPoint mouse_point = event->pos();

    if ( p->mouse_mode == Panning )
    {
        // drag view
        QPointF delta = mouse_point - p->drag_point;
        delta /= zoomFactor();
        translate(delta);
    }

    p->drag_point = mouse_point;
}

void GraphicsWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if ( p->mouse_mode == Panning && event->button() == Qt::MiddleButton )
    {
        setCursor(Qt::ArrowCursor);
        p->mouse_mode = Resting;
    }
}

void GraphicsWidget::wheelEvent(QWheelEvent *event)
{
    if ( event->modifiers() & Qt::ControlModifier )
    {
        if ( event->delta() < 0 )
            zoom(0.8);
        else
            zoom(1.25);
    }
}

} // namespace view
