/**
 * \file
 *
 * \author Mattia Basaglia
 *
 * \section License
 *
 * Copyright (C) 2015 Mattia Basaglia
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
 *
 */

#include "layer.hpp"
#include "document.hpp"
#include "visitor.hpp"
#include "command/move_child_layers.hpp"

namespace document {

Layer::Layer(class Document* owner, const QString& name, Layer* parentLayer)
    : name_(name), owner_(owner), parent_(parentLayer)
{
    owner->registerElement(this);

    if ( parent_ )
        parent_->insertLayer(this, -1);

    connect(this, &Layer::opacityChanged, this, &DocumentElement::edited);
    connect(this, &Layer::visibleChanged, this, &DocumentElement::edited);
    connect(this, &Layer::blendModeChanged, this, &DocumentElement::edited);
}

Layer::~Layer()
{
    for ( auto frame : frames_ )
        delete frame;
}

void Layer::onInsertLayer(Layer* layer)
{
    layer->parent_ = this;
}

void Layer::onRemoveLayer(Layer* layer)
{
    layer->parent_ = nullptr;
}

Layer* Layer::parentLayer()
{
    return parent_;
}

QString Layer::name() const
{
    return name_;
}

void Layer::setName(const QString& name)
{
    if ( name_ != name )
        emit nameChanged( name_ = name );
}

qreal Layer::opacity() const
{
    return opacity_;
}

void Layer::setOpacity(qreal opacity)
{
    emit opacityChanged( opacity_ = opacity );
}

bool Layer::visible() const
{
    return visible_;
}

void Layer::setVisible(bool visible)
{
    if ( visible_ != visible )
        emit visibleChanged( visible_ = visible );
}

bool Layer::locked() const
{
    return locked_;
}

void Layer::setLocked(bool locked)
{
    if ( locked != locked_ )
        emit lockedChanged( locked_ = locked );
}

QList<Image*> Layer::frameImages()
{
    return frames_;
}

Image* Layer::addFrameImage()
{
    Image* image = new Image(this, owner_->imageSize());
    frames_.push_back(image);
    return image;
}

Image* Layer::addFrameImage(const QImage& qimage)
{
    Image* image = new Image(this, qimage);
    frames_.push_back(image);
    return image;
}

void Layer::apply(Visitor& visitor)
{
    if ( visitor.enter(*this) )
    {
        LayerContainer::apply(visitor);

        for ( Image* img : frames_ )
            img->apply(visitor);

        visitor.leave(*this);
    }
}

Document* Layer::parentDocument() const
{
    return owner_;
}

QPainter::CompositionMode Layer::blendMode() const
{
    return blend_mode_;
}

void Layer::setBlendMode(QPainter::CompositionMode blendMode)
{
    if ( blendMode != blend_mode_ )
        emit blendModeChanged( blend_mode_ = blendMode );
}

Image* Layer::frameImage(Frame* frame)
{
    for ( auto image : frames_ )
        if ( image->frame() == frame )
            return image;
    return nullptr;
}

} // namespace document
