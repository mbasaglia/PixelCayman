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
    /**
     * \brief Command arguments
     *
     * If an argument is in the form ${tempfile}.png
     * it will create a temporary file for the current document and
     * pass the name of that file instead of the argument
     */
    QStringList args;
};

/**
 * \brief Class that manages external tools
 */
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

    /**
     * \brief Register a tool
     * \return \b false on failure (a tool with the same ID exists)
     */
    bool addTool(const ExternalTool& tool);

    /**
     * \brief Remove a tool by id
     */
    void removeTool(const QString& id);

    /**
     * \brief Execute the command from a tool
     * \return \b true if the command has been started
     */
    bool execute(const QString& id);

    /**
     * \brief Whether a tool needs to the current document to operate
     */
    bool usesDocument(const ExternalTool& tool) const;

signals:
    void toolAdded(const ExternalTool& tool);
    void toolRemoved(const ExternalTool& tool);

private:
    ExternalTools(){}

    QMap<QString, ExternalTool> tools_;
    /// \todo Allow for extra arguments to select the frame and
    ///       other format-specific options
    QRegularExpression          regex_temparg{R"(\$\{temp\}\.(.+))"};
};

} // namespace extools

extern "C" Q_DECL_EXPORT bool addTool(const extools::ExternalTool& tool);
extern "C" Q_DECL_EXPORT void removeTool(const QString& id);

#endif // PIXEL_CAYMAN_PLUGINS_EXTERNAL_TOOLS_HPP
