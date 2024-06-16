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

#include <sstream>
#include <algorithm>
#include "character/cCustomers.h"
#include "character/cPlayer.h"
#include "CLog.h"

#include "brothel/cBrothel.h"
#include "cGangs.h"
#include "XmlMisc.h"
#include "cObjectiveManager.hpp"
#include "IGame.h"
#include "jobs/cJobManager.h"
#include "cTariff.h"
#include "utils/string.hpp"
#include "cGirls.h"

extern cRng                    g_Dice;

// // ----- Strut sBrothel Create / destroy
sBrothel::sBrothel() :
    cBuilding(BuildingType::BROTHEL, "Brothel")
{
    m_TotalCustomers = m_RejectCustomersRestrict = m_RejectCustomersDisease = m_MiscCustomers = 0;

    m_FirstJob = JOB_RESTING;
    m_LastJob = JOB_WHORESTREETS;
    m_MatronJob = JOB_MATRON;
}

sBrothel::~sBrothel() = default;

void sBrothel::OnShiftPrepared(bool is_night) {
    g_Game->customers().GenerateCustomers(*this, is_night);
    m_TotalCustomers += g_Game->GetNumCustomers();
}
//        // Runaway, Depression & Drug checking
//        if (runaway_check(current))
//        {
//            current.run_away();
//            return;
//        }
//    });
//}

bool sBrothel::runaway_check(sGirl& girl)
{
/*
    *    nothing interesting happens here unless the girl is miserable
    *
    *    WD: added m_DaysUnhappy tracking
    */

    bool flightrisk = (girl.any_active_trait({traits::KIDNAPPED, traits::EMPRISONED_CUSTOMER}));

    if (flightrisk && girl.happiness() > 50)    // Girls here totally against their will are more likely to try to get away
    {
        girl.m_DaysUnhappy--;                    // and they don't reset days to 0 but instead reduce day count
        if (girl.m_DaysUnhappy < 0)
            girl.m_DaysUnhappy = 0;            // until it gets to 0
        return false;
    }
    else if ((girl.any_active_trait({traits::HOMELESS, traits::ADVENTURER})) && girl.happiness() > 10)
    {    // homeless girls and adventurers know they can survive on their own so are more likely to runaway
        if (girl.m_DaysUnhappy > 3)
            girl.m_DaysUnhappy /= 2;        // they don't reset days to 0 but instead divide day count in half
        else girl.m_DaysUnhappy--;            // or just lower by 1
        return false;
    }
    else if (girl.happiness() > 10)
    {
        girl.m_DaysUnhappy = 0;
        return false;
    }

    /*
    *    `J` this was only adding up for free girls
    *    I made it add up for all girls
    *    and free girls become unhappy faster
    */
    girl.m_DaysUnhappy++;
    if (!girl.is_slave()) girl.m_DaysUnhappy++;
    /*
    *    now there's a matron on duty, she has a chance of fending off
    *    bad things.
    *
    *    previously, it was a 75% chance if a matron was employed
    *    so since we have two shifts, let's have a 35% chance per
    *    shift with a matron
    *
    *    with matrons being girls now, we have some opportunities
    *    for mischief here. For instance, is there still a matron skill?
    *    this should depend on that, if so. Also on how motivated the
    *    matron is. An unhappy matron, or one who hates the PC
    *    may be inclined to turn a blind eye to runaway attempts
    */
    //    int matron_chance = brothel->matron_count() * 35;
    int matron_chance = matron_count() * 35;

    if (g_Dice.percent(matron_chance)) return false;    // if there is a matron 70%

    if (girl.m_DayJob == JOB_REHAB && (num_girls_on_job(JOB_COUNSELOR, true) > 0 || num_girls_on_job(JOB_COUNSELOR, false) > 0))
    {
        if (g_Dice.percent(70)) return false;
    }

    /*
    *    mainly here, we're interested in the chance that she might run away
    */
    if (girl.disobey_check(EActivity::GENERIC))    // check if the girl will run away
    {
        if (g_Dice.percent(g_Game->job_manager().guard_coverage() - girl.m_DaysUnhappy)) return false;

        girl.AddMessage("She ran away.", EImageBaseType::PROFILE, EVENT_DANGER);
        girl.set_stat(STAT_TIREDNESS, 0);
        girl.set_stat(STAT_HEALTH, 100);
        girl.m_RunAway = 6;
        std::stringstream smess;
        smess << girl.FullName() << " has run away.\nSend your goons after her to attempt recapture.\nShe will escape for good after 6 weeks.\n";
        g_Game->push_message(smess.str(), COLOR_WARNING);
        return true;
    }

    if (girl.m_Money <= 50) { return false; }
    if (g_Dice.percent(80 - girl.m_DaysUnhappy)) { return false; }
    /*
    *    if she is unhappy she may turn to drugs
    */
    bool starts_drugs = false;
    //Crazy changed it to this might not be the best // `J` made it better :p
    std::string drug;
    int i = 0;
    if (girl.happiness() <= 20 && girl.has_active_trait( traits::FORMER_ADDICT))
    {
        while (!starts_drugs && i<10)        // `J` She will try to find a drug she used to be addicted to
        {                                    // and if she can't find it in 10 tries she will take what is available
            int d = g_Dice % 8;                // with a slight advantage to alcohol and fairy dust
            switch (d)
            {
            case 1:            drug = traits::SHROUD_ADDICT;            break;    // 12.5%
            case 2: case 3:    drug = traits::FAIRY_DUST_ADDICT;        break;    // 25%
            case 4:            drug = traits::VIRAS_BLOOD_ADDICT;    break;    // 12.5%
            default:           drug = traits::ALCOHOLIC;                break;    // 50%
            }
            if (girl.has_dormant_trait(drug.c_str()))
            {
                starts_drugs = true;
            }
            i++;
        }
    }
    else if (girl.happiness() <= 3 && g_Dice.percent(50) && !girl.has_active_trait( traits::VIRAS_BLOOD_ADDICT))
    {
        drug = traits::VIRAS_BLOOD_ADDICT;
    }
    else if (girl.happiness() <= 5 && g_Dice.percent(50) && !girl.has_active_trait( traits::SHROUD_ADDICT))
    {
        drug = traits::SHROUD_ADDICT;
    }
    else if (girl.happiness() <= 8 && g_Dice.percent(50) && !girl.has_active_trait( traits::FAIRY_DUST_ADDICT))
    {
        drug = traits::FAIRY_DUST_ADDICT;
    }
    else if (girl.happiness() <= 10 && !girl.has_active_trait( traits::ALCOHOLIC))
    {
        drug = traits::ALCOHOLIC;
    }

    /*
    *    if she Just Said No then we're done
    */
    if (drug.empty())
    {
        return false;
    }

    girl.gain_trait(drug.c_str());
    girl.lose_trait(traits::FORMER_ADDICT);

    /*
    *    otherwise, report the sad occurrence
    */
    std::stringstream ss;
    ss << "This girl's unhappiness has turned her into " << (drug == traits::ALCOHOLIC ? "an" : "a") << " " << drug << ".";
    girl.AddMessage(ss.str(), EImageBaseType::PROFILE, EVENT_WARNING);
    return false;
}

void sBrothel::OnEndWeek() {

    std::stringstream ss;
    // get the misc customers
    m_TotalCustomers += m_MiscCustomers;

    ss.str("");
    ss << m_TotalCustomers << " customers visited the building.";
    if (m_RejectCustomersRestrict > 0) ss << "\n \n" << m_RejectCustomersRestrict << " were turned away because of your sex restrictions.";
    if (m_RejectCustomersDisease > 0) ss << "\n \n" << m_RejectCustomersDisease << " were turned away because they had an STD.";
    AddMessage(ss.str());

    // empty rooms cost 2 gold to maintain
    m_Finance.building_upkeep(g_Game->tariff().empty_room_cost(*this));

    // update brothel stats
    if (!girls().empty())
        m_Fame = (total_fame() / girls().num());
    if (m_Happiness > 0 && g_Game->GetNumCustomers())
        m_Happiness = std::min(100, m_Happiness / m_TotalCustomers);


    // advertising costs are set independently for each brothel
    m_Finance.advertising_costs(g_Game->tariff().advertising_costs(m_AdvertisingBudget));

    ss.str("");
    ss << "Your advertising budget for this brothel is " << m_AdvertisingBudget << " gold.";
    if (g_Game->tariff().advertising_costs(m_AdvertisingBudget) != m_AdvertisingBudget)
    {
        ss << " However, due to your configuration, you instead had to pay " <<
           g_Game->tariff().advertising_costs(m_AdvertisingBudget) << " gold.";
    }
    AddMessage(ss.str());

    // `J` include antipreg potions in summary
    ss.str("");
    if (m_AntiPregPotions > 0 || m_AntiPregUsed > 0)
    {
        int num = m_AntiPregPotions;
        int used = m_AntiPregUsed;
        bool stocked = m_KeepPotionsStocked;
        bool matron = num_girls_on_job(m_MatronJob, false) >= 1;
        bool skip = false;    // to allow easy skipping of unneeded lines
        bool error = false;    // in case there is an error this makes for easier debugging

        // first line: previous stock
        if (stocked && num > 0)        ss << "You keep a regular stock of " << num << " Anti-Pregnancy potions in this brothel.\n \n";
        else if (num + used > 0)    ss << "You " << (used > 0 ? "had" : "have") << " a stock of " << (num + used) << " Anti-Pregnancy potions in this brothel.\n \n";
        else { skip = true;            ss << "You have no Anti-Pregnancy potions in this brothel."; }

        // second line: number used
        /* */if (skip){}    // skip the rest of the lines
        else if (used == 0)            { skip = true;    ss << "None were used.\n \n"; }
        else if (num == 0)            { skip = true;    ss << "All have been used.\n \n"; }
        else if (used > 0 && stocked)                ss << used << " were " << (used > num ? "needed" : "used") << " this week.\n \n";
        else if (used > 0 && num > 0 && !stocked)    ss << used << " were used this week leaving " << num << " in stock.\n \n";
        else
        {    // `J` put this in just in case I missed something
            ss << "error code::  BAP02|" << m_AntiPregPotions << "|" << m_AntiPregUsed << "|" << m_KeepPotionsStocked << "  :: Please report it to pinkpetal.org so it can be fixed";
            error = true;
        }

        // third line: budget
        if (!skip && stocked)
        {
            int cost = 0;
            if (used > num)
            {
                ss << used - num << " more than were in stock were needed so an emergency restock had to be made.\n";
                ss << "Normally they cost " << g_Game->tariff().anti_preg_price(1) << " gold, but our supplier charges five times the normal price for unscheduled deliveries.\n \n";
                cost += g_Game->tariff().anti_preg_price(num);
                cost += g_Game->tariff().anti_preg_price(used - num) * 5;
            }
            else
            {
                cost += g_Game->tariff().anti_preg_price(used);
            }

            ss << "Your budget for Anti-Pregnancy potions for this brothel is " << cost << " gold.";

            if (matron && used > num)
            {
                int newnum = (((used / 10) + 1) * 10) + 10;

                AddAntiPreg(newnum - num);
                ss << "\n \nThe Matron of this brothel has increased the quantity of Anti-Pregnancy potions for further orders to " << m_AntiPregPotions << ".";
            }
        }
        if (error) {
            g_LogFile.log(ELogLevel::ERROR, ss.str());
        }
        AddMessage(ss.str());
    }
}

// ----- Stats

void updateGirlTurnBrothelStats(sGirl& girl)
{
    /*
    *    WD: Update each turn the stats for girl in brothel
    *
    *    Uses scaling formula in the form of
    *        bonus = (60 - STAT_HOUSE) / div
    *
    *                div =
    *        STAT    30    20    15
    *        0        2    3    4
    *        1        1    2    3
    *        10        1    2    3
    *        11        1    2    3
    *        20        1    2    2
    *        21        1    1    2
    *        30        1    1    2
    *        31        0    1    1
    *        40        0    1    1
    *        41        0    0    1
    *        50        0    0    0
    *        51        0    0    0
    *        60        0    0    0
    *        61        -1    -1    -1
    *        70        -1    -1    -1
    *        71        -1    -1    -1
    *        80        -1    -1    -2
    *        81        -1    -2    -2
    *        90        -1    -2    -2
    *        91        -2    -2    -3
    *        100        -2    -2    -3
    *
    *
    */

    // Sanity check. Abort on dead girl
    if (girl.is_dead()) { return; }

    std::stringstream ss;
    std::string girlName = girl.FullName();
    int statHouse = girl.house();
    int bonus = (60 - statHouse) / 30;

    if (girl.is_slave())
    {
        if (bonus > 0)                        // Slaves don't get penalties
        {
            girl.obedience(bonus);            // bonus vs house stat    0: 31-60, 1: 01-30, 2: 00
            girl.pcfear(-bonus);
            girl.pclove(bonus);
            bonus = (60 - statHouse) / 15;
            girl.happiness(bonus);            // bonus vs house stat    0: 46-60, 1: 31-45, 2: 16-30, 3: 01-15, 4: 00
        }
    }
    else                                    // Free girls
    {
        girl.obedience(bonus);                // bonus vs house stat    -2: 91-100, -1: 61-90, 0: 31-60, 1: 01-30, 2: 00

        if (bonus > 0)                        // no increase for hate or fear
        {
            girl.pcfear(-bonus);
            girl.pclove(bonus);
        }

        bonus = (60 - statHouse) / 15;
        girl.happiness(bonus);                // bonus vs house stat    -3: 91-100, -2: 76-90, -1: 61-75, 0: 46-60, 1: 31-45, 2: 16-30, 3: 01-15, 4: 00
    }
}
