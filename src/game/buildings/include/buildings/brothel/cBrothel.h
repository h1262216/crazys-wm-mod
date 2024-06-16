/*
 * Copyright 2009, 2010, The Pink Petal Development Team.
 * The Pink Petal Devloment Team are defined as the game's coders
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

#ifndef __CBROTHEL_H
#define __CBROTHEL_H

#include "buildings/cBuilding.h"

// ---------------------------------------------------------------------------------------------------------------------

// defines a single brothel
struct sBrothel : public cBuilding
{
    explicit sBrothel();
    ~sBrothel() override;

    // currently unused for brothel
    void auto_assign_job(sGirl& target, std::stringstream& message, bool is_night) override {};

    bool runaway_check(sGirl& girl);

    void OnEndWeek() override;
    void OnBeginWeek() override;
};

#endif  /* __CBROTHEL_H */
