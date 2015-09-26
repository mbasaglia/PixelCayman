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
#ifndef MISCLIB_MATH_HPP
#define MISCLIB_MATH_HPP

#include <utility>

namespace math {

/**
 * \brief Maximum between two values
 */
template<class T>
    inline constexpr T max(T&& a, T&& b)
    {
        return a < b ? b : a;
    }

/**
 * \brief Maximum among several values
 */
template<class T, class...Ts>
    inline constexpr T max(T&& a,  Ts&&... b)
    {
        return max(std::forward<T>(a), max(std::forward<Ts>(b)...));
    }

/**
 * \brief Minimum between two values
 */
template<class T>
    inline constexpr T min(T&& a, T&& b)
    {
        return !(a < b) ? b : a;
    }

/**
 * \brief Minimum among several values
 */
template<class T, class...Ts>
    inline constexpr T min(T&& a,  Ts&&... b)
    {
        return min(std::forward<T>(a), min(std::forward<Ts>(b)...));
    }

/**
 * \brief Absolute value
 */
template<class T>
    inline constexpr T abs(T&& x)
    {
        return x < 0 ? -x : x;
    }

/**
 * \brief Limit \p value to be in [min_value, max_value]
 * \pre min_value < max_value
 * \post value in [min_value, max_value]
 */
template<class T>
    inline constexpr T bound(T&& min_value,  T&& value, T&& max_value)
    {
        return max(std::forward<T>(min_value),
            min(std::forward<T>(value), std::forward<T>(max_value))
        );
    }

/**
 * \brief Normalize a value
 * \pre  value in [min, max] && min < max
 * \post value in [0, 1]
 */
template<class Real>
    inline constexpr Real normalize(Real value, Real min, Real max)
{
    return (value - min) / (max - min);
}

/**
 * \brief Denormalize a value
 * \pre  value in [0, 1] && min < max
 * \post value in [min, max]
 */
template<class Real>
    inline constexpr Real denormalize(Real value, Real min, Real max)
{
    return value * (max - min) + min;
}


} // namespace math
#endif // MISCLIB_MATH_HPP
