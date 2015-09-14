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

using plugin::CaymanPlugin;
/**
 * \brief Generate boilerplate code for the plugin
 *
 * Creates the required symbols to make the plugin accessible from the application
 * and a global variable called \b globalPlugin that can be used to access the
 * plugin metadata.
 */
#define INIT_PLUGIN(pluginClass) \
    pluginClass* globalPlugin = nullptr; \
    extern "C" Q_DECL_EXPORT plugin::Plugin* PLUGIN_INIT_FUNCTION() { \
        if ( globalPlugin ) \
            return nullptr; \
        globalPlugin = new pluginClass(); \
        QObject::connect(globalPlugin, &QObject::destroyed, \
            []{ globalPlugin = nullptr; }); \
        return globalPlugin; \
    }

#endif // PIXEL_CAYMAN_PLUGINS_PLUGIN_HPP
