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
#ifndef PIXEL_CAYMAN_IO_BITMAP_HPP
#define PIXEL_CAYMAN_IO_BITMAP_HPP

#include "io.hpp"

namespace document {

/**
 * \brief Reads and writes bitmap images with the Qt image reader/writer
 *
 * Can be used as base class for formats that need to render the file to a
 * bitmap before saving it to a file
 */
class FormatBitmap : public AbstractFormat
{
public:
    QString id() const override;
    QString name() const override;
    QStringList extensions(Action action) const override;
    bool canSave() const override { return true; }
    bool canOpen() const override { return true; }

protected:

    bool onSave(document::Document* input, QIODevice* device) final;
    Document* onOpen(QIODevice* device) final;

    /**
     * \brief Format used by for the QImage generated from the document
     */
    virtual QImage::Format imageFormat(const Document* input, const QIODevice* device) const;

    /**
     * \brief The color used to fill the QImage generated from the document
     */
    virtual QColor fillColor(const Document* input, const QIODevice* device) const;

    /**
     * \brief Saves the image to the device
     */
    virtual bool saveImage(const QImage& img, QIODevice* device, const Document* document);

    /**
     * \brief Opens an image from the device
     */
    virtual QImage openImage(QIODevice* device);

    /**
     * \brief Called after a document has been created
     */
    virtual void imageOpened(Document* document);

    /**
     * \brief Image writer/reader format
     */
    virtual QByteArray physicalFormat() const;
};

} // namespace document
#endif // PIXEL_CAYMAN_IO_BITMAP_HPP
