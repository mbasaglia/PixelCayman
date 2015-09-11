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

#include <memory>
#include <vector>
#include "tool.hpp"

namespace tool {

/**
 * \brief Keeps track of the registered tools
 */
class Registry
{
public:
    /**
     * \brief Simple class to register a tool from a translation unit
     */
    template<class T>
    struct StaticRegistrant
    {
        template<class... Args>
            StaticRegistrant(Args&&... arg)
            {
                instance().register_tool<T>(std::forward<Args>(arg)...);
            }
    };

    static Registry& instance()
    {
        static Registry singleton;
        return singleton;
    }

    template<class T, class... Args>
        void register_tool(Args&&... arg)
        {
            tools_.push_back(std::unique_ptr<T>(new T(std::forward<Args>(arg)...)));
        }

    const std::vector<std::unique_ptr<Tool>>& tools() const
    {
        return tools_;
    }

private:
    Registry(){}
    ~Registry(){}
    std::vector<std::unique_ptr<Tool>> tools_;
};

} // namespace tool
#endif // PIXEL_CAYMAN_TOOL_REGISTRY_HPP
