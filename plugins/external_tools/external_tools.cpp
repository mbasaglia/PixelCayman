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

#include "external_tools.hpp"
#include "external_tools_plugin.hpp"
#include "cayman/data.hpp"

namespace extools {

bool ExternalTools::addTool(const ExternalTool& tool)
{
    if ( tools_.contains(tool.id) )
        return false;

    ExternalTool& reference = tools_[tool.id];
    reference = tool;
    emit toolAdded(reference);
    return true;
}

void ExternalTools::removeTool(const QString& id)
{
    auto it = tools_.find(id);
    if ( it == tools_.end() )
        return;
    emit toolRemoved(*it);
    tools_.erase(it);
}

bool ExternalTools::execute(const QString& id)
{
    auto it = tools_.find(id);
    if ( it == tools_.end() )
        return false;

    auto args = it->args;
    // Search arguments that need expansion
    for ( auto& arg : args )
    {
        QRegularExpressionMatch match = regex_temparg.match(arg);
        if ( match.hasMatch() )
        {
            if ( !plugin::api().currentDocument() )
                return false; // No document to open with the external tool
            // Create a temp file
            QTemporaryFile file(cayman::data().tempDir()+"tempXXXXXX."+match.captured(1));
            file.setAutoRemove(false);
            if ( !file.open() )
                return false; // Could not open the temp file
            arg = file.fileName();
            // Save the temp file
            auto format = io::formats().formatFromFileName(file.fileName(), io::Formats::Action::Save);
            if ( !format )
                return false; // Could not find a format
            if ( !format->save(plugin::api().currentDocument(), &file) )
                return false; // Error while saving
        }
    }
    return QProcess::startDetached(it->command, args);
}

bool ExternalTools::usesDocument(const ExternalTool& tool) const
{
    return tool.args.indexOf(regex_temparg) != -1;
}

} // namespace extools

extern "C" bool addTool(const extools::ExternalTool& tool)
{
    return extools::ExternalTools::instance().addTool(tool);
}

extern "C" void removeTool(const QString& id)
{
    return extools::ExternalTools::instance().removeTool(id);
}

INIT_PLUGIN(extools::ExternalToolsPlugin)
