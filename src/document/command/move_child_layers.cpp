/**
 * \file
 *
 * \author Mattia Basaglia
 *
 * \section License
 *
 * Copyright (C) 2015 Mattia Basaglia
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

#include "move_child_layers.hpp"
#include "document/layer_container.hpp"

namespace document {
namespace command {

MoveChildLayers::MoveChildLayers(
        const QString&  name,
        LayerContainer* parent,
        QList<Layer*>*  target,
        QList<Layer*>   before,
        QList<Layer*>   after,
        QUndoCommand*   parent_command
    ) : QUndoCommand(name, parent_command),
        parent(parent),
        target(target),
        before(before),
        after(after)
{}

void MoveChildLayers::undo()
{
    *target = before;

    emit parent->layersChanged();
}

void MoveChildLayers::redo()
{
    *target = after;

    emit parent->layersChanged();
}

} // namespace command
} // namespace document
