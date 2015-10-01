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
#ifndef PIXEL_CAYMAN_UI_METADATA_WIDGET_HPP
#define PIXEL_CAYMAN_UI_METADATA_WIDGET_HPP

#include <QTableWidget>
#include "document/document_element.hpp"

/**
 * \brief Data to edit metadata in a document element
 * \todo Empty row at the end for insertion
 * \todo Clearing a key should remove the row
 * \todo Detect if there has been any change
 */
class MetaDataWidget : public QTableWidget
{
    Q_OBJECT
public:
    using QTableWidget::QTableWidget;

    void populate(const document::Metadata& data)
    {
        clear();
        int row = 0;
        for ( auto it = data.begin(); it != data.end(); ++it )
        {
            setItem(row, 0, new QTableWidgetItem(it.key()));
            setItem(row, 1, new QTableWidgetItem(it.value()));
            row++;
        }
    }

    void populate(const document::DocumentElement& element)
    {
        populate(element.metadata());
    }

    document::Metadata getMetaData() const
    {
        document::Metadata data;
        for ( int i = 0; i < rowCount(); i++ )
            data[item(i, 0)->text()] = item(i, 1)->text();
        return data;
    }
};

#endif // PIXEL_CAYMAN_UI_METADATA_WIDGET_HPP
