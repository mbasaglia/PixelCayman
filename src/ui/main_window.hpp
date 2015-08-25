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
#ifndef MAIN_WINDOW_HPP
#define MAIN_WINDOW_HPP

#include "ui_main_window.h"

#include <QDockWidget>
#include "color_editor.hpp"
#include "color_palette_widget.hpp"
#include "color_palette_model.hpp"
#include "color_selector.hpp"
#include "color_line_edit.hpp"
#include "ui_current_color.h"

class MainWindow : public QMainWindow, Ui::MainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);

public slots:
    void set_active_color(const QColor& color);

protected:
    void changeEvent(QEvent* event) override;

private:
    QDockWidget* create_dock(QWidget* widget, const QString& theme_icon);
    QDockWidget* create_dock(QWidget* widget, const QIcon& icon);
    void init_docks();
    void translate_docks();

    void init_menus();

    void load_settings();

    QDockWidget* dock_set_color;
    ColorEditor* color_editor;

    Ui::CurrentColor current_color_selector;
    QDockWidget* dock_current_color;

    color_widgets::ColorPaletteModel palette_model;
    color_widgets::ColorPaletteWidget* palette_widget;
    color_widgets::ColorPaletteWidget* palette_editor;
    QDockWidget* dock_palette;
    QDockWidget* dock_palette_editor;
};

#endif // MAIN_WINDOW_HPP
