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
#ifndef PIXEL_CAYMAN_LAYER_WIDGET_HPP
#define PIXEL_CAYMAN_LAYER_WIDGET_HPP

#include <QWidget>
#include "document/document.hpp"
#include "ui_layer_widget.h"
#include "item/layer_tree.hpp"

/**
 * \brief Widget to display and edit the layers in a document
 */
class LayerWidget : public QWidget, private Ui::LayerWidget
{
    Q_OBJECT

    Q_PROPERTY(::document::Document* document READ document WRITE setDocument)

    Q_PROPERTY(::document::Layer* activeLayer READ activeLayer WRITE setActiveLayer NOTIFY activeLayerChanged)

public:
    LayerWidget();

    ::document::Document* document() const;
    ::document::Layer* activeLayer() const;

public slots:
    void setDocument(::document::Document* document);
    void setActiveLayer(::document::Layer* activeLayer);
    void addLayer();
    void removeLayer();

signals:
    void activeLayerChanged(::document::Layer* activeLayer);

protected:
    void changeEvent(QEvent* event) override;

private:
    ::model::LayerTree model;

};

#endif // PIXEL_CAYMAN_LAYER_WIDGET_HPP
