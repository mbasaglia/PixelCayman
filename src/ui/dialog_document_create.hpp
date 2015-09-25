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

#ifndef PIXEl_CAYMAN_DIALOG_DOCUMENT_CREATE_HPP
#define PIXEl_CAYMAN_DIALOG_DOCUMENT_CREATE_HPP

#include <QDialog>
#include <QEvent>
#include "ui_dialog_document_create.h"

class DialogDocumentCreate : public QDialog, private Ui::DialogDocumentCreate
{
    Q_OBJECT
public:

    explicit DialogDocumentCreate(QWidget* parent = nullptr)
        : QDialog(parent)
    {
        setupUi(this);

    }

    QColor background() const
    {
        return color_background->color();
    }

    QSize imageSize() const
    {
        return size_image->value();
    }

    void setImageSize(const QSize& value) const
    {
        size_image->setValue(value);
    }

protected:
    void changeEvent(QEvent* event) override
    {
        if ( event->type() == QEvent::LanguageChange )
        {
            retranslateUi(this);
        }

        QWidget::changeEvent(event);
    }

private:
    Ui::DialogDocumentCreate* ui;
};

#endif // PIXEl_CAYMAN_DIALOG_DOCUMENT_CREATE_HPP
