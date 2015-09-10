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
#ifndef PIXEL_CAYMAN_MAIN_WINDOW_PRIVATE_HPP
#define PIXEL_CAYMAN_MAIN_WINDOW_PRIVATE_HPP

#include "main_window.hpp"

#include <QDockWidget>
#include <QFileInfo>
#include <QTreeView>
#include <QUndoGroup>
#include <QUndoView>

#include "color_editor.hpp"
#include "color_line_edit.hpp"
#include "color_palette_model.hpp"
#include "color_palette_widget.hpp"
#include "color_selector.hpp"
#include "document/io.hpp"
#include "item/layer_tree.hpp"
#include "settings.hpp"
#include "style/dockwidget_style_icon.hpp"
#include "tool/tool.hpp"
#include "util.hpp"
#include "view/graphics_widget.hpp"
#include "layer_widget.hpp"

#include "ui_current_color.h"
#include "ui_main_window.h"

class MainWindow::Private : public Ui_MainWindow
{
public:

    enum DocumentSaveFormat
    {
        Cayman,
        Bitmap,
        Unknown
    };

    Private(MainWindow* parent) : parent(parent) {}

    QDockWidget* createDock(QWidget* widget, const QString& theme_icon,
                            const QString& object_name);
    QDockWidget* createDock(QWidget* widget, const QIcon& icon,
                            const QString& object_name);
    void initDocks();
    void translateDocks();
    void initMenus();
    void loadSettings();
    void saveSettings();
    QAction* recentFileAction(const QString& file_name);

    void pushRecentFile(const QString& name);

    /**
     * \brief Saves \p doc
     */
    bool save(document::Document* doc, DocumentSaveFormat file_format);

    /**
     * \brief Link colorChanged and setColor between two classes
     */
    template<class A, class B>
        static void linkColor(A* a, B* b)
        {
            QObject::connect(a, &A::colorChanged, b, &B::setColor);
            QObject::connect(b, &B::colorChanged, a, &A::setColor);
        }

    /**
     * \brief Unlink colorChanged and setColor between two classes
     */
    template<class A, class B>
        static void unlinkColor(A* a, B* b)
        {
            QObject::disconnect(a, &A::colorChanged, b, &B::setColor);
            QObject::disconnect(b, &B::colorChanged, a, &A::setColor);
        }

    /**
     * \brief Connect a signal/slot pair in both directions
     */
    template<class A, class R1, class R2, class Arg>
        static void linkSame(A* a, A* b, R1 (A::*signal)(Arg), R2 (A::*slot)(Arg) )
        {
            QObject::connect(a, signal, b, slot);
            QObject::connect(b, signal, a, slot);
        }


    view::GraphicsWidget* widget(int tab)
    {
        return qobject_cast<view::GraphicsWidget*>(main_tab->widget(tab));
    }

    QString documentName(document::Document* doc)
    {
        /// \todo Maybe it could enforce "New Image" as filename on document creation
        if ( !doc->fileName().isEmpty() )
        {
            /// \todo Options to display fileName() or the full path
            return QFileInfo(doc->fileName()).baseName();
        }
        else
        {
            return tr("New Image");
        }
    }

    int addDocument(document::Document* doc, bool set_current);
    void setCurrentTab(int tab);

    void updateTitle();

    view::GraphicsWidget* current_view = nullptr;
    tool::Tool* current_tool = nullptr;

    QList<tool::Tool*> tools;
    QActionGroup* tools_group;

    QUndoGroup undo_group;

    QDockWidget* dock_set_color;
    ColorEditor* color_editor;

    Ui::CurrentColor current_color_selector;
    QDockWidget* dock_current_color;

    color_widgets::ColorPaletteModel palette_model;
    color_widgets::ColorPaletteWidget* palette_widget;
    color_widgets::ColorPaletteWidget* palette_editor;
    QDockWidget* dock_palette;
    QDockWidget* dock_palette_editor;

    QDockWidget* dock_undo_history;

    QDockWidget* dock_layers;
    LayerWidget* layer_widget;

    QDockWidget* dock_tool_options;

    QStringList recent_files;

    MainWindow* parent;

    /**
     * \brief UI layout version, used by saveState
     *
     * If the UI layout changes significantly, this value should be increased
     */
    static constexpr int ui_version = 0;
};

QDockWidget* MainWindow::Private::createDock(QWidget* widget,
                                             const QString& theme_icon,
                                             const QString& object_name)
{
    return createDock(widget, QIcon::fromTheme(theme_icon), object_name);
}

QDockWidget* MainWindow::Private::createDock(QWidget* widget,
                                             const QIcon& icon,
                                             const QString& object_name)
{
    QDockWidget* dock = new QDockWidget(parent);
    dock->setWidget(widget);
    dock->setWindowIcon(icon);
    QAction* action = dock->toggleViewAction();
    action->setIcon(icon);
    menu_docks->addAction(action);
    dock->setStyle(new DockWidgetStyleIcon(dock));
    dock->setObjectName(object_name);
    return dock;
}

void MainWindow::Private::initDocks()
{
    // Color editor
    color_editor = new ColorEditor;
    dock_set_color = createDock(color_editor, "format-stroke-color", "dock_set_color");
    parent->addDockWidget(Qt::RightDockWidgetArea, dock_set_color);

    // Color display
    {
        QWidget* container = new QWidget;
        current_color_selector.setupUi(container);
        dock_current_color = createDock(container, "format-stroke-color", "dock_current_color");
        parent->addDockWidget(Qt::RightDockWidgetArea, dock_current_color);
        linkColor(color_editor, current_color_selector.color);
    }

    // Palette display
    using PalWid = color_widgets::ColorPaletteWidget;
    palette_widget = new PalWid;
    palette_widget->setModel(&palette_model);
    palette_widget->setReadOnly(true);
    dock_palette = createDock(palette_widget, "preferences-desktop-icons", "dock_palette");
    parent->addDockWidget(Qt::RightDockWidgetArea, dock_palette);
    connect(palette_widget, util::overload<const QColor&>(&PalWid::currentColorChanged),
            current_color_selector.color, &color_widgets::ColorSelector::setColor);
    connect(current_color_selector.color, &color_widgets::ColorSelector::colorChanged,
        [this](const QColor& color){
            if ( color != palette_widget->currentColor() )
                palette_widget->setCurrentColor(-1);
    });

    // Palette editor
    palette_editor = new PalWid;
    palette_editor->setModel(&palette_model);
    dock_palette_editor = createDock(palette_editor, "preferences-desktop-icons", "dock_palette_editor");
    parent->addDockWidget(Qt::RightDockWidgetArea, dock_palette_editor);
    parent->tabifyDockWidget(dock_palette, dock_palette_editor);
    dock_palette->raise();
    linkSame(palette_widget, palette_editor, &PalWid::currentRowChanged, &PalWid::setCurrentRow);
    linkSame(palette_widget, palette_editor,
        util::overload<int>(&PalWid::currentColorChanged),
        util::overload<int>(&PalWid::setCurrentColor));

    // Undo history
    QUndoView* undo_view = new QUndoView(&undo_group);
    dock_undo_history = createDock(undo_view, "view-history", "dock_undo_history");
    parent->addDockWidget(Qt::LeftDockWidgetArea, dock_undo_history);

    // Layers
    layer_widget = new LayerWidget();
    dock_layers = createDock(layer_widget, "format-list-unordered", "dock_layers");
    parent->addDockWidget(Qt::LeftDockWidgetArea, dock_layers);

    // Tool Options
    dock_tool_options = createDock(nullptr, "preferences-other", "dock_tool_options");
    parent->addDockWidget(Qt::LeftDockWidgetArea, dock_tool_options);
    parent->tabifyDockWidget(dock_tool_options, dock_undo_history);
    dock_tool_options->raise();

    // Common stuff
    translateDocks();

    for ( auto* dock : parent->findChildren<QDockWidget*>() )
        dock->setEnabled(false);
}

void MainWindow::Private::translateDocks()
{
    dock_set_color->setWindowTitle(tr("Select Color"));
    dock_palette->setWindowTitle(tr("Palette"));
    dock_palette_editor->setWindowTitle(tr("Edit Palette"));
    dock_current_color->setWindowTitle(tr("Current Color"));
    dock_undo_history->setWindowTitle(tr("Action History"));
    dock_tool_options->setWindowTitle(tr("Tool Options"));
    dock_layers->setWindowTitle(tr("Layers"));
}

void MainWindow::Private::initMenus()
{
    // File
    action_new->setShortcut(QKeySequence::New);
    action_open->setShortcut(QKeySequence::Open);
    action_save->setShortcut(QKeySequence::Save);
    action_save_as->setShortcut(QKeySequence::SaveAs);
    action_close->setShortcut(QKeySequence::Close);
    action_print->setShortcut(QKeySequence::Print);
    action_quit->setShortcut(QKeySequence::Quit);

    // Edit
    QAction* action_after_undo_redo = menu_edit->actions()[0];
    QAction *action_undo  = undo_group.createUndoAction(parent);
    action_undo->setIcon(QIcon::fromTheme("edit-undo"));
    action_undo->setShortcut(QKeySequence::Undo);
    menu_edit->insertAction(action_after_undo_redo, action_undo);
    QAction *action_redo  = undo_group.createRedoAction(parent);
    action_redo->setIcon(QIcon::fromTheme("edit-redo"));
    action_redo->setShortcut(QKeySequence::Redo);
    menu_edit->insertAction(action_after_undo_redo, action_redo);

    // Help
    connect(action_about_qt, &QAction::triggered, &QApplication::aboutQt);

    // Tools
    tools_group = new QActionGroup(parent);
    tools_group->setExclusive(true);
}

void MainWindow::Private::loadSettings()
{
    palette_model.addSearchPath("/usr/share/gimp/2.0/palettes/");
    palette_model.addSearchPath("/usr/share/inkscape/palettes/");
    palette_model.addSearchPath("/usr/share/kde4/apps/calligra/palettes/");
    palette_model.load();

    recent_files = settings::get("file/recent", QStringList{});
    if ( !recent_files.empty() )
    {
        menu_open_recent->removeAction(action_no_recent_files);
        for ( const QString& file : recent_files )
            menu_open_recent->addAction(recentFileAction(file));
    }

    /// \todo Save the original state locally, so it can be reset when
    /// the window is open
    SETTINGS_GROUP("ui/mainwindow")
    {
        parent->restoreGeometry(settings::get<QByteArray>("geometry"));
        parent->restoreState(settings::get<QByteArray>("state"), ui_version);
    }
}


void MainWindow::Private::saveSettings()
{
    settings::put("file/recent", recent_files);

    SETTINGS_GROUP("ui/mainwindow")
    {
        settings::put("geometry", parent->saveGeometry());
        settings::put("state", parent->saveState(ui_version));
    }
}

bool MainWindow::Private::save(document::Document* doc, DocumentSaveFormat format)
{
    /// \todo if format == Unknown, determine from file extension

    if ( format == Cayman )
    {
        return document::save_xml(*doc);
    }
    else if ( format == Bitmap )
    {
        QImage image(doc->imageSize(), QImage::Format_ARGB32);
        /// \todo if the format doesn't support alpha, read a color from the settings
        image.fill(Qt::transparent);
        QPainter painter(&image);
        /// \todo detect frame (and fullAlpha from settings?)
        document::visitor::Paint paint(nullptr, &painter, true);
        doc->apply(paint);

        /// \todo some way to determine quality for jpg
        /// (low priority since Jpeg isn't a good format for pixel art)
        return image.save(doc->fileName());
    }

    return false;
}

QAction* MainWindow::Private::recentFileAction(const QString& file_name)
{
    QAction* action = new QAction(file_name, menu_open_recent);
    connect(action, &QAction::triggered, [this, file_name]{
        parent->openTab(file_name, true);
    });
    return action;
}

void MainWindow::Private::pushRecentFile(const QString& name)
{
    menu_open_recent->removeAction(action_no_recent_files);

    int already_there = recent_files.indexOf(name);
    if ( already_there != -1 )
    {
        recent_files.removeAt(already_there);
        QAction* old = menu_open_recent->actions().at(already_there);
        menu_open_recent->removeAction(old);
        delete old;
    }

    recent_files.push_front(name);

    QAction *before = nullptr;
    if ( !menu_open_recent->actions().empty() )
        before = menu_open_recent->actions().front();
    menu_open_recent->insertAction(before, recentFileAction(name));

    int max = settings::get("file/recent_max", 16);
    if ( recent_files.size() > max )
    {
        recent_files.erase(recent_files.begin()+max, recent_files.end());
        auto actions = menu_open_recent->actions();
        for ( auto it = actions.begin()+max; it != actions.end(); ++it )
        {
            menu_open_recent->removeAction(*it);
            delete *it;
        }
    }
}

int MainWindow::Private::addDocument(document::Document* doc, bool set_current)
{
    view::GraphicsWidget* widget = new view::GraphicsWidget(doc);

    undo_group.addStack(&doc->undoStack());

    int tab = main_tab->addTab(widget, documentName(doc));
    connect(&doc->undoStack(), &QUndoStack::cleanChanged, [this, widget]{
        main_tab->setTabIcon(main_tab->indexOf(widget), QIcon::fromTheme("document-save"));
    });

    if ( set_current )
        main_tab->setCurrentIndex(tab);

    return tab;
}

void MainWindow::Private::setCurrentTab(int tab)
{
    if ( current_view )
    {
        current_view->setCurrentTool(nullptr);
        current_view->document()->undoStack().setActive(false);
        Private::unlinkColor(current_view, current_color_selector.color);
        disconnect(layer_widget, nullptr, current_view, nullptr);
        disconnect(current_view, nullptr, layer_widget, nullptr);
    }

    if ( view::GraphicsWidget* widget = this->widget(main_tab->currentIndex()) )
    {
        widget->setCurrentTool(current_tool);
        current_color_selector.color->setColor(widget->color());
        Private::linkColor(widget, current_color_selector.color);
        current_view = widget;
        widget->document()->undoStack().setActive(true);
        layer_widget->setDocument(widget->document());
        connect(layer_widget, &LayerWidget::activeLayerChanged,
                widget, &view::GraphicsWidget::setActiveLayer);
        connect(widget, &view::GraphicsWidget::activeLayerChanged,
                layer_widget, &LayerWidget::setActiveLayer);

        for ( auto* dock : parent->findChildren<QDockWidget*>() )
            dock->setEnabled(true);
    }
    else
    {
        layer_widget->setDocument(nullptr);

        for ( auto* dock : parent->findChildren<QDockWidget*>() )
            dock->setEnabled(false);
    }

    updateTitle();
}

void MainWindow::Private::updateTitle()
{
    int tab = main_tab->currentIndex();

    bool has_documents = main_tab->count();
    bool has_active_document = tab != -1;
    action_save->setEnabled(has_active_document);
    action_save_as->setEnabled(has_active_document);
    action_save_all->setEnabled(has_documents);
    action_close->setEnabled(has_active_document);
    action_close_all->setEnabled(has_documents);
    action_print->setEnabled(has_active_document);

    if ( !has_active_document )
    {
        parent->setWindowTitle(QString());
        return;
    }

    view::GraphicsWidget* view_widget = widget(tab);
    QString title = documentName(view_widget->document());
    if ( !view_widget->document()->undoStack().isClean() )
        title = tr("%1 *").arg(title);
    parent->setWindowTitle(title);
}

#endif // PIXEL_CAYMAN_MAIN_WINDOW_PRIVATE_HPP
