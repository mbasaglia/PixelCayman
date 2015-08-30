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
#ifndef PIXEL_CAYMAN_SETTINGS_HPP
#define PIXEL_CAYMAN_SETTINGS_HPP

#include <QSettings>
#include <type_traits>

int main(int argc, char** argv);

namespace settings {

/**
 * \brief Settings singleton
 *
 * \note It can only be explicitly instanciated in main() to give it the same
 * scope as the QApplication object
 */
class Settings
{
public:
    static Settings& instance()
    {
        return *singleton;
    }

    template<class T>
        void put(const QString& name, T&& value)
        {
            settings_.setValue(name,
                QVariant::fromValue(std::forward<T>(value)));
        }

    template<class T>
        typename std::remove_reference<T>::type get(const QString& name, T&& default_value = T())
        {
            using Type = typename std::remove_reference<T>::type;
            QVariant variant = settings_.value(name);
            if ( variant.canConvert<Type>() )
                return variant.value<Type>();
            return std::forward<T>(default_value);
        }

    QSettings& settings()
    {
        return settings_;
    }

private:
    Settings()
    {
        singleton = this;
    }

    ~Settings() {}

    friend int ::main(int argc, char** argv);

    QSettings settings_;

    static Settings* singleton;
};

/**
 * \brief RAII class to group settings values
 */
class SettingsGroup
{
public:
    SettingsGroup(const QString& name)
    {
        Settings::instance().settings().beginGroup(name);
    }

    ~SettingsGroup()
    {
        Settings::instance().settings().endGroup();
    }

    /// To use it in an if()
    explicit operator bool() const
    {
        return true;
    }
};

} // namespace settings

/**
 * \brief Simple utility to introduce a settings group with a nice syntax
 *
 * \example
 * \code
 * SETTINGS_GROUP("Group")
 * {
 *      Settings::instance().put("Hello", 123);
 * }
 * \endcode
 */
#define SETTINGS_GROUP(name) \
    if ( ::settings::SettingsGroup _pixel_cayman_settings_group_ {name} )

#endif // PIXEL_CAYMAN_SETTINGS_HPP
