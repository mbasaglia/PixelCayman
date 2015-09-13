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

static QString function_prefix = "Plugin_";

template<class T>
void LibraryPlugin::resolve(T& to, const QString& name, bool required)
{
    to = library.resolve<T>(function_prefix+name);
    if ( required && !to )
        throw std::logic_error(
            tr("Missing plugin function %1 in %2")
                .arg(function_prefix+name+"()")
                .arg(library.fileName())
            .toUtf8().data()
        );
}

LibraryPlugin::LibraryPlugin(const QString& library_file, Library::LoadHints hints)
    : library(library_file, hints)
{
    resolve(func_load, "load", true);
    resolve(func_name, "name", true);
    resolve(func_version, "version", false);
    resolve(func_unload, "unload", false);
    resolve(func_deps, "dependencies", false);
}

QString LibraryPlugin::functionPrefix()
{
    return function_prefix;
}

void LibraryPlugin::setFunctionPrefix(const QString prefix)
{
    function_prefix = prefix;
}

QList<Plugin::Dependency> LibraryPlugin::onDependencies()
{
    return func_deps ? func_deps() : QList<Plugin::Dependency>();
}

bool LibraryPlugin::onLoad()
{
    return func_load();
}

QString LibraryPlugin::onName()
{
    return func_name();
}

void LibraryPlugin::onUnload()
{
    if ( func_unload )
        func_unload();
}

int LibraryPlugin::onVersion()
{
    return func_version ? func_version() : plugin::Plugin::onVersion();
}

bool LibraryPluginFactory::canCreate(const QFileInfo& file) const
{
    /// \todo System-specific suffixes
    static QString library_suffix = "so";
    return file.suffix() == library_suffix && file.isFile();
}

Plugin* LibraryPluginFactory::create(const QString& fileName)
{
    return new LibraryPlugin(fileName);
}


} // namespace plugin
