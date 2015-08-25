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

#include "main_window.hpp"
#include "style/dockwidget_style_icon.hpp"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setupUi(this);

    init_docks();
    load_settings();
}

QDockWidget* MainWindow::create_dock(QWidget* widget, const QString& theme_icon)
{
    return create_dock(widget, QIcon::fromTheme(theme_icon));
}

QDockWidget* MainWindow::create_dock(QWidget* widget, const QIcon& icon)
{
    QDockWidget* dock = new QDockWidget(this);
    dock->setWidget(widget);
    dock->setWindowIcon(icon);
    QAction* action = dock->toggleViewAction();
    action->setIcon(icon);
    menu_docks->addAction(action);
    dock->setStyle(new DockWidgetStyleIcon(dock));
    return dock;
}

void MainWindow::init_docks()
{
    color_editor = new ColorEditor;
    dock_set_color = create_dock(color_editor, "format-stroke-color");
    addDockWidget(Qt::RightDockWidgetArea, dock_set_color);

    {
        QWidget* container = new QWidget;
        current_color_selector.setupUi(container);
        dock_current_color = create_dock(container, "format-stroke-color");
        addDockWidget(Qt::RightDockWidgetArea, dock_current_color);
    }
    palette_widget = new color_widgets::ColorPaletteWidget;
    palette_widget->setModel(&palette_model);
    palette_widget->setReadOnly(true);
    dock_palette = create_dock(palette_widget, "preferences-desktop-icons");
    addDockWidget(Qt::RightDockWidgetArea, dock_palette);

    palette_editor = new color_widgets::ColorPaletteWidget;
    palette_editor->setModel(&palette_model);
    dock_palette_editor = create_dock(palette_editor, "preferences-desktop-icons");
    addDockWidget(Qt::RightDockWidgetArea, dock_palette_editor);
    tabifyDockWidget(dock_palette, dock_palette_editor);
    dock_palette->raise();


    translate_docks();
}

void MainWindow::translate_docks()
{
    dock_set_color->setWindowTitle(tr("Select Color"));
    dock_palette->setWindowTitle(tr("Palette"));
    dock_palette_editor->setWindowTitle(tr("Edit Palette"));
    dock_current_color->setWindowTitle(tr("Current Color"));
}

void MainWindow::load_settings()
{
    palette_model.addSearchPath("/usr/share/gimp/2.0/palettes/");
    palette_model.addSearchPath("/usr/share/inkscape/palettes/");
    palette_model.addSearchPath("/usr/share/kde4/apps/calligra/palettes/");
    palette_model.load();
}

void MainWindow::changeEvent(QEvent* event)
{
    if ( event->type() == QEvent::LanguageChange )
    {
        retranslateUi(this);
        current_color_selector.retranslateUi(dock_current_color->widget());
        translate_docks();
    }

    QMainWindow::changeEvent(event);
}

void MainWindow::set_active_color(const QColor& color)
{
    color_editor->setColor(color);
}
