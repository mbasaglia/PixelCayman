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
#include "xml.hpp"

#include <QMimeDatabase>
#include <QImageWriter>
#include <QImageReader>

#include "misc/composition_mode.hpp"

static QMimeType mimeType(const QString &name, const QString& fallback)
{
    QMimeDatabase mime;
    QMimeType type;
    if ( !name.isEmpty() )
        type = mime.mimeTypeForName(name);
    if ( !type.isValid() && fallback != name )
        type = mime.mimeTypeForName(fallback);
    return type;
}

namespace io {

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

bool SaverXml::enter(document::Document& document)
{
    QString format = formats().format("mela")->setting<QString>("image_format", &document);
    image_format = mimeType(format, "image/png");

    writer.writeStartElement("document");
    writeId(document);
    writer.writeAttribute("width", QString::number(document.imageSize().width()));
    writer.writeAttribute("height", QString::number(document.imageSize().height()));
    writeMetadata(document.metadata());
    const auto& settings = document.formatSettings().settings();
    if ( !settings.empty() )
    {
        writer.writeStartElement("formats");
        for ( auto i = settings.begin(); i != settings.end(); ++i )
        {
            QString element = i.key() ? i.key()->id() : "default";
            writer.writeStartElement(element);
            for ( auto j = i->begin(); j != i->end(); ++j )
                writer.writeTextElement(j.key(), j.value().toString());
            writer.writeEndElement();
        }
        writer.writeEndElement();
    }
    return true;
}

void SaverXml::leave(document::Document& document)
{
    writer.writeEndElement();
}

bool SaverXml::enter(document::Layer& layer)
{
    writer.writeStartElement("layer");
    writeId(layer);
    writer.writeAttribute("name", layer.name());
    writer.writeAttribute("opacity", QString::number(layer.opacity()));
    writer.writeAttribute("visible", QString::number(layer.visible()));
    writer.writeAttribute("locked", QString::number(layer.locked()));
    writer.writeAttribute("blend", misc::composition_to_string(layer.blendMode()));
    writeMetadata(layer.metadata());
    return true;
}

void SaverXml::leave(document::Layer& layer)
{
    writer.writeEndElement();
}

void SaverXml::visit(document::Image& image)
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

    writer.writeAttribute("type", image_format.name());
    QByteArray image_data;
    QBuffer buffer(&image_data);
    QImageWriter image_writer(&buffer, image_format.preferredSuffix().toLatin1());
    image_writer.write(image.image());
    writer.writeCharacters(image_data.toBase64());

    if ( !image.metadata().empty() )
    {
        writer.writeEndElement();
    }

    writer.writeEndElement();
}

bool SaverXml::enter(document::Animation& animation)
{
    writer.writeStartElement("animation");
    writeId(animation);
    writer.writeAttribute("name",animation.name());
    writer.writeAttribute("fps",QString::number(animation.framesPerSecond()));
    writeMetadata(animation.metadata());
    return true;
}

void SaverXml::leave(document::Animation& animation)
{
    writer.writeEndElement();
}

void SaverXml::visit(document::Frame& frame)
{
    writer.writeStartElement("frame");
    writeId(frame);
    writeMetadata(frame.metadata());
    writer.writeEndElement();
}

void SaverXml::writeMetadata(const document::Metadata& data)
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

void SaverXml::writeId(const document::DocumentElement& element, const QString& attr)
{
    if ( !element.objectName().isEmpty() && !attr.isEmpty() )
        writer.writeAttribute(attr, element.objectName());
}

void LoaderXml::root()
{
    auto doc = requireElement(xml,"document");
    builder.beginDocument();
    id(doc);
    document_ = builder.currentDocument();
    /// \todo file format version
    document_->setImageSize(QSize(
        doc.attribute("width","32").toInt(),
        doc.attribute("height","32").toInt()
                            ));
    metadata(doc.firstChildElement("metadata"));
    animations(doc.firstChildElement("animations"));
    formats(doc.firstChildElement("formats"));
    layers(doc);
    builder.endDocument();
}

void LoaderXml::metadata(const QDomElement& node)
{
    if ( node.isNull() ) return;

    auto& meta = builder.currentElement()->metadata();
    for ( auto data = node.firstChildElement("entry"); !data.isNull();
            data = data.nextSiblingElement("entry") )
    {
        if ( data.hasAttribute("name") )
            meta[data.attribute("name")] = data.text();
    }
}

void LoaderXml::layers(const QDomElement& node)
{
    for ( auto lay = node.firstChildElement("layer"); !lay.isNull();
            lay = lay.nextSiblingElement("layer") )
    {
        layer(lay);
    }
}

void LoaderXml::layer(const QDomElement& node)
{
    document::Layer* layer = builder.beginLayer();
    id(node);
    layer->setName(node.attribute("name", QObject::tr("Layer")));
    layer->setOpacity(node.attribute("opacity", "1").toDouble());
    layer->setVisible(node.attribute("visible", "1").toInt());
    layer->setLocked(node.attribute("locked", "0").toInt());
    layer->setBlendMode(misc::composition_from_string(node.attribute("blend")));

    metadata(node);

    for ( auto img = node.firstChildElement("image"); !img.isNull();
            img = img.nextSiblingElement("image") )
    {
        image(img);
    }

    layers(node);
    builder.endLayer();
}

void LoaderXml::image(const QDomElement& node)
{
    builder.beginImage();
    id(node);
    builder.setImageFrame(node.attribute("frame"));
    metadata(node);
    auto bitmap_node = node.firstChildElement("bitmap");
    if ( bitmap_node.isNull() )
        bitmap(node);
    else
        bitmap(bitmap_node);
    builder.endImage();
}

void LoaderXml::bitmap(const QDomElement& node)
{
    auto content_type = mimeType(node.attribute("type"), "image/png");
    QByteArray image_data = QByteArray::fromBase64(node.text().toLatin1());
    QBuffer buffer(&image_data);
    QImageReader reader(&buffer, content_type.preferredSuffix().toUtf8());
    reader.read(&builder.currentImage()->image());
}

void LoaderXml::id(const QDomElement& node)
{
    if ( node.hasAttribute("id") )
        builder.currentElement()->setObjectName(node.attribute("id"));
}

} // namespace document
