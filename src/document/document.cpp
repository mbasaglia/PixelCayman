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

#include "document.hpp"
#include "visitor.hpp"

namespace document {

Document::Document(const QSize& size,
                   const QString& file_name,
                   const Metadata& metadata)
    : DocumentElement(metadata),
      image_size(size),
      file_name(file_name)
{
    root = new Layer(this, QObject::tr("Layer"));
}

Document::~Document()
{
    for ( auto anim : animations_ )
        delete anim;
    delete root;
}

QString Document::filename() const
{
    return file_name;
}

void Document::setFilename(const QString& file_name)
{
    this->file_name = file_name;
}

QSize Document::imageSize() const
{
    return image_size;
}

const Layer* Document::rootLayer() const
{
    return root;
}

Layer* Document::rootLayer()
{
    return root;
}

void Document::apply(Visitor& visitor)
{
    if ( visitor.enter(*this) )
    {
        root->apply(visitor);
        for ( auto anim : animations_ )
            anim->apply(visitor);
        visitor.leave(*this);
    }
}

} // namespace document
