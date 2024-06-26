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
#include "Constants.h"

class cScreenBuildingSetup : public screens::cBuildingSetupScreenBase
{
public:
    cScreenBuildingSetup();

    void init(bool back) override;

    void buy_rooms();
    void buy_potions(int buypotions);

private:
    void setup_callbacks() override;
    void set_sex_type_allowed(SKILLS sex_type, bool is_forbidden);

    struct STCB {
        int id;
        SKILLS skill;
    };

    std::vector<STCB> m_SexTypeAllowedMap;
};
