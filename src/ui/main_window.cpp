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
#include "util.hpp"
#include "ui_main_window.h"
#include <QGraphicsView>
#include "document/graphics_widget.hpp"

/**
 * \brief link colorChanged and setColor between two classes
 */
template<class A, class B>
    static void link_color(A* a, B* b)
    {
        QObject::connect(a, &A::colorChanged, b, &B::setColor);
        QObject::connect(b, &B::colorChanged, a, &A::setColor);
    }

template<class A, class R1, class R2, class Arg>
    static void link_same(A* a, A* b, R1 (A::*signal)(Arg), R2 (A::*slot)(Arg) )
    {
        QObject::connect(a, signal, b, slot);
        QObject::connect(b, signal, a, slot);
    }

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setupUi(this);

    init_docks();
    init_menus();
    load_settings();

    current_color_selector.color->setColor(Qt::black);
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
        link_color(color_editor, current_color_selector.color);
    }

    using PalWid = color_widgets::ColorPaletteWidget;
    palette_widget = new PalWid;
    palette_widget->setModel(&palette_model);
    palette_widget->setReadOnly(true);
    dock_palette = create_dock(palette_widget, "preferences-desktop-icons");
    addDockWidget(Qt::RightDockWidgetArea, dock_palette);
    connect(palette_widget, util::overload<const QColor&>(&PalWid::currentColorChanged),
            current_color_selector.color, &color_widgets::ColorSelector::setColor);
    connect(current_color_selector.color, &color_widgets::ColorSelector::colorChanged,
        [this](const QColor& color){
            if ( color != palette_widget->currentColor() )
                palette_widget->setCurrentColor(-1);
    });

    palette_editor = new PalWid;
    palette_editor->setModel(&palette_model);
    dock_palette_editor = create_dock(palette_editor, "preferences-desktop-icons");
    addDockWidget(Qt::RightDockWidgetArea, dock_palette_editor);
    tabifyDockWidget(dock_palette, dock_palette_editor);
    dock_palette->raise();
    link_same(palette_widget, palette_editor, &PalWid::currentRowChanged, &PalWid::setCurrentRow);
    link_same(palette_widget, palette_editor,
        util::overload<int>(&PalWid::currentColorChanged),
        util::overload<int>(&PalWid::setCurrentColor));


    translate_docks();
}

void MainWindow::translate_docks()
{
    dock_set_color->setWindowTitle(tr("Select Color"));
    dock_palette->setWindowTitle(tr("Palette"));
    dock_palette_editor->setWindowTitle(tr("Edit Palette"));
    dock_current_color->setWindowTitle(tr("Current Color"));
}

void MainWindow::init_menus()
{
    action_new->setShortcut(QKeySequence::New);
    action_open->setShortcut(QKeySequence::Open);
    action_save->setShortcut(QKeySequence::Save);
    action_save_as->setShortcut(QKeySequence::SaveAs);
    action_close->setShortcut(QKeySequence::Close);
    action_print->setShortcut(QKeySequence::Print);
    action_quit->setShortcut(QKeySequence::Quit);
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

void MainWindow::setActiveColor(const QColor& color)
{
    color_editor->setColor(color);
}

void MainWindow::documentNew()
{
    /// \todo Show dialog to get the size
    /// \todo Keep track of documents and clean up when the document is closed
    document::Document* doc = new document::Document(QSize(32,32));
    main_tab->addTab(new document::GraphicsWidget(doc), tr("New Image"));
}
