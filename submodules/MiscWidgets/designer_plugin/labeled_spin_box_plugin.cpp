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
#include "labeled_spin_box_plugin.hpp"
#include "labeled_spinbox.hpp"

QWidget* LabeledSpinBox_Plugin::createWidget(QWidget *parent)
{
    LabeledSpinBox *widget = new LabeledSpinBox(parent);
    return widget;
}

QIcon LabeledSpinBox_Plugin::icon() const
{
    return QIcon();
}

QString LabeledSpinBox_Plugin::domXml() const
{
    return "<ui language=\"c++\">\n"
           " <widget class=\"LabeledSpinBox\" name=\"labeled_spin_box\">\n"
           " </widget>\n"
           "</ui>\n";
}

bool LabeledSpinBox_Plugin::isContainer() const
{
    return false;
}

LabeledSpinBox_Plugin::LabeledSpinBox_Plugin(QObject *parent) :
    QObject(parent), initialized(false)
{
}

void LabeledSpinBox_Plugin::initialize(QDesignerFormEditorInterface *)
{
    initialized = true;
}

bool LabeledSpinBox_Plugin::isInitialized() const
{
    return initialized;
}

QString LabeledSpinBox_Plugin::name() const
{
    return "LabeledSpinBox";
}

QString LabeledSpinBox_Plugin::group() const
{
    return "Misc Widgets";
}

QString LabeledSpinBox_Plugin::toolTip() const
{
    return "A QLabel and a QSpinBox";
}

QString LabeledSpinBox_Plugin::whatsThis() const
{
    return toolTip();
}

QString LabeledSpinBox_Plugin::includeFile() const
{
    return "labeled_spinbox.hpp";
}

