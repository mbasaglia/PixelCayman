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

#include "layer_tree.hpp"

using document::Layer;
using document::Document;

namespace model {

LayerTree::LayerTree(::document::Document* document)
    : document_(document)
{}

int LayerTree::columnCount(const QModelIndex& parent) const
{
    /// \todo More columns for info like opacity et all
    return 1;
}

QModelIndex LayerTree::index(int row, int column, const QModelIndex& parent) const
{
    if ( !document_ || !hasIndex(row, column, parent) )
        return QModelIndex();

    if ( parent.isValid() )
    {
        Layer* parent_layer = static_cast<Layer*>(parent.internalPointer());
        if ( Layer* layer = parent_layer->child(row) )
            return createIndex(row, column, layer);
    }
    else
    {
        return createIndex(row, column, document_->layers()[row]);
    }

    return QModelIndex();
}

QModelIndex LayerTree::parent(const QModelIndex& index) const
{
    if ( !index.isValid() || !document_ )
        return QModelIndex();

    Layer* layer = static_cast<Layer*>(index.internalPointer());
    Layer* parent = layer->parentLayer();

    if ( !parent )
        return QModelIndex();

    return createIndex(parent->children().indexOf(layer), 0, parent);
}

int LayerTree::rowCount(const QModelIndex& index) const
{
    if ( !document_ )
        return 0;

    if ( !index.isValid() )
        return document_->layers().size();

    Layer* layer = static_cast<Layer*>(index.internalPointer());
    return layer->children().size();
}

QVariant LayerTree::data(const QModelIndex& index, int role) const
{
    if ( !index.isValid() || !document_ )
        return QVariant();

    Layer* layer = static_cast<Layer*>(index.internalPointer());

    if ( role == Qt::DisplayRole || role == Qt::EditRole )
    {
        if ( index.column() == 0 )
        {
            return layer->name();
        }
    }

    return QVariant();
}

Qt::ItemFlags LayerTree::flags(const QModelIndex& index) const
{
    if ( !index.isValid() || !document_ )
        return 0;

    auto flags = QAbstractItemModel::flags(index);

    flags |= Qt::ItemIsEditable;

    if ( index.column() == 0 )
        flags |= Qt::ItemIsDragEnabled;

    return flags;
}

document::Document* LayerTree::document() const
{
    return document_;
}

void LayerTree::setDocument(::document::Document* document)
{
    if ( document == document_ )
        return;

    beginResetModel();
    document_ = document;
    endResetModel();
}

} // namespace model
