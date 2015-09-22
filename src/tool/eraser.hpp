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
#ifndef PIXEL_CAYMAN_TOOL_ERASE_HPP
#define PIXEL_CAYMAN_TOOL_ERASE_HPP

#include "brush.hpp"
#include "ui/tool_paint_widget.hpp"

namespace tool {

class Eraser : public Brush
{
public:

    QIcon icon() const override
    {
        return QIcon::fromTheme("draw-eraser");
    }

    QString name() const override
    {
        return tr("Eraser");
    }

    QString description() const override
    {
        return tr("Overwrite pixels with transparency");
    }

    QWidget* optionsWidget() override
    {
        Widget* options_widget = qobject_cast<Widget*>(Brush::optionsWidget());
        options_widget->setImageBrushEnabled(false);
        return options_widget;
    }

protected:
    QColor color(view::GraphicsWidget*) const override
    {
        return Qt::transparent;
    }

    QPainter::CompositionMode blend(view::GraphicsWidget*) const override
    {
        return QPainter::CompositionMode_Source;
    }

    QString actionName(view::GraphicsWidget*) const override
    {
        return tr("Erase");
    }
};

} // namespace tool
#endif // PIXEL_CAYMAN_TOOL_ERASE_HPP
