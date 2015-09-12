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

#include "plugin.hpp"

#include <QDir>

namespace plugin {

void PluginRegistry::load()
{
    if ( !plugins_.empty() )
        throw std::logic_error("Cannot call PluginRegistry::load() before calling PluginRegistry::unload()");

    // Queue plugins
    for ( QDir dir : search_paths_ )
    {
        if ( dir.exists() )
        {
            for ( const QFileInfo& file : dir.entryInfoList() )
            {
                if ( !queue(file) )
                {
                    warning(tr("Plugin file could not be loaded: %1")
                        .arg(file.canonicalFilePath()));
                }
            }
        }
    }

    // Check dependencies
    while ( !queued_.isEmpty() )
    {
        int size = queued_.size();

        // For each plugin, load it if all of its dependencies are met
        auto iter = queued_.begin();
        while ( iter != queued_.end() )
        {
            if ( meetsDependency((*iter)->dependencies()) )
            {
                addPlugin(*iter);
                /// \todo Read from settings which plugins have to be loaded
                (*iter)->load();
                /// \todo Check load() was successful
                iter = queued_.erase(iter);
            }
            else
            {
                ++iter;
            }
        }

        // No plugin has been loaded in this iteration, so bail out
        if ( size == queued_.size() )
        {
            /// \todo Use a custom message system
            QStringList names;
            for ( auto plugin : queued_ )
            {
                names << plugin->name();
                delete plugin;
            }
            warning(tr("Plugins with missing dependencies:\n%1")
                .arg(names.join("\n")));
            queued_.clear();
            break;
        }
    }

}

bool PluginRegistry::queue(const QFileInfo& file)
try {
    for ( auto factory : factories_ )
    {
        if ( factory->canCreate(file) )
        {
            if ( Plugin* plugin = factory->create(file.canonicalFilePath()) )
            {
                queued_.push_back(plugin);
                return true;
            }
            return false;
        }
    }
    return false;
} catch (const std::exception& exc) {
    warning(tr("Exception during plugin creation: %1").arg(exc.what()));
    return false;
}

void PluginRegistry::unload()
{
    for ( auto plugin : plugins_ )
    {
        plugin->unload();
        delete plugin;
    }

    plugins_.clear();
}

void PluginRegistry::addPlugin(Plugin* plugin)
{
    plugin->setParent(this);
    connect(plugin, &Plugin::loaded, [this, plugin]{ emit loaded(plugin); });
    connect(plugin, &Plugin::unloaded, [this, plugin]{ emit unloaded(plugin); });
    connect(plugin, &QObject::destroyed, [this, plugin]{ removePlugin(plugin); });
    plugins_[plugin->name()] = plugin;
    emit created(plugin);
}

void PluginRegistry::removePlugin(Plugin* plugin)
{
    plugins_.remove(plugin->name());
    emit destroyed(plugin);
}

} // namespace plugin
