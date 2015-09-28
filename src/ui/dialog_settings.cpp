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
#include "cayman/application.hpp"

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
        cayman::settings::put("file/confirm_close", check_warn_unsaved->isChecked());
        QString lang = combo_language->currentData().toString();
        cayman::settings::put("language", lang);
        qApp->setLanguage(lang);

        // Ui
        if ( clear_ui )
            cayman::settings::Settings::instance().remove("ui/mainwindow");

        QString stylesheet = cayman::settings::get("ui/stylesheet", QString());
        if ( text_ui_stylesheet->text() != stylesheet )
        {
            stylesheet = text_ui_stylesheet->text();
            cayman::settings::put("ui/stylesheet", stylesheet);
            qApp->setStyleSheetFile(stylesheet);
        }

            // Toolbars
        SETTINGS_GROUP("ui/toolbars")
        {
            if ( combo_tb_icon_size->currentIndex() == -1 )
                cayman::settings::Settings::instance().remove("icon_size");
            else
                cayman::settings::put("icon_size", combo_tb_icon_size->currentData().toInt());

            if ( combo_tb_button_style->currentIndex() == -1 )
                cayman::settings::Settings::instance().remove("button_style");
            else
                cayman::settings::put("button_style",
                    combo_tb_button_style->currentData().toInt());
        }

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
    p->check_warn_unsaved->setChecked(cayman::settings::get<bool>("file/confirm_close"));
    for ( const auto& lang : qApp->availableLanguages() )
        p->combo_language->addItem(qApp->languageName(lang), lang);
    p->combo_language->setCurrentIndex(
        p->combo_language->findData(qApp->currentLanguage())
    );

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
    p->text_ui_stylesheet->setText(cayman::settings::get<QString>("ui/stylesheet"));

    // Toolbars
    SETTINGS_GROUP("ui/toolbars")
    {
        p->combo_tb_icon_size->addItem(tr("Small (16x16)"), 16);
        p->combo_tb_icon_size->addItem(tr("Medium (22x22)"), 22);
        p->combo_tb_icon_size->addItem(tr("Large (48x48)"), 48);
        p->combo_tb_icon_size->addItem(tr("Huge (64x64)"), 64);
        p->combo_tb_icon_size->setCurrentIndex(p->combo_tb_icon_size->findData(
            cayman::settings::get<int>("icon_size")));

        p->combo_tb_button_style->addItem(tr("Icon Only"), Qt::ToolButtonIconOnly);
        p->combo_tb_button_style->addItem(tr("Text Only"), Qt::ToolButtonTextOnly);
        p->combo_tb_button_style->addItem(tr("Text Beside Icon"), Qt::ToolButtonTextBesideIcon);
        p->combo_tb_button_style->addItem(tr("Text Under Icon"), Qt::ToolButtonTextUnderIcon);
        p->combo_tb_button_style->addItem(tr("Follow System Style"), Qt::ToolButtonFollowStyle);
        p->combo_tb_button_style->setCurrentIndex(p->combo_tb_button_style->findData(
            cayman::settings::get<int>("button_style")));
        /// \todo Toolbar editor
    }

    /// \todo Hooks for plugin settings
    /// \todo Tool settings
    /// \todo Plugin repo
    /// \todo Language
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

