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
 *
 */
#include "application.hpp"

#include <QIcon>
#include <QLocale>
#include <QTranslator>

#include "data.hpp"
#include "io/bitmap.hpp"
#include "io/xml.hpp"
#include "message.hpp"
#include "plugin/library_plugin.hpp"
#include "plugin/plugin_api.hpp"
#include "plugin/plugin.hpp"
#include "tool/eraser.hpp"
#include "tool/flood_fill.hpp"
#include "tool/registry.hpp"

namespace cayman {

Application::Application(int& argc, char** argv)
    : QApplication(argc, argv)
{
    initInfo();
    settings_ = new settings::Settings;
    QString stylesheet = settings_->get("ui/stylesheet", QString());
    if ( !stylesheet.isEmpty() )
        setStyleSheetFile(stylesheet);
}

Application::~Application()
{
    delete settings_;
}

void Application::setStyleSheetFile(const QString& filename)
{
    if ( !filename.isEmpty() )
    {
        QFile qss(filename);
        if ( qss.open(QFile::ReadOnly|QFile::Text) )
            setStyleSheet(QString::fromUtf8(qss.readAll()));
    }
    else
    {
        setStyleSheet(QString());
    }
}

bool Application::event(QEvent* event)
{
    if ( event->type() == QEvent::LanguageChange )
        plugin::registry().retranslate();

    return QApplication::event(event);
}

void Application::initSubsystems()
{
    // Initialize Icon theme
    // NOTE: this is broken in Qt 5.4.1
    /*QIcon::setThemeSearchPaths(
        QIcon::themeSearchPaths()
        << data().readable("icons")
    );
    QIcon::setThemeName("pixel-cayman");*/

    initFormats();
    initTools();
    initPlugins();
    initLanguages();
}

void Application::initFormats()
{
    io::formats().addFormat<io::FormatXmlMela>();
    io::formats().addFormat<io::FormatBitmap>();
}

void Application::initTools()
{
    tool::Registry::instance().addTool<tool::Brush>("brush");
    tool::Registry::instance().addTool<tool::Eraser>("eraser");
    tool::Registry::instance().addTool<tool::FloodFill>("flood_fill");
}

void Application::initPlugins()
{
    // Register factories (plugin types)
    plugin::registry().addFactory(new plugin::LibraryPluginFactory(PLUGIN_INIT_FUNCTION_STRING));

    // Connect warnings
    QObject::connect(&plugin::registry(), &plugin::PluginRegistry::warning,
        [](const QString& msg) { Message(Message::Error|Message::Stream) << msg; });

    // Refresh the list of available paths every time the plugins are loaded
    QObject::connect(&plugin::registry(), &plugin::PluginRegistry::beginLoad,
        []{ plugin::registry().setSearchPaths(data().readableList("plugins")); }
    );

    // Load the plugins that are supposed to be enabled
    QObject::connect(&plugin::registry(), &plugin::PluginRegistry::created,
        [](plugin::Plugin* plugin){
            if ( plugin->dependenciesMet() &&
                    ::plugin::settingsGet(plugin, "loaded", true) )
            {
                plugin->load();
            }

        });

    // Update settings each time a plugin is loaded or unloaded
    QObject::connect(&plugin::registry(), &plugin::PluginRegistry::loadedChanged,
        [](plugin::Plugin* plugin, bool loaded){
            ::plugin::settingsPut(plugin, "loaded", loaded);
    });

    // Load all plugins
    plugin::PluginRegistry::instance().load();
}

QString Application::languageName(const QString& code) const
{
    QLocale lang_loc = QLocale(code);
    QString name = lang_loc.nativeLanguageName();
    if ( !name.isEmpty() )
        name[0] = name[0].toUpper();
    return name;
}

void Application::initLanguages()
{
    current_language = default_language;
    translators[default_language] = nullptr;

    static QRegularExpression lang_code("^.*([a-z]{2}(?:_[A-Z]{2}))\\.qm$");
    for ( const auto& qm : data().readableWildcard("translations", "*.qm") )
    {
        auto match = lang_code.match(qm);
        if ( !match.hasMatch() )
            continue;

        QString code = match.captured(1);
        QString language = languageName(code);
        if ( language.isEmpty() )
            language = code;

        QTranslator* trans = new QTranslator(this);
        if ( !trans->load(qm) )
        {
            Message(Msg::Stream|Msg::Error) <<
            /*:
             * %1 is the file name,
             * %2 is the human-readable language code
             * %3 is the ISO language code
             */
            tr("Error on loading translation file %1 for language %2 (%3)")
                .arg(qm).arg(language).arg(code);
            delete trans;
        }
        else
        {
            translators[code] = trans;
        }
    }

    setLanguage(settings_->get("language", default_language));
}

bool Application::setLanguage(const QString& code)
{
    if ( code == current_language )
        return true;

    auto it = translators.find(code);
    if ( it == translators.end() )
    {
        Message(Msg::Stream|Msg::Error)
            << tr("Translation for %1 is not available").arg(code);
        return false;
    }

    if ( !it.value() || installTranslator(it.value()) )
    {
        removeTranslator(translators[current_language]);
        current_language = it.key();
        emit languageChanged(current_language);
        return true;
    }

    Message(Msg::Stream|Msg::Error)
        << tr("Error while switching to language %1").arg(code);
    return false;
}

settings::Settings* settings::Settings::singleton = nullptr;


} // namespace cayman
