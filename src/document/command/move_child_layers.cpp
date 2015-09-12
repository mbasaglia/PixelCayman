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

#include "move_child_layers.hpp"
#include "document/layer_container.hpp"

namespace document {
namespace command {

AddLayer::AddLayer(
        const QString&  name,
        LayerContainer* parent,
        Layer*          layer,
        int             index,
        QUndoCommand*   parent_command
    ) : QUndoCommand(name, parent_command),
        parent(parent),
        layer(layer),
        index(index)
{}

void AddLayer::undo()
{
    parent->removeLayerRaw(layer);
}

void AddLayer::redo()
{
    parent->insertLayerRaw(layer, index);
}

} // namespace command
} // namespace document
