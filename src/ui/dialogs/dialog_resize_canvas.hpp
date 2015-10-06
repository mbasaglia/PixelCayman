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
#ifndef PIXEL_CAYMAN_DIALOG_RESIZE_CANVAS_HPP
#define PIXEL_CAYMAN_DIALOG_RESIZE_CANVAS_HPP

#include <QDialog>
#include <QEvent>
#include "ui_dialog_resize_canvas.h"

/**
 * \brief Dialog shown when resizing the image boundaries
 */
class DialogResizeCanvas : public QDialog, private Ui::DialogResizeCanvas
{
    Q_OBJECT

public:
    explicit DialogResizeCanvas(QWidget* parent = nullptr)
    : QDialog(parent)
    {
        setupUi(this);
        /// \todo Analog widget to edit x/y
        connect(size_widget, &SizeWidget::valueChanged, [this](const QSize& size){
            spin_x->setMinimum(qMin(original_size.width() - size.width(), 0));
            spin_x->setMaximum(qMax(original_size.width() - size.width(), 0));
            spin_y->setMinimum(qMin(original_size.height() - size.height(), 0));
            spin_y->setMaximum(qMax(original_size.height() - size.height(), 0));
        });
        connect(button_center, &QAbstractButton::clicked, [this]{
            spin_x->setValue((spin_x->minimum() + spin_x->maximum()) / 2);
            spin_y->setValue((spin_y->minimum() + spin_y->maximum()) / 2);
        });
    }

    void setOriginal(const QSize& size)
    {
        original_size = size;
        size_widget->setValue(size);
    }

    QRect result() const
    {
        return {{spin_x->value(), spin_y->value()}, size_widget->value()};
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

private:
    QSize original_size;
};

#endif // PIXEL_CAYMAN_DIALOG_RESIZE_CANVAS_HPP
