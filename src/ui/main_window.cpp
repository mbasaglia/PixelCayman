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
#include "main_window_p.hpp"

#include <QImageReader>
#include <QImageWriter>
#include <QFileDialog>
#include <QMessageBox>

#include "data.hpp"
#include "confirm_close_dialog.hpp"

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
        p->setCurrentTab(tab);
    });

    p->current_color_selector.color->setColor(Qt::black);

}

MainWindow::~MainWindow()
{
    p->saveSettings();
    p->dock_tool_options->setWidget(nullptr);
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
    document::AbstractFormat* format = nullptr;
    if ( p->current_view )
    {
        if ( !p->current_view->document()->fileName().isEmpty() )
            default_dir = QFileInfo(p->current_view->document()->fileName()).dir().path();
        /// \todo set format to the preferred format for the document
    }

    auto action = document::Formats::Action::Open;
    QFileDialog open_dialog(this, tr("Open Image"), default_dir);
    open_dialog.setFileMode(QFileDialog::ExistingFiles);
    open_dialog.setAcceptMode(QFileDialog::AcceptOpen);
    open_dialog.setNameFilters(document::formats().nameFilters(action));
    if ( format )
        open_dialog.selectNameFilter(format->nameFilter(action));

    if ( !open_dialog.exec() )
        return false;

    format = document::formats().formatFromNameFilter(open_dialog.selectedNameFilter(), action);

    int tab = -1;
    for ( const QString& file_name : open_dialog.selectedFiles() )
    {
        int new_tab = openTab(file_name, false, format);
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

    auto action = document::Formats::Action::Save;
    /// \todo Store preferred format in the document
    document::AbstractFormat* format = document::formats().formatFromFileName(doc->fileName(),action);
    if ( prompt || !format )
    {
        // Ensure the the image is visible so the user knows what they are saving
        if ( tab != p->main_tab->currentIndex() )
            p->main_tab->setCurrentIndex(tab);

        QFileDialog save_dialog(this, tr("Save Image"), doc->fileName());
        save_dialog.setFileMode(QFileDialog::AnyFile);
        save_dialog.setAcceptMode(QFileDialog::AcceptSave);
        save_dialog.setNameFilters(document::formats().nameFilters(action));
        if ( format )
            save_dialog.selectNameFilter(format->nameFilter(action));

        if ( !save_dialog.exec() )
            return false;

        /// \todo Ask confirmation if the file exists
        /// QFileDialog should already do this but it doesnt,
        /// At least not with the native KDE dialog.

        QString selected_file = save_dialog.selectedFiles().front();

        format = document::formats().formatFromNameFilter(save_dialog.selectedNameFilter(), action);
        if ( !format )
        {
            format = document::formats().formatFromFileName(selected_file, action);
            /// \todo Show an error message (Unknown format)
            if ( !format )
                return false;
        }

        doc->setFileName(selected_file);
        p->main_tab->setTabText(tab, p->documentName(doc));
        p->updateTitle();
    }

    if ( format->save(doc) )
    {
        doc->undoStack().setClean();
        p->pushRecentFile(doc->fileName());
    }

    /// \todo Show an error message (Error while saving)
    return false;
}

int MainWindow::openTab(const QString& file_name, bool set_current,
                        document::AbstractFormat* format)
{
    if ( !format )
    {
        format = document::formats().formatFromFileName(file_name,document::Formats::Action::Open);
        if ( !format )
            return -1; /// \todo Error message (unknown format)
    }

    /// \todo Open should set the preferred format
    document::Document* doc = format->open(file_name);
    if ( doc )
    {
        p->pushRecentFile(doc->fileName());
        return p->addDocument(doc, set_current);
    }

    return -1; /// \todo Error message (error while opening)
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

    if ( prompt && !widget->document()->undoStack().isClean() )
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
        p->setCurrentView(nullptr);
    }
    
    p->undo_group.removeStack(&widget->document()->undoStack());
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
    /// \todo Retranslate them(?)
    QAction* tool_action = new QAction(tool->icon(), tool->name(), p->tools_group);
    tool_action->setCheckable(true);
    p->menu_tools->addAction(tool_action);
    p->tool_bar->addAction(tool_action);
    connect(tool_action, &QAction::triggered, [this, tool](bool checked){
        ::tool::Tool* used_tool = checked ? tool : nullptr;
        p->current_tool = used_tool;
        if ( p->current_view )
            p->current_view->setCurrentTool(used_tool);
        p->dock_tool_options->setWidget(used_tool ? used_tool->optionsWidget() : nullptr);
    });
}

bool MainWindow::documentCloseAll()
{
    if ( p->main_tab->count() != 0 )
    {
        ConfirmCloseDialog dlg(this);

        bool has_dirty_documents = false;
        for ( int i = 0; i < p->main_tab->count(); i++ )
        {
            auto widget = p->widget(i);
            if ( !widget->document()->undoStack().isClean() )
            {
                dlg.addFile(i, p->documentName(widget->document()));
                has_dirty_documents = true;
            }
        }

        if ( has_dirty_documents )
        {
            if ( !dlg.exec() )
                return false;

            /// \todo should return false here only if the user has canceled the save
            for ( int i : dlg.saveFiles() )
                if ( !save(i, false) )
                    return false;
        }
        
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
