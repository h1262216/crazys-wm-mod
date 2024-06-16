/*
 * Copyright 2023, The Pink Petal Development Team.
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

#pragma once

#ifndef CRAZYS_WM_MOD_IBUILDING_H
#define CRAZYS_WM_MOD_IBUILDING_H

#include "jobs/IGenericJob.h"

class IBuilding {
public:
    virtual ~IBuilding() = default;

    virtual void BeginWeek() = 0;
    virtual void EndWeek() = 0;

    virtual void RunShift(bool is_night) = 0;
};


#endif // CRAZYS_WM_MOD_IBUILDING_H