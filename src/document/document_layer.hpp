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

#ifndef PIXEL_CAYMAN_DOCUMENT_LAYER_HPP
#define PIXEL_CAYMAN_DOCUMENT_LAYER_HPP

#include <QImage>
#include <QColor>

/**
 * \brief Non-image data associated with a layer
 */
struct LayerMetadata
{
    QString name;       ///< Human-readable name for the layer
    qreal opacity = 1;  ///< Opacity level (0 = invisible, 1 = fully visible
    bool visible = true;///< Whether the layer is going to be rendered
    bool locked = false;///< Whether the layer accepts modifications
};

/**
 * \brief A single layer
 */
class DocumentLayer
{
public:
    DocumentLayer(LayerMetadata* metadata, const QSize& size, const QColor& color = Qt::transparent);
    DocumentLayer(LayerMetadata* metadata, const QImage& image);

    const LayerMetadata* metadata() const;
    LayerMetadata* metadata();

    /**
     * \brief Layer size
     */
    QSize size() const;

    /**
     * \brief Image handled by the layer
     */
    const QImage& image() const;
    void setImage(const QImage& image);

    /**
     * \brief Paint the layer
     */
    void paint(QPainter& painter) const;

    /**
     * \brief Paint the layer, disregarding \c visible and \c opacity
     */
    void paintFullAlpha(QPainter& painter) const;

private:
    LayerMetadata* metadata_;
    QImage image_;
};

#endif // PIXEL_CAYMAN_DOCUMENT_LAYER_HPP
