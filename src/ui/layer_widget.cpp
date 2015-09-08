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
#include "style/bool_icon_delegate.hpp"
#include <QInputDialog>

LayerWidget::LayerWidget()
{
    setupUi(this);

    tree_view->setModel(&model);
    tree_view->header()->setSectionResizeMode(
        QHeaderView::ResizeToContents);
    tree_view->header()->setSectionResizeMode(
        model::LayerTree::Name,
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

        model.addLayer(name, index.isValid() ? index.row()+1 : -1, parent);
        /// \todo Undo command
    }
}

void LayerWidget::removeLayer()
{
    /// \todo
}
