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
#ifndef PIXEL_CAYMAN_APPLICATION_HPP
#define PIXEL_CAYMAN_APPLICATION_HPP

#include <QApplication>
#include "settings.hpp"

namespace cayman {

class Application : public QApplication
{
    Q_OBJECT
public:
    Application(int &argc, char **argv);
    ~Application();

    void initSubsystems();

    void setStyleSheetFile(const QString& filename);

    /**
     * \brief Localized language name from the locale code
     */
    QString languageName(const QString& code) const;

    /**
     * \brief Set translation language
     */
    bool setLanguage(const QString& code);

    /**
     * \brief List of available language codes
     */
    QStringList availableLanguages() const
    {
        return translators.keys();
    }

    QString currentLanguage() const
    {
        return current_language;
    }

signals:
    /**
     * \brief Emitted when the language changes
     */
    void languageChanged(const QString& code);

protected:
    bool event(QEvent* event) override;

private:
    void initInfo();
    void initPlugins();
    void initFormats();
    void initTools();
    void initLanguages();

    settings::Settings* settings_;
    QMap<QString, QTranslator*> translators; ///< Maps language code -> translator
    QString current_language;
    QString default_language = "en_US";
};


} // namespace cayman
#endif // PIXEL_CAYMAN_APPLICATION_HPP

#ifdef qApp
#   undef qApp
#endif
#define qApp (static_cast<cayman::Application*>(QCoreApplication::instance()))
