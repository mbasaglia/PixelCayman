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
#include "ansi.hpp"

namespace ansi {

QList<int> colorRgbCodes(int init, QColor color)
{
    return {init, 2, color.red(), color.green(), color.blue()};
}

QString color(QColor color, int alpha_threshold,
              bool foreground, bool background, ColorMode mode)
{
    if ( !foreground && !background )
        return "";

    QList<int> codes;
    int bg = 40;
    int fg = 30;
    int color_int = 9;

    if ( color.alpha() > alpha_threshold )
    {
        if ( mode == ColorMode::Rgb )
        {
            if ( background )
                codes += colorRgbCodes(48, color);
            if ( foreground )
                codes += colorRgbCodes(38, color);
            return sgr(codes);
        }

        color_int = misc::color::to4bit(color);

        if ( mode == ColorMode::Standard )
        {
            if ( color_int & 8 )
                codes.push_back(1);
            else
                codes.push_back(22);
        }
        else if ( color_int & 8 )
        {
            bg = 100;
            fg = 90;
        }

        color_int &= ~8;
    }

    if ( background )
        codes.push_back(bg+color_int);
    if ( foreground )
        codes.push_back(fg+color_int);

    return sgr(codes);
}

} // namespace ansi
