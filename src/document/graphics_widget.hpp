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
#ifndef PIXEL_CAYMAN_DOCUMENT_GRAPHICS_WIDGET_HPP
#define PIXEL_CAYMAN_DOCUMENT_GRAPHICS_WIDGET_HPP

#include <QGraphicsView>
#include "graphics_item.hpp"

namespace document {

class GraphicsWidget : public QGraphicsView
{
    Q_OBJECT

public:
    explicit GraphicsWidget(Document* document);

    Document* document() const;

protected:
    void drawBackground(QPainter * painter, const QRectF & rect) override;
    void drawForeground(QPainter * painter, const QRectF & rect) override;

private:
    Document* document_;
};

} // namespace document
#endif // PIXEL_CAYMAN_DOCUMENT_GRAPHICS_WIDGET_HPP
