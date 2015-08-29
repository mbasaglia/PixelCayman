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
#ifndef PIXEL_CAYMAN_DOCUMENT_IMAGE_HPP
#define PIXEL_CAYMAN_DOCUMENT_IMAGE_HPP

#include "frame.hpp"
#include <QImage>
#include <QColor>

namespace document {

class Layer;

/**
 * \brief Lead image, a single frame in a single layer
 */
class Image : public DocumentElement
{
public:
    explicit Image(Layer* layer, const QImage& image, Frame* frame = nullptr);
    explicit Image(Layer* layer,
                   const QSize& size,
                   const QColor& color = Qt::transparent,
                   Frame* frame = nullptr);

    const QImage& image() const;
    QImage& image();

    /**
    * \brief Paints the image with the given opacity [0,1]
    */
    void paint(QPainter& painter, qreal opacity) const;

    /**
    * \brief Paints the image
    */
    void paint(QPainter& painter) const;

    void apply(Visitor& visitor) override;

    /**
     * \brief Frame associated with this image
     */
    Frame* frame();
    const Frame* frame() const;
    void setFrame(Frame* frame);

    /**
     * \brief Layer associated with this image
     */
    Layer* layer();
    const Layer* layer() const;

private:
    QImage image_;
    Frame* frame_;
    Layer* layer_;
};

} // namespace document
#endif // PIXEL_CAYMAN_DOCUMENT_IMAGE_HPP
