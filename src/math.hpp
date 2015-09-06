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
#ifndef PIXEL_CAYMAN_MATH_HPP
#define PIXEL_CAYMAN_MATH_HPP
#include <QPoint>
#include <qmath.h>
#include <limits>

namespace math {

/**
 * \brief p-norm distance between two points
 * \param p1 First point
 * \param p2 Second point
 * \param p norm factor, non-positive values can be used for infinity
 */
inline qreal pdistance(QPointF p1, QPointF p2, qreal p)
{
    if ( p <= 0 || p == std::numeric_limits<qreal>::infinity() )
        return qMax(qAbs(p1.x() - p2.x()), qAbs(p1.y() - p2.y()));

    return qPow(
        qPow(qAbs(p1.x() - p2.x()), p) +
        qPow(qAbs(p1.y() - p2.y()), p),
    1/p);
}

} // namespace math
#endif // PIXEL_CAYMAN_MATH_HPP
