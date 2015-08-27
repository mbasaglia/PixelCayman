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
#include "document_layer.hpp"
#include <QPainter>

DocumentLayer::DocumentLayer(LayerMetadata* metadata, const QImage& image)
    : metadata_(metadata), image_(image)
{
}

DocumentLayer::DocumentLayer(LayerMetadata* metadata, const QSize& size, const QColor& color)
    : metadata_(metadata), image_(size, QImage::Format_ARGB32)
{
    image_.fill(color);
}

const QImage& DocumentLayer::image() const
{
    return image_;
}

void DocumentLayer::setImage(const QImage& image)
{
    image_ = image;
}

const LayerMetadata* DocumentLayer::metadata() const
{
    return metadata_;
}

LayerMetadata* DocumentLayer::metadata()
{
    return metadata_;
}

QSize DocumentLayer::size() const
{
    return image_.size();
}

void DocumentLayer::paint(QPainter& painter) const
{
    if ( !metadata_->visible )
        return;
    qreal opacity = painter.opacity();
    painter.setOpacity(metadata_->opacity);
    paintFullAlpha(painter);
    painter.setOpacity(opacity);
}

void DocumentLayer::paintFullAlpha(QPainter& painter) const
{
    painter.drawImage(0,0,image_);
}

