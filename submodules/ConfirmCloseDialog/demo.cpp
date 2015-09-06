/**
 * \file
 *
 * \author Mattia Basaglia
 *
 * \copyright Copyright (C) 2014 Mattia Basaglia
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
 *
 */
#include <QApplication>
#include <QDebug>
#include "confirm_close_dialog.hpp"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    QVector<QString> files;
    files.push_back("File 1");
    files.push_back("File 2");
    files.push_back("File 3");
    
    ConfirmCloseDialog dlg;
    for ( int i = 0; i < files.size(); i++ )
        dlg.addFile(i, files[i]);
    
    if ( dlg.exec() == QDialog::Accepted )
    {
        QList<int> save = dlg.saveFiles();
        foreach ( int i, save )
            qDebug() << files[i];
    }
    
    return 0;
}

