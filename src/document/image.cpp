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

Image::Image(const QImage& image)
    : image_(image)
{}

Image::Image(const QSize& size, const QColor& color)
    : image_(size, QImage::Format_ARGB32)
{
    image_.fill(color);
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

} // namespace document

