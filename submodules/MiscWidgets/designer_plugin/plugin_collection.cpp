/**
 * \file
 *
 * \author Mattia Basaglia
 *
 * \copyright Copyright (C) 2013-2015 Mattia Basaglia
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "plugin_collection.hpp"
#include "labeled_spin_box_plugin.hpp"
#include "log_view_plugin.hpp"
#include "size_widget_plugin.hpp"
// add new plugin headers above this line

MiscWidgets_PluginCollection::MiscWidgets_PluginCollection(QObject *parent) :
    QObject(parent)
{
    widgets.push_back(new LabeledSpinBox_Plugin(this));
    widgets.push_back(new LogView_Plugin(this));
    widgets.push_back(new SizeWidget_Plugin(this));
    // add new plugins above this line
}

QList<QDesignerCustomWidgetInterface *> MiscWidgets_PluginCollection::customWidgets() const
{
    return widgets;
}
