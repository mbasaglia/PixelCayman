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
#include <exception>
#include <QApplication>
#include <QDebug>
#include <QIcon>

#include "ui/main_window.hpp"
#include "info.hpp"
#include "settings.hpp"
#include "data.hpp"
#include "tool/registry.hpp"
#include "plugin/plugin.hpp"
#include "plugin/library_plugin.hpp"
#include "plugin/plugin_api.hpp"
#include "document/io.hpp"

void initPlugins()
{
    // Register factories (plugin types)
    plugin::registry().addFactory(new plugin::LibraryPluginFactory(PLUGIN_INIT_FUNCTION_STRING));

    // Connect warnings
    QObject::connect(&plugin::registry(), &plugin::PluginRegistry::warning,
        [](const QString& msg) { qWarning() << msg; });

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

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    init_info();
    settings::Settings settings_object;

    try
    {
        document::Formats::instance().addFormat(new document::FormatXmlMela);
        document::Formats::instance().addFormat(new document::FormatBitmap);

        // Initialize Icon theme
        // NOTE: this is broken in Qt 5.4.1
        /*QIcon::setThemeSearchPaths(
            QIcon::themeSearchPaths()
            << data().readable("icons")
        );
        QIcon::setThemeName("pixel-cayman");*/

        initPlugins();

        MainWindow window;
        for ( const auto& tool : ::tool::Registry::instance().tools() )
            window.addTool(tool.get());

        window.show();
        return app.exec();
    }
    catch ( const std::exception& exc )
    {
        qWarning() << "Exception: " << exc.what();
    }
    catch ( ... )
    {
        qWarning() << "Unknown Exception";
    }

    return 1;
}

settings::Settings* settings::Settings::singleton = nullptr;
