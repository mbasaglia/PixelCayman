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
#ifndef PIXEL_CAYMAN_PLUGINS_EXTERNAL_TOOLS_HPP
#define PIXEL_CAYMAN_PLUGINS_EXTERNAL_TOOLS_HPP

#include <QObject>
#include <QStringList>
#include <QMap>
#include <QProcess>
#include <QTemporaryFile>
#include <QRegularExpression>
#include <QApplication>

#include "plugin/plugin_api.hpp"
#include "io/formats.hpp"
#include "data.hpp"

namespace extools {

/**
 * \brief Descriptor of an external tool
 */
struct ExternalTool
{
    QString     id;     ///< Unique ID
    QString     name;   ///< Name shown on the UI
    QString     command;///< Name of the command
    QStringList args;   ///< Command arguments \todo tempfile
};

class ExternalTools : public QObject
{
    Q_OBJECT

public:
    static ExternalTools& instance()
    {
        static ExternalTools singleton;
        return singleton;
    }

    ExternalTool tool(const QString& id) const
    {
        return tools_[id];
    }

    QList<ExternalTool> tools() const
    {
        return tools_.values();
    }

    bool addTool(const ExternalTool& tool);

    void removeTool(const QString& id);

    bool execute(const QString& id);

signals:
    void toolAdded(const ExternalTool& tool);
    void toolRemoved(const ExternalTool& tool);

private:
    ExternalTools(){}

    QMap<QString, ExternalTool> tools_;
};

} // namespace extools

extern "C" Q_DECL_EXPORT bool addTool(const extools::ExternalTool& tool);
extern "C" Q_DECL_EXPORT void removeTool(const QString& id);

#endif // PIXEL_CAYMAN_PLUGINS_EXTERNAL_TOOLS_HPP
