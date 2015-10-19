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
#ifndef PIXEL_CAYMAN_VIEW_GRAPHICS_ITEM_HPP
#define PIXEL_CAYMAN_VIEW_GRAPHICS_ITEM_HPP

#include <QGraphicsItem>
#include "document/visitor.hpp"

namespace view {

/**
 * \brief Class to render a document on a graphics view
 * \todo option for the frame
 */
class GraphicsItem : public QGraphicsObject
{
    Q_OBJECT
    Q_PROPERTY(bool fullAlpha READ fullAlpha WRITE setFullAlpha NOTIFY fullAlphaChanged)

public:
    GraphicsItem( ::document::Document* document )
        : document_(document)
    {
        connect(document, &::document::Document::edited, this, &GraphicsItem::updateSlot);
    }

    QRectF boundingRect() const override
    {
        return QRectF(QPointF(), document_->imageSize());
    }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) override
    {
        ::document::visitor::Paint renderer(nullptr, painter, full_alpha);
        document_->apply(renderer);
    }

    ::document::Document* document() const
    {
        return document_;
    }

	bool fullAlpha() const
	{
		return full_alpha;
	}

	void setFullAlpha(bool fullAlpha)
	{
		if ( fullAlpha != full_alpha )
			emit fullAlphaChanged(full_alpha = fullAlpha);
	}

signals:
	void fullAlphaChanged(bool fullAlpha);

private slots:
    void updateSlot()
    {
        update();
    }

private:
    ::document::Document* document_;
	bool full_alpha = true;
};

} // namespace view
#endif // PIXEL_CAYMAN_VIEW_GRAPHICS_ITEM_HPP
