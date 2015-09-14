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
#include <QTextStream>

#include "plugin.hpp"
#include "document/io_bitmap.hpp"
#include "ansi.hpp"
#include "color_parser.hpp"
/**
 * \brief Handles ANSI-colored text files
 */
class FormatAnsi : public document::FormatBitmap
{
public:
    QString id() const override
    {
        return "ansi";
    }

    QString name() const override
    {
        return QObject::tr("Ansi text files");
    }

    QStringList extensions(Action action) const override
    {
        return {"ansi", "txt"};
    }

    QImage openImage(QIODevice* device) override
    {
        return ansi::ColorParser(device, ansi::ColorParser::Option::Foreground).image();
    }

    bool saveImage(const QImage& img, QIODevice* device,
                   const ::document::Document* document) override
    {
        /// \todo Read pixel string, newline and ANSI options from config
        QTextStream stream(device);
        device->setTextModeEnabled(true);
        QString pixel = "..";
        for ( int y = 0; y < img.height(); y++ )
        {
            for ( int x = 0; x < img.width(); x++ )
                stream << ansi::color(img.pixel(x, y)) << pixel;
            stream << '\n';
        }
        stream << ansi::sgrClear();
        return true;
    }

};

class Ansi : public CaymanPlugin
{
protected:
    QString onId() override
    {
        return "ansi";
    }

    bool onLoad() override
    {
        document::formats().addFormat(new FormatAnsi);
        return true;
    }

    void onUnload() override
    {
        document::formats().deleteFormat(document::formats().format("ansi"));
    }

    QString onName() override
    {
        return tr("Ansi I/O");
    }

};

INIT_PLUGIN(Ansi)
