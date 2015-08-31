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

#include "ui_main_window.h"
#include "ui_current_color.h"

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
     * \brief link colorChanged and setColor between two classes
     */
    template<class A, class B>
        static void linkColor(A* a, B* b)
        {
            QObject::connect(a, &A::colorChanged, b, &B::setColor);
            QObject::connect(b, &B::colorChanged, a, &A::setColor);
        }

    template<class A, class R1, class R2, class Arg>
        static void linkSame(A* a, A* b, R1 (A::*signal)(Arg), R2 (A::*slot)(Arg) )
        {
            QObject::connect(a, signal, b, slot);
            QObject::connect(b, signal, a, slot);
        }


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


    translateDocks();
}

void MainWindow::Private::translateDocks()
{
    dock_set_color->setWindowTitle(tr("Select Color"));
    dock_palette->setWindowTitle(tr("Palette"));
    dock_palette_editor->setWindowTitle(tr("Edit Palette"));
    dock_current_color->setWindowTitle(tr("Current Color"));
}

void MainWindow::Private::initMenus()
{
    action_new->setShortcut(QKeySequence::New);
    action_open->setShortcut(QKeySequence::Open);
    action_save->setShortcut(QKeySequence::Save);
    action_save_as->setShortcut(QKeySequence::SaveAs);
    action_close->setShortcut(QKeySequence::Close);
    action_print->setShortcut(QKeySequence::Print);
    action_quit->setShortcut(QKeySequence::Quit);
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

QString MainWindow::Private::tabText(QString file_name)
{
    QFileInfo file(file_name);
    /// \todo Options to display fileName() or the full path
    return file.baseName();
}

QAction* MainWindow::Private::recentFileAction(const QString& file_name)
{
    QAction* action = new QAction(file_name, menu_open_recent);
    connect(action, &QAction::triggered, [this, file_name]{
        parent->openTab(file_name);
    });
    return action;
}

void MainWindow::Private::pushRecentFile(const QString& name)
{
    recent_files.removeOne(name);
    recent_files.push_front(name);
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

    menu_open_recent->removeAction(action_no_recent_files);

    QAction *before = nullptr;
    if ( !menu_open_recent->actions().empty() )
        before = menu_open_recent->actions().front();
    menu_open_recent->insertAction(before, recentFileAction(name));
}


MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), p(new Private(this))
{
    p->setupUi(this);

    p->initDocks();
    p->initMenus();
    p->loadSettings();

    connect(p->main_tab, &QTabWidget::tabCloseRequested, [this](int tab) {
        /// \todo If the document is dirty, propmt to save
        delete p->main_tab->widget(tab);
    });

    p->current_color_selector.color->setColor(Qt::black);

}

MainWindow::~MainWindow()
{
    p->saveSettings();
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

void MainWindow::documentNew()
{
    /// \todo Show dialog to get the size
    /// \todo Keep track of documents and clean up when the document is closed
    document::Document* doc = new document::Document(QSize(32,32));
    int tab = p->main_tab->addTab(new view::GraphicsWidget(doc), tr("New Image"));
    p->main_tab->setCurrentIndex(tab);
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
        int new_tab = openTab(file_name);
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

document::Document* MainWindow::currentDocument()
{
    if ( view::GraphicsWidget* widget =
            qobject_cast<view::GraphicsWidget*>(p->main_tab->currentWidget()) )
        return widget->document();
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
    view::GraphicsWidget* widget =
        qobject_cast<view::GraphicsWidget*>(p->main_tab->currentWidget());

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
        p->main_tab->setTabText(tab, p->tabText(doc->fileName()));
        updateTitle();
    }

    if ( p->save(doc, format) )
    {
        /// \todo Mark the document as clean
        p->pushRecentFile(doc->fileName());
    }

    return false;
}

void MainWindow::updateTitle()
{
    int tab = p->main_tab->currentIndex();

    if ( tab == -1 )
    {
        setWindowTitle(QString());
        return;
    }

    /// \todo If the document is dirty, add a *
    document::Document* doc = currentDocument();
    if ( !doc->fileName().isEmpty() )
        setWindowTitle(doc->fileName());
    else
        setWindowTitle(tr("New Image"));
}

int MainWindow::openTab(const QString& file_name)
{
    int tab = -1;

    QImage image(file_name);

    if ( !image.isNull() )
    {
        document::Document* doc = new document::Document(image, file_name);
        tab = p->main_tab->addTab(new view::GraphicsWidget(doc), p->tabText(file_name));
        p->pushRecentFile(doc->fileName());
    }

    return tab;
}
