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
#ifndef PIXEL_CAYMAN_MISC_COLOR_HPP
#define PIXEL_CAYMAN_MISC_COLOR_HPP

#include <QColor>

namespace misc {
namespace color {

/**
 * \brief Returns a 4 bit integer for the color.
 *
 * The bits follow this pattern:
 *  * 1 red
 *  * 2 green
 *  * 4 blue
 *  * 8 bright
 */
int to4bit(const QColor& c) noexcept;


} // namespace color
} // namespace misc
#endif // PIXEL_CAYMAN_MISC_COLOR_HPP
