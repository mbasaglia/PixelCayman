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
#ifndef PIXEL_CAYMAN_PLUGINS_EXTERNAL_TOOLS_PLUGIN_HPP
#define PIXEL_CAYMAN_PLUGINS_EXTERNAL_TOOLS_PLUGIN_HPP

#include "plugin.hpp"
#include "ui/menu.hpp"
#include "external_tools.hpp"

namespace extools {

/**
 * \brief Inkscape plugin
 */
class ExternalToolsPlugin : public CaymanPlugin
{
    Q_OBJECT

public:
    ExternalToolsPlugin()
    {
        connect(&ExternalTools::instance(), &ExternalTools::toolAdded,
                this, &ExternalToolsPlugin::toolAdded);
        connect(&ExternalTools::instance(), &ExternalTools::toolRemoved,
                this, &ExternalToolsPlugin::toolRemoved);
    }

protected:
    QString onId() override
    {
        return "external_tools";
    }

    bool onLoad() override
    {
        menu = new QMenu(tr("E&xternal"));
        for ( const auto& tool : ExternalTools::instance().tools() )
            createAction(tool);
        Menu::instance().addMenu(onId(), menu);
        return true;
    }

    void onUnload() override
    {
        Menu::instance().removeMenu(onId());
        menu = nullptr;
    }

    QString onName() override
    {
        return tr("External tools");
    }

private slots:
    void toolAdded(const ExternalTool& tool)
    {
        if ( !menu )
            return;

        menu->addAction(createAction(tool));
    }

    void toolRemoved(const ExternalTool& tool)
    {
        if ( !menu )
            return;

        if ( QAction* action = menu->findChild<QAction*>(actionName(tool)) )
            menu->removeAction(action);
    }

private:
    QAction* createAction(const ExternalTool& tool)
    {
        QAction* action = new QAction(tool.name, menu);
        action->setObjectName(actionName(tool));
        auto id = tool.id;
        connect(action, &QAction::triggered, [id]{
            ExternalTools::instance().execute(id);
        });
        menu->addAction(action);
        return action;
    }

    QString actionName(const ExternalTool& tool)
    {
        return "action_"+onId()+"_"+tool.id;
    }

   QMenu* menu = nullptr;
};

} // namespace extools
#endif // PIXEL_CAYMAN_PLUGINS_EXTERNAL_TOOLS_PLUGIN_HPP
