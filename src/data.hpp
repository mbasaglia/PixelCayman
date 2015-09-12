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
#ifndef PIXEl_CAYMAN_DATA_HPP
#define PIXEl_CAYMAN_DATA_HPP

#include <QStringList>

/**
 * \brief Class managing data file paths
 */
class Data
{
public:
    /**
     * \brief List of directories that can be used to read data files
     */
    QStringList directories() const;

    /**
     * \brief Returns the list of existing files matching the given name
     */
    QStringList readableList(const QString& file) const;

    /**
     * \brief Returns the path to a readable data file
     * \return A path to an existing file matching \p file,
     * an empty string if such file doesn't exists
     */
    QString readable(const QString& file) const;

    /**
     * \brief Returns the path to a writable data file
     *
     * This will create missing directories but not the file itself.
     *
     * \return The path to a file path that can be used for writing.
     */
    QString writable(const QString& file) const;

    /**
     * \brief Add a directory used to store read-only data
     */
    void addDirectory(const QString& path);

    /**
     * \brief Set a path to be used to write data files
     * \todo Directory for temporary files?
     */
    void setWriteDirectory(const QString& path);

private:
    Data();
    Data(const Data&) = delete;
    ~Data() {};
    friend Data& data();

    QStringList read_dirs;
    QString     write_dir;
};

/**
 * \brief Data singleton
 * \note Out-of-class so it's less cumbersome to use
 */
inline Data& data()
{
    static Data singleton;
    return singleton;
}

#endif // PIXEl_CAYMAN_DATA_HPP
