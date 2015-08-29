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

namespace document {

GraphicsWidget::GraphicsWidget(Document* document)
    : document_(document)
{
    QGraphicsScene* scene = new QGraphicsScene(this);
    scene->setSceneRect(QRectF(QPointF(),document->imageSize()));
    GraphicsItem* item = new GraphicsItem(document);
    scene->addItem(item);
    setScene(scene);

    setFrameStyle(QFrame::NoFrame);
}

Document* GraphicsWidget::document() const
{
    return document_;
}

void GraphicsWidget::drawBackground(QPainter * painter, const QRectF & rect)
{
    /// \todo Make it available as an object in the color_widgets library
    static QBrush transparency(QPixmap(QLatin1String(":/color_widgets/alphaback.png")));

    QGraphicsView::drawBackground(painter, rect);

    painter->setBrush(transparency);
    painter->setPen(QPen(Qt::transparent));
    painter->drawRect(sceneRect());
}

void GraphicsWidget::drawForeground(QPainter * painter, const QRectF & rect)
{
    static QPen outline(Qt::gray, 1, Qt::DashLine);

    QGraphicsView::drawForeground(painter, rect);

    painter->setBrush(Qt::transparent);
    painter->setPen(outline);
    painter->drawRect(sceneRect().adjusted(-1,-1,0,0));
}

} // namespace document
