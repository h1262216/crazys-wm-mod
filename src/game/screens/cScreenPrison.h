/*
 * Copyright 2009-2023, The Pink Petal Development Team.
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

#include "ScreenBase.h"

class cScreenPrison : public screens::cPrisonScreenBase
{
public:
    cScreenPrison();

    void init(bool back) override;
    void more_button();
    void release_button();
    int PrisonReleaseCost(sGirl& girl);
    void update_details();
    sGirl* get_selected_girl();
    void selection_change(int selection);

private:
    int selection  = -1;
    int DetailLevel = 0;
    void setup_callbacks() override;
};
