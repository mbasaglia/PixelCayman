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
#include <QDebug>

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
                    /// \todo Use a custom message system
                    qWarning() << QObject::tr("Plugin file could not be loaded: %1")
                        .arg(file.canonicalFilePath());
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
                /// \todo Read from settings which plugins have to be loaded
                (*iter)->load();
                /// \todo Check load() was successful
                plugins_[(*iter)->name()] = *iter;
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
            qWarning() << "Some plugins have missing dependencies:";
            for ( auto plugin : queued_ )
            {
                qWarning() << plugin->name();
                delete plugin;
            }
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
    /// \todo Use a custom message system
    qWarning() << QObject::tr("Exception during plugin creation: %1").arg(exc.what());
    return false;
}

void PluginRegistry::unload()
{
    for ( auto plugin : plugins_ )
    {
        /// \todo Check the plugin was loaded
        plugin->unload();
        delete plugin;
    }

    plugins_.clear();
}

} // namespace plugin
