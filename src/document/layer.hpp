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
#include "layer_container.hpp"

#include <QPainter>

namespace document {

class Document;

/**
 * \brief A layer in the document
 */
class Layer : public LayerContainer
{
    Q_OBJECT

    /**
     * \brief Human-readable name for the layer
     */
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)

    /**
     * \brief Layer opacity [0-1]
     */
    Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity NOTIFY opacityChanged)

    /**
     * \brief Whether the layer is visible or not
     */
    Q_PROPERTY(bool visible READ visible WRITE setVisible NOTIFY visibleChanged)
    
    /**
     * \brief Whether the layer allows edits to its image and children
     */
    Q_PROPERTY(bool locked READ locked WRITE setLocked NOTIFY lockedChanged)

    /**
     * \brief Alpha blending operation
     */
    Q_PROPERTY(QPainter::CompositionMode blendMode READ blendMode WRITE setBlendMode NOTIFY blendModeChanged)

public:
    explicit Layer(Document* owner, const QString& name, Layer* parentLayer = nullptr);
    Layer(const Layer&) = delete;
    Layer& operator=(const Layer&) = delete;
    ~Layer();

    /**
     * \brief Parent layer
     */
    Layer* parentLayer();

    QString name() const;
    void setName(const QString& name);

    qreal opacity() const;
    void setOpacity(qreal opacity);

    bool visible() const;
    void setVisible(bool visible);

    bool locked() const;
    void setLocked(bool locked);

    /**
     * \brief Images for this layer
     */
    QList<Image*> frameImages();

    /**
     * \brief Get the image associated with the given frame
     */
    Image* frameImage(Frame* frame);

    /**
     * \brief Creates a new frame for this layer
     * \returns The created image, the layer keeps its ownership
     */
    Image* addFrameImage(const QColor& background_color = Qt::transparent);

    Image* addFrameImage(const QImage& image);

    void apply(Visitor& visitor) override;
    Document* parentDocument() const override;

    QPainter::CompositionMode blendMode() const;
    void setBlendMode(QPainter::CompositionMode blendMode);

    
signals:
    void nameChanged(const QString& name);
    void lockedChanged(bool locked);
    void visibleChanged(bool visible);
    void opacityChanged(qreal opacity);
    void blendModeChanged(QPainter::CompositionMode blendMode);

protected:
    void onInsertLayer(Layer* layer) override;
    void onRemoveLayer(Layer* layer) override;
    
private:
    QString name_;
    QList<Image*> frames_;
    bool visible_ = true;
    qreal opacity_ = 1;
    bool locked_ = false;
    Document* owner_;
    Layer* parent_;
    QPainter::CompositionMode blend_mode_ = QPainter::CompositionMode_SourceOver;

    friend class Document;
};

} // namespace document
#endif // PIXEL_CAYMAN_DOCUMENT_LAYER_HPP
