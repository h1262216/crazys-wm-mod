/*
 * Copyright 2023, The Pink Petal Development Team.
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

#include "cScreenPlayerOffice.h"
#include <sstream>
#include "IGame.h"
#include "character/cPlayer.h"
#include "character/sGirl.h"
#include "cGold.h"
#include "sStorage.h"
#include "cGangManager.hpp"
#include "cObjectiveManager.hpp"
#include "buildings/cBuildingManager.h"
#include "InterfaceProcesses.h"

cScreenPlayerOffice::cScreenPlayerOffice() = default;

std::string get_objective();

void cScreenPlayerOffice::setup_callbacks()
{
    SetButtonCallback(m_BuyInteract_id, [this]() { buy_interactions(1); });
    SetButtonCallback(m_BuyInteract10_id, [this]() { buy_interactions(10); });

    SetButtonHotKey(m_Menu_id, SDLK_ESCAPE);
    SetButtonHotKey(m_VisitTown_id, SDLK_t);
    SetButtonHotKey(m_VisitBuildings_id, SDLK_b);
    SetButtonHotKey(m_Dungeon_id, SDLK_d);

    SetButtonHotKey(m_GangManagement_id, SDLK_g);
    SetButtonHotKey(m_InventoryBtn_id, SDLK_i);
    SetButtonHotKey(m_Settings_id, SDLK_s);
    SetButtonHotKey(m_NextWeek_id, SDLK_n);
    SetButtonHotKey(m_TurnSummary_id, SDLK_m);

    SetButtonCallback(m_NextWeek_id, [this]() {
        if (!is_ctrl_held()) { AutoSaveGame(); }
        // need to switch the windows first, so that any new events will show up!
        push_window("Turn Summary");
        g_Game->NextWeek();
    });
}

void cScreenPlayerOffice::init(bool back)
{
    std::stringstream ss;

    ss << "Day: " << g_Game->date().day << " Month: " << g_Game->date().month << " Year: " << g_Game->date().year,
    EditTextItem(ss.str(), m_DateLbl_id);

    ss.str("");
    ss << "Gold: \t" << g_Game->gold().ival()
       << "\nBank account: \t" << g_Game->GetBankMoney();
    ss << "\nDisposition: \t" << g_Game->player().disposition_text();
    ss << "\nSuspicion: \t" << g_Game->player().suss_text();
    ss << "\nGirls: \t" << get_total_player_girls();
    ss << "\nGangs: \t" << g_Game->gang_manager().GetNumGangs();
    ss << "\nBeasts: \t" << g_Game->storage().beasts();
    ss << "\nBusinesses: \t" << g_Game->gang_manager().GetNumBusinessExtorted();
    ss << "\nRunaways: \t" << g_Game->GetNumRunaways();
    if (g_Game->GetNumRunaways() > 0)
    {
        for(const auto& rgirl : g_Game->GetRunaways())
        {
            ss << "\n\t" << rgirl->FullName() << " (" << rgirl->m_RunAway << ")";
        }
    }

    ss << "\nCurrent Objective: \t" << get_objective();

    EditTextItem(ss.str(), m_InfoLbl_id, true);

    ss.str(""); ss << "Interactions Left: ";
    if (g_Game->allow_cheats()) ss << "\nInfinite Cheat";
    else ss << g_Game->GetTalkCount() << "\nBuy more for 1000 each.";
    EditTextItem(ss.str(), m_InteractText_id);

    if (m_BuyInteract_id >= 0) DisableWidget(m_BuyInteract_id, g_Game->allow_cheats() || g_Game->gold().ival() < 1000);
    if (m_BuyInteract10_id >= 0) DisableWidget(m_BuyInteract10_id, g_Game->allow_cheats() || g_Game->gold().ival() < 10000);

    ClearListBox(m_BuildingList_id);
    for(auto& bld : g_Game->buildings().buildings()) {
        std::vector<FormattedCellData> data;
        data.push_back(mk_text(bld->name()));
        data.push_back(FormattedCellData{bld->num_girls(), std::to_string(bld->num_girls()) + "/" + std::to_string(bld->m_NumRooms)});
        data.push_back(mk_num(bld->security()));
        data.push_back(mk_num(bld->filthiness()));
        data.push_back(mk_num(bld->m_Finance.total_profit()));
        AddToListBox(m_BuildingList_id, 0, data);
    }

    Focused();
}

std::shared_ptr<cInterfaceWindow> screens::cPlayerOfficeScreenBase::create() {
    return std::make_shared<cScreenPlayerOffice>();
}

void cScreenPlayerOffice::buy_interactions(int num)
{
    if (g_Game->gold().misc_debit(1000 * num)) g_Game->AddTalkCount(num);
    init(false);
}



static std::string fmt_objective(std::stringstream &ss, std::string desc, int limit, int sofar = -1)
{
    ss << desc;
    if (limit != -1) { ss << " in " << limit << " weeks"; }
    if (sofar > -1) { ss << "\n\t" << sofar << " acquired so far"; }
    ss << ".";
    return ss.str();
}

std::string get_objective() {
    std::stringstream ss;
    sObjective* obj = g_Game->get_objective();
    if (obj)
    {
        switch (obj->m_Objective)
        {
            case OBJECTIVE_REACHGOLDTARGET:
                ss << "End the week with " << obj->m_Target << " gold in the bank";
                if (obj->m_Limit != -1) ss << " within " << obj->m_Limit << " weeks";
                ss << "\n\t" << g_Game->GetBankMoney() << " gathered so far.";
                break;
            case OBJECTIVE_GETNEXTBROTHEL:
                fmt_objective(ss, "Purchase the next brothel", obj->m_Limit);
                break;
                /*----
                case OBJECTIVE_PURCHASENEWGAMBLINGHALL:
                fmt_objective(ss, "Purchase a gambling hall", obj->m_Limit);
                break;
                case OBJECTIVE_PURCHASENEWBAR:
                fmt_objective(ss, "Purchase a bar", obj->m_Limit);
                break;
                ----*/
            case OBJECTIVE_LAUNCHSUCCESSFULATTACK:
                fmt_objective(ss, "Launch a successful attack", obj->m_Limit);
                break;
            case OBJECTIVE_HAVEXGOONS:
                ss << "Have " << obj->m_Target << " gangs";
                fmt_objective(ss, "", obj->m_Limit);
                break;
            case OBJECTIVE_STEALXAMOUNTOFGOLD:
                ss << "Steal " << obj->m_Target << " gold";
                fmt_objective(ss, "", obj->m_Limit, obj->m_SoFar);
                break;
            case OBJECTIVE_CAPTUREXCATACOMBGIRLS:
                ss << "Capture " << obj->m_Target << " girls from the catacombs";
                fmt_objective(ss, "", obj->m_Limit, obj->m_SoFar);
                break;
            case OBJECTIVE_HAVEXMONSTERGIRLS:
                ss << "Have a total of " << obj->m_Target << " monster (non-human) girls";
                fmt_objective(ss, "", obj->m_Limit, get_total_player_monster_girls());
                break;
            case OBJECTIVE_KIDNAPXGIRLS:
                ss << "Kidnap " << obj->m_Target << " girls from the streets";
                fmt_objective(ss, "", obj->m_Limit, obj->m_SoFar);
                break;
            case OBJECTIVE_EXTORTXNEWBUSINESS:
                ss << "Control " << obj->m_Target << " city business";
                fmt_objective(ss, "", obj->m_Limit, obj->m_SoFar);
                break;
            case OBJECTIVE_HAVEXAMOUNTOFGIRLS:
                ss << "Have a total of " << obj->m_Target << " girls";
                fmt_objective(ss, "", obj->m_Limit, get_total_player_girls());
                break;
        }
    }
    else ss << "None";
    return ss.str();
}