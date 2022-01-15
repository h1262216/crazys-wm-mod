/*
* Copyright 2009, 2010, The Pink Petal Development Team.
* The Pink Petal Devloment Team are defined as the game's coders
* who meet on http://pinkpetal.org     // old site: http://pinkpetal .co.cc
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

#include "cGameWindow.h"

class static_brothel_data;

class cScreenTown : public cGameWindow
{
private:
    int back_id;            // Back button
    int gold_id;            // Player Gold
    int walk_id;            // Walk Around Town button
    int curbrothel_id;      // Current Brothel text
    int slavemarket_id;     // Slave Market map button
    int shop_id;            // Shop map button
    int house_id;           // Player's House map button
    int clinic_id;          // Clinic's House map button
    int studio_id;          // Studio's House map button
    int arena_id;           // Arena's House map button
    int centre_id;          // Centre's House map button
    int farm_id;            // Farm's map button
    int brothel0_id;        // Brothel 0 map button
    int brothel1_id;        // Brothel 1 map button
    int brothel2_id;        // Brothel 2 map button
    int brothel3_id;        // Brothel 3 map button
    int brothel4_id;        // Brothel 4 map button
    int brothel5_id;        // Brothel 5 map button
    int brothel6_id;        // Brothel 6 map button
    int setup_id;           // setup map button
    int girlimage_id;       // Girl image
    int next_week_id;       // Next Week Button

    bool m_first_walk;
    std::shared_ptr<sGirl> m_MeetingGirl;

    void set_ids() override;
    bool buy_building(static_brothel_data* bck);
    void check_building(int BrothelNum);
public:
    cScreenTown();

    void init(bool back) override;
    void process() override;
    void do_walk();
    std::string walk_no_luck();

    void UpdateImage(EBaseImage imagetype) override;
};
