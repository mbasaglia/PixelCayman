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

#include <QGraphicsView>
#include <QImageReader>
#include <QImageWriter>
#include <QFileInfo>
#include <QFileDialog>
#include <QDockWidget>
#include <QUndoGroup>
#include <QUndoView>
#include <QMessageBox>

#include "color_editor.hpp"
#include "color_palette_widget.hpp"
#include "color_palette_model.hpp"
#include "color_selector.hpp"
#include "color_line_edit.hpp"
#include "style/dockwidget_style_icon.hpp"
#include "util.hpp"
#include "view/graphics_widget.hpp"
#include "document/io.hpp"
#include "settings.hpp"
#include "data.hpp"
#include "tool/tool.hpp"
#include "confirm_close_dialog.hpp"

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

    QDockWidget* createDock(QWidget* widget, const QString& theme_icon);
    QDockWidget* createDock(QWidget* widget, const QIcon& icon);
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

    QDockWidget* dock_undo_hitory;

    QStringList recent_files;

    MainWindow* parent;
};

QDockWidget* MainWindow::Private::createDock(QWidget* widget, const QString& theme_icon)
{
    return createDock(widget, QIcon::fromTheme(theme_icon));
}

QDockWidget* MainWindow::Private::createDock(QWidget* widget, const QIcon& icon)
{
    QDockWidget* dock = new QDockWidget(parent);
    dock->setWidget(widget);
    dock->setWindowIcon(icon);
    QAction* action = dock->toggleViewAction();
    action->setIcon(icon);
    menu_docks->addAction(action);
    dock->setStyle(new DockWidgetStyleIcon(dock));
    return dock;
}

void MainWindow::Private::initDocks()
{
    color_editor = new ColorEditor;
    dock_set_color = createDock(color_editor, "format-stroke-color");
    parent->addDockWidget(Qt::RightDockWidgetArea, dock_set_color);

    {
        QWidget* container = new QWidget;
        current_color_selector.setupUi(container);
        dock_current_color = createDock(container, "format-stroke-color");
        parent->addDockWidget(Qt::RightDockWidgetArea, dock_current_color);
        linkColor(color_editor, current_color_selector.color);
    }

    using PalWid = color_widgets::ColorPaletteWidget;
    palette_widget = new PalWid;
    palette_widget->setModel(&palette_model);
    palette_widget->setReadOnly(true);
    dock_palette = createDock(palette_widget, "preferences-desktop-icons");
    parent->addDockWidget(Qt::RightDockWidgetArea, dock_palette);
    connect(palette_widget, util::overload<const QColor&>(&PalWid::currentColorChanged),
            current_color_selector.color, &color_widgets::ColorSelector::setColor);
    connect(current_color_selector.color, &color_widgets::ColorSelector::colorChanged,
        [this](const QColor& color){
            if ( color != palette_widget->currentColor() )
                palette_widget->setCurrentColor(-1);
    });

    palette_editor = new PalWid;
    palette_editor->setModel(&palette_model);
    dock_palette_editor = createDock(palette_editor, "preferences-desktop-icons");
    parent->addDockWidget(Qt::RightDockWidgetArea, dock_palette_editor);
    parent->tabifyDockWidget(dock_palette, dock_palette_editor);
    dock_palette->raise();
    linkSame(palette_widget, palette_editor, &PalWid::currentRowChanged, &PalWid::setCurrentRow);
    linkSame(palette_widget, palette_editor,
        util::overload<int>(&PalWid::currentColorChanged),
        util::overload<int>(&PalWid::setCurrentColor));

    QUndoView* undo_view = new QUndoView(&undo_group);
    dock_undo_hitory = createDock(undo_view, "view-history");
    parent->addDockWidget(Qt::LeftDockWidgetArea, dock_undo_hitory);

    translateDocks();
}

void MainWindow::Private::translateDocks()
{
    dock_set_color->setWindowTitle(tr("Select Color"));
    dock_palette->setWindowTitle(tr("Palette"));
    dock_palette_editor->setWindowTitle(tr("Edit Palette"));
    dock_current_color->setWindowTitle(tr("Current Color"));
    dock_undo_hitory->setWindowTitle(tr("Action History"));
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
}


void MainWindow::Private::saveSettings()
{
    settings::put("file/recent", recent_files);
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

    undo_group.addStack(&widget->undoStack());

    int tab = main_tab->addTab(widget, documentName(doc));
    connect(&widget->undoStack(), &QUndoStack::cleanChanged, [this, widget]{
        main_tab->setTabIcon(main_tab->indexOf(widget), QIcon::fromTheme("document-save"));
    });

    if ( set_current )
        main_tab->setCurrentIndex(tab);

    return tab;
}

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), p(new Private(this))
{
    p->setupUi(this);

    /// \todo Add this stuff in a function in data to work around
    /// the QIcon::fromTheme bug
    QIcon icon = QIcon::fromTheme("pixel-cayman");
    if ( icon.isNull() )
    {
        icon.addFile(::data().readable("icons/pixel-cayman/16x16/pixel-cayman-eye.png"));
        icon.addFile(::data().readable("icons/pixel-cayman/22x22/pixel-cayman-eye.png"));
        icon.addFile(::data().readable("icons/pixel-cayman/24x24/pixel-cayman-eye.png"));
        //icon.addFile(::data().readable("icons/pixel-cayman/32x32/pixel-cayman-eye.png"));
    }
    setWindowIcon(icon);

    p->initDocks();
    p->initMenus();
    p->loadSettings();

    connect(p->main_tab, &QTabWidget::tabCloseRequested,
            this, &MainWindow::closeTabPrompt);

    connect(p->main_tab, &QTabWidget::currentChanged, [this](int tab) {

        if ( p->current_view )
        {
            p->current_view->setCurrentTool(nullptr);
            Private::unlinkColor(p->current_view, p->current_color_selector.color);
        }

        if ( view::GraphicsWidget* widget = p->widget(p->main_tab->currentIndex()) )
        {
            widget->setCurrentTool(p->current_tool);
            p->current_color_selector.color->setColor(widget->color());
            Private::linkColor(widget, p->current_color_selector.color);
            p->current_view = widget;
            p->undo_group.setActiveStack(&widget->undoStack());
        }

        p->updateTitle();
    });

    p->current_color_selector.color->setColor(Qt::black);

}

MainWindow::~MainWindow()
{
    p->saveSettings();
    delete p;
}

void MainWindow::changeEvent(QEvent* event)
{
    if ( event->type() == QEvent::LanguageChange )
    {
        p->retranslateUi(this);
        p->current_color_selector.retranslateUi(p->dock_current_color->widget());
        p->translateDocks();
    }

    QMainWindow::changeEvent(event);
}

void MainWindow::setActiveColor(const QColor& color)
{
    p->color_editor->setColor(color);
}

bool MainWindow::documentNew()
{
    /// \todo Show dialog to get the size
    document::Document* doc = new document::Document(QSize(32,32));
    p->addDocument(doc, true);
    return true;
}

bool MainWindow::documentOpen()
{
    QString default_dir;
    if ( document::Document* current = currentDocument() )
    {
        if ( !current->fileName().isEmpty() )
            default_dir = QFileInfo(current->fileName()).dir().path();
    }

    QString image_formats;
    for ( const auto& ba : QImageReader::supportedImageFormats() )
        image_formats += " *."+QString(ba);

    QStringList file_formats = QStringList()
        << tr("All Bitmap Images (%1)").arg(image_formats)
        << tr("All Files (*)");
    QFileDialog open_dialog(this, tr("Open Image"), default_dir);
    open_dialog.setFileMode(QFileDialog::ExistingFiles);
    open_dialog.setAcceptMode(QFileDialog::AcceptOpen);
    open_dialog.setNameFilters(file_formats);

    if ( !open_dialog.exec() )
        return false;

    int tab = -1;
    for ( const QString& file_name : open_dialog.selectedFiles() )
    {
        int new_tab = openTab(file_name, false);
        if ( new_tab != -1 )
            tab = new_tab;
    }

    if ( tab != -1 )
    {
        p->main_tab->setCurrentIndex(tab);
        return true;
    }

    return false;
}

/// \todo Maybe can be removed (and use Private::widget)
document::Document* MainWindow::currentDocument()
{
    if ( p->current_view )
        return p->current_view->document();
    return nullptr;
}

bool MainWindow::documentSave()
{
    return save(p->main_tab->currentIndex(), false);
}

bool MainWindow::documentSaveAs()
{
    return save(p->main_tab->currentIndex(), true);
}

bool MainWindow::save(int tab, bool prompt)
{
    view::GraphicsWidget* widget = p->widget(tab);
    if ( !widget )
        return false;

    document::Document* doc = widget->document();

    if ( !doc )
        return false;

    if ( doc->fileName().isEmpty() )
        prompt = true;

    Private::DocumentSaveFormat format = Private::Unknown;
    if ( prompt )
    {
        // Ensure the the image is visible so the user knows what they are saving
        if ( tab != p->main_tab->currentIndex() )
            p->main_tab->setCurrentIndex(tab);

        QString image_formats;
        for ( const auto& ba : QImageWriter::supportedImageFormats() )
            image_formats += " *."+QString(ba);

        QStringList file_formats = QStringList()
            << tr("Cayman Files (*.mela)")
            /// \todo split "All Bitmap Images" in png etc
            << tr("All Bitmap Images (%1)").arg(image_formats)
            << tr("All Files (*)");

        /// \todo if doc->filename is a bitmap, select that filter
        QFileDialog save_dialog(this, tr("Save Image"), doc->fileName());
        save_dialog.setFileMode(QFileDialog::AnyFile);
        save_dialog.setAcceptMode(QFileDialog::AcceptSave);
        save_dialog.setNameFilters(file_formats);

        if ( !save_dialog.exec() )
            return false;

        /// \todo Ask confirmation if the file exists
        format = Private::DocumentSaveFormat(file_formats.indexOf(save_dialog.selectedNameFilter()));
        doc->setFileName(save_dialog.selectedFiles().front());
        p->main_tab->setTabText(tab, p->documentName(doc));
        p->updateTitle();
    }

    if ( p->save(doc, format) )
    {
        widget->undoStack().setClean();
        p->pushRecentFile(doc->fileName());
    }

    return false;
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
    if ( !view_widget->undoStack().isClean() )
        title = tr("%1 *").arg(title);
    parent->setWindowTitle(title);
}

int MainWindow::openTab(const QString& file_name, bool set_current)
{
    QImage image(file_name);

    if ( !image.isNull() )
    {
        document::Document* doc = new document::Document(image, file_name);
        p->pushRecentFile(doc->fileName());
        return p->addDocument(doc, set_current);
    }

    return -1;
}

bool MainWindow::documentClose()
{
    return closeTabPrompt(p->main_tab->currentIndex());
}

bool MainWindow::closeTab(int tab, bool prompt)
{
    view::GraphicsWidget* widget = p->widget(tab);

    if ( !widget )
        return false;

    if ( prompt && !widget->undoStack().isClean() )
    {
         int reply = QMessageBox::question(this,
            tr("Close file"),
            tr("\"%1\" has been modified.\nDoyou want to save the changes")
                .arg(p->documentName(widget->document())),
            QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel,
            QMessageBox::Yes
        );
        if ( reply == QMessageBox::Yes )
            save(tab, false);
        else if ( reply == QMessageBox::Cancel )
            return false;
    }

    if ( widget == p->current_view )
    {
        p->current_view->setCurrentTool(nullptr);
        p->current_view = nullptr;
    }

    p->undo_group.removeStack(&widget->undoStack());
    delete widget->document();
    delete widget;

    return true;
}

bool MainWindow::closeTabPrompt(int tab)
{
    return closeTab(tab, true);
}

void MainWindow::addTool(::tool::Tool* tool)
{
    if ( !tool || p->tools.contains(tool) )
        return;

    p->tools.push_back(tool);
    /// \todo Retranslate them
    QAction* tool_action = new QAction(tool->icon(), tool->name(), p->tools_group);
    tool_action->setCheckable(true);
    p->menu_tools->addAction(tool_action);
    p->tool_bar->addAction(tool_action);
    connect(tool_action, &QAction::triggered, [this, tool](bool checked){
        ::tool::Tool* used_tool = checked ? tool : nullptr;
        p->current_tool = used_tool;
        if ( p->current_view )
            p->current_view->setCurrentTool(used_tool);
    });
}

bool MainWindow::documentCloseAll()
{
    if ( p->main_tab->count() != 0 )
    {
        ConfirmCloseDialog dlg(this);

        for ( int i = 0; i < p->main_tab->count(); i++ )
        {
            auto widget = p->widget(i);
            if ( !widget->undoStack().isClean() )
                dlg.addFile(i, p->documentName(widget->document()));
        }

        if ( !dlg.exec() )
            return false;

        /// \todo should return false here only if the user has canceled the save
        for ( int i : dlg.saveFiles() )
            if ( !save(i, false) )
                return false;

        while ( p->main_tab->count() != 0 )
            closeTab(0, false);
    }

    return true;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if ( !documentCloseAll() )
    {
        event->ignore();
        return;
    }

    QMainWindow::closeEvent(event);
}
