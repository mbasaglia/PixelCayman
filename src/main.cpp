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

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    init_info();
    settings::Settings settings_object;

    try
    {
        plugin::PluginRegistry::instance().setSearchPaths(data().readableList("plugins"));
        plugin::PluginRegistry::instance().load();
        // Initialize Icon theme
        // NOTE: this is broken in Qt 5.4.1
        /*QIcon::setThemeSearchPaths(
            QIcon::themeSearchPaths()
            << data().readable("icons")
        );
        QIcon::setThemeName("pixel-cayman");*/

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
