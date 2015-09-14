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

#include "io.hpp"
#include "misc/composition_mode.hpp"

#include <QImageReader>
#include <QImageWriter>
#include <QFileInfo>
#include <QMimeDatabase>

namespace document {

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
    QString format = formats().format("mela")->setting<QString>("image_format", "image/png", &document);
    QMimeDatabase mime;
    image_format = QMimeDatabase().mimeTypeForName(format);

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

void SaverXml::leave(Document& document)
{
    writer.writeEndElement();
}

bool SaverXml::enter(Layer& layer)
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

bool AbstractFormat::save(document::Document* document, const QString& filename)
{
    if ( filename.isEmpty() )
        return false;

    QFile file(filename);

    if ( !file.open(QFile::WriteOnly) )
        return false;

    return save(document, &file);
}

bool AbstractFormat::save(document::Document* document)
{
    return save(document, document->fileName());
}

Document* AbstractFormat::open(const QString& filename)
{
    if ( filename.isEmpty() )
        return nullptr;

    QFile file(filename);

    if ( !file.open(QFile::ReadOnly) )
        return nullptr;

    return open(&file);
}

bool FormatBitmap::save(Document* input, QIODevice* device)
{
    QImage image(input->imageSize(), imageFormat(input, device));
    image.fill(Qt::transparent);
    QPainter painter(&image);
    painter.fillRect(image.rect(), fillColor(input, device));
    /// \todo detect frame (and fullAlpha?) from settings
    document::visitor::Paint paint(nullptr, &painter, true);
    input->apply(paint);

    return saveImage(image, device);
}

Document* FormatBitmap::open(QIODevice* device)
{
    QImage image = openImage(device);
    if ( image.isNull() )
        return nullptr;
    return new Document(image, fileName(device));
}

QString FormatBitmap::id() const
{
    return "bitmap";
}

QString FormatBitmap::name() const
{
    return QObject::tr("All Bitmap Images");
}

QStringList FormatBitmap::extensions(Action action) const
{
    QByteArrayList bytearr = action == Action::Save ?
        QImageWriter::supportedImageFormats() :
        QImageReader::supportedImageFormats();
    QStringList formats;
    formats.reserve(bytearr.size());
    for ( const auto& fmt : bytearr )
        formats.push_back(fmt);
    return formats;
}

QImage::Format FormatBitmap::imageFormat(const document::Document* input, const QIODevice* device) const
{
    return QImage::Format_ARGB32;
}

QColor FormatBitmap::fillColor(const document::Document* input, const QIODevice* device) const
{
    /// \todo if the format doesn't support alpha, read a color from the settings
    return Qt::transparent;
}

bool FormatBitmap::saveImage(const QImage& image, QIODevice* device)
{
    QImageWriter writer(device, QByteArray());

    /// \todo some way to determine quality for jpg
    /// (low priority since Jpeg isn't a good format for pixel art)

    return writer.write(image);
}

QImage FormatBitmap::openImage(QIODevice* device)
{
    QImageReader reader(device);
    return reader.read();
}

Formats::~Formats()
{
    for ( auto format : formats_ )
        delete format;
}

bool Formats::addFormat(AbstractFormat* format)
{
    for ( auto fmt : formats_ )
        if ( fmt->id() == format->id() )
        {
            delete format;
            return false;
        }
    formats_.push_back(format);
    return true;
}

bool Formats::deleteFormat(AbstractFormat* format)
{
    for ( auto it = formats_.begin(); it != formats_.end(); ++it )
        if ( *it == format )
        {
            delete *it;
            formats_.erase(it);
            return true;
        }
    return false;
}

AbstractFormat* Formats::format(const QString& id) const
{
    for ( auto format : formats_ )
        if ( id == format->id() )
            return format;
    return nullptr;
}

QList<AbstractFormat*> Formats::formats() const
{
    return formats_;
}

AbstractFormat* Formats::formatFromFileName(const QString& file, Formats::Action action) const
{
    QString ext = QFileInfo(file).suffix().toLower();
    for ( auto format : formats_ )
        if ( format->extensions(action).contains(ext) )
            return format;
    return nullptr;
}

QStringList Formats::nameFilters(Formats::Action action, bool all_files) const
{
    QStringList filters;

    for ( auto* format : formats_ )
        if ( format->supportsAction(action) )
            filters << format->nameFilter(action);

    if ( all_files )
        filters << QObject::tr("All Files (*)");

    return filters;
}

AbstractFormat* Formats::formatFromNameFilter(const QString& filter, Formats::Action action) const
{
    for ( auto* format : formats_ )
        if ( format->supportsAction(action) && format->nameFilter(action) == filter )
            return format;
    return nullptr;
}

} // namespace document
