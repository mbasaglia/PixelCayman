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
#ifndef PIXEL_CAYMAN_TOOL_PAINT_WIDGET_HPP
#define PIXEL_CAYMAN_TOOL_PAINT_WIDGET_HPP

#include "tool/brush.hpp"
#include "ui_tool_paint_widget.h"

namespace tool {

class Brush::Widget : public QWidget, private Ui_ToolPaintWidget
{
    Q_OBJECT

private:
    enum BrushType
    {
        Rectangle,
        Circle,
        SuperCircle,
        Image
    };

public:
    explicit Widget(Brush* tool = nullptr);

    void setImageBrushEnabled(bool enabled);
    void setTool(Brush* tool);

    void updatePreview();

protected:
    void changeEvent(QEvent* event) override;

private slots:
    void updateBrush();

private:
    qreal ratio = 1;
    Brush* tool;
};


} // namespace tool

#endif // PIXEL_CAYMAN_TOOL_PAINT_WIDGET_HPP
