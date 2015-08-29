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
#ifndef PIXEL_CAYMAN_DOCUMENT_VISITOR_HPP
#define PIXEL_CAYMAN_DOCUMENT_VISITOR_HPP

#include "document.hpp"

namespace document {

/**
 * \brief Visitor for document elements
 */
class Visitor
{
public:
    virtual ~Visitor() {}

    /**
     * \brief Visit a generic element, makes the virtual call
     */
    void visit(DocumentElement& element)
    {
        element.apply(*this);
    }

    /**
     * \brief Begin processing a document
     *
     * \return \b true if the document should be processed
     *
     * If returns true, the document's children will be visited
     * and after that, leave() will be called for that document
     */
    virtual bool enter(Document& document) = 0;
    /**
     * \brief Finish processing a document
     */
    virtual void leave(Document& document) = 0;

    /**
     * \brief Begin processing a layer
     *
     * \return \b true if the layer should be processed
     *
     * If returns true, the layer's children will be visited
     * and after that, leave() will be called for that layer
     */
    virtual bool enter(Layer& layer) = 0;
    /**
     * \brief Finish processing a layer
     */
    virtual void leave(Layer& layer) = 0;

    /**
     * \brief Process an image
     */
    virtual void visit(Image& image) = 0;

    /**
     * \brief Begin processing an animation
     *
     * \return \b true if the animation should be processed
     *
     * If returns true, the animation's children will be visited
     * and after that, leave() will be called for that animation
     */
    virtual bool enter(Animation& animation) = 0;
    /**
     * \brief Finish processing an animation
     */
    virtual void leave(Animation& animation) = 0;

    /**
     * \brief Process a frame
     */
    virtual void visit(Frame& frame) = 0;
};

} // namespace document
#endif // PIXEL_CAYMAN_DOCUMENT_VISITOR_HPP
