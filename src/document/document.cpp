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
#include <QFileInfo>

namespace document {

Document::Document(const QSize& size,
                   const QString& file_name,
                   const Metadata& metadata)
    : DocumentElement(metadata),
      image_size(size),
      file_name(file_name)
{
    Layer* layer = new Layer(this, QObject::tr("Layer"));
    layer->addFrameImage();
    insertLayer(layer);
    
    connect(this, &Document::layersChanged, this, &DocumentElement::edited);
}

Document::Document(const QImage& image, const QString& file_name)
    : image_size(image.size()),
      file_name(file_name)
{
    /// \todo Read exif metadata (maybe with Exiv2?)
    Layer* layer = new Layer(this, QFileInfo(file_name).baseName());
    layer->addFrameImage(image);
    insertLayer(layer);

    connect(this, &Document::layersChanged, this, &DocumentElement::edited);
}

Document::~Document()
{
    // layers/animations are cleaned uoìp by QObject
}

QString Document::fileName() const
{
    return file_name;
}

void Document::setFileName(const QString& file_name)
{
    if ( this->file_name != file_name )
        emit fileNameChanged( this->file_name = file_name );
}

QSize Document::imageSize() const
{
    return image_size;
}

void Document::apply(Visitor& visitor)
{
    if ( visitor.enter(*this) )
    {
        for ( auto layer : layers_ )
            layer->apply(visitor);

        for ( auto anim : animations_ )
            anim->apply(visitor);

        visitor.leave(*this);
    }
}

Document* Document::parentDocument() const
{
    return const_cast<Document*>(this);
}

QList<Layer*> Document::layers()
{
    return layers_;
}

void Document::insertLayer(document::Layer* layer, int index)
{
    if ( layer->owner_ != this )
    {
        registerElement(layer);
        layer->owner_ = this;
    }

    if ( index < 0 || index >= layers_.size() )
        layers_.append(layer);
    else
        layers_.insert(index, layer);
    connect(layer, &Layer::layersChanged, this, &Document::layersChanged);

    emit layersChanged();
}

void Document::registerElement(DocumentElement* element, const QMetaObject& meta)
{
    if ( element->parent() && element->parent() != this )
        throw std::logic_error("Document::registerElement: Registering an element belonging to an extraneous object");

    element->setParent(this);
    connect(element, &DocumentElement::edited, this, &DocumentElement::edited);

    if ( element->objectName().isEmpty() )
    {
        QString classname = meta.className();

        int colon = classname.lastIndexOf(':');
        if ( colon != -1 )
            classname.remove(0, colon+1);

        element->setObjectName(classname.toLower()
            +"_"+QString::number(quintptr(element)));
    }
}


const QUndoStack& Document::undoStack() const
{
    return undo_stack;
}

QUndoStack& Document::undoStack()
{
    return undo_stack;
}

void Document::pushCommand(QUndoCommand* command)
{
    undo_stack.push(command);
}

} // namespace document
