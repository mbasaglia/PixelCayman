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
#ifndef PIXEL_CAYMAN_DOCUMENT_COMMAND_SET_PROPERTY_HPP
#define PIXEL_CAYMAN_DOCUMENT_COMMAND_SET_PROPERTY_HPP

#include <QUndoCommand>
#include "command.hpp"
#include "util.hpp"

namespace document {
namespace command {

class SetPropertyMergeId
{
public:
    template<class Class, class Type>
        SetPropertyMergeId(Class* object, Type Class::*member)
        : object(object), member(util::offsetOf(object, member) + 1)
        {}

    SetPropertyMergeId() = default;

    int commandId() const
    {
        static int id = command::commandId();
        return object && object ? id : -1;
    }

    bool operator==(const SetPropertyMergeId& rhs) const
    {
        return object && object && object == rhs.object && member == rhs.member;
    }

    explicit operator bool() const
    {
        return object && member;
    }

private:
    void* object = nullptr;
    std::size_t member = 0;
};

template<class Type, class Functor>
    class SetProperty : public QUndoCommand
{
public:
    SetProperty(const QString& name,
                const Type& old_value,
                const Type& new_value,
                const Functor& setter,
                SetPropertyMergeId merge_id = {},
                QUndoCommand* parent = nullptr)
        : QUndoCommand(name, parent),
          old_value(old_value),
          new_value(new_value),
          setter(setter),
          merge_id(merge_id)
    {}

    void undo() override
    {
        setter(old_value);
    }

    void redo() override
    {
        setter(new_value);
    }

    int id() const override
    {
        return merge_id.commandId();
    }

    bool mergeWith(const QUndoCommand *other) override
    {
        if ( other->id() != id() || !merge_id )
            return false;

        auto casted = static_cast<const SetProperty*>(other);
        if ( casted->merge_id == this->merge_id )
        {
            new_value = casted->new_value;
            return true;
        }
        return false;
    }

private:
    Type old_value;
    Type new_value;
    Functor setter;
    SetPropertyMergeId merge_id;
};

template<class Type, class Functor>
    SetProperty<Type,Functor>* newSetProperty(
        const QString& name,
        const Type& old_value,
        const Type& new_value,
        const Functor& setter,
        SetPropertyMergeId merge_id = {})
    {
        return new SetProperty<Type,Functor>(name, old_value, new_value, setter, merge_id);
    }

} // namespace command
} // namespace document
#endif // PIXEL_CAYMAN_DOCUMENT_COMMAND_SET_PROPERTY_HPP
