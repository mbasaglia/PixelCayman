/**
 * \file
 *
 * \author Mattia Basaglia
 *
 * \copyright Copyright (C) 2012-2014 Mattia Basaglia
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
#ifndef DIALOG_CONFIRM_CLOSE_HPP
#define DIALOG_CONFIRM_CLOSE_HPP

#include <QDialog>

/**
 * \brief A dialog asking to whether to save multiple files
 * 
 * exec() might return QDialog::Accepted, QDialog::Rejected or 
 * ConfirmCloseDialog::DontSave.
 * 
 * * \b Accepted is used when the user wants to close the files and
 *               save the selected ones.
 * * \b DontSave is used when the user wants to close all the files
 *               but they don't want to save any of them.
 * * \b Rejected means that the user doesn't want to close after all.
 */
class ConfirmCloseDialog : public QDialog
{
    Q_OBJECT
    
public:
    static const int DontSave = Accepted+1;

    explicit ConfirmCloseDialog(QWidget *parent = 0);
    ~ConfirmCloseDialog();

    /**
     * \brief Insert file to be saved
     * \param index  An identifier used by the caller to associate to the file
     * \param name   File name
     * \post  has_file() == true
     * \note  \c index should be unique, for a single file
     */
    void addFile(int index, QString name);

    bool hasFiles() const;

    /**
     * \brief Get files to be saved
     * \return A list of indices representing the files that need to be saved
     */
    QList<int> saveFiles() const;
    
protected:
    void changeEvent(QEvent *e);

private slots:
    void fileToogled(int i);
    void on_button_dont_save_clicked();
    
private:
    class Private;
    Private* p;
};

#endif // DIALOG_CONFIRM_CLOSE_HPP
