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

#include "io.hpp"
namespace document {

bool save_xml(Document& doc)
{
    if ( doc.fileName().isEmpty() )
        return false;

    QFile file(doc.fileName());

    if ( !file.open(QFile::WriteOnly) )
        return false;

    save_xml(doc, &file);

    return true;
}

namespace visitor {

SaverXml::SaverXml(QIODevice* output)
    : writer(output)
{
    writer.setAutoFormatting(true);
    writer.writeStartDocument();
}

SaverXml::~SaverXml()
{
    writer.writeEndDocument();
}

bool SaverXml::enter(Document& document)
{
    writer.writeStartElement("document");
    writeId(document);
    writer.writeAttribute("width", QString::number(document.imageSize().width()));
    writer.writeAttribute("height", QString::number(document.imageSize().height()));
    writeMetadata(document.metadata());
    return true;
}

void SaverXml::leave(Document& document)
{
    writer.writeEndElement();
}

bool SaverXml::enter(Layer& layer)
{
    writer.writeStartElement("layer");
    writeId(layer);
    writer.writeAttribute("name",layer.name());
    writer.writeAttribute("opacity",QString::number(layer.opacity()));
    writer.writeAttribute("visible",QString::number(layer.visible()));
    writer.writeAttribute("locked",QString::number(layer.locked()));
    writeMetadata(layer.metadata());
    return true;
}

void SaverXml::leave(Layer& layer)
{
    writer.writeEndElement();
}

void SaverXml::visit(Image& image)
{
    writer.writeStartElement("image");
    writeId(image);

    if ( image.frame() )
        writeId(*image.frame(), "frame");

    if ( !image.metadata().empty() )
    {
        writeMetadata(image.metadata());
        writer.writeStartElement("bitmap");
    }

    /// \todo Option to choose the format
    writer.writeAttribute("type", "image/png");
    QByteArray image_data;
    QBuffer buffer(&image_data);
    image.image().save(&buffer, "PNG");
    writer.writeCharacters(image_data.toBase64());

    if ( !image.metadata().empty() )
    {
        writer.writeEndElement();
    }

    writer.writeEndElement();
}

bool SaverXml::enter(Animation& animation)
{
    writer.writeStartElement("animation");
    writeId(animation);
    writer.writeAttribute("name",animation.name());
    writer.writeAttribute("fps",QString::number(animation.framesPerSecond()));
    writeMetadata(animation.metadata());
    return true;
}

void SaverXml::leave(Animation& animation)
{
    writer.writeEndElement();
}

void SaverXml::visit(Frame& frame)
{
    writer.writeStartElement("frame");
    writeId(frame);
    writeMetadata(frame.metadata());
    writer.writeEndElement();
}

void SaverXml::writeMetadata(const Metadata& data)
{
    if ( data.empty() )
        return;

    writer.writeStartElement("metadata");

    for ( auto it = data.begin(), end = data.end(); it != end; ++it )
    {
        writer.writeStartElement("entry");
        writer.writeAttribute("name", it.key());
        writer.writeCharacters(it.value());
    }

    writer.writeEndElement();
}

void SaverXml::writeId(const DocumentElement& element, const QString& attr)
{
    if ( !element.objectName().isEmpty() && !attr.isEmpty() )
        writer.writeAttribute(attr, element.objectName());
}

} // namespace visitor

} // namespace document
