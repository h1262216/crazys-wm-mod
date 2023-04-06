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
#include "cScreenBuildingSetup.h"
#include "interface/cWindowManager.h"
#include "cGold.h"
#include "cTariff.h"
#include "IGame.h"
#include "CLog.h"

static std::stringstream ss;

cScreenBuildingSetup::cScreenBuildingSetup() = default;

void cScreenBuildingSetup::setup_callbacks()
{
    SetButtonCallback(m_BuildRoomsBtn_id, [this](){ buy_rooms(); });
    SetButtonCallback(m_10PotionsBtn_id, [this](){ buy_potions(10); });
    SetButtonCallback(m_20PotionsBtn_id, [this](){ buy_potions(20); });

    m_SexTypeAllowedMap = { {m_ProhibitAnalToggle_id, SKILL_ANAL}, {m_ProhibitBdsmtoggle_id, SKILL_BDSM},
                            {m_ProhibitBeastToggle_id, SKILL_BEASTIALITY}, {m_ProhibitFootJobToggle_id, SKILL_FOOTJOB},
                            {m_ProhibitGroupToggle_id, SKILL_GROUP}, {m_ProhibitHandJobToggle_id, SKILL_HANDJOB},
                            {m_ProhibitLesbianToggle_id, SKILL_LESBIAN}, {m_ProhibitNormalToggle_id, SKILL_NORMALSEX},
                            {m_ProhibitOralToggle_id, SKILL_ORALSEX}, {m_ProhibitStripToggle_id, SKILL_STRIP},
                            {m_ProhibitTittyToggle_id, SKILL_TITTYSEX}};

    for(const auto& data : m_SexTypeAllowedMap) {
        SetCheckBoxCallback(data.id, [this, skill=data.skill](bool on){ set_sex_type_allowed(skill, on); });
    }

    SetCheckBoxCallback(m_AutoBuyPotionToggle_id, [this](bool on) {
        active_building().m_KeepPotionsStocked = on;
    });

    SetSliderCallback(m_AdvertisingSlider_id, [this](int value) {
        active_building().m_AdvertisingBudget = value * 50;
        ss.str(""); ss << "Advertising Budget: " << g_Game->tariff().advertising_costs(value * 50) << " gold / week";
        EditTextItem(ss.str(), m_AdvertisingValue_id);
    });
}

void cScreenBuildingSetup::init(bool back)
{;
    Focused();

    int rooms = 20, maxrooms = 200, antipregnum = 0, antipregused = 0, advert = 0;
    std::string brothel;
    cBuilding& building = active_building();
    brothel = building.name();

    rooms = building.m_NumRooms;
    maxrooms = building.m_MaxNumRooms;
    antipregnum = building.GetNumPotions();
    antipregused = building.m_AntiPregUsed;
    advert = building.m_AdvertisingBudget / 50;

    // setup check boxes
    SetCheckBox(m_AutoBuyPotionToggle_id, building.GetPotionRestock());
    for(const auto& data : m_SexTypeAllowedMap) {
        SetCheckBox(data.id, !building.is_sex_type_allowed(data.skill));
    }

    if (m_Gold_id >= 0)
    {
        ss.str(""); ss << "Gold: " << g_Game->gold().ival();
        EditTextItem(ss.str(), m_Gold_id);
    }

    EditTextItem(brothel, m_CurrentBrothel_id);
    ss.str("");    ss << "Anti-Preg Potions: " << g_Game->tariff().anti_preg_price(1) << " gold each.";
    EditTextItem(ss.str(), m_PotionCost_id);

    // let's limit advertising budget to multiples of 50 gold (~3 added customers), from 0 - 2000
    advert = SliderRange(m_AdvertisingSlider_id, 0, (2000 / 50), advert, 4);  // set slider min/max range
    ss.str("");    ss << "Advertising Budget: " << (advert * 50) << " gold / week";
    EditTextItem(ss.str(), m_AdvertisingValue_id);

    if (antipregused < 0) antipregused = 0;
    ss.str("");    ss << "         You have: " << antipregnum << "\nUsed Last Turn: " << antipregused;
    EditTextItem(ss.str(), m_AvailablePotions_id);
    DisableWidget(m_AutoBuyPotionToggle_id, antipregnum < 1);

    ss.str("");    ss << "Add Rooms: " << g_Game->tariff().add_room_cost(5) << " gold\nCurrent: " << rooms << "\nMaximum: " << maxrooms << std::endl;
    EditTextItem(ss.str(), m_RoomAddCost_id);
    DisableWidget(m_BuildRoomsBtn_id, rooms >= maxrooms);
}

void cScreenBuildingSetup::buy_potions(int buypotions)
{
    int buynum = buypotions;
    int buysum = buynum;
    int antipregnum = 0;
    if (!g_Game->gold().afford(g_Game->tariff().anti_preg_price(buynum)))    push_message("You don't have enough gold", COLOR_WARNING);
    else
    {
        int MaxSupplies = g_Game->MaxSupplies();

        auto& building = active_building();
        antipregnum = building.m_AntiPregPotions;
        if (antipregnum + buynum > MaxSupplies) buysum = std::max(0, MaxSupplies - antipregnum);
        building.m_AntiPregPotions += buysum;

        if (buysum < buynum)
        {
            ss.str(""); ss << "You can only store up to " << MaxSupplies << " potions.";
            if (buysum > 0)
            {
                ss << "\nYou buy " << buysum << " to fill the stock.";
                g_Game->gold().item_cost(g_Game->tariff().anti_preg_price(buysum));
            }
            push_message(ss.str(), 0);
        }
        else g_Game->gold().item_cost(g_Game->tariff().anti_preg_price(buynum));
    }
    init(false);
}

void cScreenBuildingSetup::buy_rooms()
{
    if (!g_Game->gold().brothel_cost(g_Game->tariff().add_room_cost(5)))
    {
        ss.str("");
        ss << "You Need " << g_Game->tariff().add_room_cost(5) << " gold to add 5 rooms.";
        push_message(ss.str(), COLOR_WARNING);
    }
    else
    {
        auto& target = active_building();
        target.m_NumRooms += 5;
        int rooms = target.m_NumRooms;
        int maxrooms = target.m_MaxNumRooms;

        ss.str(""); ss << "Add Rooms: " << g_Game->tariff().add_room_cost(5) << " gold\nCurrent: " << rooms << "\nMaximum: " << maxrooms << std::endl;
        EditTextItem(ss.str(), m_RoomAddCost_id);
        DisableWidget(m_BuildRoomsBtn_id, rooms >= maxrooms);
        init(false);
    }
}

void cScreenBuildingSetup::set_sex_type_allowed(SKILLS sex_type, bool is_forbidden)
{
    active_building().set_sex_type_allowed(sex_type, !is_forbidden);
}

std::shared_ptr<cInterfaceWindow> screens::cBuildingSetupScreenBase::create() {
    return std::make_shared<cScreenBuildingSetup>();
}
