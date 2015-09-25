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
#include "log_view_plugin.hpp"
#include "log_view.hpp"

QWidget* LogView_Plugin::createWidget(QWidget *parent)
{
    LogView *widget = new LogView(parent);
    return widget;
}

QIcon LogView_Plugin::icon() const
{
    return QIcon();
}

QString LogView_Plugin::domXml() const
{
    return "<ui language=\"c++\">\n"
           " <widget class=\"LogView\" name=\"log_view\">\n"
           " </widget>\n"
           "</ui>\n";
}

bool LogView_Plugin::isContainer() const
{
    return false;
}

LogView_Plugin::LogView_Plugin(QObject *parent) :
    QObject(parent), initialized(false)
{
}

void LogView_Plugin::initialize(QDesignerFormEditorInterface *)
{
    initialized = true;
}

bool LogView_Plugin::isInitialized() const
{
    return initialized;
}

QString LogView_Plugin::name() const
{
    return "LogView";
}

QString LogView_Plugin::group() const
{
    return "Misc Widgets";
}

QString LogView_Plugin::toolTip() const
{
    return "A widget to display some log";
}

QString LogView_Plugin::whatsThis() const
{
    return toolTip();
}

QString LogView_Plugin::includeFile() const
{
    return "log_view.hpp";
}

