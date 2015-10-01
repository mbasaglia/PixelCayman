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
#ifndef PIXEL_CAYMAN_LAYER_PROPERTIES_WIDGET_HPP
#define PIXEL_CAYMAN_LAYER_PROPERTIES_WIDGET_HPP

#include <QWidget>
#include <QEvent>
#include "ui_layer_properties_widget.h"
#include "document/layer.hpp"
#include "misc/composition_mode.hpp"

/**
 * \brief Widget to change layer properties
 */
class LayerPropertiesWidget : public QWidget, private Ui::LayerPropertiesWidget
{
    Q_OBJECT

public:
    explicit LayerPropertiesWidget(QWidget* parent = nullptr)
        : QWidget(parent)
    {
        setupUi(this);
        for ( const auto& p : misc::composition_names )
            combo_blend->addItem(p.second, p.first);
    }

    void populate(const document::Layer& layer)
    {
        text_name->setText(layer.name());
        check_locked->setChecked(layer.locked());
        check_visible->setChecked(layer.visible());
        combo_blend->setCurrentText(misc::composition_to_string(layer.blendMode()));
        color_background->setColor(layer.backgroundColor());
        spin_opacity->setValue(layer.opacity()*100);
        table_metadata->populate(layer);
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

#endif // PIXEL_CAYMAN_LAYER_PROPERTIES_WIDGET_HPP

