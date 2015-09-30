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

namespace cayman {

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

QStringList Data::readableWildcard(const QString& directory, const QString& pattern) const
{
    QMap<QString, QString> paths;

    for ( QDir dir : readableList(directory) )
    {
        dir.setNameFilters({pattern});
        dir.setFilter(QDir::Readable|QDir::AllEntries|QDir::NoDotAndDotDot);
        for ( const QFileInfo& entry : dir.entryInfoList() )
        {
            auto it = paths.find(entry.fileName());
            if ( it == paths.end() )
                paths[entry.fileName()] = entry.canonicalFilePath();
        }
    }

    return paths.values();
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
    addDirectory(QCoreApplication::applicationDirPath()+"/../lib/"+QCoreApplication::applicationName());
    addDirectory(QCoreApplication::applicationDirPath()+"/../share/"+QCoreApplication::applicationName());
}

QString Data::tempDir()
{
    QString dir = QDir::tempPath()+"/"+QCoreApplication::applicationName()+"/";
    if ( !QDir().exists(dir) )
        QDir().mkdir(dir);
    return dir;
}

QIcon Data::caymanIcon(const QString& name, int max_size, const QString& icon_set)
{
    auto it = cached_icons.find(icon_set+'/'+name);
    if ( it != cached_icons.end() )
        return *it;

    QString parent_path = readable("icons/"+icon_set);
    if ( parent_path.isEmpty() )
        return {};

    QIcon icon;
    QDir parent(parent_path);
    for ( QString size_path : parent.entryList({"*x*"}, QDir::Dirs, QDir::Name) )
    {
        if ( max_size > 0 && size_path.split("x")[0].toInt() > max_size )
            break;
        if ( !parent.cd(size_path) )
            continue;
        if ( parent.exists(name+".svg") )
            icon.addFile(parent.absoluteFilePath(name+".svg"));
        else if ( parent.exists(name+".png") )
            icon.addFile(parent.absoluteFilePath(name+".png"));
        parent.cdUp();
    }
    if ( !icon.isNull() )
        cached_icons[icon_set+'/'+name] = icon;

    return icon;
}

} // namespace cayman
