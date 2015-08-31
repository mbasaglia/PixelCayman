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

#include "style/dockwidget_style_icon.hpp"
#include "util.hpp"
#include "ui_main_window.h"
#include "view/graphics_widget.hpp"
#include "document/io.hpp"
#include "settings.hpp"

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

    connect(main_tab, &QTabWidget::tabCloseRequested, [this](int tab) {
        /// \todo If the document is dirty, propmt to save
        delete main_tab->widget(tab);
    });

    current_color_selector.color->setColor(Qt::black);

}

MainWindow::~MainWindow()
{
    save_settings();
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

    recent_files = settings::get("file/recent", QStringList{});
    if ( !recent_files.empty() )
    {

        menu_open_recent->removeAction(action_no_recent_files);
        for ( const QString& file : recent_files )
            menu_open_recent->addAction(file);
    }
}


void MainWindow::save_settings()
{
    settings::put("file/recent", recent_files);
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
    int tab = main_tab->addTab(new view::GraphicsWidget(doc), tr("New Image"));
    main_tab->setCurrentIndex(tab);
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
        QImage image(file_name);

        if ( !image.isNull() )
        {
            document::Document* doc = new document::Document(image, file_name);
            tab = main_tab->addTab(new view::GraphicsWidget(doc), tabText(file_name));
            pushRecentFile(doc->fileName());
        }
    }

    if ( tab != -1 )
    {
        main_tab->setCurrentIndex(tab);
        return true;
    }

    return false;
}

document::Document* MainWindow::currentDocument()
{
    if ( view::GraphicsWidget* widget =
            qobject_cast<view::GraphicsWidget*>(main_tab->currentWidget()) )
        return widget->document();
    return nullptr;
}

bool MainWindow::documentSave()
{
    return save(main_tab->currentIndex(), false);
}

bool MainWindow::documentSaveAs()
{
    return save(main_tab->currentIndex(), true);
}

namespace {
enum DocumentSaveFormat
{
    Cayman,
    Bitmap,
    Unknown
};
} // namespace

bool MainWindow::save(int tab, bool prompt)
{
    view::GraphicsWidget* widget =
        qobject_cast<view::GraphicsWidget*>(main_tab->currentWidget());

    if ( !widget )
        return false;

    document::Document* doc = widget->document();

    if ( !doc )
        return false;

    if ( doc->fileName().isEmpty() )
        prompt = true;

    DocumentSaveFormat format = Unknown;
    if ( prompt )
    {
        // Ensure the the image is visible so the user knows what they are saving
        if ( tab != main_tab->currentIndex() )
            main_tab->setCurrentIndex(tab);

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
        format = DocumentSaveFormat(file_formats.indexOf(save_dialog.selectedNameFilter()));
        doc->setFileName(save_dialog.selectedFiles().front());
        main_tab->setTabText(tab, tabText(doc->fileName()));
        updateTitle();
    }

    if ( doSave(doc, format) )
    {
        /// \todo Mark the document as clean
        pushRecentFile(doc->fileName());
    }

    return false;
}


bool MainWindow::doSave(document::Document* doc, int format)
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

void MainWindow::updateTitle()
{
    int tab = main_tab->currentIndex();

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

QString MainWindow::tabText(QString file_name)
{
    QFileInfo file(file_name);
    /// \todo Options to display fileName() or the full path
    return file.baseName();
}

void MainWindow::pushRecentFile(const QString& name)
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
    menu_open_recent->insertAction(before, new QAction(name, menu_open_recent));
}

