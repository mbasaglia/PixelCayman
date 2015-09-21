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

#include "data.hpp"

#include <QDir>
#include <QFileInfo>
#include <QCoreApplication>
#include <QStandardPaths>

void Data::addDirectory(const QString& path)
{
    QString canonical = QFileInfo(path).canonicalFilePath();
    if ( !path.isEmpty() && canonical != write_dir && !read_dirs.contains(canonical) )
        read_dirs.push_back(canonical);
}

QStringList Data::directories() const
{
    QStringList all = read_dirs;
    all.push_front(write_dir);
    return all;
}

void Data::setWriteDirectory(const QString& path)
{
    QFileInfo dir(path);
    if ( !dir.exists() )
        QDir().mkpath(path);
    write_dir = dir.canonicalFilePath();
}

QString Data::readable(const QString& file) const
{
    for ( const QString& dir_path : directories() )
    {
        QDir dir(dir_path);
        if ( dir.exists(file) )
            return dir.absoluteFilePath(file);
    }

    return QString();
}

QStringList Data::readableList(const QString& file) const
{
    QStringList paths;
    for ( const QString& dir_path : directories() )
    {
        QDir dir(dir_path);
        if ( dir.exists(file) )
            paths << dir.absoluteFilePath(file);
    }

    return paths;

}

QString Data::writable(const QString& file) const
{
    QFileInfo rel_file = file;
    QDir write = write_dir;
    QString parent = rel_file.dir().path();
    if ( !write.exists(parent) )
        write.mkdir(parent);
    return write.absoluteFilePath(file);
}

Data::Data()
{
    setWriteDirectory(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));

    for ( const QString& path : QStandardPaths::standardLocations(QStandardPaths::AppDataLocation) )
    {
        addDirectory(path);
    }
    addDirectory(QCoreApplication::applicationDirPath()+"/../share/"+QCoreApplication::applicationName());
}

QString Data::tempDir()
{
    QString dir = QDir::tempPath()+"/"+QCoreApplication::applicationName()+"/";
    if ( !QDir().exists(dir) )
        QDir().mkdir(dir);
    return dir;
}

