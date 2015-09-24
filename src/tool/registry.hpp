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
#ifndef PIXEL_CAYMAN_TOOL_REGISTRY_HPP
#define PIXEL_CAYMAN_TOOL_REGISTRY_HPP

#include "tool.hpp"

namespace tool {

/**
 * \brief Keeps track of the registered tools
 */
class Registry : public QObject
{
    Q_OBJECT

public:
    /**
     * \brief Singleton instance
     */
    static Registry& instance()
    {
        static Registry singleton;
        return singleton;
    }

    /**
     * \brief Registers a tool
     * \tparam ToolType Class derived from tool::Tool
     * \tparam Args     Types of the constructor arguments (deduced by the call)
     * \param  id       Unique identifier for the tool
     * \param  args     Contructor arguments
     *
     * Creates an object of type \p ToolType and attempts to register it.
     * Emits toolAdded() on successful insertions.
     *
     * \returns The created format object on success, \b nullptr on failure
     */
    template<class ToolType, class... Args>
        ToolType* addTool(const QString& id, Args&&... args)
        {
            if ( tools_.contains(id) )
                return nullptr;

            auto ptr = new ToolType(std::forward<Args>(args)...);
            tools_[id] = ptr;
            emit toolAdded(ptr);
            return ptr;
        }

    /**
     * \brief Removes a tool by id
     *
     * Emits toolRemoved() on a successful removal
     */
    void removeTool(const QString& id)
    {
        auto it = tools_.find(id);
        if ( it != tools_.end() )
        {
            emit toolRemoved(*it);
            delete *it;
            tools_.erase(it);
        }
    }

    /**
     * \brief Get a tool by id
     */
    Tool* tool(const QString& id) const
    {
        return tools_[id];
    }

    /**
     * \brief All tools
     */
    QList<Tool*> tools() const
    {
        return tools_.values();
    }

signals:
    void toolAdded(Tool* tool);
    void toolRemoved(Tool* tool);

private:
    Registry(){}
    ~Registry()
    {
        for ( auto tool : tools_ )
            delete tool;
    }

    QMap<QString, Tool*> tools_;
};

} // namespace tool
#endif // PIXEL_CAYMAN_TOOL_REGISTRY_HPP
