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
#ifndef PIXEL_CAYMAN_DOCUMENT_VISITOR_RESIZE_CANVAS_HPP
#define PIXEL_CAYMAN_DOCUMENT_VISITOR_RESIZE_CANVAS_HPP

#include "document/visitor.hpp"

namespace document {
namespace visitor {

/**
 * \brief Changes the canvas size
 */
class ResizeCanvas : public Visitor
{
    Q_DECLARE_TR_FUNCTIONS(ResizeCanvas)
public:
    explicit ResizeCanvas(const QRect& new_rect) : new_rect(new_rect) {}

    bool enter(Document& document) override
    {
        document.undoStack().beginMacro(tr("Resize Document"));
        document.setImageSize(new_rect.size());
        return true;
    }

    void leave(Document& document) override
    {
        document.undoStack().endMacro();
    }

    bool enter(Layer& layer) override
    {
        return true;
    }

    void visit(Image& image) override
    {
        image.resize(new_rect);
    }

private:
    QRect new_rect;
};

} // namespace visitor
} // namespace document
#endif // PIXEL_CAYMAN_DOCUMENT_VISITOR_RESIZE_CANVAS_HPP
