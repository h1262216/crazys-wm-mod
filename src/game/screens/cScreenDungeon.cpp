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

#include <string>
#include <sstream>
#include <algorithm>
#include "widgets/IListBox.h"
#include "cScreenDungeon.h"
#include "buildings/cBuildingManager.h"
#include "buildings/cDungeon.h"
#include "character/cPlayer.h"
#include "cTariff.h"
#include "cGirlGangFight.h"
#include "cGirlTorture.h"
#include "IGame.h"

extern cRng                    g_Dice;

static std::stringstream ss;

cScreenDungeon::cScreenDungeon()
{
    selection = -1;
}

void cScreenDungeon::setup_callbacks()
{
    m_MainImageId   = get_id("GirlImage");

    struct RelBtn {
        const char* name;
        BuildingType type;
        int index;
    };

    RelBtn btns[] = {
            {"Brothel0", BuildingType::BROTHEL, 0},
            {"Brothel1", BuildingType::BROTHEL, 1},
            {"Brothel2", BuildingType::BROTHEL, 2},
            {"Brothel3", BuildingType::BROTHEL, 3},
            {"Brothel4", BuildingType::BROTHEL, 4},
            {"Brothel5", BuildingType::BROTHEL, 5},
            {"Brothel6", BuildingType::BROTHEL, 6},
            {"House", BuildingType::HOUSE, 0},
            {"Clinic", BuildingType::CLINIC, 0},
            {"Studio", BuildingType::STUDIO, 0},
            {"Arena", BuildingType::ARENA, 0},
            {"Centre", BuildingType::CENTRE, 0},
            {"Farm", BuildingType::FARM, 0},
    };

    for(const auto& btn : btns) {
        SetButtonCallback(get_id(btn.name), [this, btn]() {
            change_release(btn.type, btn.index);
        });
    }

    SetButtonCallback(m_BrandSlaveBtn_id, [this]() { enslave(); });
    SetButtonCallback(m_SelectAllCust_id, [this]() { select_all_customers(); });
    SetButtonCallback(m_SellBtn_id, [this]() { sell_slaves(); });
    SetButtonCallback(m_DetailsBtn_id, [this]() { view_girl(); });
    SetButtonHotKey(m_DetailsBtn_id, SDLK_SPACE);
    SetButtonCallback(m_SelectAllGirls_id, [this]() {
        select_all_girls();
    });

    SetButtonCallback(m_StopFeedingBtn_id, [this]() {
        stop_feeding();
        selection = -1;
    });

    SetButtonCallback(m_AllowFoodBtn_id, [this]() {
        start_feeding();
        selection = -1;
    });

    SetButtonCallback(m_InteractBtn_id, [this]() {
        if (selection == -1) return;
        talk();
    });

    SetButtonCallback(m_TortureBtn_id, [this]() {
        torture();
        selection = -1;
    });

    SetButtonCallback(m_ReleaseBtn_id, [this]() {
        release();
        selection = -1;
        init(false);
    });

    SetListBoxSelectionCallback(m_GirlList_id, [this](int selected) {
        selection_change();
        if (IsMultiSelected(m_GirlList_id))         // disable buttons based on multiselection
        {
            DisableWidget(m_InteractBtn_id, true);
        }
        update_image();
    });
    SetListBoxDoubleClickCallback(m_GirlList_id, [this](int selected) {
        view_girl();
    });
    SetListBoxHotKeys(m_GirlList_id, SDLK_a, SDLK_d);
}

void cScreenDungeon::init(bool back)
{
    if(!back)
        m_ReleaseBuilding = &active_building();

    Focused();
    ClearListBox(m_GirlList_id);                // clear the lists

    if (m_Gold_id >= 0)
    {
        ss.str(""); ss << "Gold: " << g_Game->gold().ival();
        EditTextItem(ss.str(), m_Gold_id);
    }

    ss.str("");    ss << "Your Dungeon where " << g_Game->dungeon().GetNumDied() << " people have died.";
    EditTextItem(ss.str(), m_DungeonHeader_id);
    // Fill the list box
    selection = g_Game->dungeon().GetNumGirls() > 0 ? 0 : -1;
    for (int i = 0; i < g_Game->dungeon().GetNumGirls(); i++)                                                // add girls
    {
        auto* dgirl = g_Game->dungeon().GetGirl(i);         // get the i-th girl
        if (selected_girl().get() == dgirl->m_Girl.get()) selection = i;                                                            // if selected_girl is this girl, update selection
        dgirl->m_Girl->m_DayJob = dgirl->m_Girl->m_NightJob = JOB_INDUNGEON;
        int col = ((dgirl->m_Girl->health() <= 30) || (dgirl->m_Girl->happiness() <= 30)) ? COLOR_WARNING : COLOR_NEUTRAL;            // if she's low health or unhappy, flag her entry to display in red // Anon21
        GetListBox(m_GirlList_id)->AddRow(i, dgirl, col);
    }
    // now add the customers
    int offset = g_Game->dungeon().GetNumGirls();
    for (int i = 0; i < g_Game->dungeon().GetNumCusts(); i++)    // add customers
    {
        sDungeonCust* cust = g_Game->dungeon().GetCust(i);
        int col = (cust->m_Health <= 30) ? COLOR_WARNING : COLOR_NEUTRAL;
        GetListBox(m_GirlList_id)->AddRow(i + g_Game->dungeon().GetNumGirls(), cust, col);
    }

    // disable some buttons
    DisableWidget(m_AllowFoodBtn_id);
    DisableWidget(m_StopFeedingBtn_id);
    DisableWidget(m_InteractBtn_id);
    if (m_InteractCount_id >= 0)
    {
        ss.str(""); ss << "Interactions Left: ";
        if (g_Game->allow_cheats()) ss << "Infinate Cheat";
        else if (g_Game->GetTalkCount() <= 0) ss << "0 (buy in House screen)";
        else ss << g_Game->GetTalkCount();
        EditTextItem(ss.str(), m_InteractCount_id);
    }
    DisableWidget(m_ReleaseBtn_id);
    DisableWidget(m_BrandSlaveBtn_id);
    DisableWidget(m_TortureBtn_id);
    DisableWidget(m_SellBtn_id);
    //    cerr << "::init: disabling torture" << endl;    // `J` commented out
    DisableWidget(m_DetailsBtn_id);
    DisableWidget(m_SelectAllGirls_id, (g_Game->dungeon().GetNumGirls() <= 0));    // only enable "release all girls" if there are girls to release
    DisableWidget(m_SelectAllCust_id, (g_Game->dungeon().GetNumCusts() <= 0));    // similarly...

    HideWidget(m_MainImageId, g_Game->dungeon().GetNumGirls() <= 0);

    ss.str("");    ss << "Release Girl to: " << m_ReleaseBuilding->name();
    EditTextItem(ss.str(), m_ReleaseTo_id);
    ss.str("");    ss << "Room for " << m_ReleaseBuilding->free_rooms() << " more girls.";
    EditTextItem(ss.str(), m_RoomsFree_id);

    int mum_brothels = g_Game->buildings().num_buildings(BuildingType::BROTHEL);
    HideWidget(m_Brothel1_id, mum_brothels < 2);
    HideWidget(m_Brothel2_id, mum_brothels < 3);
    HideWidget(m_Brothel3_id, mum_brothels < 4);
    HideWidget(m_Brothel4_id, mum_brothels < 5);
    HideWidget(m_Brothel5_id, mum_brothels < 6);
    HideWidget(m_Brothel6_id, mum_brothels < 7);
    HideWidget(m_Clinic_id, !g_Game->has_building(BuildingType::CLINIC));
    HideWidget(m_Studio_id, !g_Game->has_building(BuildingType::STUDIO));
    HideWidget(m_Arena_id, !g_Game->has_building(BuildingType::ARENA));
    HideWidget(m_Centre_id, !g_Game->has_building(BuildingType::CENTRE));
    HideWidget(m_Farm_id, !g_Game->has_building(BuildingType::FARM));

    // if a selection of girls is stored, try to re-select them
    if (selection >= 0) SetSelectedItemInList(m_GirlList_id, selection);
}

void cScreenDungeon::selection_change()
{
    selection = GetSelectedItemFromList(m_GirlList_id);
    // if nothing is selected, then we just need to disable some buttons and we're done
    if (selection == -1)
    {
        set_active_girl(nullptr);
        DisableWidget(m_BrandSlaveBtn_id);
        DisableWidget(m_AllowFoodBtn_id);
        DisableWidget(m_StopFeedingBtn_id);
        DisableWidget(m_InteractBtn_id);
        DisableWidget(m_ReleaseBtn_id);
        DisableWidget(m_TortureBtn_id);
        DisableWidget(m_DetailsBtn_id);
        DisableWidget(m_SellBtn_id);
        return;
    }
    // otherwise, we need to enable some buttons...
    DisableWidget(m_SellBtn_id);
    DisableWidget(m_TortureBtn_id, !torture_possible());
    DisableWidget(m_InteractBtn_id, g_Game->GetTalkCount() == 0 || IsMultiSelected(m_GirlList_id));
    EnableWidget(m_ReleaseBtn_id);
    DisableWidget(m_BrandSlaveBtn_id);
    // and then decide if this is a customer selected, or a girl customer is easiest, so we do that first
    if ((selection - g_Game->dungeon().GetNumGirls()) >= 0)
    {
        // It's a customer! All we need to do is toggle some buttons
        int num = (selection - g_Game->dungeon().GetNumGirls());
        sDungeonCust* cust = g_Game->dungeon().GetCust(num);
        if(cust == nullptr) {
            push_message("Could not select the target. If you encounter this error, please"
                         "provide us a description / save game of how you managed this.", COLOR_WARNING);
            return;
        }
        DisableWidget(m_DetailsBtn_id);
        DisableWidget(m_InteractBtn_id);
        DisableWidget(m_AllowFoodBtn_id, cust->m_Feeding);
        DisableWidget(m_StopFeedingBtn_id, !cust->m_Feeding);
        return;
    }
    // Not a customer then. Must be a girl...
    int num = selection;
    auto dgirl = g_Game->dungeon().GetGirl(num);
    auto girl = dgirl->m_Girl;
    // again, we're just enabling and disabling buttons
    EnableWidget(m_DetailsBtn_id);

    DisableWidget(m_AllowFoodBtn_id, dgirl->m_Feeding);
    DisableWidget(m_StopFeedingBtn_id, !dgirl->m_Feeding);
    // some of them partly depend upon whether she's a slave or not
    if (dgirl->m_Girl->is_slave())
    {
        EnableWidget(m_SellBtn_id);
        DisableWidget(m_BrandSlaveBtn_id);
    }
    else
    {
        EnableWidget(m_BrandSlaveBtn_id);
        DisableWidget(m_SellBtn_id);
    }
    set_active_girl(std::move(girl));
}

int cScreenDungeon::view_girl()
{
    selection = GetSelectedItemFromList(m_GirlList_id);

    if (selection == -1) return Continue;                            // nothing selected, nothing to do.
    if ((selection - g_Game->dungeon().GetNumGirls()) >= 0) return Continue;    // if this is a customer, we're not interested
    reset_cycle_list();

    if(IsMultiSelected(m_GirlList_id)) {
        // if multiple girls are selected, put them into the selection list
        ForAllSelectedItems(m_GirlList_id, [&](int sel) {
            if(sel < g_Game->dungeon().GetNumGirls()) {
                add_to_cycle_list(g_Game->dungeon().GetGirl(sel)->m_Girl);
            }
        });
    } else {
        auto girl = g_Game->dungeon().GetGirl(selection)->m_Girl;
        // TODO can this happen?
        if(!girl) return Continue;
        else if(girl->is_dead()) {
            push_message("This is a dead girl. She has ceased to be.", COLOR_WARNING);
            return Return;
        }
        // if only a single girl is selected, allow iterating over all
        for(auto& g : g_Game->dungeon().girls()) {
            if (!g.m_Girl->is_dead()) {
                add_to_cycle_list(g.m_Girl);
            }
        }
        cycle_to(girl.get());
    }
    push_window("Girl Details");
    return Return;
}

int cScreenDungeon::enslave_customer(int girls_removed, int custs_removed)
{
    /*
    *    mod - docclox - nerfed the cash for selling a customer.
    *    a fat smelly brothel creeper probably shouldn't raise as much as
    *    a sexy young slavegirl. Feel free to un-nerf if you disagree.
    */
    long gold = (g_Dice % 200) + 63;
    g_Game->gold().slave_sales(gold);
    ss.str("");    ss << "You force the customer into slavery lawfully for committing a crime against your business and sell them for " << gold << " gold.";
    push_message(ss.str(), 0);
    g_Game->player().evil(1);
    int customer_index = selection - g_Game->dungeon().GetNumGirls();    // get the index of the about-to-be-sold customer
    customer_index += girls_removed;
    customer_index -= custs_removed;
    sDungeonCust* cust = g_Game->dungeon().GetCust(customer_index);        // get the customer record
    g_Game->dungeon().RemoveCust(cust);        // remove the customer from the dungeon room for an overload here
    return 0;
}
int cScreenDungeon::enslave()
{
    int numCustsRemoved = 0;
    int numGirlsRemoved = 0;
    int deadcount = 0;

    std::vector<std::string> submitted;

    // roll on vectors!
    ForAllSelectedItems(m_GirlList_id, [&](int selection) {
        if ((selection - (g_Game->dungeon().GetNumGirls() + numGirlsRemoved)) >= 0)    // it is a customer
        {
            enslave_customer(numGirlsRemoved, numCustsRemoved);
            numCustsRemoved++;
            return;
        }
        // it is a girl
        sGirl *girl = g_Game->dungeon().GetGirl(selection - numGirlsRemoved)->m_Girl.get();
        if (girl->is_slave()) return;                    // nothing to do if she's _already_ enslaved
        if (girl->is_dead()) { deadcount++; return; }    // likewise, dead girls can't be enslaved

        auto result = AttemptEscape(*girl);
        switch(result) {
        case EGirlEscapeAttemptResult::SUBMITS:
            cGirls::SetSlaveStats(*girl);
            submitted.push_back(girl->FullName());
            break;
        case EGirlEscapeAttemptResult::STOPPED_BY_GOONS:
            g_Game->player().evil(5);    // evil up the player for doing a naughty thing and adjust the girl's stats
            cGirls::SetSlaveStats(*girl);
            ss.str("");
            ss << girl->FullName() << " puts up a fight "
               << "but your goons control her as the enchanted slave tattoo is placed upon her.";
            push_message(ss.str(), COLOR_WARNING);    // and queue the message
            break;
        case EGirlEscapeAttemptResult::STOPPED_BY_PLAYER:
            // adjust the girl's stats to reflect her new status and then evil up the player because he forced her into slavery
            g_Game->player().evil(5);
            cGirls::SetSlaveStats(*girl);
            ss.str("");
            ss << girl->FullName()
               << " breaks free from your goons' control. You restrain her personally while the slave tattoo placed upon her.";
            push_message(ss.str(), COLOR_WARNING);
            break;
        case EGirlEscapeAttemptResult::SUCCESS:
            ss.str("");
            ss << "After defeating you goons and you, she escapes to the outside.\n";
            ss << "She will escape for good in 6 weeks if you don't send someone after her.";
            girl->run_away();
            numGirlsRemoved++;
            g_Game->player().suspicion(15);                    // suspicion goes up
            g_Game->player().evil(15);                        // so does evil
            push_message(ss.str(), COLOR_WARNING);    // add to the message queue
        }
    });
    if (deadcount > 0) push_message("There's not much point in using a slave tattoo on a dead body.", 0);

    if(!submitted.empty()) {
        ss.str("");
        ss << submitted.front();
        if (submitted.size() > 1) {
            for(int i = 1; i < submitted.size(); ++i) {
                ss << ", " << submitted[i];
            }
            ss << " submit the the enchanted slave tattoo being placed upon them.";
        } else {
            ss << " submits the the enchanted slave tattoo being placed upon her.";
        }
        push_message(ss.str(), 0);
    }

    init(false);
    return Return;
}

void cScreenDungeon::sell_slaves()
{
    int paid = 0, count = 0, deadcount = 0;
    std::vector<int> girl_array;
    get_selected_girls(&girl_array);  // get and sort array of girls/customers
    std::vector<std::string> girl_names;
    std::vector<int> sell_gold;
    for (int i = girl_array.size(); i-- > 0;)
    {
        selection = girl_array[i];
        if ((selection - g_Game->dungeon().GetNumGirls()) >= 0) continue;    // if this is a customer, we skip to the next list entry

        sGirl *girl = g_Game->dungeon().GetGirl(selection)->m_Girl.get();                                // and get the sGirl

        if (!girl->is_slave()) continue;                    // if she's not a slave, the player isn't allowed to sell her
        if (girl->is_dead())                                        // likewise, dead slaves can't be sold
        {
            deadcount++;
            continue;
        }
        // she's a living slave, she's out of here
        cGirls::UpdateAskPrice(*girl, false);
        int cost = g_Game->tariff().slave_sell_price(*girl);                    // get the sell price of the girl. This is a little on the occult side
        g_Game->gold().slave_sales(cost);
        paid += cost;
        count++;
        auto removed_girl = g_Game->dungeon().RemoveGirl(g_Game->dungeon().GetGirl(selection));    // remove her from the dungeon, add her back into the general pool
        girl_names.push_back(girl->FullName());
        sell_gold.push_back(cost);
        g_Game->girl_pool().GiveGirl(removed_girl);
    }
    if (deadcount > 0) push_message("Nobody is currently in the market for dead girls.", COLOR_ATTENTION);
    if (count <= 0) return;

    ss.str(""); ss << "You sold ";
    if (count == 1)        { ss << girl_names[0] << " for " << sell_gold[0] << " gold."; }
    else
    {
        ss << count << " slaves:";
        for (int i = 0; i < count; i++)
        {
            ss << "\n    " << girl_names[i] << "   for " << sell_gold[i] << " gold";
        }
        ss << "\nFor a total of " << paid << " gold.";
    }
    push_message(ss.str(), 0);
    selection = -1;
    init(false);
}

void cScreenDungeon::select_all_girls()
{
    for (int i = 0; i < g_Game->dungeon().GetNumGirls(); i++)
    {
        SetSelectedItemInList(m_GirlList_id, i, false, false);
    }
    selection_change();
}

void cScreenDungeon::select_all_customers()
{
    int offset = g_Game->dungeon().GetNumGirls();
    for (int i = 0; i < g_Game->dungeon().GetNumCusts(); i++)
    {
        SetSelectedItemInList(m_GirlList_id, i + offset, false, false);
    }
    selection_change();
}


void cScreenDungeon::stop_feeding()
{
    ForAllSelectedItems(m_GirlList_id, [&](int selection) {
        g_Game->dungeon().SetFeeding(selection, false);
        SetSelectedItemColumnText(m_GirlList_id, selection, "No", "Feeding");
    });
    selection_change();
}

void cScreenDungeon::start_feeding()
{
    ForAllSelectedItems(m_GirlList_id, [&](int selection) {
        g_Game->dungeon().SetFeeding(selection, true);
        SetSelectedItemColumnText(m_GirlList_id, selection, "Yes", "Feeding");
    });
    selection_change();
}

void cScreenDungeon::torture_customer(int girls_removed)
{
    ss.str("Customer: ");
    int cust_index = selection - g_Game->dungeon().GetNumGirls() + girls_removed;    // get the index number for the customer
    sDungeonCust* cust = g_Game->dungeon().GetCust(cust_index);                        // get the customer record from the dungeon

    if (!cust) return;
    if (cust->m_Tort && !g_Game->allow_cheats())         // don't let the PL torture more than once a day (unless cheating is enabled)
    {
        ss << "You may only torture someone once per week.";
        push_message(ss.str(), 0);
        return;
    }
    cust->m_Tort = true;        // flag the customer as tortured, decrement his health
    cust->m_Health -= 6;
    ss << "Screams fill the dungeon ";
    if (cust->m_Health > 0)
    {
        ss << "until the customer is battered, bleeding and bruised.\nYou leave them sobbing "
            << (cust->m_Health >= 30 ? "uncontrollably." : "and near to death.");
    }
    else
    {
        cust->m_Health = 0;
        ss<<" gradually growing softer until it stops completely.\nThey are dead.";
        g_Game->player().evil(2);
    }
    push_message(ss.str(), 0);
}

/*
* If we have a multiple selection, then the torture button
* should be enabled if just one of the selected rows can
* be tortured
*/
bool cScreenDungeon::torture_possible()
{
    int nNumGirls = g_Game->dungeon().GetNumGirls();
    bool can_torture = false;
    ForAllSelectedItems(m_GirlList_id, [&](int nSelection) {
        bool not_yet_tortured = false;
        // get the customer or girl under selection and find out if they've been tortured this turn
        if (nSelection >= nNumGirls)
        {
            sDungeonCust* dcust = g_Game->dungeon().GetCust(nSelection - nNumGirls);
            assert(dcust);
            if(dcust) not_yet_tortured = !dcust->m_Tort;
        }
        else
        {
            sDungeonGirl* dgirl = g_Game->dungeon().GetGirl(nSelection);
            assert(dgirl);
            if(dgirl) not_yet_tortured = !dgirl->m_Girl->m_Tort;
        }
        if (not_yet_tortured) can_torture = true;    // we only need one torturable prisoner so if we found one, we can go home
    });
    return false;                            // we only get here if no-one in the list was torturable
}

void cScreenDungeon::torture()
{
    int pos = 0;
    int numGirlsRemoved = 0;

    ForAllSelectedItems(m_GirlList_id, [&](int nSelection) {
        // if it's a customer, we have a separate routine
        if ((selection - (g_Game->dungeon().GetNumGirls() + numGirlsRemoved)) >= 0)
        {
            g_Game->player().evil(5);
            torture_customer(numGirlsRemoved);
            return;
        }
        // If we get here, it's a girl
        sDungeonGirl* dgirl = g_Game->dungeon().GetGirl(selection - numGirlsRemoved);
        cGirlTorture gt(dgirl);
    });
}

void cScreenDungeon::change_release(BuildingType building, int index)
{
    m_ReleaseBuilding = g_Game->buildings().building_with_type(building, index);
    init(true);
}

void cScreenDungeon::release()
{
    std::vector<int> girl_array;
    get_selected_girls(&girl_array);            // get and sort array of girls/customers
    for (int i = girl_array.size(); i--> 0;)
    {
        selection = girl_array[i];
        // check in case its a customer
        if ((selection - g_Game->dungeon().GetNumGirls()) >= 0)
        {
            int num = selection - g_Game->dungeon().GetNumGirls();
            sDungeonCust* cust = g_Game->dungeon().GetCust(num);
            g_Game->dungeon().RemoveCust(cust);
            // player did a nice thing: suss and evil go down :)
            g_Game->player().suspicion(-5);
            g_Game->player().evil(-5);
            continue;
        }

        // if there's room, remove her from the dungeon and add her to the current brothel
        int num = selection;
        if ((m_ReleaseBuilding->free_rooms()) > 0)
        {
            g_Game->dungeon().ReleaseGirl(num, *m_ReleaseBuilding);
            continue;
        }
        // if we run out of space
        push_message("The current building has no more room.\nBuy a new one, get rid of some girls, or change the building you are currently releasing girls to.", 0);
        break;        // no point in running round the loop any further we're out of space
    }
}

void cScreenDungeon::talk()
{
    if (g_Game->GetTalkCount() <= 0) return;    // if we have no talks left, we can go home
    // customers are always last in the list, so we can determine if this is a customer by simple aritmetic
    if ((selection - g_Game->dungeon().GetNumGirls()) >= 0) return;        // it is a customer

    // OK, it wasn't a customer
    int num = selection;
    sDungeonGirl* girl = g_Game->dungeon().GetGirl(num);

    g_Game->TalkToGirl(*girl->m_Girl);

}

void cScreenDungeon::update_image()
{
    if ((selection - g_Game->dungeon().GetNumGirls()) >= 0)    // Makes it so when on a customer it doesnt keep the last girls pic up
    {
        HideWidget(m_MainImageId, true);
    }
    else if (selected_girl() && !IsMultiSelected(m_GirlList_id))
    {
        UpdateImage(selected_girl()->m_Tort ? EImageBaseType::TORTURE : EImageBaseType::JAIL);
    }
    else
    {
        HideWidget(m_MainImageId, true);
    }
}

void cScreenDungeon::get_selected_girls(std::vector<int> *girl_array)
{  // take passed vector and fill it with sorted list of girl/customer IDs
    ForAllSelectedItems(m_GirlList_id, [&](int sel) {
        girl_array->push_back(sel);
    });
    sort(girl_array->begin(), girl_array->end());
}

sGirl* cScreenDungeon::get_image_girl() {
    return selected_girl().get();
}

std::shared_ptr<cInterfaceWindow> screens::cDungeonScreenBase::create() {
    return std::make_shared<cScreenDungeon>();
}

