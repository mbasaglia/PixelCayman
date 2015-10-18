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
#ifndef PIXEL_CAYMAN_DIALOG_INDEXED_COLORS_HPP
#define PIXEL_CAYMAN_DIALOG_INDEXED_COLORS_HPP

#include <QDialog>
#include <QEvent>
#include "ui_dialog_indexed_colors.h"

class DialogIndexedColors : public QDialog, private Ui::DialogIndexedColors
{
    Q_OBJECT

public:
    explicit DialogIndexedColors(QWidget* parent = nullptr)
    : QDialog(parent)
    {
        setupUi(this);
        /// \todo use a proxy model to display the palettes
        /// Passed in ctor is a color palette model
        /// then it provides an optional extra palette for the current document
        /// modifying this palette shall not edit the document directly
    }

    const color_widgets::ColorPalette& palette() const
    {
        return palette_widget->currentPalette();
    }

protected:
    void changeEvent(QEvent* event) override
    {
        if ( event->type() == QEvent::LanguageChange )
        {
            retranslateUi(this);
        }

        QDialog::changeEvent(event);
    }
};

#endif // PIXEL_CAYMAN_DIALOG_INDEXED_COLORS_HPP
