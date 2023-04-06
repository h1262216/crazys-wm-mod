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

class cScreenGangs : public screens::cGangsScreenBase
{
public:
    cScreenGangs();

    void init(bool back) override;

    int set_mission_desc(int mid);
    std::string mission_desc(int mid);
    std::string short_mission_desc(int mid);

    void buy_nets(int buynets);

    void buy_potions(int buypots);

    void on_select_mission(int missing_id);

    void on_select_gang(int selection);

    void update_sliders();
    void update_wpn_info();
    void update_recruit_btn();

private:
    void setup_callbacks() override;
    int        sel_pos;
    void hire_recruitable();
};
