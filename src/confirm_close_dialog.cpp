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
#include "confirm_close_dialog.hpp"
#include <QCheckBox>

ConfirmCloseDialog::ConfirmCloseDialog(QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);
    connect(&mapper,SIGNAL(mapped(int)),SLOT(file_toogled(int)));
}

void ConfirmCloseDialog::add_file(int index, QString name)
{
     files[index] = name;
     int row = tableWidget->rowCount();
     tableWidget->insertRow(row);
     QCheckBox* cb = new QCheckBox(name);
     cb->setChecked(true);
     mapper.setMapping(cb,index);
     connect(cb,SIGNAL(toggled(bool)),&mapper,SLOT(map()));
     tableWidget->setCellWidget(row,0,cb);
}

QList<int> ConfirmCloseDialog::save_files()
{
    QList<int> r;
    foreach(int i, files.keys())
    {
        if ( files[i].save )
            r.push_back(i);
    }
    return r;
}

void ConfirmCloseDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
        case QEvent::LanguageChange:
            retranslateUi(this);
            break;
        default:
            break;
    }
}

void ConfirmCloseDialog::file_toogled(int i)
{
    if ( files.contains(i) )
        files[i].save = !files[i].save;
}

void ConfirmCloseDialog::on_button_dont_save_clicked()
{
    done(DontSave);
}
