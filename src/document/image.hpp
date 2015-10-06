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

#include <QImage>
#include <QColor>

#include "frame.hpp"
#include "command/change_image.hpp"

namespace document {

class Layer;

/**
 * \brief Lead image, a single frame in a single layer
 */
class Image : public DocumentElement
{
    Q_OBJECT
public:
    explicit Image(Layer* layer, const QImage& image, Frame* frame = nullptr);
    explicit Image(Layer* layer,
                   const QSize& size,
                   const QColor& color = Qt::transparent,
                   Frame* frame = nullptr);

    ~Image();

    /**
     * \brief Underlying image
     */
    const QImage& image() const;
    QImage& image();

    /**
     * \brief Begins a painting operation
     * \param text Human-readable name of the operation
     */
    void beginPainting(const QString& text);
    /**
     * \brief Ends the current paint operation and pushes the changes to the
     *        document undo stack
     */
    void endPainting();

    /**
    * \brief Paints the image
    */
    void paint(QPainter& painter) const;

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

    /**
     * \brief Resize the image
     * \param new_rect The new QRect relative to the old one
     */
    void resize(const QRect& new_rect);


    void apply(Visitor& visitor) override;
    Document* parentDocument() const override;

protected:
    void parentDocumentSet(Document* doc) override;

private:
    void setColors();

    QImage image_;
    Frame* frame_;
    Layer* layer_;
    command::ChangeImage* command_ = nullptr;
};

} // namespace document
#endif // PIXEL_CAYMAN_DOCUMENT_IMAGE_HPP
