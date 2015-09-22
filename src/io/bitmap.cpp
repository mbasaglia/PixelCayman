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

#include "bitmap.hpp"

#include <QImageReader>
#include <QImageWriter>

namespace io {

bool FormatBitmap::onSave(document::Document* input, QIODevice* device)
{
    QImage image(input->imageSize(), imageFormat(input, device));
    image.fill(fillColor(input, device));
    QPainter painter(&image);
    /// \todo detect frame
    document::visitor::Paint paint(nullptr, &painter, setting("full_alpha", input, true));
    input->apply(paint);

    return saveImage(image, device, input);
}

document::Document* FormatBitmap::onOpen(QIODevice* device)
{
    QImage image = openImage(device);
    if ( image.isNull() )
        return nullptr;
    auto doc = new document::Document(image, fileName(device));
    imageOpened(doc);
    return doc;
}

void FormatBitmap::imageOpened(document::Document* document)
{

}

QByteArray FormatBitmap::physicalFormat() const
{
    return QByteArray();
}

QString FormatBitmap::id() const
{
    return "bitmap";
}

QString FormatBitmap::name() const
{
    return tr("All Bitmap Images");
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

bool FormatBitmap::saveImage(const QImage& image, QIODevice* device, const document::Document* document)
{
    QImageWriter writer(device, physicalFormat());

    bool ok = writer.write(image);
    if ( !ok )
        setError(writer.errorString());
    return ok;
}

QImage FormatBitmap::openImage(QIODevice* device)
{
    QImageReader reader(device, physicalFormat());
    QImage img = reader.read();
    if ( img.isNull() )
        setError(reader.errorString());
    return img;
}

} // namespace io
