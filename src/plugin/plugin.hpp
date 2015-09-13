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
#ifndef PIXEL_CAYMAN_PLUGIN_HPP
#define PIXEL_CAYMAN_PLUGIN_HPP

#include <QStringList>
#include <QFileInfo>
#include <QHash>
#include <QObject>
#include <QAction>

namespace plugin {

/**
 * \brief Interface for plugin types
 */
class Plugin : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString name READ name)
    Q_PROPERTY(int version READ version)

public:
    /**
     * \brief Plugin dependency descriptor
     */
    struct Dependency
    {
        /**
         * \brief Name of the plugin that is required to be loaded
         */
        QString name;

        /**
         * \brief Minimum version.
         *
         * Zero means no minimum version
         */
        int minimum_version = 0;

        /**
         * \brief Maximum version.
         *
         * Zero means no maximum version
         */
        int maximum_version = 0;

        /**
         * \brief Whether a plugin matches this dependency
         */
        bool match(Plugin* plugin) const
        {
            if ( plugin->name() != name )
                return false;
            if ( minimum_version && minimum_version > plugin->version() )
                return false;
            if ( maximum_version && maximum_version < plugin->version() )
                return false;
            return true;
        }
    };

    virtual ~Plugin() {}

    /**
     * \brief Loads the plugin functionality to make it ready for use
     * \note Checks dependencies
     * \returns \b true on success
     */
    bool load()
    {
        if ( loaded_ || !checkDependencies() )
            return false;

        loaded_ = onLoad();

        if ( loaded_ )
        {
            emit loaded(QPrivateSignal());
            emit loadedChanged(true, QPrivateSignal());
        }

        return loaded_;
    }

    /**
     * \brief Removes the plugin functionality to disable the plugin
     */
    void unload()
    {
        if ( loaded_ )
        {
            loaded_ = false;
            onUnload();
            emit unloaded(QPrivateSignal());
            emit loadedChanged(false, QPrivateSignal());
        }
    }

    /**
     * \brief Calls either load() or unload() depending on \p loaded
     */
    void setLoaded(bool loaded)
    {
        if ( loaded )
            load();
        else
            unload();
    }

    /**
     * \brief Whether the plugin has been loaded
     */
    bool isLoaded()
    {
        return loaded_;
    }

    /**
     * \brief Returns the plugin name
     */
    QString name()
    {
        if ( name_.isEmpty() )
            name_ = onName();
        return name_;
    }

    /**
     * \brief Plugin version number
     */
    int version()
    {
        if ( version_ < 0 )
            version_ = onVersion();
        return version_;
    }

    /**
     * \brief List of dependencies to other plugins
     */
    QList<Dependency> dependencies()
    {
        if ( !dependencies_loaded_ )
        {
            dependencies_loaded_ = true;
            dependencies_ = onDependencies();
        }

        return dependencies_;
    }

    /**
     * \brief Checks whether the dependencies are met
     */
    bool checkDependencies();

    /**
     * \brief Returns whether the dependencies are met
     *
     * It doesn't perform the check but returns the value returned by the
     * last call of checkDependencies()
     */
    bool dependenciesMet() const
    {
        return dependencies_met_;
    }

    /**
     * \brief Creates a QAction which can be used to toggle the plugin
     */
    QAction* createAction(QObject* parent = nullptr);

signals:
    void loaded(QPrivateSignal);
    void unloaded(QPrivateSignal);
    void loadedChanged(bool loaded, QPrivateSignal);
    void dependenciesChecked(bool met);

protected:
    /**
     * \brief Loads the plugin functionality to make it ready for use
     * \returns \b true on success
     */
    virtual bool onLoad() = 0;

    /**
     * \brief Removes the plugin functionality to disable the plugin
     */
    virtual void onUnload() {}

    /**
     * \brief Returns the plugin name
     *
     * Must be unique across all plugins and not empty
     */
    virtual QString onName() = 0;

    /**
     * \brief Plugin version number
     */
    virtual int onVersion() { return 0; }

    /**
     * \brief List of dependencies to other plugins
     */
    virtual QList<Dependency> onDependencies() { return {}; }

private:
    QString name_;
    bool loaded_ = false;
    int version_ = -1;
    bool dependencies_loaded_ = false;
    QList<Dependency> dependencies_;
    bool dependencies_met_ = false;
};

/**
 * \brief Interface for classes that can create plugin objects from files
 */
class PluginFactory
{
public:
    virtual ~PluginFactory() {}

    /**
     * \brief Returns whether \p file is in a format that the plugin factory can handle
     */
    virtual bool canCreate(const QFileInfo& file) const = 0;

    /**
     * \brief Creates a plugin object from file
     */
    virtual Plugin* create(const QString& fileName) = 0;
};

/**
 * \brief Class that handles plugins
 */
class PluginRegistry : public QObject
{
    Q_OBJECT

public:
    static PluginRegistry& instance()
    {
        static PluginRegistry singleton;
        return singleton;
    }

    /**
     * \brief The list of available plugins
     */
    QList<Plugin*> plugins() const
    {
        return plugins_.values();
    }

    /**
     * \brief The plugin with the given name
     */
    Plugin* plugin(const QString& name) const
    {
        return plugins_.value(name);
    }

    /**
     * \brief Whether there is a loaded plugin that meets the dependency
     */
    bool meetsDependency(const Plugin::Dependency& dependency)
    {
        if ( auto plugin = plugins_.value(dependency.name) )
        {
            if ( dependency.match(plugin) )
                return true;
        }
        return false;
    }

    /**
     * \brief Whether all the dependencies in the list are met
     */
    bool meetsDependency(const QList<Plugin::Dependency>& dependencies)
    {
        for ( const auto& dep : dependencies )
            if ( ! meetsDependency(dep) )
                return false;
        return true;
    }

    /**
     * \brief Adds a plugin factory
     *
     * Takes ownership of the factory object
     */
    void addFactory(PluginFactory* factory)
    {
        factories_.push_back(factory);
    }

    /**
     * \brief Creates the plugin objects and loads them
     */
    void load();

    /**
     * \brief Unloads all plugins and removes them
     */
    void unload();

    /**
     * \brief To search plugins into
     */
    QStringList searchPaths() const
    {
        return search_paths_;
    }

    void setSearchPaths(const QStringList& search_paths)
    {
        search_paths_ = search_paths;
    }

    void addSearchPath(const QString& path)
    {
        search_paths_.push_back(path);
    }

signals:
    /**
     * \brief Emitted when a plugin has been created
     *
     * Can be used to load the plugin automatically
     */
    void created(Plugin* plugin);
    /**
     * \brief Emitted when a plugin is being destroyed
     */
    void destroyed(Plugin* plugin);
    /**
     * \brief Emitted when a plugin has been enabled
     */
    void loaded(Plugin* plugin);
    /**
     * \brief Emitted when a plugin has been disabled
     */
    void unloaded(Plugin* plugin);
    /**
     * \brief Emitted when a plugin has been enabled or disabled
     */
    void loadedChanged(Plugin* plugin, bool loaded);

    /**
     * \brief Emitted on minor issues with the plugins
     */
    void warning(const QString& msg);

    /**
     * \brief Emitted before the plugins get initialized
     */
    void beginLoad();

    /**
     * \brief Emitted after the plugins has been initialized
     */
    void endLoad();

private:
    PluginRegistry() {}

    ~PluginRegistry()
    {
        for ( auto factory : factories_ )
            delete factory;
    }

    /**
     * \brief Queue a plugin file for loading
     */
    bool queue(const QFileInfo& file);

    void addPlugin(Plugin* plugin);
    void removePlugin(Plugin* plugin);

    QHash<QString, Plugin*> plugins_;
    QList<Plugin*>          queued_;
    QList<PluginFactory*>   factories_;
    QStringList             search_paths_;
};

inline PluginRegistry& registry()
{
    return PluginRegistry::instance();
}

} // namespace plugin
#endif // PIXEL_CAYMAN_PLUGIN_HPP
