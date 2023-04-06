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

#include "cScreenGangs.h"
#include "interface/cWindowManager.h"
#include "cGold.h"
#include "cTariff.h"
#include "InterfaceProcesses.h"
#include "cGangs.h"
#include "cGangManager.hpp"
#include "IGame.h"
#include "CLog.h"
#include <vector>

static std::stringstream ss;

cScreenGangs::cScreenGangs() = default;

void cScreenGangs::setup_callbacks()
{
    //Set the default sort order for columns, so listboxes know the order in which data will be sent
    std::vector<std::string> RecruitColumns{ "GangName", "Number", "Combat", "Magic", "Intelligence", "Agility", "Constitution", "Charisma", "Strength", "Service" };
    SortColumns(m_RecruitList_id, RecruitColumns);
    
    std::vector<std::string> GangColumns{ "GangName", "Number", "Mission", "Combat", "Magic", "Intelligence", "Agility", "Constitution", "Charisma", "Strength", "Service" };
    SortColumns(m_GangList_id, GangColumns);
    
    // set button callbacks
    SetButtonCallback(m_GangFireBtn_id, [this](){
        int selection = GetLastSelectedItemFromList(m_GangList_id);
        if (selection != -1)
        {
            g_Game->gang_manager().FireGang(selection);
            init(false);
        };
    });

    SetButtonCallback(m_GangHireBtn_id, [this]() {
        hire_recruitable();
    });

    SetButtonCallback(m_WeaponUpBtn_id, [this]() {
        ForAllSelectedItems(m_GangList_id, [&](int selection) {
            sGang* gang = g_Game->gang_manager().GetGang(selection);
            if(gang) {
                int wlev = gang->weapon_level();
                int cost = g_Game->tariff().goon_weapon_upgrade(wlev);
                if (g_Game->gold().item_cost(cost) && wlev < 3)
                {
                    gang->set_weapon_level(wlev + 1);                }
            }
        });
        init(false);
    });

    SetButtonCallback(m_BuyNetsBtn_id,    [this]() { buy_nets(1);});
    SetButtonCallback(m_BuyNetsBtn10_id,  [this]() { buy_nets(10);});
    SetButtonCallback(m_BuyNetsBtn20_id,  [this]() { buy_nets(20);});
    SetButtonCallback(m_BuyHealPotBtn_id,   [this]() { buy_potions(1);});
    SetButtonCallback(m_BuyHealPotBtn10_id, [this]() { buy_potions(10);});
    SetButtonCallback(m_BuyHealPotBtn20_id, [this]() { buy_potions(20);});

    SetListBoxSelectionCallback(m_MissionList_id, [this](int sel) { on_select_mission(sel); });
    SetListBoxHotKeys(m_MissionList_id, SDLK_w, SDLK_s);
    SetListBoxSelectionCallback(m_GangList_id, [this](int sel) { on_select_gang(sel); });
    SetListBoxHotKeys(m_GangList_id, SDLK_a, SDLK_d);
    SetListBoxSelectionCallback(m_RecruitList_id, [this](int sel) {
        update_recruit_btn();
    });
    SetListBoxDoubleClickCallback(m_RecruitList_id, [this](int sel) {hire_recruitable(); });
    SetListBoxHotKeys(m_RecruitList_id, SDLK_q, SDLK_e);

    AddKeyCallback(SDLK_SPACE, [this](){ hire_recruitable(); });

    SetCheckBoxCallback(m_AutoBuyNetsToggle_id, [this](bool on) {
        int nets = g_Game->gang_manager().GetNets();
        g_Game->gang_manager().KeepNetStocked(on ? nets : 0);
    });

    SetCheckBoxCallback(m_AutoBuyHealToggle_id, [this](bool on) {
        int potions = g_Game->gang_manager().GetHealingPotions();
        g_Game->gang_manager().KeepHealStocked(on ? potions : 0);
    });

    SetSliderCallback(m_GirlsPercSlider_id, [this](int value) {
        int s1 = value;
        int s2 = SliderValue(m_ItemsPercSlider_id);
        if (s2 < s1)
        {
            s2 = s1;
            SliderRange(m_ItemsPercSlider_id, 0, 100, s2, 1);
        }
        update_sliders();
    });

    SetSliderCallback(m_ItemsPercSlider_id, [this](int value) {
        int s1 = value;
        int s2 = SliderValue(m_GirlsPercSlider_id);
        if (s1 < s2)
        {
            s2 = s1;
            SliderRange(m_GirlsPercSlider_id, 0, 100, s2, 1);
        }
        update_sliders();
    });
}

void cScreenGangs::init(bool back)
{
    Focused();

    int selection = GetLastSelectedItemFromList(m_GangList_id);

    ClearListBox(m_MissionList_id);
    AddToListBox(m_MissionList_id, 0, "GUARDING");
    AddToListBox(m_MissionList_id, 1, "SABOTAGE");
    AddToListBox(m_MissionList_id, 2, "SPY ON GIRLS");
    AddToListBox(m_MissionList_id, 3, "RECAPTURE");
    AddToListBox(m_MissionList_id, 4, "ACQUIRE TERRITORY");
    AddToListBox(m_MissionList_id, 5, "PETTY THEFT");
    AddToListBox(m_MissionList_id, 6, "GRAND THEFT");
    AddToListBox(m_MissionList_id, 7, "KIDNAPPING");
    AddToListBox(m_MissionList_id, 8, "CATACOMBS");
    AddToListBox(m_MissionList_id, 9, "TRAINING");
    AddToListBox(m_MissionList_id, 10, "RECRUITING");
    AddToListBox(m_MissionList_id, 11, "SERVICE");

    SliderRange(m_GirlsPercSlider_id, 0, 100, g_Game->gang_manager().Gang_Gets_Girls(), 1);
    SliderRange(m_ItemsPercSlider_id, 0, 100, g_Game->gang_manager().Gang_Gets_Girls() + g_Game->gang_manager().Gang_Gets_Items(), 1);
    ss.str("");    ss << "Girls : " << g_Game->gang_manager().Gang_Gets_Girls() << "%";    EditTextItem(ss.str(), m_GangGetsGirls_id);
    ss.str("");    ss << "Items : " << g_Game->gang_manager().Gang_Gets_Items() << "%";    EditTextItem(ss.str(), m_GangGetsItems_id);
    ss.str("");    ss << "Beasts : " << g_Game->gang_manager().Gang_Gets_Beast() << "%";    EditTextItem(ss.str(), m_GangGetsBeast_id);

    SetCheckBox(m_AutoBuyNetsToggle_id, (g_Game->gang_manager().GetNetRestock() > 0));
    SetCheckBox(m_AutoBuyHealToggle_id, (g_Game->gang_manager().GetHealingRestock() > 0));

    // weapon upgrades
    update_wpn_info();

    int nets = g_Game->gang_manager().GetNets();
    ss.str(""); ss << "Nets (" << g_Game->tariff().nets_price(1) << "g each): " << nets;
    EditTextItem(ss.str(), m_NetDescription_id);
    DisableWidget(m_BuyNetsBtn_id, nets >= 60);
    DisableWidget(m_BuyNetsBtn10_id, nets >= 60);
    DisableWidget(m_BuyNetsBtn20_id, nets >= 60);
    DisableWidget(m_AutoBuyNetsToggle_id, nets < 1);

    int potions = g_Game->gang_manager().GetHealingPotions();
    ss.str(""); ss << "Heal Potions (" << g_Game->tariff().healing_price(1) << "g each): " << potions;
    EditTextItem(ss.str(), m_HealPotDescription_id);
    DisableWidget(m_BuyHealPotBtn_id, potions >= 200);
    DisableWidget(m_BuyHealPotBtn10_id, potions >= 200);
    DisableWidget(m_BuyHealPotBtn20_id, potions >= 200);
    DisableWidget(m_AutoBuyHealToggle_id, potions < 1);

    int cost = 0;
    if (g_Game->gang_manager().GetNumGangs() > 0)
    {
        for(auto& gang : g_Game->gang_manager().GetPlayerGangs()) {
            cost += g_Game->tariff().goon_mission_cost(gang->m_MissionID);
        }
    }
    ss.str(""); ss << "Weekly Cost: " << cost;
    EditTextItem(ss.str(), m_TotalCost_id);
    if (m_Gold_id >= 0)
    {
        ss.str(""); ss << "Gold: " << g_Game->gold().ival();
        EditTextItem(ss.str(), m_Gold_id);
    }

    ClearListBox(m_GangList_id);
    int num = 0;

    // loop through the gangs, populating the list box
    g_LogFile.log(ELogLevel::DEBUG, "Setting gang mission descriptions\n");
    for(auto& gang : g_Game->gang_manager().GetPlayerGangs())
    {
        // format the string with the gang name, mission and number of men
        std::vector<FormattedCellData> Data(11);

        Data[0]  = mk_text(gang->name());
        Data[1]  = mk_num(gang->m_Num);
        Data[2]  = mk_text(short_mission_desc(gang->m_MissionID));

        Data[3]  = mk_percent(gang->m_Skills[SKILL_COMBAT]);
        Data[4]  = mk_percent(gang->m_Skills[SKILL_MAGIC]);
        Data[5]  = mk_percent(gang->m_Stats[STAT_INTELLIGENCE]);
        Data[6]  = mk_percent(gang->m_Stats[STAT_AGILITY]);
        Data[7]  = mk_percent(gang->m_Stats[STAT_CONSTITUTION]);
        Data[8]  = mk_percent(gang->m_Stats[STAT_CHARISMA]);
        Data[9]  = mk_percent(gang->m_Stats[STAT_STRENGTH]);
        Data[10] = mk_percent(gang->m_Skills[SKILL_SERVICE]);

        //        cerr << "Gang:\t" << Data[0] << "\t" << Data[1] << "\t" << Data[2]
        //            << "\t" << Data[3] << "\t" << Data[4] << "\t" << Data[5] << "\t" << Data[6] << endl;

        /*
        *            add the box to the list; red highlight gangs that are low on numbers
        */
        int color = (gang->m_Num < 6 ? COLOR_WARNING : COLOR_NEUTRAL);
        if (gang->m_Num < 6 && (gang->m_MissionID == MISS_SERVICE || gang->m_MissionID == MISS_TRAINING)) color = COLOR_ATTENTION;
        AddToListBox(m_GangList_id, num++, std::move(Data), color);
    }

    ClearListBox(m_RecruitList_id);
    num = 0;

    // loop through the gangs, populating the list box
    g_LogFile.log(ELogLevel::DEBUG, "Setting recruitable gang info\n");
    for (auto& current : g_Game->gang_manager().GetHireableGangs())
    {
        // format the string with the gang name, mission and number of men
        std::vector<FormattedCellData> Data(10);
        Data[0] = mk_text(current->name());
        Data[1] = mk_num(current->m_Num);
        Data[2] = mk_percent(current->combat());
        Data[3] = mk_percent(current->magic());
        Data[4] = mk_percent(current->intelligence());
        Data[5] = mk_percent(current->agility());
        Data[6] = mk_percent(current->constitution());
        Data[7] = mk_percent(current->charisma());
        Data[8] = mk_percent(current->strength());
        Data[9] = mk_percent(current->service());

        //           add the box to the list
        int color = current->m_Num < 6 ? COLOR_WARNING : COLOR_NEUTRAL;
        AddToListBox(m_RecruitList_id, num++, std::move(Data), color);
    }

    if (selection == -1 && GetListBoxSize(m_GangList_id) >= 1) selection = 0;

    if (selection >= 0)
    {
        while (selection > GetListBoxSize(m_GangList_id) && selection != -1) selection--;
    }
    if (selection >= 0) SetSelectedItemInList(m_GangList_id, selection);

    update_recruit_btn();
    DisableWidget(m_GangFireBtn_id, (g_Game->gang_manager().GetNumGangs() <= 0) || (selection == -1));
}

void cScreenGangs::update_sliders()
{
    int s1 = SliderValue(m_GirlsPercSlider_id);
    int s2 = SliderValue(m_ItemsPercSlider_id);
    g_Game->gang_manager().Gang_Gets_Girls(s1);
    g_Game->gang_manager().Gang_Gets_Items(s2 - s1);
    g_Game->gang_manager().Gang_Gets_Beast(100 - s2);
    ss.str("");
    ss << "Girls : " << g_Game->gang_manager().Gang_Gets_Girls() << "%";
    EditTextItem(ss.str(), m_GangGetsGirls_id);
    ss.str("");
    ss << "Items : " << g_Game->gang_manager().Gang_Gets_Items() << "%";
    EditTextItem(ss.str(), m_GangGetsItems_id);
    ss.str("");
    ss << "Beasts : " << g_Game->gang_manager().Gang_Gets_Beast() << "%";
    EditTextItem(ss.str(), m_GangGetsBeast_id);
}

void cScreenGangs::on_select_gang(int selection)
{
    if (selection != -1)
    {
        sGang* gang = g_Game->gang_manager().GetGang(selection);
        SetSelectedItemInList(m_MissionList_id, gang->m_MissionID, false);
        set_mission_desc(gang->m_MissionID);        // set the long description for the mission
    }

    update_wpn_info();
}

void cScreenGangs::on_select_mission(int mission_id)
{
    if(mission_id != -1) {
        set_mission_desc(mission_id);        // set the textfield with the long description and price for this mission
    }

    ForAllSelectedItems(m_GangList_id, [&](int selection) {
        sGang* gang = g_Game->gang_manager().GetGang(selection);
        /*
        *                make sure we found the gang - pretty catastrophic
        *                if not, so log it if we do
        */
        if (gang == nullptr)
        {
            g_LogFile.log(ELogLevel::ERROR, "No gang for index ", selection);
            return;
        }
        /*
        *                if the mission id is -1, nothing else to do
        *                (moving this to before the recruitment check
        *                since -1 most likely means nothing selected in
        *                the missions list)
        */
        if (mission_id == -1) { return; }
        /*
        *                if the gang is already doing <whatever>
        *                then let them get on with it
        */
        if (gang->m_MissionID == mission_id) { return; }
        /*
        *                if they were recruiting, turn off the
        *                auto-recruit flag
        */
        if (gang->m_MissionID == MISS_RECRUIT && gang->m_AutoRecruit)
        {
            gang->m_AutoRecruit = false;
            gang->m_LastMissID = -1;
        }
        gang->m_MissionID = mission_id;
    });
    init(false);

    int cost = 0;
    if (g_Game->gang_manager().GetNumGangs() > 0)
    {
        for (int i = 0; i < g_Game->gang_manager().GetNumGangs(); i++)
        {
            sGang* g = g_Game->gang_manager().GetGang(i);
            cost += g_Game->tariff().goon_mission_cost(g->m_MissionID);
        }
    }
    ss.str("");
    ss << "Weekly Cost: " << cost;
    EditTextItem(ss.str(), m_TotalCost_id);
    if (m_Gold_id >= 0)
    {
        ss.str(""); ss << "Gold: " << g_Game->gold().ival();
        EditTextItem(ss.str(), m_Gold_id);
    }
}

std::string cScreenGangs::mission_desc(int mid)
{
    switch (mid) {
    case MISS_GUARDING:        return "Your men will guard your property.";
    case MISS_SABOTAGE:        return "Your men will move about town and destroy and loot enemy businesses.";
    case MISS_SPYGIRLS:        return "Your men will spy on your working girls, looking for the ones who take extra for themselves.";
    case MISS_CAPTUREGIRL:    return "Your men will set out to re-capture any girls who have recently run away.";
    case MISS_EXTORTION:    return "Sends your men out to force local gangs out of their areas of town.";
    case MISS_PETYTHEFT:    return "Your men will mug people in the street.";
    case MISS_GRANDTHEFT:    return "Your men will attempt to rob a bank or other risky place with high rewards.";
    case MISS_KIDNAPP:        return "Your men will kidnap beggar, homeless or lost girls from the street and also lure other girls into working for you.";
    case MISS_CATACOMBS:    return "Your men will explore the catacombs for treasure.";
    case MISS_RECRUIT:        return "Your men will replace their missing men (up to " + std::to_string(sGang::max_members()) + ").";
    case MISS_TRAINING:        return "Your men will improve their skills slightly (1-" + std::to_string(sGang::max_members()) + " members ok).";
    case MISS_SERVICE:        return "Your men will help out in the community (1-" + std::to_string(sGang::max_members()) + " members ok).";
    default:
        break;
    }
    ss.str(""); ss << "Error: unexpected mission ID: " << mid;
    return ss.str();
}

std::string cScreenGangs::short_mission_desc(int mid)
{
    switch (mid)
    {
    case MISS_GUARDING:        return "Guarding";
    case MISS_SABOTAGE:        return "Sabotaging";
    case MISS_SPYGIRLS:        return "Watching Girls";
    case MISS_CAPTUREGIRL:    return "Finding escaped girls";
    case MISS_EXTORTION:    return "Acquiring Territory";
    case MISS_PETYTHEFT:    return "Mugging people";
    case MISS_GRANDTHEFT:    return "Robbing places";
    case MISS_KIDNAPP:        return "Kidnapping Girls";
    case MISS_CATACOMBS:    return "Exploring Catacombs";
    case MISS_TRAINING:        return "Training Skills";
    case MISS_RECRUIT:        return "Recruiting Men";
    case MISS_SERVICE:        return "Serving the Community";
    default:                return "Error: Unknown";
    }
}

int cScreenGangs::set_mission_desc(int mid)
{
    int price = g_Game->tariff().goon_mission_cost(mid);            // OK: get the difficulty-adjusted price for this mission
    std::string desc = mission_desc(mid);                    // and get a description of the mission
    ss.str(""); ss << desc << " (" << price << "g)";                // stick 'em both together ...
    EditTextItem(ss.str(), m_MissionDescription_id);                // ... and set the text field
    return price;                                        // return the mission price
}

void cScreenGangs::hire_recruitable()
{
    int max_hire = g_Game->gang_manager().GetNumHireableGangs();
    std::vector<int> translate(max_hire);
    for(int i = 0; i < max_hire; ++i) {
        translate[i] = i;
    }

    ForAllSelectedItems(m_RecruitList_id, [&](int sel_recruit) {
        if ((g_Game->gang_manager().GetNumGangs() >= g_Game->gang_manager().GetMaxNumGangs()) || (sel_recruit == -1)) return;
        int translated = translate.at(sel_recruit);
        g_Game->gang_manager().HireGang(translated);
        for(int i = sel_recruit; i < max_hire; ++i) {
            --translate[i];
        }
    });

    init(false);
}

void cScreenGangs::buy_potions(int buypots)
{
    g_Game->gang_manager().BuyHealingPotions(buypots, IsCheckboxOn(m_AutoBuyHealToggle_id));
    init(false);
}

void cScreenGangs::buy_nets(int buynets)
{
    g_Game->gang_manager().BuyNets(buynets, IsCheckboxOn(m_AutoBuyNetsToggle_id));
    init(false);
}

void cScreenGangs::update_wpn_info() {
    ss.str("");    ss << "Weapon Level: ";
    int wpn_cost = 0;
    ForAllSelectedItems(m_GangList_id, [&](int selection) {
        sGang* gang = g_Game->gang_manager().GetGang(selection);
        if(gang) {
            ss << gang->weapon_level() << " ";
            if ( gang->weapon_level() < 3)
            {
                wpn_cost += g_Game->tariff().goon_weapon_upgrade(gang->weapon_level());
            }
        }
    });

    if(wpn_cost == 0) {
        DisableWidget(m_WeaponUpBtn_id);
    }
    else {
        EnableWidget(m_WeaponUpBtn_id);
        ss << " Next: " << wpn_cost << "g";
    }
    EditTextItem(ss.str(), m_WeaponDescription_id);
}

void cScreenGangs::update_recruit_btn() {
    int sel_recruit = GetLastSelectedItemFromList(m_RecruitList_id);
    DisableWidget(m_GangHireBtn_id, (g_Game->gang_manager().GetNumHireableGangs() <= 0) ||
                               (g_Game->gang_manager().GetNumGangs() >= g_Game->gang_manager().GetMaxNumGangs()) ||
                               (sel_recruit == -1));
}

std::shared_ptr<cInterfaceWindow> screens::cGangsScreenBase::create() {
    return std::make_shared<cScreenGangs>();
}

