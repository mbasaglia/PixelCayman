/*
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
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "dialog_settings.hpp"

#include <QEvent>
#include <QFileDialog>
#include "ui_dialog_settings.h"
#include "cayman/settings.hpp"

class DialogSettings::Private : public Ui::DialogSettings
{
public:
    void applySettings()
    {
        if ( clear_all )
        {
            cayman::settings::Settings::instance().clear();
            return;
        }

        // General
        cayman::settings::put("file/recent_max", spin_recent_files->value());
        if ( clear_recent )
            cayman::settings::put("file/recent", QStringList{});

        // Ui
        if ( clear_ui )
        {
            cayman::settings::Settings::instance().remove("ui/mainwindow");
        }
        /// \todo Stylesheet

    }


    bool clear_recent = false;
    bool clear_all = false;
    bool clear_ui = false;
};


DialogSettings::DialogSettings(QWidget* parent)
    : QDialog(parent), p(new Private)
{
    p->setupUi(this);

    // General
    p->spin_recent_files->setValue(cayman::settings::get<int>("file/recent_max"));
    connect(p->button_recent_files, &QPushButton::clicked, [this]{
        p->clear_recent = true;
    });
    connect(p->button_clear_all, &QPushButton::clicked, [this]{
        p->clear_recent = true;
    });

    // Ui
    connect(p->button_ui_reset, &QPushButton::clicked, [this]{
        p->clear_recent = true;
    });
    connect(p->button_ui_reset, &QPushButton::clicked, [this]{
        p->clear_ui = true;
    });
    connect(p->button_ui_stylesheet, &QPushButton::clicked, [this]{
        QString file = QFileDialog::getOpenFileName(this, tr("Open Stylesheet"),
            QString(), tr("Qt StyleSheets (*.qss);;All Files (*)"));
        if ( !file.isEmpty() )
            p->text_ui_stylesheet->setText(file);
    });


    /// \todo Hooks for plugin settings
    /// \todo Tool settings
    /// \todo Plugin repo
    /// \todo Language
    /// \todo Toolbars
    /// \todo Shortcuts
    /// \todo File format options
}

DialogSettings::~DialogSettings() = default;

void DialogSettings::changeEvent(QEvent* event)
{
    if ( event->type() == QEvent::LanguageChange )
    {
        p->retranslateUi(this);
    }

    QWidget::changeEvent(event);
}

void DialogSettings::accept()
{
    p->applySettings();
    QDialog::accept();
}

