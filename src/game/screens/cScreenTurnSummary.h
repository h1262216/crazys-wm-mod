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

#include "ScreenBase.h"
#include "cEvents.h"
#include <deque>

class cScreenTurnSummary : public screens::cTurnSummaryBase
{
private:
    enum SummaryCategory {
        Summary_GIRLS,
        Summary_GANGS,
        Summary_BUILDINGS,
        Summary_DUNGEON,
        Summary_RIVALS,
    };

    SummaryCategory m_ActiveCategory = SummaryCategory::Summary_GIRLS;
    std::shared_ptr<const CombatReport> m_ActiveReport;

public:
    cScreenTurnSummary();

    void init(bool back) override;
    void process() override;

    void Fill_Items_GANGS();
    void Fill_Items_BUILDINGS();
    void Fill_Items_DUNGEON();
    void Fill_Items_GIRLS(cBuilding * building);

    void Fill_Events(sGirl* girl);
    void Fill_Events_Gang(int gang_id);
    void Fill_Events_Buildings(int building_id);

    void change_category(SummaryCategory);

    void change_item(int selection);

    void change_event(int selection);

    void goto_selected();

    void Fill_Items_RIVALS();
    void Fill_Events_Rivals();

    void set_backdrop(const std::string& bd);

private:
    // Tracking recently presented images
    struct sRecent {
        const CEvent* Event;
        int Week;
        std::string Image;
    };
    std::deque<sRecent> m_RecentImages;

    void present_image(const CEvent& event);
    void setup_callbacks() override;
};