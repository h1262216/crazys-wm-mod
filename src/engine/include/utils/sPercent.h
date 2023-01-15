/*
 * Copyright 2019-2022, The Pink Petal Development Team.
 * The Pink Petal Development Team are defined as the game's coders
 * who meet on http://pinkpetal.org
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

#ifndef WM_SPERCENT_H
#define WM_SPERCENT_H

#include <stdexcept>

struct sPercent {
    explicit sPercent(float v) : value(v) {
        if(v < 0 || 1.f < v) {
            throw std::invalid_argument("Invalid percentage");
        }
    }

    explicit sPercent(int v) : sPercent(float(v) / 100.f) {};

    explicit operator float() const { return value; }

    float as_ratio() const { return value; }
    float as_percentage() const { return 100.f * value; }
private:
    float value;
};

inline sPercent operator*(sPercent a, sPercent b) { return sPercent{a.as_ratio() * b.as_ratio()}; }

#endif //WM_SPERCENT_H
