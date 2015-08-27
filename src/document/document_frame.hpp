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
#ifndef PIXEL_CAYMAN_DOCUMENT_FRAME_HPP
#define PIXEL_CAYMAN_DOCUMENT_FRAME_HPP

#include <QList>
#include <QStringList>

#include "document_layer.hpp"

/**
 * \brief A frame in an animation
 */
class DocumentFrame
{
public:
    DocumentFrame(const QList<LayerMetadata*>& layers = {},
                  const QSize& size = QSize(),
                  const QColor& color = Qt::transparent)
    {
        for ( const LayerMetadata& md : layers )
            layers_.push_back(DocumentLayer(&md, size, color));
    }

    /**
     * \brief Layers in the frame
     */
    const std::list<DocumentLayer>& layers() const { return layers_; }
    std::list<DocumentLayer>& layers() { return layers_; }


private:
    std::list<DocumentLayer> layers_;
};

#endif // PIXEL_CAYMAN_DOCUMENT_FRAME_HPP
