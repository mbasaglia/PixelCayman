/**

@author Mattia Basaglia

@section License

    Copyright (C) 2013 Mattia Basaglia

    This file is part of Color Widgets.

    Color Widgets is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Color Widgets is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Color Widgets.  If not, see <http://www.gnu.org/licenses/>.

*/
#include "tabwidget_plugin.hpp"
#include "extended_tab_widget.hpp"

Extended_Tabwidget_Plugin::Extended_Tabwidget_Plugin(QObject *parent) :
    QObject(parent), initialized(false)
{
}

void Extended_Tabwidget_Plugin::initialize(QDesignerFormEditorInterface *)
{
    initialized = true;
}

bool Extended_Tabwidget_Plugin::isInitialized() const
{
    return initialized;
}

QWidget *Extended_Tabwidget_Plugin::createWidget(QWidget *parent)
{
    return new ExtendedTabWidget(parent);
}

QString Extended_Tabwidget_Plugin::name() const
{
    return "ExtendedTabWidget";
}

QString Extended_Tabwidget_Plugin::group() const
{
    return "Containers";
}

QIcon Extended_Tabwidget_Plugin::icon() const
{
    return QIcon::fromTheme("tab-new");
}

QString Extended_Tabwidget_Plugin::toolTip() const
{
    return "Extended TabWidget";
}

QString Extended_Tabwidget_Plugin::whatsThis() const
{
    return "A TabWidget with support for middle-mouse closing and dragging";
}

bool Extended_Tabwidget_Plugin::isContainer() const
{
    return false;
}

QString Extended_Tabwidget_Plugin::domXml() const
{
    return "<ui language=\"c++\">\n"
           " <widget class=\"ExtendedTabWidget\" name=\"tabWidget\">\n"
           " </widget>\n"
           "</ui>\n";
}

QString Extended_Tabwidget_Plugin::includeFile() const
{
    return "extended_tab_widget.hpp";
}


