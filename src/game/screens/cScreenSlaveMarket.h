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
class ITextRepository;

class cScreenSlaveMarket : public screens::cSlavemarketScreenBase
{
private:
    int m_SelectedGirl;
    int DetailLevel;
    int ImageNum;
    int sel_pos;
    void setup_callbacks() override;

    cBuilding* m_TargetBuilding = nullptr;

    struct RelBtnData {
        int id;
        BuildingType type;
        int index;
    };

    std::vector<RelBtnData> m_ReleaseButtons;
    std::unique_ptr<ITextRepository> m_TextRepo;
public:
    cScreenSlaveMarket();
    ~cScreenSlaveMarket();

    void init(bool back) override;
    void process() override;

    bool buy_slaves();            // `J` used when buying slaves
    void update_release_text();
    void preparescreenitems(sGirl* girl);

    int &image_num() { return ImageNum; }
    int &selected_item() { return m_SelectedGirl; }
    bool change_selected_girl(int selected);
    std::string get_buy_slave_string(sGirl* girl);

    void affect_girl_by_disposition(sGirl& girl) const;
    void affect_dungeon_girl_by_disposition(sGirl& girl) const;
};
