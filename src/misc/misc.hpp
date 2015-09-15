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
#ifndef PIXEL_CAYMAN_MISC_HPP
#define PIXEL_CAYMAN_MISC_HPP

#include <QFile>

namespace misc {

/**
 * \brief Extract a file name from a i/o device
 */
inline QString fileName(const QIODevice* device, const QString& def = QString())
{
    if ( const QFile* file = qobject_cast<const QFile*>(device) )
        return file->fileName();
    return def;
}

} // namespace misc
#endif // PIXEL_CAYMAN_MISC_HPP
