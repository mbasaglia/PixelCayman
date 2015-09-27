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
#ifndef PIXEl_CAYMAN_PLUGIN_ANSI_HPP
#define PIXEl_CAYMAN_PLUGIN_ANSI_HPP
#include <QStringList>
#include "misc/color.hpp"
namespace ansi {

/**
 * \brief Control Sequence Introducer
 */
inline QString csi()
{
    return "\x1b[";
}

/**
 * \brief Generic CSI code
 */
inline QString code(const QStringList& parameters, QChar letter = 'm')
{
    return csi()+parameters.join(";")+letter;
}

/**
 * \brief Select Graphic Rendition code
 */
inline QString sgr(const QList<int>& parameters)
{
    QStringList string_parameters;
    string_parameters.reserve(parameters.size());
    for ( int p : parameters )
        string_parameters.push_back(QString::number(p));
    return code(string_parameters);
}

/**
 * \brief Resets all graphics to the default
 */
inline QString sgrClear()
{
    return code({"0"});
}

/**
 * \brief How color() shoall operate
 */
enum class ColorMode
{
    Standard, ///< Standard ANSI code (8 colors + bold)
    XTerm,    ///< Standard for dark colors, special codes for bright
    Rgb       ///< Full 24 bit color support
};

/**
 * \brief Creates a color code
 * \param color             Color to render
 * \param alpha_threshold   Minimum alpha below which the color will be considered fully transparent
 * \param foreground        Whether to set the foreground color
 * \param background        Whether to set the background color
 * \param mode              Standard compliance mode
 */
QString color(QColor color, int alpha_threshold = 32,
              bool foreground = true, bool background = true,
              ColorMode mode = ColorMode::Standard);

} // namespace ansi
#endif // PIXEl_CAYMAN_PLUGIN_ANSI_HPP
