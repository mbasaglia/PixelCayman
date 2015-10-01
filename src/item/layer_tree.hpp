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
#ifndef PIXEL_CAYMAN_LAYER_MODEL_HPP
#define PIXEL_CAYMAN_LAYER_MODEL_HPP

#include "document/document.hpp"
#include <QAbstractItemModel>

namespace model {

/**
 * \brief Tree model for the layer structure
 */
class LayerTree : public QAbstractItemModel
{
    Q_OBJECT

public:
    enum Columns
    {
        Name,
        Visible,
        Locked,
        Opacity,
        BlendMode,
        BackgroundColor,

        ColumnCount
    };

    explicit LayerTree(::document::Document* document = nullptr);

    QVariant data(const QModelIndex &index, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;

    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
                      
    QModelIndex parent(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    bool moveRows(const QModelIndex &sourceParent, int sourceRow, int count,
                  const QModelIndex &destinationParent, int destinationChild) override;

    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    bool removeLayer(int row, const QModelIndex &parent = QModelIndex());

    Qt::DropActions supportedDropActions() const override;
    Qt::DropActions supportedDragActions() const override;
    QStringList mimeTypes() const override;
    QMimeData* mimeData(const QModelIndexList & indexes) const override;
    bool dropMimeData(const QMimeData *data, Qt::DropAction action,
                      int row, int column, const QModelIndex &parent) override;

    ::document::Document* document() const;
    void setDocument(::document::Document* document);

    /**
     * \brief Add an empty layer with the given name as the row^th child of parent
     */
    QModelIndex addLayer(const QString& name, int row,
                         const QColor& background_color,
                         const QModelIndex& parent);

    /**
     * \brief Layer corresponding to the given index
     */
    ::document::Layer* layer(const QModelIndex& index) const;

    /**
     * \brief Index corresponding to the given layer
     */
    QModelIndex index(::document::Layer* layer) const;

    /**
     * \brief Index corresponding to the parent of the given layer
     */
    QModelIndex parentIndex(::document::Layer* layer) const;

private slots:
    void onLayerAdded(::document::Layer* layer, ::document::LayerContainer* parent, int index);
    void onLayerRemoved(::document::Layer* layer, ::document::LayerContainer* parent, int index);

signals:
    void rowDragged(const QModelIndex& destination);

private:
    ::document::LayerContainer* container(const QModelIndex& index) const;
    /**
     * \brief Inverts a row number so it would be the equivalent index
     *        if the layers were reversed
     */
    int reverseRowNumber(::document::LayerContainer* parent, int row) const;
    int reverseInsertRowNumber(::document::LayerContainer* parent, int row) const;
    ::document::Layer* reverseLayer(::document::LayerContainer* parent, int row) const;
    int reverseLayerIndex(document::LayerContainer* parent, document::Layer* layer) const;

    ::document::Document* document_;
};

} // namespace model
#endif // PIXEL_CAYMAN_LAYER_MODEL_HPP
