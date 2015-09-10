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

#include "layer_container.hpp"
#include "document.hpp"
#include "command/move_child_layers.hpp"

namespace document {

LayerContainer::LayerContainer(const Metadata& metadata)
    : DocumentElement(metadata)
{
    connect(this, &Layer::layersChanged, this, &DocumentElement::edited);
}

void LayerContainer::apply(Visitor& visitor)
{
    for ( auto layer : layers_ )
        layer->apply(visitor);
}

void LayerContainer::insertLayer(document::Layer* layer, int index)
{
    auto layers_copy = layers_;

    insertLayerRaw(layer, index);

    parentDocument()->pushCommand(new command::MoveChildLayers(
        tr("Add Layer"), this, &layers_, layers_copy, layers_
    ));
}

void LayerContainer::insertLayerRaw(Layer* layer, int index)
{
    if ( layer->parentDocument() != parentDocument() )
        parentDocument()->registerElement(layer);

    onInsert(layer);

    if ( index < 0 || index >= layers_.size() )
        layers_.append(layer);
    else
        layers_.insert(index, layer);

    connect(layer, &LayerContainer::layersChanged, this, &LayerContainer::layersChanged);

    emit layersChanged();
}

Layer* LayerContainer::layer(int index)
{
    return index < 0 || index >= layers_.size() ? nullptr : layers_[index];
}

} // namespace document
