/**
 * \file
 *
 * \author Mattia Basaglia
 *
 * \copyright Copyright (C) 2015 Mattia Basaglia
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
#include "size_widget_plugin.hpp"
#include "size_widget.hpp"

QWidget* SizeWidget_Plugin::createWidget(QWidget *parent)
{
    SizeWidget *widget = new SizeWidget(parent);
    return widget;
}

QIcon SizeWidget_Plugin::icon() const
{
    return QIcon();
}

QString SizeWidget_Plugin::domXml() const
{
    return "<ui language=\"c++\">\n"
           " <widget class=\"SizeWidget\" name=\"size_widget\">\n"
           " </widget>\n"
           "</ui>\n";
}

bool SizeWidget_Plugin::isContainer() const
{
    return false;
}

SizeWidget_Plugin::SizeWidget_Plugin(QObject *parent) :
    QObject(parent), initialized(false)
{
}

void SizeWidget_Plugin::initialize(QDesignerFormEditorInterface *)
{
    initialized = true;
}

bool SizeWidget_Plugin::isInitialized() const
{
    return initialized;
}

QString SizeWidget_Plugin::name() const
{
    return "SizeWidget";
}

QString SizeWidget_Plugin::group() const
{
    return "Misc Widgets";
}

QString SizeWidget_Plugin::toolTip() const
{
    return "Widget to edit a QSize";
}

QString SizeWidget_Plugin::whatsThis() const
{
    return toolTip();
}

QString SizeWidget_Plugin::includeFile() const
{
    return "size_widget.hpp";
}

