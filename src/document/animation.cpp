/**
 * \file
 *
 * \author Mattia Basaglia
 *
 * \section License
 *
 * Copyright (C) 2015 Mattia Basaglia
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

#include "animation.hpp"
#include "visitor.hpp"

namespace document {

Animation::Animation(const QString& name, int frames_per_second)
    : name_(name), fps_(frames_per_second)
{
}

void Animation::apply(Visitor& visitor)
{
    if ( visitor.enter(*this) )
    {
        for ( auto frame : frames_ )
            frame->apply(visitor);

        visitor.leave(*this);
    }
}

QString Animation::name() const
{
    return name_;
}

void Animation::setName(const QString& name)
{
    name_ = name;
}

int Animation::framesPerSecond() const
{
    return fps_;
}

void Animation::setFramesPerSecond(int fps)
{
    fps_ = fps;
}

} // namespace document
