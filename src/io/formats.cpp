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

#include "formats.hpp"
#include <QFileInfo>

namespace io {

bool AbstractFormat::save(document::Document* document, const QString& filename)
{
    if ( filename.isEmpty() )
        return false;

    QFile file(filename);

    if ( !file.open(QFile::WriteOnly) )
        return false;

    return save(document, &file);
}

bool AbstractFormat::save(document::Document* document)
{
    return save(document, document->fileName());
}

document::Document* AbstractFormat::open(const QString& filename)
{
    if ( filename.isEmpty() )
        return nullptr;

    QFile file(filename);

    if ( !file.open(QFile::ReadOnly) )
        return nullptr;

    auto doc = open(&file);
    if ( doc )
        doc->setFileName(filename);
    return doc;
}

Formats::~Formats()
{
    for ( auto format : formats_ )
        delete format;
}

bool Formats::deleteFormat(AbstractFormat* format)
{
    for ( auto it = formats_.begin(); it != formats_.end(); ++it )
        if ( *it == format )
        {
            delete *it;
            formats_.erase(it);
            return true;
        }
    return false;
}

AbstractFormat* Formats::format(const QString& id) const
{
    for ( auto format : formats_ )
        if ( id == format->id() )
            return format;
    return nullptr;
}

QList<AbstractFormat*> Formats::formats() const
{
    return formats_;
}

AbstractFormat* Formats::formatFromFileName(const QString& file, Formats::Action action) const
{
    QString ext = QFileInfo(file).suffix().toLower();
    for ( auto format : formats_ )
        if ( format->extensions(action).contains(ext) )
            return format;
    return nullptr;
}

QStringList Formats::nameFilters(Formats::Action action, bool all_files) const
{
    QStringList filters;
    QStringList extensions;

    for ( auto* format : formats_ )
    {
        if ( format->supportsAction(action) )
        {
            filters << format->nameFilter(action);
            if ( all_files )
            {
                for ( const auto& ext : format->extensions(action) )
                    if ( !extensions.contains(ext) )
                        extensions.push_back(ext);
            }
        }
    }

    filters.sort();
    
    if ( all_files )
    {
        QString namefilters;
        for ( const auto& ext : extensions )
            namefilters += " *."+ext;
        filters << tr("All Supported Files (%1)").arg(namefilters);
        filters << tr("All Files (*)");
    }

    return filters;
}

AbstractFormat* Formats::formatFromNameFilter(const QString& filter, Formats::Action action) const
{
    for ( auto* format : formats_ )
        if ( format->supportsAction(action) && format->nameFilter(action) == filter )
            return format;
    return nullptr;
}

} // namespace io
