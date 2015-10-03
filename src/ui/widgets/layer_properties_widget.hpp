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
#include <document/document.hpp>
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

    /**
     * \brief Populate the fields of the widget from the properties of the layer
     */
    void populate(const document::Layer& layer)
    {
        text_name->setText(layer.name());
        text_name->setPlaceholderText(layer.name());
        check_locked->setChecked(layer.locked());
        check_visible->setChecked(layer.visible());
        combo_blend->setCurrentText(misc::composition_to_string(layer.blendMode()));
        color_background->setColor(layer.backgroundColor());
        spin_opacity->setValue(qRound(layer.opacity()*100));
        table_metadata->populate(layer);
    }

    /**
     * \brief Apply the changes to the layer
     */
    void apply(document::Layer& target)
    {
        bool set_name = !text_name->text().isEmpty() && text_name->text() != target.name();
        bool set_locked = check_locked->isChecked() != target.locked();
        bool set_visible = check_visible->isChecked() != target.visible();
        bool set_blend = combo_blend->currentData().toInt() != target.blendMode();
        bool set_color = color_background->color() != target.backgroundColor();
        bool set_opacity = spin_opacity->value() != qRound(target.opacity()*100);
        auto meta = table_metadata->getMetaData();
        bool set_meta = meta != target.metadata();

        bool set_any = set_name || set_locked || set_visible || set_blend ||
                       set_color || set_opacity || set_meta;
        if ( !set_any )
            return;

        target.parentDocument()->undoStack().beginMacro(tr("Change layer properties"));

        if ( set_name )
            target.setName(text_name->text());

        if ( set_locked )
            target.setLocked(check_locked->isChecked());

        if ( set_visible )
            target.setLocked(check_visible->isChecked());

        if ( set_blend )
            target.setBlendMode(QPainter::CompositionMode(combo_blend->currentData().toInt()));

        if ( set_color )
            target.setBackgroundColor(color_background->color());

        if ( set_opacity )
            target.setOpacity(spin_opacity->value()/100.0);

        if ( set_meta )
            target.setMetadata(meta);

        target.parentDocument()->undoStack().endMacro();
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

