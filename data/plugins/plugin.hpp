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
#ifndef PIXEL_CAYMAN_PLUGINS_PLUGIN_HPP
#define PIXEL_CAYMAN_PLUGINS_PLUGIN_HPP
#include "plugin/plugin_api.hpp"

/**
 * \brief Class that structures the required functions of a library plugin
 */
class Plugin : public plugin::Plugin
{
public:

    /**
     * \brief Writes a plugin setting
     */
    template<class T>
        void settingsPut(const QString& key, T&& value)
        {
            settings::put(plugin::settingsKey(this, key), std::forward<T>(value));
        }

    /**
     * \brief Reads a plugin setting
     */
    template<class T>
        typename std::remove_reference<T>::type
            settingsGet(const QString& key, T&& default_value = T())
        {
            return settings::get(plugin::settingsKey(this, key), std::forward<T>(default_value));
        }
};

/**
 * \see INIT_PLUGIN()
 */
#define INIT_PLUGIN_RAW(pluginClass, prefix) \
    pluginClass globalPlugin; \
    extern "C" Q_DECL_EXPORT bool prefix##load() \
    { return globalPlugin.load(); } \
    extern "C" Q_DECL_EXPORT QString prefix##name() \
    { return globalPlugin.name(); } \
    extern "C" Q_DECL_EXPORT int prefix##version() \
    { return globalPlugin.version(); } \
    extern "C" Q_DECL_EXPORT void prefix##unload() \
    { return globalPlugin.unload(); } \
    extern "C" Q_DECL_EXPORT QList<plugin::Plugin::Dependency> prefix##dependencies() \
    { return globalPlugin.dependencies(); }

/**
 * \brief Generate boilerplate code for the plugin
 *
 * Creates the required symbols to make the plugin accessible from the application
 * and a global variable called \b globalPlugin that can be used to access the
 * plugin metadata.
 */
#define INIT_PLUGIN(pluginClass) INIT_PLUGIN_RAW(pluginClass, PixelCaymanPlugin_)

#endif // PIXEL_CAYMAN_PLUGINS_PLUGIN_HPP
