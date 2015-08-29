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

#include "image.hpp"

namespace document {

class Document;

/**
 * \brief A layer in the document
 */
class Layer : public DocumentElement
{
public:
    explicit Layer(Document* owner, const QString& name);
    Layer(const Layer&) = delete;
    Layer& operator=(const Layer&) = delete;
    ~Layer();

    /**
     * \brief Child layers
     */
    QList<Layer*> children();
    QList<const Layer*> children() const;

    /**
     * \brief Human-readable name for the layer
     */
    QString name() const;
    void setName(const QString& name);

    /**
     * \brief Layer opacity [0-1]
     */
    qreal opacity() const;
    void setOpacity(qreal opacity);

    /**
     * \brief Whether the layer is visible or not
     */
    bool visible() const;
    void setVisible(bool visible);

    /**
     * \brief Whether the layer allows edits to its image and children
     */
    bool locked() const;
    void setLocked(bool locked);

    /**
     * \brief Images for this layer
     */
    QList<Image*> frameImages();
    QList<const Image*> frameImages() const;

    /**
     * \brief Creates a new frame for this layer
     * \returns The created image, the layer keeps its ownership
     */
    Image* addFrameImage();

    void apply(Visitor& visitor) override;

    /**
     * \brief Paint the layer
     */
    void paint(QPainter& painter) const;

    /**
     * \brief Paint the layer, disregarding \c visible and \c opacity
     */
    void paintFullAlpha(QPainter& painter) const;

private:
    QList<Layer*> children_;
    QString name_;
    QList<Image*> frames_;
    bool visible_ = true;
    qreal opacity_ = 1;
    bool locked_ = false;
    Document* owner_;
};

} // namespace document
#endif // PIXEL_CAYMAN_DOCUMENT_LAYER_HPP
