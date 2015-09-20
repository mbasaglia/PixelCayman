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
#ifndef PIXEL_CAYMAN_DOCUMENT_BUILDER_HPP
#define PIXEL_CAYMAN_DOCUMENT_BUILDER_HPP

#include <QHash>
#include "document.hpp"

namespace document {

/**
 * \brief Assists the creation of a document tree
 */
class Builder
{
private:
    /**
     * \brief Used to keep track of frames to keep references consistent
     */
    struct FrameReference
    {
        Frame* frame = nullptr; ///< Frame corresponding to this
        QList<Image*> images;   ///< Images that are part of this frame
    };

public:
    virtual ~Builder(){}

    /**
     * \brief Starts the creation of a new document
     * \return \b true if that can be done
     *         (ie: it isn't already building a document)
     * \post currentDocument() != nullptr && currentElement() != nullptr
     */
    bool beginDocument()
    {
        if ( document )
            return false;
        document = new Document();
        element = document;
        return true;
    }

    /**
     * \brief The document currently being created
     * \pre Called after beginDocument()
     */
    Document* currentDocument()
    {
        return document;
    }

    /**
     * \brief Returns the result and finishes the construction process
     * \pre Called after beginDocument()
     * \post only beginDocument() can be safely called
     */
    Document* endDocument()
    {
        if ( !document )
            return nullptr;
        // Finish pending operations
        endAnimation();
        endLayer();
        // Clear current document
        auto doc = document;
        document = nullptr;
        element = nullptr;
        // Assign the frames to their images
        linkImageFrames();
        // Remove undo actions
        doc->undoStack().clear();
        // Return the finished document
        return doc;
    }

    /**
     * \brief Currently edited element
     * \pre Called after beginDocument()
     */
    DocumentElement* currentElement()
    {
        return element;
    }

    /**
     * \brief Begins the construction of a layer
     * \pre Called after beginDocument()
     * \post currentLayer() != nullptr
     *
     * If called after a different call to beginLayer() and before the
     * corresponding endLayer(), it will be created as a child layer
     */
    Layer* beginLayer()
    {
        layer = new Layer(document, QString(), layer);
        if ( !layer->parentLayer() )
            document->insertLayer(layer);
        element = layer;
        return layer;
    }

    /**
     * \brief Layer currently being edited
     * \pre Called after beginLayer()
     */
    Layer* currentLayer()
    {
        return layer;
    }

    /**
     * \brief Finishes the current layer
     * \pre Called after beginLayer()
     */
    void endLayer()
    {
        // Finish editing the image
        endImage();
        if ( !layer )
            return;
        // Go back editing the parent
        layer = layer->parentLayer();
        // If no parent, go back to the document
        element = layer;
        if ( !layer )
            element = document;
    }

    /**
     * \brief Creates an image
     * \pre Called after beginLayer(), the currentDocument() has a valid size
     * \post currentImage() != nullptr
     */
    Image* beginImage()
    {
        endImage();
        image = layer->addFrameImage();
        element = image;
        return image;
    }

    /**
     * \brief Currently edited Image
     * \pre Called after beginImage()
     */
    Image* currentImage()
    {
        return image;
    }

    /**
     * \brief Set a temporary frame ID for the current image
     * \pre Called after beginImage()
     */
    void setImageFrame(const QString& frame_id)
    {
        if ( !frame_id.isEmpty() )
            frames[frame_id].images.push_back(image);
    }

    /**
     * \brief Finish editing the current image
     * \pre Called after beginImage()
     * \post currentImage() == nullptr
     */
    void endImage()
    {
        element = layer;
        image = nullptr;
    }

    Animation* beginAnimation()
    {
        /// \todo Animations
        return nullptr;
    }

    Animation* currentAnimation()
    {
        /// \todo Animations
        return nullptr;
    }

    void endAnimation()
    {
        /// \todo Animations
    }

    Frame* beginFrame(const QString& id = QString())
    {
        /// \todo Animations (insert in frames)
        return nullptr;
    }

    void setFrameId(const QString& id)
    {
        /// \todo insert in frames
    }

    Frame* currentFrame()
    {
        /// \todo Animations
        return nullptr;
    }

    void endFrame()
    {
        /// \todo Animations
    }

private:
    /**
     * \brief Assigns the correct frames from their temporary ID
     */
    void linkImageFrames()
    {
        for ( const auto& frame : frames )
        {
            if ( !frame.frame )
            {
                /// \todo Warning?
                continue;
            }

            for ( auto* image : frame.images )
                image->setFrame(frame.frame);
        }
        frames.clear();
    }

    Document* document = nullptr;
    DocumentElement* element = nullptr;
    Layer* layer = nullptr;
    Image* image = nullptr;
    QHash<QString, FrameReference>  frames;
};

} // namespace document
#endif // PIXEL_CAYMAN_DOCUMENT_BUILDER_HPP
