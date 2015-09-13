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

QAction* Plugin::createAction(QObject* parent)
{
    QAction *action = new QAction(this->name(), parent);
    action->setCheckable(true);
    action->setChecked(this->loaded_);
    action->setEnabled(this->dependencies_met_);
    connect(this, &Plugin::loadedChanged, action, &QAction::setChecked);
    connect(this, &Plugin::dependenciesChecked, action, &QAction::setEnabled);
    connect(this, &QObject::destroyed, action, &QObject::deleteLater);
    connect(action, &QAction::triggered, this, &Plugin::setLoaded);
    return action;
}

bool Plugin::checkDependencies()
{
    dependencies_met_ = registry().meetsDependency(dependencies());
    emit dependenciesChecked( dependencies_met_ );
    return dependencies_met_;
}

void PluginRegistry::load()
{
    if ( !plugins_.empty() )
        throw std::logic_error("Cannot call PluginRegistry::load() before calling PluginRegistry::unload()");

    emit beginLoad();

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

        // For each plugin, add it if all of its dependencies are met
        auto iter = queued_.begin();
        while ( iter != queued_.end() )
        {
            if ( (*iter)->checkDependencies() )
            {
                // This emits created() which can be used to load the plugin
                addPlugin(*iter);
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
            QStringList names;
            for ( auto plugin : queued_ )
            {
                addPlugin(plugin);
            }
            queued_.clear();
            break;
        }
    }

    emit endLoad();
}

bool PluginRegistry::queue(const QFileInfo& file)
try {
    QString name = file.fileName();
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
    return true;
} catch (const std::exception& exc) {
    warning(tr("Exception during plugin creation: %1").arg(exc.what()));
    return false;
}

void PluginRegistry::unload()
{
    for ( auto plugin : plugins_ )
    {
        plugin->unload();
        emit destroyed(plugin);
        delete plugin;
    }

    plugins_.clear();
}

void PluginRegistry::addPlugin(Plugin* plugin)
{
    plugin->setParent(this);
    connect(plugin, &Plugin::loaded, [this, plugin]{
        for ( auto p : plugins_ )
            if ( !p->dependenciesMet() )
                p->checkDependencies();
        emit loaded(plugin);
    });
    connect(plugin, &Plugin::unloaded, [this, plugin]{
        for ( auto p : plugins_ )
            if ( p->dependenciesMet() )
                p->checkDependencies();
        emit unloaded(plugin);
    });
    connect(plugin, &Plugin::loadedChanged, [this, plugin](bool loaded){
        emit loadedChanged(plugin, loaded);
    });
    plugins_[plugin->name()] = plugin;
    emit created(plugin);
}

void PluginRegistry::removePlugin(Plugin* plugin)
{
    plugins_.remove(plugin->name());
    emit destroyed(plugin);
}

} // namespace plugin
