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

#include "library_plugin.hpp"

namespace plugin {

LibraryPluginFactory::LibraryPluginFactory(const QString& init_function)
    : init_function(init_function)
{}

bool LibraryPluginFactory::canCreate(const QFileInfo& file) const
{
    /// \todo System-specific suffixes
    static QString library_suffix = "so";
    return file.suffix() == library_suffix && file.isFile();
}

Plugin* LibraryPluginFactory::create(const QString& fileName)
{
    Library* lib = new Library(fileName);
    auto init = lib->resolve<Plugin*()>(init_function);

    if ( lib->error() )
    {
        auto error = lib->errorString();
        if ( !error.contains(fileName) )
            error = tr("Error loading %1: %2").arg(fileName).arg(error);
        emit registry().warning(error);
        delete lib;
        return nullptr;
    }

    if ( !init )
    {
        delete lib;
        emit registry().warning(tr("Missing function %1() in %2")
            .arg(init_function).arg(fileName));
        return nullptr;
    }

    if ( auto plugin = init() )
    {
        connect(plugin, &QObject::destroyed, lib, &QObject::deleteLater);
        libraries[plugin] = lib;
        connect(plugin, &QObject::destroyed, [plugin]{
            libraries.remove(plugin);
        });
        return plugin;
    }
    else
    {
        delete lib;
        emit registry().warning(tr("Could not create plugin from %1")
            .arg(fileName));
        return nullptr;
    }
}

QMap<const Plugin*, Library*> LibraryPluginFactory::libraries;

} // namespace plugin
