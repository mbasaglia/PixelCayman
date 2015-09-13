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
#ifndef PIXEL_CAYMAN_PLUGIN_API_HPP
#define PIXEL_CAYMAN_PLUGIN_API_HPP

#include "settings.hpp"
#include "plugin.hpp"

namespace plugin {

/**
 * \brief A machine-readable name
 * \returns A string all lowercase alphanumric or underscore characters
 *          derived from the plugin name
 */
QString machineName(Plugin* plugin);

/**
 * \brief Returns a string representing a settings key for the plugin
 */
inline QString settingsKey(Plugin* plugin, const QString& key )
{
    return QString("plugins/%1/%2").arg(machineName(plugin)).arg(key);
}

/**
 * \brief Writes a plugin setting
 */
template<class T>
    void settingsPut(Plugin* plugin, const QString& key, T&& value)
    {
        settings::put(settingsKey(plugin, key), std::forward<T>(value));
    }

/**
 * \brief Reads a plugin setting
 */
template<class T>
    typename std::remove_reference<T>::type
        settingsGet(Plugin* plugin, const QString& key, T&& default_value = T())
    {
        return settings::get(settingsKey(plugin, key), std::forward<T>(default_value));
    }

} // namespace plugin
#endif // PIXEL_CAYMAN_PLUGIN_API_HPP
