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
        Opacity
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

    ::document::Document* document() const;
        void setDocument(document::Document* document);

private:
    ::document::Document* document_;
};

} // namespace model
#endif // PIXEL_CAYMAN_LAYER_MODEL_HPP
