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
#ifndef PIXEL_CAYMAN_DOCUMENT_VISITOR_GETHER_PALETTE_HPP
#define PIXEL_CAYMAN_DOCUMENT_VISITOR_GETHER_PALETTE_HPP

#include "document/visitor.hpp"
#include <QSet>

namespace document {
namespace visitor {

/**
 * \brief Creates a palette from the colors used in the document
 */
class GatherPalette : public Visitor
{
public:

    color_widgets::ColorPalette palette() const
    {
        return color_widgets::ColorPalette::fromColorTable(colors);
    }

    bool enter(Document& document) override
    {
        return true;
    }

    bool enter(Layer& layer) override
    {
        return true;
    }

    void visit(Image& image) override
    {
        if ( image.image().format() == QImage::Format_Indexed8 )
        {
            for ( auto color : image.image().colorTable() )
                insertColor(color);
            return;
        }
        // The temporary created by convertToFormat will have its lifetime
        // extended to the lifetime of img since it's a const reference
        const QImage& img = image.image().format() == QImage::Format_ARGB32 ?
            image.image() : image.image().convertToFormat(QImage::Format_ARGB32);
        const QRgb* data = reinterpret_cast<const QRgb*>(img.constBits());
        for ( int i = 0; i < img.byteCount() / sizeof(QRgb); i++ )
                colors.insert(data[i]);
    }

private:
    void insertColor(QRgb color)
    {
        auto it = std::lower_bound(colors.begin(), colors.end());
        if ( it == colors.end() || *it != color )
            colors.insert(it, color);
    }

    QVector<QRgb> colors;
};

} // namespace visitor
} // namespace document
#endif // PIXEL_CAYMAN_DOCUMENT_VISITOR_GETHER_PALETTE_HPP
