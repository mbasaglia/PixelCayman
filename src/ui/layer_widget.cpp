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
#include "layer_widget.hpp"

#include <QInputDialog>

#include "style/bool_icon_delegate.hpp"
#include "style/slider_delegate.hpp"
#include "style/enum_combo_delegate.hpp"
#include "misc/composition_mode.hpp"

LayerWidget::LayerWidget()
{
    setupUi(this);

    tree_view->setModel(&model);
    tree_view->header()->setSectionResizeMode(
        QHeaderView::ResizeToContents);
    tree_view->header()->setSectionResizeMode(
        model::LayerTree::Name,
        QHeaderView::Stretch);
    tree_view->header()->setSectionResizeMode(
        model::LayerTree::Opacity,
        QHeaderView::Stretch);

    auto delegate_visible = new BoolIconDelegate(
        QIcon::fromTheme("layer-visible-on"),
        QIcon::fromTheme("layer-visible-off"),
        this);
    tree_view->setItemDelegateForColumn(::model::LayerTree::Visible, delegate_visible);

    auto delegate_locked = new BoolIconDelegate(
        QIcon::fromTheme("object-locked"),
        QIcon::fromTheme("object-unlocked"),
        this);
    tree_view->setItemDelegateForColumn(::model::LayerTree::Locked, delegate_locked);

    auto delegate_opacity = new SliderDelegate(this);
    tree_view->setItemDelegateForColumn(::model::LayerTree::Opacity, delegate_opacity);

    auto delegate_blend_mode = new EnumComboDelegate(this);
    delegate_blend_mode->setNames(misc::composition_names);
    delegate_blend_mode->setUnknownValueName(tr("Custom"));
    tree_view->setItemDelegateForColumn(::model::LayerTree::BlendMode, delegate_blend_mode);

    connect(tree_view->selectionModel(), &QItemSelectionModel::currentChanged,
        [this](const QModelIndex& index)
        {
            if ( model.rowCount() == 0 )
                emit activeLayerChanged( nullptr );
            else if ( index.isValid() )
                emit activeLayerChanged( model.layer(index) );
        });

    auto expand = [this](const QModelIndex& parent) {
        for ( QModelIndex index = parent; index.isValid(); index = model.parent(index) )
            tree_view->expand(index);
    };

    connect(&model, &QAbstractItemModel::rowsInserted, expand);
    connect(&model, &QAbstractItemModel::rowsRemoved, expand);;
    connect(&model, &::model::LayerTree::rowDragged,
    [this](const QModelIndex& index) {
        tree_view->selectionModel()->setCurrentIndex(index,
            QItemSelectionModel::SelectCurrent | QItemSelectionModel::Rows | QItemSelectionModel::Clear);
    });
}

void LayerWidget::changeEvent(QEvent* event)
{
    if ( event->type() == QEvent::LanguageChange )
    {
        retranslateUi(this);
    }

    QWidget::changeEvent(event);
}

document::Document* LayerWidget::document() const
{
    return model.document();
}

void LayerWidget::setDocument(document::Document* document)
{
    model.setDocument(document);
    tree_view->setCurrentIndex(model.index(0, 0));
}

void LayerWidget::addLayer()
{
    bool ok = false;
    QString name = QInputDialog::getText(this, tr("New Layer"),
        tr("Layer name"), QLineEdit::Normal, tr("Layer"), &ok );

    if ( ok )
    {
        QModelIndex index = tree_view->currentIndex();
        QModelIndex parent = model.parent(index);

        QModelIndex new_index =
            model.addLayer(name, index.isValid() ? index.row() : -1, parent);

        if ( new_index.isValid() )
            tree_view->setCurrentIndex(new_index);
    }
}

void LayerWidget::removeLayer()
{
    auto index = tree_view->currentIndex();
    if ( index.isValid() )
        model.removeLayer(index.row(), model.parent(index));
}

document::Layer* LayerWidget::activeLayer() const
{
    return model.layer(tree_view->currentIndex());
}

void LayerWidget::setActiveLayer(document::Layer* activeLayer)
{
    tree_view->setCurrentIndex(model.index(activeLayer));
}
