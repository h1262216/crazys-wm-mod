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

class cScreenDungeon : public screens::cDungeonScreenBase
{
public:
    cScreenDungeon();

    enum {
        GirlLoses = 0,
        GirlWins = 1
    };
    bool    torture_possible();
    int     enslave();
    int     enslave_customer(int girls_removed, int custs_removed);
    int     view_girl();
    void    init(bool back) override;
    void    process() override {};
    void    selection_change();
    void    select_all_customers();
    void    select_all_girls();
    void    sell_slaves();
    void    stop_feeding();
    void    start_feeding();
    void    torture_customer(int girls_removed);
    void    torture();
    void    change_release(BuildingType type, int index);
    void    release();
    void    talk();
    void    get_selected_girls(std::vector<int> *sel);
    void    update_image();

    sGirl* get_image_girl() override;

private:
    int        sel_pos;
    int        selection;
    bool    no_guards;
    enum    {
        Return,
        Continue
    };

    void setup_callbacks() override;

    cBuilding* m_ReleaseBuilding = nullptr;
};
