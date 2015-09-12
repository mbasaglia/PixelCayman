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

#include "layer_tree.hpp"
#include <QMimeData>

using document::Layer;
using document::Document;
using document::LayerContainer;

static QString mime_type = "application/x-pixel-cayman-layer-row";

namespace model {

LayerTree::LayerTree(::document::Document* document)
    : document_(document)
{}

int LayerTree::columnCount(const QModelIndex& parent) const
{
    return ColumnCount;
}

LayerContainer* LayerTree::container(const QModelIndex& index) const
{
    if ( index.isValid() )
        return static_cast<Layer*>(index.internalPointer());
    return document_;
}

QModelIndex LayerTree::index(int row, int column, const QModelIndex& parent) const
{
    if ( !document_ || !hasIndex(row, column, parent) )
        return QModelIndex();

    LayerContainer* parent_container = container(parent);

    if ( Layer* layer = reverseLayer(parent_container, row) )
        return createIndex(row, column, layer);

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

    return createIndex(parent->layerIndex(layer), 0, parent);
}

int LayerTree::rowCount(const QModelIndex& index) const
{
    if ( !document_ )
        return 0;

    return container(index)->countLayers();
}

QVariant LayerTree::data(const QModelIndex& index, int role) const
{
    if ( !index.isValid() || !document_ )
        return QVariant();

    Layer* layer = static_cast<Layer*>(index.internalPointer());

    if ( role == Qt::DisplayRole || role == Qt::EditRole )
    {
        switch ( index.column() )
        {
            case Name:      return layer->name();
            case Visible:   return layer->visible();
            case Locked:    return layer->locked();
            case Opacity:   return layer->opacity();
            case BlendMode: return layer->blendMode();
        }
    }

    return QVariant();
}

bool LayerTree::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if ( !index.isValid() || !document_ )
        return false;

    Layer* layer = static_cast<Layer*>(index.internalPointer());

    if ( role == Qt::DisplayRole || role == Qt::EditRole )
    {
        switch ( index.column() )
        {
            case Name:
                layer->setName(value.toString());
                return true;
            case Visible:
                layer->setVisible(value.toBool());
                return true;
            case Locked:
                layer->setLocked(value.toBool());
                return true;
            case Opacity:
                layer->setOpacity(value.toReal());
                return true;
            case BlendMode:
                layer->setBlendMode(QPainter::CompositionMode(value.toInt()));
                return true;
        }
    }

    return false;
}

Qt::ItemFlags LayerTree::flags(const QModelIndex& index) const
{
    if ( !document_ )
        return 0;

    if ( !index.isValid() )
        return Qt::ItemIsDropEnabled | Qt::ItemIsDragEnabled;

    auto flags = QAbstractItemModel::flags(index);

    flags |= Qt::ItemIsEditable | Qt::ItemIsDropEnabled | Qt::ItemIsDragEnabled;

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

    if ( document_ )
        disconnect(document_, nullptr, this, nullptr);

    if ( document )
    {
        connect(document, &Document::layerAdded, this, &LayerTree::onLayerAdded);
        connect(document, &Document::layerRemoved, this, &LayerTree::onLayerRemoved);
    }

    beginResetModel();
    document_ = document;
    endResetModel();
}

QModelIndex LayerTree::addLayer(const QString& name, int row, const QModelIndex& parent)
{
    if ( !document_ )
        return QModelIndex();

    Layer* new_layer = new Layer(document_, name);

    /// \todo Should add a frame image for every frame (should use a visitor for that)
    new_layer->addFrameImage();

    auto cont = container(parent);
    cont->insertLayer(new_layer, reverseInsertRowNumber(cont, row));

    return index(new_layer);
}


Layer* LayerTree::layer(const QModelIndex& index) const
{
    if ( !index.isValid() || index.model() != this || !document_ )
        return nullptr;
    return static_cast<Layer*>(index.internalPointer());
}

QModelIndex LayerTree::index(Layer* layer) const
{
    if ( !layer || layer->parentDocument() != document_ || !document_ )
        return QModelIndex();

    LayerContainer* parent_layer = layer->parentLayer();
    if ( !parent_layer )
        parent_layer = document_;

    int index = reverseLayerIndex(parent_layer, layer);

    if ( index != -1 )
        return createIndex(index, 0, layer);

    return QModelIndex();
}

bool LayerTree::moveRows(const QModelIndex &sourceParent, int sourceRow, int count,
                         const QModelIndex &destinationParent, int destinationChild)
{
    if ( count != 1 || !document_ )
        return false;

    LayerContainer* from = container(sourceParent);
    LayerContainer* to = container(destinationParent);
    Layer* subject = reverseLayer(from, sourceRow);

    if ( !subject )
        return false;

    int layer_index = from->layerIndex(subject);
    int insert_row = reverseInsertRowNumber(to, destinationChild);

    if ( from == to )
    {
        // Do nothing if dropping back to the same spot
        if ( layer_index == insert_row )
            return false;

        /*if ( sourceRow < destinationChild )
            destinationChild += 1;
        else
            destinationChild -= 1;*/
    }
    // The attempt to use the proper utilities to keep persistent indexes
    // consistent failed
    /*bool qt_wants_to_move = beginMoveRows(sourceParent, sourceRow, sourceRow,
                                destinationParent, destinationChild);
    auto old_index = index(subject);*/
    document_->undoStack().beginMacro("Move Layer");
    from->removeLayer(subject);
    to->insertLayer(subject, insert_row);
    document_->undoStack().endMacro();
    /*if ( qt_wants_to_move )
        endMoveRows();
    else
        changePersistentIndex(old_index, index(subject));*/

    return true;
}

bool LayerTree::removeRows(int row, int count, const QModelIndex &parent)
{
    return false;
}

/**
 * \note This isn't implemented in removeRows() because QTreeView
 *       calls it on drag start
 */
bool LayerTree::removeLayer(int row, const QModelIndex &parent)
{
    LayerContainer* cont = container(parent);
    return cont->removeLayer(reverseLayer(cont, row));
}

Qt::DropActions LayerTree::supportedDropActions() const
{
    return Qt::MoveAction;
}

Qt::DropActions LayerTree::supportedDragActions() const
{
    return Qt::MoveAction;
}

QStringList LayerTree::mimeTypes() const
{
    return {mime_type};
}

QMimeData* LayerTree::mimeData(const QModelIndexList & indexes) const
{
    if ( indexes.empty() || !indexes[0].isValid() )
        return nullptr;

    QMimeData* data = new QMimeData;
    QByteArray encoded;
    QDataStream stream(&encoded, QIODevice::WriteOnly);
    stream << quintptr(document_) << quintptr(indexes[0].internalPointer());
    data->setData(mime_type, encoded);
    return data;
}

bool LayerTree::dropMimeData(const QMimeData *data, Qt::DropAction action,
                             int row, int column, const QModelIndex &parent)
{
    if ( !data->hasFormat(mime_type) || !(supportedDropActions() & action) )
        return false;


    QByteArray encoded = data->data(mime_type);
    QDataStream stream(&encoded, QIODevice::ReadOnly);

    quintptr doc_int;
    quintptr layer_int;
    stream >> doc_int >> layer_int;
    Document* source_doc = reinterpret_cast<Document*>(doc_int);
    Layer* source_layer = reinterpret_cast<Layer*>(layer_int);

    if ( stream.status() != QDataStream::Ok || source_doc != document_ )
        return false;

    if ( parent.isValid() && row == -1 )
        row = 0;

    auto layer_index = index(source_layer);
    if ( moveRow(layer_index.parent(), layer_index.row(), parent, row) )
    {
        emit rowDragged(index(source_layer));
        return true;
    }

    return false;
}

void LayerTree::onLayerAdded(::document::Layer* layer, ::document::LayerContainer* parent, int index)
{
    beginInsertRows(parentIndex(layer), reverseRowNumber(parent, index), reverseRowNumber(parent, index));
    endInsertRows();
}

void LayerTree::onLayerRemoved(::document::Layer* layer, ::document::LayerContainer* parent, int index)
{
    beginRemoveRows(parentIndex(layer), reverseRowNumber(parent, index), reverseRowNumber(parent, index));
    endRemoveRows();
}

QModelIndex LayerTree::parentIndex(Layer* layer) const
{
    return layer && layer->parentLayer() ? index(layer->parentLayer()) : QModelIndex();
}

int LayerTree::reverseRowNumber(LayerContainer* parent, int row) const
{
    if ( row < 0 || row >= parent->countLayers() )
        return 0;
    return parent->countLayers() - 1 - row;
}

int LayerTree::reverseInsertRowNumber(LayerContainer* parent, int row) const
{
    if ( row < 0 || row >= parent->countLayers() )
        return 0;
    return parent->countLayers() - row;
}

Layer* LayerTree::reverseLayer(LayerContainer* parent, int row) const
{
    return parent->layer(reverseRowNumber(parent, row));
}

int LayerTree::reverseLayerIndex(LayerContainer* parent, Layer* layer) const
{
    return reverseRowNumber(parent, parent->layerIndex(layer));
}

} // namespace model
