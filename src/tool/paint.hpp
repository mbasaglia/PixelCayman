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
    }

    void mouseMoveEvent(const QMouseEvent* event, view::GraphicsWidget* widget) override
    {
        point = widget->mapToImage(event->pos());
        /// \todo Find active layer, color and frame
        if ( (event->buttons() & Qt::LeftButton) && widget->document()->imageRect().contains(point) )
        {
            color = widget->color();
            widget->document()->apply(*this);
        }
    }

    void mouseReleaseEvent(const QMouseEvent* event, view::GraphicsWidget* widget) override
    {
    }

    void drawForeground(QPainter* painter, view::GraphicsWidget* widget) override
    {
        QPen pen(Qt::white, 2);
        pen.setCosmetic(true);
        painter->setPen(pen);
        painter->setBrush(Qt::transparent);
        painter->drawEllipse(point, 8, 8);

        pen.setColor(Qt::black);
        pen.setWidth(0);
        painter->setPen(pen);
        painter->drawEllipse(point, 8, 8);
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
        image.image().setPixel(point, color.rgba());
    }

private:
    QPoint point;
    QColor color;
};

} // namespace tool
#endif // PIXEL_CAYMAN_TOOL_PAINT_HPP
