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
#ifndef PIXEL_CAYMAN_DRAW_HPP
#define PIXEL_CAYMAN_DRAW_HPP

#include <QLine>
#include <QImage>
#include <QRegion>

namespace misc {
namespace draw {

/**
 * \brief Draw a line (Bresenham's line algorithm)
 * \tparam Callback A function type that accepts a QPoint
 * \param line Line to rasterize
 * \param func Function called to draw each pixel
 */
template<class Callback>
    void line(const QLine& line, Callback&& func)
    {
        // Sign of delta x/y
        int sx = line.dx() > 0 ? 1 : -1;
        int sy = line.dy() > 0 ? 1 : -1;
        // Value that determines whether to move on X or Y
        int error = 0;
        // Absolute value of delta x/y
        int deltaerry = line.dy() * sy;
        int deltaerrx = line.dx() * sx;

        QPoint point = line.p1();

        while ( point != line.p2() )
        {
            func(point);

            error += deltaerry;
            while ( error >= deltaerrx / 2 && point.y() != line.y2() )
            {
                func(point);
                point.setY(point.y() + sy);
                error -= deltaerrx;
            }

            if ( point.x() != line.x2() )
                point.setX(point.x() + sx);
        }

        func(point);
    }

namespace detail {
/**
 * \brief Select the pixels in \p img near \p pt matching \p pred into \p output
 */
template<class Predicate>
    void floodFill(const QImage& img, const QPoint& pt, const Predicate& pred,
                   QRegion& output, std::vector<char>& map)
    {
        auto map_index = pt.y()*img.width()+pt.x();
        if ( !img.rect().contains(pt) || !pred(img.pixel(pt)) || map[map_index] )
            return;
        map[map_index] = true;
        output |= QRect(pt, QSize{1,1});
        floodFill(img, {pt.x()+1, pt.y()}, pred, output, map);
        floodFill(img, {pt.x()-1, pt.y()}, pred, output, map);
        floodFill(img, {pt.x(), pt.y()+1}, pred, output, map);
        floodFill(img, {pt.x(), pt.y()-1}, pred, output, map);
    }

} // namespace detail

/**
 * \brief Select the pixels in \p img near \p pt matching \p pred into \p output
 */
template<class Predicate>
    QRegion floodFill(const QImage& img, const QPoint& pt, const Predicate& pred)
    {
        QRegion region;
        std::vector<char> map(img.width()*img.height());
        detail::floodFill(img, pt, pred, region, map);
        return region;
    }

} // namespace draw
} // namespace misc
#endif // PIXEL_CAYMAN_DRAW_HPP
