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
#ifndef MISCLIB_UTIL_HPP
#define MISCLIB_UTIL_HPP

#include <utility>
#include <tuple>

namespace util {

/**
 * \brief Utility to get a cleaner syntax when disambiguating function pointers
 */
template<class ...Args, class C, class R>
    auto overload(R (C::*f)(Args...)) -> R (C::*)(Args...)
    {
        return f;
    }

/**
 * \brief Determine the distance between a data member and its object
 */
template <typename Class, typename Type>
    std::size_t offset_of(const Class* object, Type Class::* member)
    {
        return reinterpret_cast<const char *>(&(object->*member)) -
               reinterpret_cast<const char *>(object);
    }

/**
 * \brief Template to retrieve information about a function signature
 *
 * Use as FunctionSignature<Ret (Args...)> or FunctionSignature<Pointer>
 */
template<class T>
    struct FunctionSignature;

template<class Ret, class...Args>
    struct FunctionSignature<Ret(Args...)>
    {
        using pointer_type = Ret (*) (Args...);
        using return_type = Ret;
        using arguments_types = std::tuple<Args...>;
    };

template<class Ret, class...Args>
    struct FunctionSignature<Ret(*)(Args...)>
        : public FunctionSignature<Ret(Args...)>
    {
    };

/**
 * \brief Clean syntax to get a function pointer type
 */
template<class T>
    using FunctionPointer = typename FunctionSignature<T>::pointer_type;

} // namespace util
#endif // MISCLIB_UTIL_HPP
