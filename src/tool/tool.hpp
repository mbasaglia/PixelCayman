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
#ifndef PIXEL_CAYMAN_TOOL_HPP
#define PIXEL_CAYMAN_TOOL_HPP

#include "document/visitor.hpp"
#include "view/graphics_widget.hpp"
#include <QMouseEvent>

namespace tool {

/**
 * \brief A tool that is used to edit a Document in a GraphicsWidget
 */
class Tool
{
public:
    virtual ~Tool(){}

    /**
     * \brief Tool button icon
     */
    virtual QIcon icon() const = 0;

    /**
     * \brief Tool name
     */
    virtual QString name() const = 0;

    /**
     * \brief Short description
     */
    virtual QString description() const = 0;

    /**
     * \brief Initialize the tool for a document
     *
     * Called when the tool is selected to work on the document
     *
     * \return Whether the initialization has been successful
     */
    virtual bool initialize(view::GraphicsWidget* widget) = 0;

    /**
     * \brief Finalize the tool for a document
     *
     * Called when the tool or document has changed
     */
    virtual void finalize(view::GraphicsWidget* widget) = 0;

    /**
     * \brief Mouse press event
     *
     * Called by GraphicsWidget
     */
    virtual void mousePressEvent(const QMouseEvent* event, view::GraphicsWidget* widget) = 0;

    /**
     * \brief Mouse move event
     *
     * Called by GraphicsWidget
     */
    virtual void mouseMoveEvent(const QMouseEvent* event, view::GraphicsWidget* widget) = 0;


    /**
     * \brief Mouse release event
     *
     * Called by GraphicsWidget
     */
    virtual void mouseReleaseEvent(const QMouseEvent* event, view::GraphicsWidget* widget) = 0;

    /**
     * \brief Used to render the tool on the view
     */
    virtual void drawForeground(QPainter* painter, view::GraphicsWidget* widget) = 0;

    /**
     * \brief Returns a widget that ca be used to change the tool behaviour
     *
     * The tool is responsible to keep ownership
     * and update it if the document changes
     */
    virtual QWidget* optionsWidget() = 0;

    /**
     * \brief Cursor to be displayed
     */
    virtual QCursor cursor(const view::GraphicsWidget* widget) const = 0;
};

} // namespace tool
#endif // PIXEL_CAYMAN_TOOL_HPP
