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
#ifndef PIXEL_CAYMAN_DIALOG_LAYER_CREATE_HPP
#define PIXEL_CAYMAN_DIALOG_LAYER_CREATE_HPP

#include <QDialog>
#include <QEvent>
#include "ui_dialog_layer_create.h"

/**
 * \brief Dialog to get the information needed to create a new layer
 */
class DialogLayerCreate : public QDialog, private Ui::DialogLayerCreate
{
public:
    enum Position
    {
        Above,
        Below,
        Child,
        Top
    };

    explicit DialogLayerCreate(QWidget* parent = nullptr)
        : QDialog(parent)
    {
        setupUi(this);

    }

    void setDefaultName(const QString& name)
    {
        text_name->setPlaceholderText(name);
    }

    QString name() const
    {
        QString name = text_name->text();
        return name.isEmpty() ? text_name->placeholderText() : name;
    }

    QColor background() const
    {
        return color_background->color();
    }

    Position position() const
    {
        return Position(combo_position->currentIndex());
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
};

#endif // PIXEL_CAYMAN_DIALOG_LAYER_CREATE_HPP
