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
    /// \todo Filter out images not in the document
    writer.writeStartElement("image");

    /// \todo Have a proper id system
    if ( image.frame() )
        writer.writeAttribute("frame",QString("frame_%1").arg(qintptr(image.frame())));

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
    /// \todo Have a proper id system
    writer.writeAttribute("id",QString("frame_%1").arg(qintptr(&frame)));
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

} // namespace visitor

} // namespace document
