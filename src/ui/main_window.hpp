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

class MainWindow : public QMainWindow
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
    
    /**
     * \brief Open a file
     * \return The index of the newly opened tab, -1 on failure
     */
    int openTab(const QString& file_name);

    document::Document* currentDocument();

private:
    class Private;
    Private* p;
};

#endif // MAIN_WINDOW_HPP
