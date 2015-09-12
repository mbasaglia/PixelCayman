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

#include "composition_mode.hpp"

namespace misc {

const QVector<QPair<CompositionMode, QString>> composition_names = {
    {QPainter::CompositionMode_SourceOver, "SourceOver"},
    {QPainter::CompositionMode_DestinationOver, "DestinationOver"},
    {QPainter::CompositionMode_Clear, "Clear"},
    {QPainter::CompositionMode_Source, "Source"},
    {QPainter::CompositionMode_Destination, "Destination"},
    {QPainter::CompositionMode_SourceIn, "SourceIn"},
    {QPainter::CompositionMode_DestinationIn, "DestinationIn"},
    {QPainter::CompositionMode_SourceOut, "SourceOut"},
    {QPainter::CompositionMode_DestinationOut, "DestinationOut"},
    {QPainter::CompositionMode_SourceAtop, "SourceAtop"},
    {QPainter::CompositionMode_DestinationAtop, "DestinationAtop"},
    {QPainter::CompositionMode_Xor, "Xor"},

    //svg 1.2 blend modes
    {QPainter::CompositionMode_Plus, "Plus"},
    {QPainter::CompositionMode_Multiply, "Multiply"},
    {QPainter::CompositionMode_Screen, "Screen"},
    {QPainter::CompositionMode_Overlay, "Overlay"},
    {QPainter::CompositionMode_Darken, "Darken"},
    {QPainter::CompositionMode_Lighten, "Lighten"},
    {QPainter::CompositionMode_ColorDodge, "ColorDodge"},
    {QPainter::CompositionMode_ColorBurn, "ColorBurn"},
    {QPainter::CompositionMode_HardLight, "HardLight"},
    {QPainter::CompositionMode_SoftLight, "SoftLight"},
    {QPainter::CompositionMode_Difference, "Difference"},
    {QPainter::CompositionMode_Exclusion, "Exclusion"},

    // ROPs
    {QPainter::RasterOp_SourceOrDestination, "SourceOrDestination"},
    {QPainter::RasterOp_SourceAndDestination, "SourceAndDestination"},
    {QPainter::RasterOp_SourceXorDestination, "SourceXorDestination"},
    {QPainter::RasterOp_NotSourceAndNotDestination, "NotSourceAndNotDestination"},
    {QPainter::RasterOp_NotSourceOrNotDestination, "NotSourceOrNotDestination"},
    {QPainter::RasterOp_NotSourceXorDestination, "NotSourceXorDestination"},
    {QPainter::RasterOp_NotSource, "NotSource"},
    {QPainter::RasterOp_NotSourceAndDestination, "NotSourceAndDestination"},
    {QPainter::RasterOp_SourceAndNotDestination, "SourceAndNotDestination"},
    {QPainter::RasterOp_NotSourceOrDestination, "NotSourceOrDestination"},
    {QPainter::RasterOp_SourceOrNotDestination, "SourceOrNotDestination"},
    {QPainter::RasterOp_ClearDestination, "ClearDestination"},
    {QPainter::RasterOp_SetDestination, "SetDestination"},
    {QPainter::RasterOp_NotDestination, "NotDestination"}
};

QString composition_to_string(CompositionMode mode)
{
    for ( const auto& pair : composition_names )
        if ( pair.first == mode )
            return pair.second;
    return {};
}

CompositionMode composition_from_string(const QString& name)
{
    for ( const auto& pair : composition_names )
        if ( pair.second == name )
            return pair.first;

    return CompositionMode(0);
}

} // namespace misc
