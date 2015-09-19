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
#ifndef PIXEL_CAYMAN_PLUGIN_BITMAPIO_FORMATS_HPP
#define PIXEL_CAYMAN_PLUGIN_BITMAPIO_FORMATS_HPP

#include <QImageWriter>
#include <QImageReader>
#include "io/bitmap.hpp"

namespace bitmapio {

/**
 * \brief Base class for formats that only specify a single bitmap format
 */
class SingleBitmapFormat : public io::FormatBitmap
{
public:
    QString id() const = 0;
    QString name() const = 0;
    bool canSave() const override { return supported(Action::Save); }
    bool canOpen() const override { return supported(Action::Open); }
    QStringList extensions(Action action) const override { return {id()}; }

protected:
    QByteArray physicalFormat() const override
    {
        return id().toUtf8();
    }

    bool supported(Action action) const
    {
        if ( action == Action::Save )
            return QImageWriter::supportedImageFormats().contains(physicalFormat());
        return QImageReader::supportedImageFormats().contains(physicalFormat());
    }
};

class FormatPng : public SingleBitmapFormat
{
public:
    QString id() const override
    {
        return "png";
    }

    QString name() const override
    {
        return QObject::tr("Portable Network Graphics images");
    }
};

class FormatBmp : public SingleBitmapFormat
{
public:
    QString id() const override
    {
        return "bmp";
    }

    QString name() const override
    {
        return QObject::tr("Windows Bitmap images");
    }
};

class FormatPpm : public SingleBitmapFormat
{
public:
    QString id() const override
    {
        return "ppm";
    }

    QString name() const override
    {
        return QObject::tr("Portable Pixmap images");
    }

    QStringList extensions(Action action) const override
    {
        return {"ppm", "pbm", "pgm"};
    }
};

class FormatXbm : public SingleBitmapFormat
{
public:
    QString id() const override
    {
        return "xbm";
    }

    QString name() const override
    {
        return QObject::tr("X BitMap images");
    }

    QStringList extensions(Action action) const override
    {
        return {"xbm", "icon", "bitmap"};
    }
};

class FormatXpm : public SingleBitmapFormat
{
public:
    QString id() const override
    {
        return "xpm";
    }

    QString name() const override
    {
        return QObject::tr("X PixMap images");
    }
};

class FormatTga : public SingleBitmapFormat
{
public:
    QString id() const override
    {
        return "tga";
    }
    QString name() const override
    {
        return QObject::tr("TarGA images");
    }
    QStringList extensions(Action action) const override
    {
        return {"tga", "vda", "icb", "vst"};
    }
};

class FormatTiff : public SingleBitmapFormat
{
public:
    QString id() const override
    {
        return "tiff";
    }

    QString name() const override
    {
        return QObject::tr("TIFF images");
    }

    QStringList extensions(Action action) const override
    {
        return {"tiff", "tif"};
    }
};

class FormatIco : public SingleBitmapFormat
{
public:
    QString id() const override
    {
        return "ico";
    }

    QString name() const override
    {
        return QObject::tr("Windows icons");
    }

    QStringList extensions(Action action) const override
    {
        return {"ico", "cur"};
    }
};

class FormatWebp : public SingleBitmapFormat
{
public:
    QString id() const override
    {
        return "webp";
    }

    QString name() const override
    {
        return QObject::tr("WebP images");
    }
};

class FormatEps : public SingleBitmapFormat
{
public:
    QString id() const override
    {
        return "eps";
    }

    QString name() const override
    {
        return QObject::tr("PostScript");
    }

    QStringList extensions(Action action) const override
    {
        return {"eps", "epsf", "epsi"};
    }
};


} // namespace bitmapio
#endif // PIXEL_CAYMAN_PLUGIN_BITMAPIO_FORMATS_HPP
