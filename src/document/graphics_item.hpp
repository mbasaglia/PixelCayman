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
#ifndef PIXEL_CAYMAN_DOCUMENT_GRAPHICS_ITEM_HPP
#define PIXEL_CAYMAN_DOCUMENT_GRAPHICS_ITEM_HPP

#include <QGraphicsItem>
#include "document.hpp"
#include "visitor.hpp"

namespace document {

/**
 * \brief Class to render a document on a graphics view
 * \todo option for fullAlpha and frame
 */
class GraphicsItem : public QGraphicsItem
{
public:
    GraphicsItem( Document* document )
        : document_(document)
    {}

    QRectF boundingRect() const override
    {
        return QRectF(QPointF(), document_->imageSize());
    }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) override
    {
        visitor::Paint renderer(nullptr, painter);
        document_->apply(renderer);
    }

private:
    Document* document_;
};

} // namespace document
#endif // PIXEL_CAYMAN_DOCUMENT_GRAPHICS_ITEM_HPP
