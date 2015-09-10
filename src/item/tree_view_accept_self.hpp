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
#ifndef PIXEL_CAYMAN_TREEVIEW_ACCEPT_SELF_HPP
#define PIXEL_CAYMAN_TREEVIEW_ACCEPT_SELF_HPP

#include <QTreeView>
#include <QDropEvent>
#include <QMimeData>
#include "style/itemview_style_rowdrop.hpp"

class TreeViewAcceptSelf : public QTreeView
{
    //Q_OBJECT

public:
    explicit TreeViewAcceptSelf(QWidget* parent = nullptr)
        : QTreeView(parent)
    {
        auto new_style = new ItemViewStyleRowDrop(style());
        new_style->setParent(this);
        setStyle(new_style);
    }

protected:
    void dragEnterEvent(QDragEnterEvent *event) override
    {
        QTreeView::dragEnterEvent(event);

        if ( event->proposedAction() & model()->supportedDropActions() )
        {
            for ( const auto& type : model()->mimeTypes() )
            {
                if ( event->mimeData()->hasFormat(type) )
                {
                    event->accept();
                    return;
                }
            }
        }
    }
};

#endif // PIXEL_CAYMAN_TREEVIEW_ACCEPT_SELF_HPP
