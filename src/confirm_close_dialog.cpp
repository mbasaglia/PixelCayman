/**
 * \file
 *
 * \author Mattia Basaglia
 *
 * \copyright Copyright (C) 2012-2015 Mattia Basaglia
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

class ConfirmCloseDialog::Private : public Ui::ConfirmCloseDialog
{
public:
    /**
     * \brief Simple helper class
     */
    struct SaveFile
    {
        QString file_name;
        bool save;
        SaveFile(QString file_name = "") : file_name(file_name), save(true) {}
    };

    QMap<int,SaveFile> files;
};   
    
ConfirmCloseDialog::ConfirmCloseDialog(QWidget *parent) :
    QDialog(parent), p(new Private)
{
    p->setupUi(this);
}

ConfirmCloseDialog::~ConfirmCloseDialog()
{
    delete p;
}

void ConfirmCloseDialog::addFile(int index, QString name)
{
    p->files[index] = name;
    int row = p->tableWidget->rowCount();
    p->tableWidget->insertRow(row);
    QCheckBox* checkbox = new QCheckBox(name);
    checkbox->setChecked(true);
    connect(checkbox, &QCheckBox::toggled, [this, index](bool toggled){
        if ( p->files.contains(index) )
            p->files[index].save = toggled;
    });
    p->tableWidget->setCellWidget(row, 0, checkbox);
}

bool ConfirmCloseDialog::hasFiles() const
{ 
    return !p->files.empty();
}

QList<int> ConfirmCloseDialog::saveFiles() const
{
    QList<int> ret;
    for ( int i : p->files.keys() )
    {
        if ( p->files[i].save )
            ret.push_back(i);
    }
    return ret;
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

void ConfirmCloseDialog::on_button_dont_save_clicked()
{
    for ( int i : p->files.keys() )
    {
        p->files[i].save = false;
    }
    
    accept();
}
