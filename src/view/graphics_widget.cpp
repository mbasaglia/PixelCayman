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
#include "tool/tool.hpp"

#include <QMouseEvent>
#include <QApplication>
#include <QScrollBar>

namespace view {

class GraphicsWidget::Private
{
public:

    enum MouseMode {
        Resting,
        Panning,
    };

    void setCursor(GraphicsWidget* widget)
    {
        return widget->setCursor(tool ? tool->cursor(widget) : Qt::ArrowCursor);
    }

    void generateMouseEvent(GraphicsWidget* widget)
    {
        if ( !tool )
            return;

        QMouseEvent event(QEvent::MouseMove,
                          widget->mapFromGlobal(QCursor::pos()),
                          Qt::NoButton,
                          QApplication::mouseButtons(),
                          QApplication::keyboardModifiers()
                         );
        tool->mouseMoveEvent(&event, widget);
    }

    GraphicsItem*       document_item;
    QPoint              drag_point;
    MouseMode           mouse_mode = Resting;
    ::tool::Tool*       tool = nullptr;
    QColor              color = Qt::black;
    ::document::Layer*  active_layer = nullptr;
};

GraphicsWidget::GraphicsWidget(::document::Document* document)
    : p(new Private)
{
    if ( !document )
    {
        delete p;
        throw std::logic_error("GraphicsWidget::GraphicsWidget called with a null document");
    }

    QGraphicsScene* scene = new QGraphicsScene(this);
    scene->setSceneRect(QRectF(QPointF(),document->imageSize()));
    p->document_item = new GraphicsItem(document);
    scene->addItem(p->document_item);
    setScene(scene);

    setFrameStyle(QFrame::NoFrame);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    setMouseTracking(true);
    setRenderHint(QPainter::Antialiasing);

    connect(horizontalScrollBar(), &QAbstractSlider::sliderReleased,
            this, &GraphicsWidget::fitSceneRect);
    connect(verticalScrollBar(), &QAbstractSlider::sliderReleased,
            this, &GraphicsWidget::fitSceneRect);
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

void GraphicsWidget::fitSceneRect()
{
    QRectF viewport( mapToScene(0, 0), mapToScene(width(), height()));
    QRectF scene_rect = p->document_item->sceneBoundingRect().adjusted(-4, -4, 4, 4);
    setSceneRect(scene_rect.united(viewport));
}

void GraphicsWidget::translate(const QPointF& delta)
{
    p->document_item->setPos(p->document_item->pos()+delta);
    fitSceneRect();
}

void GraphicsWidget::drawBackground(QPainter* painter, const QRectF & rect)
{
    /// \todo Make it available as an object in the color_widgets library
    static QBrush transparency(QPixmap(QLatin1String(":/color_widgets/alphaback.png")));

    QGraphicsView::drawBackground(painter, rect);

    painter->setBrush(transparency);
    painter->setPen(QPen(Qt::transparent));
    painter->drawRect(p->document_item->sceneBoundingRect());
}

void GraphicsWidget::drawForeground(QPainter* painter, const QRectF & rect)
{
    QPen outline(Qt::black, 1, Qt::DotLine);
    outline.setCosmetic(true);

    QGraphicsView::drawForeground(painter, rect);

    painter->setBrush(Qt::transparent);
    painter->setPen(outline);
    painter->drawRect(p->document_item->sceneBoundingRect());

    if ( p->tool && p->mouse_mode != Private::Panning )
    {
        painter->translate(p->document_item->pos());
        p->tool->drawForeground(painter, this);
    }
}

void GraphicsWidget::mousePressEvent(QMouseEvent *event)
{
    p->drag_point = event->pos();

    if ( p->mouse_mode == Private::Resting && event->button() == Qt::MiddleButton )
    {
        // drag view
        setCursor(Qt::ClosedHandCursor);
        p->mouse_mode = Private::Panning;
    }

    if ( p->tool )
    {
        p->tool->mousePressEvent(event, this);
    }

    viewport()->update();
}

void GraphicsWidget::mouseMoveEvent(QMouseEvent *event)
{
    QPoint mouse_point = event->pos();

    if ( p->mouse_mode == Private::Panning )
    {
        // drag view
        QPointF delta = mouse_point - p->drag_point;
        delta /= zoomFactor();
        translate(delta);
    }

    if ( p->tool )
    {
        p->tool->mouseMoveEvent(event, this);
    }

    p->drag_point = mouse_point;

    viewport()->update();
}

void GraphicsWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if ( p->mouse_mode == Private::Panning && event->button() == Qt::MiddleButton )
    {
        p->setCursor(this);
        p->mouse_mode = Private::Resting;
    }

    if ( p->tool )
    {
        p->tool->mouseReleaseEvent(event, this);
    }

    viewport()->update();
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

    p->generateMouseEvent(this);
    viewport()->update();
}

tool::Tool* GraphicsWidget::currentTool() const
{
    return p->tool;
}

void GraphicsWidget::setCurrentTool(tool::Tool* tool)
{
    if ( p->tool )
        p->tool->finalize(this);

    p->tool = tool;

    if ( p->tool )
        p->tool->initialize(this);

    p->setCursor(this);
    p->generateMouseEvent(this);

    viewport()->update();
}

QPoint GraphicsWidget::mapToImage(const QPoint& point)
{
    QPointF pointf = p->document_item->mapFromScene(mapToScene(point));
    return QPoint(pointf.x(), pointf.y());
}

QPoint GraphicsWidget::mapFromImage(const QPoint& point)
{
    return mapFromScene(p->document_item->mapToScene(point));
}

QColor GraphicsWidget::color() const
{
    return p->color;
}

void GraphicsWidget::setColor(const QColor& color)
{
    if ( color != p->color )
        emit ( p->color = color );
}

document::Layer* GraphicsWidget::activeLayer() const
{
    /// \todo Handle the active layer being removed
    if ( !p->active_layer && !p->document_item->document()->layers().empty() )
        return p->document_item->document()->layers().back();
    return p->active_layer;
}

void GraphicsWidget::setActiveLayer(document::Layer* layer)
{
    p->active_layer = layer;
}

} // namespace view
