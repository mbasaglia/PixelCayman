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
#ifndef PIXEL_CAYMAN_MISC_COMPOSITION_MODE_HPP
#define PIXEL_CAYMAN_MISC_COMPOSITION_MODE_HPP

#include <QPainter>
#include <QPair>
#include <QVector>
#include <QString>

namespace misc {

using CompositionMode = QPainter::CompositionMode;

QString composition_to_string(CompositionMode mode);
CompositionMode composition_from_string(const QString& name);
extern const QVector<QPair<CompositionMode, QString>> composition_names;

} // namespace misc
#endif // PIXEL_CAYMAN_MISC_COMPOSITION_MODE_HPP
