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
#include "buildings/cBrothel.h"
#include "cScreenBuildingSetup.h"
#include "interface/cWindowManager.h"
#include "cGold.h"
#include "cTariff.h"
#include "Game.hpp"
#include "CLog.h"

static stringstream ss;

cScreenBuildingSetup::cScreenBuildingSetup() : cInterfaceWindowXML("building_setup_screen.xml")
{
}

void cScreenBuildingSetup::set_ids()
{
	g_LogFile.write("set_ids in cScreenBuildingSetup");

	back_id			/**/ = get_id("BackButton", "Back");
	curbrothel_id	/**/ = get_id("CurrentBrothel");
	gold_id			/**/ = get_id("Gold");
	potioncost_id	/**/ = get_id("PotionCost");
	potionavail_id	/**/ = get_id("AvailablePotions");
	potions10_id	/**/ = get_id("10PotionsButton");
	potions20_id	/**/ = get_id("20PotionsButton");
	autopotions_id	/**/ = get_id("AutoBuyPotionToggle");
	barstaff_id		/**/ = get_id("BarStaffCost", "*Unused*");//
	barhire_id		/**/ = get_id("BarHireButton", "*Unused*");//
	barfire_id		/**/ = get_id("BarFireButton", "*Unused*");//
	casinostaff_id	/**/ = get_id("CasinoStaffCost", "*Unused*");//
	casinohire_id	/**/ = get_id("CasinoHireButton", "*Unused*");//
	casinofire_id	/**/ = get_id("CasinoFireButton", "*Unused*");//
	roomcost_id		/**/ = get_id("RoomAddCost");
	buyrooms_id		/**/ = get_id("BuildRoomsButton");
	restrict_id		/**/ = get_id("SexRestrictions", "*Unused*");//

	noanal_id		/**/ = get_id("ProhibitAnalToggleRes");
	nobdsm_id		/**/ = get_id("ProhibitBDSMToggleRes");
	nobeast_id		/**/ = get_id("ProhibitBeastToggleRes");
	nofoot_id		/**/ = get_id("ProhibitFootJobToggleRes");
	nogroup_id		/**/ = get_id("ProhibitGroupToggleRes");
	nohand_id		/**/ = get_id("ProhibitHandJobToggleRes");
	nolesbian_id	/**/ = get_id("ProhibitLesbianToggleRes");
	nonormal_id		/**/ = get_id("ProhibitNormalToggleRes");
	nooral_id		/**/ = get_id("ProhibitOralToggleRes");
	nostrip_id		/**/ = get_id("ProhibitStripToggleRes");
	notitty_id		/**/ = get_id("ProhibitTittyToggleRes");

	advertsli_id	/**/ = get_id("AdvertisingSlider");
	advertamt_id	/**/ = get_id("AdvertisingValue");

	SetButtonNavigation(back_id, "<back>");
	SetButtonCallback(buyrooms_id, [this](){ buy_rooms(); });
	SetButtonCallback(potions10_id, [this](){ buy_potions(10); });
	SetButtonCallback(potions20_id, [this](){ buy_potions(20); });

	struct STCB {
	    int id;
        SKILLS skill;
	};

	STCB stcb[] = { {noanal_id, SKILL_ANAL}, {nobdsm_id, SKILL_BDSM}, {nobeast_id, SKILL_BEASTIALITY},
                    {nofoot_id, SKILL_FOOTJOB}, {nogroup_id, SKILL_GROUP}, {nohand_id, SKILL_HANDJOB},
                    {nolesbian_id, SKILL_LESBIAN}, {nonormal_id, SKILL_NORMALSEX}, {nooral_id, SKILL_ORALSEX},
                    {nostrip_id, SKILL_STRIP}, {notitty_id, SKILL_TITTYSEX}};

	for(const auto& data : stcb) {
	    SetCheckBoxCallback(data.id, [this, skill=data.skill](bool on){ set_sex_type_allowed(skill, on); });
	}

    SetCheckBoxCallback(autopotions_id, [this](bool on) {
        active_building().m_KeepPotionsStocked = on;
    });

	SetSliderCallback(advertsli_id, [this](int value) {
        active_building().m_AdvertisingBudget = value * 50;
        ss.str(""); ss << "Advertising Budget: " << g_Game->tariff().advertising_costs(value * 50) << " gold / week";
        EditTextItem(ss.str(), advertamt_id);;
	});
}

void cScreenBuildingSetup::init(bool back)
{;
	Focused();

	int rooms = 20, maxrooms = 200, antipregnum = 0, antipregused = 0, advert = 0;
	string brothel;
	IBuilding& building = active_building();
    brothel = building.name();

    rooms = building.m_NumRooms;
    maxrooms = building.m_MaxNumRooms;
    antipregnum = building.GetNumPotions();
    antipregused = building.m_AntiPregUsed;
    advert = building.m_AdvertisingBudget / 50;

    // setup check boxes
    SetCheckBox(autopotions_id, building.GetPotionRestock());
    SetCheckBox(noanal_id, !building.is_sex_type_allowed(SKILL_ANAL));
    SetCheckBox(nobdsm_id, !building.is_sex_type_allowed(SKILL_BDSM));
    SetCheckBox(nobeast_id, !building.is_sex_type_allowed(SKILL_BEASTIALITY));
    SetCheckBox(nofoot_id, !building.is_sex_type_allowed(SKILL_FOOTJOB));
    SetCheckBox(nogroup_id, !building.is_sex_type_allowed(SKILL_GROUP));
    SetCheckBox(nohand_id, !building.is_sex_type_allowed(SKILL_HANDJOB));
    SetCheckBox(nolesbian_id, !building.is_sex_type_allowed(SKILL_LESBIAN));
    SetCheckBox(nonormal_id, !building.is_sex_type_allowed(SKILL_NORMALSEX));
    SetCheckBox(nooral_id, !building.is_sex_type_allowed(SKILL_ORALSEX));
    SetCheckBox(nostrip_id, !building.is_sex_type_allowed(SKILL_STRIP));
    SetCheckBox(notitty_id, !building.is_sex_type_allowed(SKILL_TITTYSEX));


	if (gold_id >= 0)
	{
		ss.str(""); ss << "Gold: " << g_Game->gold().ival();
		EditTextItem(ss.str(), gold_id);
	}

	EditTextItem(brothel, curbrothel_id);
	ss.str("");	ss << "Anti-Preg Potions: " << g_Game->tariff().anti_preg_price(1) << " gold each.";
	EditTextItem(ss.str(), potioncost_id);

	// let's limit advertising budget to multiples of 50 gold (~3 added customers), from 0 - 2000
	advert = SliderRange(advertsli_id, 0, (2000 / 50), advert, 4);  // set slider min/max range
	ss.str("");	ss << "Advertising Budget: " << (advert * 50) << " gold / week";
	EditTextItem(ss.str(), advertamt_id);

	if (antipregused < 0) antipregused = 0;
	ss.str("");	ss << "         You have: " << antipregnum << "\nUsed Last Turn: " << antipregused;
	EditTextItem(ss.str(), potionavail_id);
    DisableWidget(autopotions_id, antipregnum < 1);

	ss.str("");	ss << "Add Rooms: " << g_Game->tariff().add_room_cost(5) << " gold\nCurrent: " << rooms << "\nMaximum: " << maxrooms << endl;
	EditTextItem(ss.str(), roomcost_id);
    DisableWidget(buyrooms_id, rooms >= maxrooms);

	/*	ss.str("");
	ss << "Bar Staff: " << g_Game->tariff().bar_staff_wages() << " gold / week";
	EditTextItem(ss.str(), barstaff_id);

	ss.str("");
	ss << "Casino Staff: " << g_Game->tariff().casino_staff_wages() << " gold / week";
	EditTextItem(ss.str(), casinostaff_id);
	*/

    HideWidget(barhire_id, true);
    HideWidget(casinohire_id, true);
    HideWidget(barfire_id, true);
    HideWidget(casinofire_id, true);
}

void cScreenBuildingSetup::buy_potions(int buypotions)
{
    int buynum = buypotions;
    int buysum = buynum;
    int antipregnum = 0;
    if (!g_Game->gold().afford(g_Game->tariff().anti_preg_price(buynum)))	push_message("You don't have enough gold", COLOR_RED);
    else
    {
        int MaxSupplies = g_Game->MaxSupplies();

        auto& building = active_building();
        antipregnum = building.m_AntiPregPotions;
        if (antipregnum + buynum > MaxSupplies) buysum = max(0, MaxSupplies - antipregnum);
        building.m_AntiPregPotions += buysum;

        if (buysum < buynum)
        {
            ss.str(""); ss << "You can only store up to " << MaxSupplies << " potions.";
            if (buysum > 0)
            {
                ss << "\nYou buy " << buysum << " to fill the stock.";
                g_Game->gold().item_cost(g_Game->tariff().anti_preg_price(buysum));
            }
            g_Game->push_message(ss.str(), 0);
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
        g_Game->push_message(ss.str(), COLOR_RED);
    }
    else
    {
        auto& target = active_building();
        target.m_NumRooms += 5;
        int rooms = target.m_NumRooms;
        int maxrooms = target.m_MaxNumRooms;

        ss.str(""); ss << "Add Rooms: " << g_Game->tariff().add_room_cost(5) << " gold\nCurrent: " << rooms << "\nMaximum: " << maxrooms << endl;
        EditTextItem(ss.str(), roomcost_id);
        DisableWidget(buyrooms_id, rooms >= maxrooms);
        init(false);
    }
}

void cScreenBuildingSetup::set_sex_type_allowed(SKILLS sex_type, bool is_forbidden)
{
    active_building().set_sex_type_allowed(sex_type, !is_forbidden);
}

