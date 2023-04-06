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

class static_brothel_data;

class cScreenTown : public screens::cTownScreenBase
{
public:
    cScreenTown();

    void init(bool back) override;
    void process() override;
    void do_walk();
    std::string walk_no_luck();

    sGirl* get_image_girl() override;

private:
    bool m_first_walk;
    std::shared_ptr<sGirl> m_MeetingGirl;

    void setup_callbacks() override;
    bool buy_building(static_brothel_data* bck);
    void check_building(int BrothelNum);
};
