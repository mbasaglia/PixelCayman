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
#ifndef PIXEL_CAYMAN_ITEM_VIEW_STYLE_ROW_DROP_HPP
#define PIXEL_CAYMAN_ITEM_VIEW_STYLE_ROW_DROP_HPP

#include <QProxyStyle>
#include <QStyle>
#include <QAbstractItemView>

/**
 * \brief Makes the drop indicator fill the entire row
 */
class ItemViewStyleRowDrop: public QProxyStyle
{
public:
    explicit ItemViewStyleRowDrop(QStyle* style = nullptr)
        :  QProxyStyle(style)
    {}

    void drawPrimitive(PrimitiveElement element,
                       const QStyleOption* option,
                       QPainter* painter,
                       const QWidget* widget ) const override
    {
        if ( element == QStyle::PE_IndicatorItemViewItemDrop && !option->rect.isNull() )
            if ( auto view = qobject_cast<const QAbstractItemView*>(widget) )
            {
                QStyleOption opt(*option);
                auto index = view->indexAt(opt.rect.topLeft());

                index = index.sibling(index.row(), 0);
                opt.rect.setLeft(view->visualRect(index).left());

                index = index.sibling(index.row(), view->model()->columnCount(index.parent())-1);
                opt.rect.setRight(/*qMin(view->visualRect(index).right()-1,*/
                                       view->viewport()->width()-2/*)*/);

                QProxyStyle::drawPrimitive(element, &opt, painter, widget);
                return;
            }

        QProxyStyle::drawPrimitive(element, option, painter, widget);
    }
};

#endif // PIXEL_CAYMAN_ITEM_VIEW_STYLE_ROW_DROP_HPP
