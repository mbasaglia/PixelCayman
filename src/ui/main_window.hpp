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
#include "document/document.hpp"

class MainWindow : public QMainWindow, Ui::MainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

public slots:
    void setActiveColor(const QColor& color);
    void documentNew();
    bool documentOpen();
    bool documentSave();
    bool documentSaveAs();

private slots:
    void updateTitle();

protected:
    void changeEvent(QEvent* event) override;
    /**
     * \brief Save the document
     * \param tab Index of the tab with the document to be saved
     * \param prompt Whether to always prompt for a file name
     * \return Whether the file has been saved successfully
     */
    bool save(int tab, bool prompt);

    document::Document* currentDocument();

private:
    QDockWidget* create_dock(QWidget* widget, const QString& theme_icon);
    QDockWidget* create_dock(QWidget* widget, const QIcon& icon);
    void init_docks();
    void translate_docks();
    void init_menus();
    void load_settings();
    void save_settings();

    void pushRecentFile(const QString& name);

    /**
     * \brief Saves \p doc
     */
    bool doSave(document::Document* doc, int file_format);

    /**
     * \brief Converts a file name to a title suitable for a tab
     */
    QString tabText(QString file_name);

    QDockWidget* dock_set_color;
    ColorEditor* color_editor;

    Ui::CurrentColor current_color_selector;
    QDockWidget* dock_current_color;

    color_widgets::ColorPaletteModel palette_model;
    color_widgets::ColorPaletteWidget* palette_widget;
    color_widgets::ColorPaletteWidget* palette_editor;
    QDockWidget* dock_palette;
    QDockWidget* dock_palette_editor;

    QStringList recent_files;
};

#endif // MAIN_WINDOW_HPP
