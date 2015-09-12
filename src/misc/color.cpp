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

#include "color.hpp"

namespace misc {
namespace color {

int to4bit(const QColor& c) noexcept
{
    int color = 0;

    float cmax = std::max({c.red(),c.green(),c.blue()});
    float cmin = std::min({c.red(),c.green(),c.blue()});
    float delta = cmax-cmin;

    if ( delta > 0 )
    {
        float hue = 0;
        if ( c.red() == cmax )
            hue = (c.green()-c.blue())/delta;
        else if ( c.green() == cmax )
            hue = (c.blue()-c.red())/delta + 2;
        else if ( c.blue() == cmax )
            hue = (c.red()-c.green())/delta + 4;

        float sat = delta / cmax;
        if ( sat >= 0.3 )
        {
            if ( hue < 0 )
                hue += 6;

            if ( hue <= 0.5 )      color = 1; // red
            else if ( hue <= 1.5 ) color = 3; // yellow
            else if ( hue <= 2.5 ) color = 2; // green
            else if ( hue <= 3.5 ) color = 6; // cyan
            else if ( hue <= 4.5 ) color = 4; // blue
            else if ( hue <= 5.5 ) color = 5; // magenta
            else                   color = 1; // red
        }
        else if ( cmax >= 0.5 )
            color = 7;

        if ( ( cmax + cmin ) / 2 >= 0.64 )
            color |= 8; // bright
    }
    else
    {
        if ( cmax > 0.8 )
            color = 15; // white
        else if ( cmax > 0.53 )
            color = 7; // silver
        else if ( cmax > 0.27 )
            color = 8; // gray
        else
            color = 0; // black
    }

    return color;
}

} // namespace color
} // namespace misc
