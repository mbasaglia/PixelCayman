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
#include "ui_confirm_close_dialog.h"

#include <QCheckBox>
#include <QSignalMapper>

class ConfirmCloseDialog::Private : public Ui::ConfirmCloseDialog
{
public:
    /**
     * \brief Simple helper class
     */
    struct Save_File
    {
        QString file_name;
        bool save;
        Save_File(QString file_name="") : file_name(file_name), save(true) {}
    };

    QMap<int,Save_File> files;
    QSignalMapper mapper;
};   
    
ConfirmCloseDialog::ConfirmCloseDialog(QWidget *parent) :
    QDialog(parent), p(new Private)
{
    p->setupUi(this);
    connect(&p->mapper,SIGNAL(mapped(int)),SLOT(file_toogled(int)));
}

ConfirmCloseDialog::~ConfirmCloseDialog()
{
    delete p;
}

void ConfirmCloseDialog::add_file(int index, QString name)
{
     p->files[index] = name;
     int row = p->tableWidget->rowCount();
     p->tableWidget->insertRow(row);
     QCheckBox* cb = new QCheckBox(name);
     cb->setChecked(true);
     p->mapper.setMapping(cb,index);
     connect(cb,SIGNAL(toggled(bool)),&p->mapper,SLOT(map()));
     p->tableWidget->setCellWidget(row,0,cb);
}

bool ConfirmCloseDialog::has_files() const
{ 
    return !p->files.empty();
}

QList<int> ConfirmCloseDialog::save_files()
{
    QList<int> r;
    foreach(int i, p->files.keys())
    {
        if ( p->files[i].save )
            r.push_back(i);
    }
    return r;
}

void ConfirmCloseDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
        case QEvent::LanguageChange:
            p->retranslateUi(this);
            break;
        default:
            break;
    }
}

void ConfirmCloseDialog::file_toogled(int i)
{
    if ( p->files.contains(i) )
        p->files[i].save = !p->files[i].save;
}

void ConfirmCloseDialog::on_button_dont_save_clicked()
{
    done(DontSave);
}
