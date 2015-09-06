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

#include <QMainWindow>
#include "document/document.hpp"
#include "tool/tool.hpp"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

    /**
     * \brief Makes a tool available to the window
     */
    void addTool(::tool::Tool* tool);

public slots:
    /**
     * \brief Changes the color used by drawing tools
     */
    void setActiveColor(const QColor& color);
    /**
     * \brief Creates a new document
     * \returns \b true if the document has been created successfully
     */
    bool documentNew();
    /**
     * \brief Opens a file that the user has selected
     * \returns \b true if the document has been opened successfully
     */
    bool documentOpen();
    /**
     * \brief Saves the current document
     * \returns \b true if the document has been saved
     */
    bool documentSave();
    /**
     * \brief Saves the current document, always propmting for a file name
     * \returns \b true if the document has been saved
     */
    bool documentSaveAs();
    /**
     * \brief Closes the document
     * \returns \b true if the document has been closed
     */
    bool documentClose();
    /**
     * \brief Closes all documents
     * \returns \b false if the user has canceled the operation
     */
    bool documentCloseAll();

protected:
    void changeEvent(QEvent* event) override;
    void closeEvent(QCloseEvent *event) override;

    /**
     * \brief Save the document
     * \param tab Index of the tab with the document to be saved
     * \param prompt Whether to always prompt for a file name
     * \return Whether the file has been saved successfully
     */
    bool save(int tab, bool prompt);
    
    /**
     * \brief Open a file
     * \return The index of the newly opened tab, -1 on failure
     */
    int openTab(const QString& file_name, bool set_current);

    /**
     * \brief Closes a tab
     * \param tab Index of the tab with the document to be closed
     * \param prompt Whether to prompt a save on modified documents
     * \returns \b true if the tab has been closed
     */
    bool closeTab(int tab, bool prompt);

    /**
     * \brief Same as closeTab(true, true)
     */
    bool closeTabPrompt(int tab);

    /**
     * \brief Document at the current tab
     */
    document::Document* currentDocument();

private:
    class Private;
    Private* p;
};

#endif // MAIN_WINDOW_HPP
