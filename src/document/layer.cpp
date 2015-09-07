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

namespace document {

Layer::Layer(class Document* owner, const QString& name, Layer* parentLayer)
    : name_(name), owner_(owner), parent_(parentLayer)
{
    owner->registerElement(this);
    if ( parent_ )
        parent_->insertChild(this, -1);
}

Layer::~Layer()
{
    for ( auto frame : frames_ )
        delete frame;
    for ( auto child : children_ )
        delete child;
}

QList<const Layer*> Layer::children() const
{
    return reinterpret_cast<const QList<const Layer*>&>(children_);
}

QList<Layer*> Layer::children()
{
    return children_;
}

void Layer::insertChild(Layer* layer, int index)
{
    if ( layer->owner_ != owner_ )
        owner_->registerElement(layer);

    layer->setParent(this);
    layer->parent_ = this;

    children_.insert(index < 0 ? children_.size() : index, layer);
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
    name_ = name;
}

qreal Layer::opacity() const
{
    return opacity_;
}

void Layer::setOpacity(qreal opacity)
{
    opacity_ = opacity;
}

bool Layer::visible() const
{
    return visible_;
}

void Layer::setVisible(bool visible)
{
    visible_ = visible;
}

bool Layer::locked() const
{
    return locked_;
}

void Layer::setLocked(bool locked)
{
    locked_ = locked;
}

QList<Image*> Layer::frameImages()
{
    return frames_;
}

QList<const Image*> Layer::frameImages() const
{
    return reinterpret_cast<const QList<const Image*>&>(frames_);;
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
        for ( Layer* child : children_ )
            child->apply(visitor);

        for ( Image* img : frames_ )
            img->apply(visitor);

        visitor.leave(*this);
    }
}

Document* Layer::parentDocument() const
{
    return owner_;
}

Layer* Layer::child(int index)
{
    return index < 0 || index >= children_.size() ? nullptr : children_[index];
}

} // namespace document
