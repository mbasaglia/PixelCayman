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
#include "layer_widget.hpp"

#include <QInputDialog>
#include <QMenu>

#include "bool_icon_delegate.hpp"
#include "slider_delegate.hpp"
#include "enum_combo_delegate.hpp"
#include "misc/composition_mode.hpp"
#include "document/visitor.hpp"
#include "ui/dialogs/dialog_layer_create.hpp"
#include "color_delegate.hpp"
#include "layer_properties_widget.hpp"
#include "simple_dialog.hpp"

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
    //: Custom/unrecognized layer blend mode name
    delegate_blend_mode->setUnknownValueName(tr("Custom"));
    tree_view->setItemDelegateForColumn(::model::LayerTree::BlendMode, delegate_blend_mode);

    auto delegate_color = new color_widgets::ColorDelegate(this);
    tree_view->setItemDelegateForColumn(::model::LayerTree::BackgroundColor, delegate_color);

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
    connect(&model, &QAbstractItemModel::rowsRemoved, expand);
    connect(&model, &::model::LayerTree::rowDragged,
    [this](const QModelIndex& index) {
        tree_view->setCurrentIndex(index);
    });

    connect(button_top, &QAbstractButton::clicked, [this]{
        auto index = tree_view->currentIndex();
        auto parent = index.parent();
        if ( index.isValid() && index.row() != 0 )
        {
            auto layer = model.layer(index);
            model.moveRow(parent, index.row(), parent, 1);
            tree_view->setCurrentIndex(model.index(layer));
        }
    });

    connect(button_bottom, &QAbstractButton::clicked, [this]{
        auto index = tree_view->currentIndex();
        auto parent = index.parent();
        if ( index.isValid() && index.row() != model.rowCount(parent) - 1 )
        {
            auto layer = model.layer(index);
            model.moveRow(parent, index.row(), parent, model.rowCount(parent));
            tree_view->setCurrentIndex(model.index(layer));
        }
    });

    connect(button_up, &QAbstractButton::clicked, [this]{
        auto index = tree_view->currentIndex();
        auto parent = index.parent();
        if ( index.isValid() && index.row() != 0 )
        {
            auto layer = model.layer(index);
            model.moveRow(parent, index.row(), parent, index.row());
            tree_view->setCurrentIndex(model.index(layer));
        }
    });

    connect(button_down, &QAbstractButton::clicked, [this]{
        auto index = tree_view->currentIndex();
        auto parent = index.parent();
        if ( index.isValid() && index.row() != model.rowCount(parent) - 1 )
        {
            auto layer = model.layer(index);
            model.moveRow(parent, index.row(), parent, index.row() + 2);
            tree_view->setCurrentIndex(model.index(layer));
        }
    });

    connect(tree_view, &QWidget::customContextMenuRequested, [this](const QPoint& pos){
        auto index = tree_view->indexAt(pos);
        if ( auto layer = model.layer(index) )
        {
            QMenu menu;
            QAction properties(QIcon::fromTheme("document-properties"), tr("Properties..."), this);
            connect(&properties, &QAction::triggered, layer, [this, layer]{
                auto layer_widget = new LayerPropertiesWidget();
                layer_widget->populate(*layer);
                SimpleDialog dlg(this, layer_widget);
                if ( dlg.exec() )
                    layer_widget->apply(*layer);
            });
            menu.addAction(&properties);
            menu.exec(tree_view->mapToGlobal(pos));
        }
    });

    /// \todo Option to choose which columns are displayed
    tree_view->hideColumn(::model::LayerTree::BlendMode);
    tree_view->hideColumn(::model::LayerTree::BackgroundColor);
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
    QString name_template = tr("Layer%1");

    QString default_name = name_template.arg("");

    using ::document::visitor::FindLayer;
    for ( int i = 1; FindLayer::find(*model.document(), default_name); i++)
    {
        default_name = name_template.arg(i);
    }

    DialogLayerCreate dialog(this);
    dialog.setDefaultName(default_name);

    if ( dialog.exec() )
    {
        QModelIndex index = tree_view->currentIndex();
        QModelIndex parent;
        int row = -1;
        switch ( dialog.position() )
        {
            case DialogLayerCreate::Above:
                parent = model.parent(index);
                row = index.isValid() ? index.row() : 0;
                break;
            case DialogLayerCreate::Below:
                parent = model.parent(index);
                row = index.isValid() ? index.row()+1 : -1;
                break;
            case DialogLayerCreate::Top:
                parent = QModelIndex();
                row = 0;
                break;
            case DialogLayerCreate::Child:
                parent = index;
                row = 0;
                break;
        }

        QModelIndex new_index = model.addLayer(dialog.name(), row, dialog.background(), parent);
        if ( new_index.isValid() )
        {
            tree_view->setCurrentIndex(new_index);
        }
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
