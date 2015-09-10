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

#include "image.hpp"
#include "visitor.hpp"
#include <QPainter>

namespace document {

Image::Image(Layer* layer, const QImage& image,  Frame* frame)
    : image_(image), frame_(frame), layer_(layer)
{
    layer->parentDocument()->registerElement(this);
}

Image::Image(Layer* layer, const QSize& size, const QColor& color,  Frame* frame)
    : image_(size, QImage::Format_ARGB32), frame_(frame), layer_(layer)
{
    image_.fill(color);
    layer->parentDocument()->registerElement(this);
}

Image::~Image()
{
    endPainting();
}

void Image::apply(Visitor& visitor)
{
    visitor.visit(*this);
}

QImage& Image::image()
{
    return image_;
}

const QImage& Image::image() const
{
    return image_;
}

void Image::paint(QPainter& painter, qreal opacity) const
{
    qreal old_op = painter.opacity();
    painter.setOpacity(opacity);
    painter.drawImage(0, 0, image_);
    painter.setOpacity(old_op);
}

void Image::paint(QPainter& painter) const
{
    painter.drawImage(0, 0, image_);
}

const Frame* Image::frame() const
{
    return frame_;
}

Frame* Image::frame()
{
    return frame_;
}

void Image::setFrame(Frame* frame)
{
    frame_ = frame;
}

const Layer* Image::layer() const
{
    return layer_;
}

Layer* Image::layer()
{
    return layer_;
}

Document* Image::parentDocument() const
{
    return layer_->parentDocument();
}

void Image::beginPainting(const QString& text)
{
    command_ = new command::ChangeImage(text, this, image_);
}

void Image::endPainting()
{
    if ( command_ )
    {
        command_->setAfterImage(image_);
        parentDocument()->pushCommand(command_);
        command_ = nullptr;
    }
}

} // namespace document

