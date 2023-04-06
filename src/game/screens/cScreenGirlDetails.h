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
#include "traits/ITraitsCollection.h"

class cScreenGirlDetails : public screens::cGirlDetailsScreenBase
{
public:
    cScreenGirlDetails();

    void init(bool back) override;
    void process() override;

    void do_interaction();

    void send_to_dungeon();

    void release_from_dungeon();

    void on_select_job(int selection, bool fulltime);

    void set_house_percentage(int value);
    void set_accomodation(int value);

    sGirl* get_image_girl() override;

private:
    void setup_callbacks() override;
    void RefreshJobList();
    void PrevGirl();
    void NextGirl();

    void take_gold(sGirl& girl);
    void set_shift(int shift);

    std::shared_ptr<sGirl> m_SelectedGirl = nullptr;

    void OnKeyPress(SDL_Keysym keysym) override;

    bool m_Refresh = false;
    bool m_EditNightShift = false;

    std::vector<sTraitInfo> m_TraitInfoCache;
    int DetailLevel = 0;
};
