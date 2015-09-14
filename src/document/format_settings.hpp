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
#ifndef PIXEl_CAYMAN_DOCUMENT_FORMAT_SETTINGS_HPP
#define PIXEl_CAYMAN_DOCUMENT_FORMAT_SETTINGS_HPP

#include <QMap>
#include <QString>
#include <QVariant>

namespace document {

class AbstractFormat;

/**
 * \brief Document format settings
 * \todo Some clean way to handle global settings
 */
class FormatSettings
{
public:
    /**
     * \brief Preferred format
     */
    AbstractFormat* preferred() const
    {
        return preferred_;
    }

    /**
     * \brief Change the preferred format
     */
    void setPreferred(AbstractFormat* format)
    {
        preferred_ = format;
    }

    /**
     * \brief All settings
     */
    const QMap<AbstractFormat*, QMap<QString, QVariant>>& settings() const
    {
        return settings_;
    }

    /**
     * \brief Settings for a given format
     */
    QMap<QString, QVariant>& settings(AbstractFormat* format)
    {
        return settings_[format];
    }

    /**
     * \brief Settings for a given format
     */
    QMap<QString, QVariant> settings(AbstractFormat* format) const
    {
        return settings_[format];
    }

    /**
     * \brief Get a single option
     */
    QVariant get(AbstractFormat* format, const QString& key) const
    {
        return settings_[format][key];
    }

    /**
     * \brief Get a single option with an explicit type
     */
    template<class T>
        T get(AbstractFormat* format, const QString& key, const T& default_value = T()) const
        {
            QVariant variant = settings_[format][key];
            if ( !variant.canConvert<typename std::remove_reference<T>::type>() )
                return default_value;
            return variant.value<T>();
        }

    /**
     * \brief Set a single option
     */
    void put(AbstractFormat* format, const QString& key, const QVariant& value)
    {
        settings_[format][key] = value;
    }

    /**
     * \brief Set a single option with an explicit type
     */
    template<class T>
        void put(AbstractFormat* format, const QString& key, T&& value)
        {
            settings_[format][key] = QVariant::fromValue(std::forward<T>(value));
        }

private:
    AbstractFormat* preferred_ = nullptr;
    QMap<AbstractFormat*, QMap<QString, QVariant>> settings_;
};

} // namespace document
#endif // PIXEl_CAYMAN_DOCUMENT_FORMAT_SETTINGS_HPP
