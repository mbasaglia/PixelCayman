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
 *
 */

#include "document.hpp"
#include "visitor.hpp"
#include <QFileInfo>

namespace document {


Document::Document(const Metadata& metadata)
    : LayerContainer(metadata)
{
    auto lambda = [this]
    {
        color_table = palette_.colorTable();
        emit paletteChanged(palette_);
    };
    connect(&palette_, &color_widgets::ColorPalette::colorsChanged, this, lambda);
    connect(&palette_, &color_widgets::ColorPalette::colorsUpdated, this, lambda);
}

Document::Document(const QSize& size,
                   const QString& file_name,
                   const QColor& background,
                   const Metadata& metadata)
    : Document(metadata)
{
    image_size = size;
    this->file_name = file_name;
    Layer* layer = new Layer(this, tr("Layer"));
    layer->setBackgroundColor(background);
    layer->addFrameImage();
    insertLayerRaw(layer, -1);
}

Document::Document(const QImage& image, const QString& file_name)
    : Document()
{
    image_size = image.size();
    this->file_name = file_name;
    /// \todo If \p image has a color table, initialize the palette
    /// \todo Read exif metadata (maybe with Exiv2?)
    Layer* layer = new Layer(this, QFileInfo(file_name).baseName());
    layer->addFrameImage(image);
    insertLayerRaw(layer, -1);
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
        LayerContainer::apply(visitor);

        for ( auto anim : animations_ )
            anim->apply(visitor);

        visitor.leave(*this);
    }
}

Document* Document::parentDocument() const
{
    return const_cast<Document*>(this);
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

    element->parentDocumentSet(this);
}

void Document::stealElement(DocumentElement* element)
{
    auto owner = element->parent();

    if ( owner == this )
        return;

    if ( owner )
    {
        disconnect(element, nullptr, owner, nullptr);
        element->setParent(nullptr);
    }

    registerElement(element);
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

void Document::onInsertLayer(Layer* layer)
{
    if ( layer->owner_ != this )
        layer->owner_ = this;
}

void Document::onRemoveLayer(Layer* layer)
{
}

void Document::setImageSize(const QSize& size)
{
    image_size = size;
}

const color_widgets::ColorPalette& Document::palette() const
{
    return palette_;
}

color_widgets::ColorPalette& Document::palette()
{
    return palette_;
}

bool Document::indexedColors() const
{
    return indexed_colors;
}

void Document::setIndexedColors(bool uses_palette)
{
    if ( uses_palette != indexed_colors )
    {
        emit indexedColorsChanged(indexed_colors = uses_palette);
        emit paletteChanged(palette_);
    }
}

} // namespace document
