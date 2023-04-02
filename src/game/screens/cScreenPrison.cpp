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

#include "buildings/cBuilding.h"
#include "buildings/cDungeon.h"
#include "cScreenPrison.h"
#include "interface/cWindowManager.h"
#include "IGame.h"
#include <sstream>
#include "character/predicates.h"
#include "character/cGirlPool.h"

cScreenPrison::cScreenPrison() = default;

void cScreenPrison::init(bool back)
{
    std::stringstream ss;
    Focused();

    DisableWidget(m_ShowMoreBtn_id, true);
    
    DisableWidget(m_ReleaseBtn_id, true);
    selection = -1;
    update_details();

    ClearListBox(m_PrisonList_id);
    auto& prison = g_Game->GetPrison();
    for(int i = 0; i < prison.num(); ++i)
    {
        sGirl* girl = prison.get_girl(i);
        int cost = PrisonReleaseCost(*girl);
        ss << girl->FullName() << "  (release cost: " << cost << " gold)";
        AddToListBox(m_PrisonList_id, i, ss.str());
        i++;
        ss.str("");
    }
}

void cScreenPrison::setup_callbacks()
{
    SetButtonCallback(m_ShowMoreBtn_id, [this]() { more_button(); });
    SetButtonCallback(m_ReleaseBtn_id, [this]() {
        release_button();
        init(false);
    });

    SetListBoxSelectionCallback(m_PrisonList_id, [this](int sel) { selection_change(0); });
    SetListBoxHotKeys(m_PrisonList_id, SDLK_a,  SDLK_d);
}

void cScreenPrison::selection_change(int selection)
{
    DisableWidget(m_ShowMoreBtn_id, (selection == -1));
    DisableWidget(m_ReleaseBtn_id, (selection == -1));
    this->selection = selection;
    update_details();
}

void cScreenPrison::update_details()
{
    EditTextItem("No Prisoner Selected", m_GirlDescription_id);
    if (selection == -1) return;
    sGirl* pgirls = get_selected_girl();
    if (!pgirls) return;

    if (DetailLevel == 1)        EditTextItem(cGirls::GetMoreDetailsString(*pgirls, true), m_GirlDescription_id, true);
    else if (DetailLevel == 2)    EditTextItem(cGirls::GetThirdDetailsString(*pgirls), m_GirlDescription_id, true);
    else                        EditTextItem(cGirls::GetDetailsString(*pgirls, true), m_GirlDescription_id, true);
}

sGirl* cScreenPrison::get_selected_girl()
{
    return g_Game->GetPrison().get_girl(selection);
}

void cScreenPrison::more_button()
{
    if (DetailLevel == 0)        DetailLevel = 1;
    else if (DetailLevel == 1)    DetailLevel = 2;
    else                        DetailLevel = 0;
    update_details();
}

void cScreenPrison::release_button()
{
    if (selection == -1) return;
    sGirl* pgirls = get_selected_girl();
    if (!pgirls) return;
    int cost = PrisonReleaseCost(*pgirls);
    if (!g_Game->gold().afford((double)cost))
    {
        push_message("You don't have enough gold", 0);
        return;
    }
    g_Game->gold().item_cost((double)cost);
    g_Game->gold().girl_support((double)cost);
    auto girl = g_Game->GetPrison().TakeGirl(pgirls);
    cBuilding& bld = active_building();
    if (bld.free_rooms() < 1)
    {
        std::string text = pgirls->FullName();
        text += " has been sent to your dungeon, since current brothel is full.";
        push_message(text, 0);
        g_Game->dungeon().AddGirl(std::move(girl), DUNGEON_NEWGIRL);
    }
    else
    {
        std::string text = pgirls->FullName();
        text += " has been sent to your current brothel.";
        push_message(text, 0);
        bld.add_girl(std::move(girl));
    }
}

int cScreenPrison::PrisonReleaseCost(sGirl& girl)
{
    cGirls::UpdateAskPrice(girl, false);
    int cost = girl.askprice() * 15;
    cost += cGirls::GetSkillWorth(girl);
    if (is_virgin(girl)) cost += int(cost / 2);    //    `J` fixed virgin adds half cost more
    cost *= 2;
    return cost;
}

std::shared_ptr<cInterfaceWindow> screens::cPrisonScreenBase::create() {
    return std::make_shared<cScreenPrison>();
}

