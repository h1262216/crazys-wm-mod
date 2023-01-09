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

#include "cBrothel.h"
#include "cGangs.h"
#include "XmlMisc.h"
#include "cObjectiveManager.hpp"
#include "IGame.h"
#include "jobs/cJobManager.h"
#include "cTariff.h"
#include "utils/string.hpp"
#include "cGirls.h"
#include "queries.h"

// fwd-declare the relevant settings
namespace settings {
    extern const char* PREG_COOL_DOWN;
    extern const char* USER_ITEMS_AUTO_USE;
    extern const char* BALANCING_FATIGUE_REGAIN;
}

extern cRng                    g_Dice;

// // ----- Strut sBrothel Create / destroy
sBrothel::sBrothel() :
    cBuilding("Brothel", {BuildingType::BROTHEL, JOB_RESTING, JOB_WHORESTREETS, JOB_MATRON})
{
    m_TotalCustomers = m_RejectCustomersRestrict = m_RejectCustomersDisease = m_MiscCustomers = 0;
}

sBrothel::~sBrothel() = default;
/*
void sBrothel::UpdateGirls(bool is_night)
{
    // TODO Fix this!
    m_AdvertisingLevel = 1.0;  // base multiplier
    IterateGirls(is_night, {JOB_ADVERTISING},
                 [&](auto& current) {
                     g_Game->job_manager().handle_simple_job(current, is_night);
                 });
    g_Game->customers().GenerateCustomers(*this, is_night);
    m_TotalCustomers += g_Game->GetNumCustomers();

    IterateGirls(is_night, {JOB_BARMAID, JOB_WAITRESS, JOB_SINGER, JOB_PIANO, JOB_DEALER, JOB_ENTERTAINMENT,
                            JOB_XXXENTERTAINMENT, JOB_SLEAZYBARMAID, JOB_SLEAZYWAITRESS, JOB_BARSTRIPPER, JOB_MASSEUSE,
                            JOB_BROTHELSTRIPPER, JOB_PEEP, JOB_WHOREBROTHEL, JOB_BARWHORE, JOB_WHOREGAMBHALL},
                 [&](auto& current) {
                     g_Game->job_manager().handle_simple_job(current, is_night);
    });

    bool matron = num_girls_on_job(*this, matron_job(), false) >= 1;
    std::stringstream ss;

    //   as for the rest of them...
    girls().apply([&](sGirl& current){
        bool refused = false;
        auto sum = EVENT_SUMMARY;

        //        ONCE DAILY processing
        //       at start of Day Shift
        if (is_night == SHIFT_DAY)
        {
            // Back to work
            if (current.m_NightJob == JOB_RESTING && current.m_DayJob == JOB_RESTING && current.m_PregCooldown < g_Game->settings().get_integer(settings::PREG_COOL_DOWN) &&
                current.health() >= 80 && current.tiredness() <= 20)
            {
                if ((matron || current.m_PrevDayJob == matron_job())                    // do we have a director, or was she the director and made herself rest?
                    && current.m_PrevDayJob != JOB_UNSET && current.m_PrevNightJob != JOB_UNSET)    // 255 = nothing, in other words no previous job stored
                {
                    g_Game->job_manager().HandleSpecialJobs(current, current.m_PrevDayJob, current.m_DayJob, false);
                    if (current.m_DayJob == current.m_PrevDayJob)  // only update night job if day job passed HandleSpecialJobs
                        current.m_NightJob = current.m_PrevNightJob;
                    else
                        current.m_NightJob = JOB_RESTING;
                    current.m_PrevDayJob = current.m_PrevNightJob = JOB_UNSET;
                    current.AddMessage("The Matron puts ${name} back to work.\n", EImageBaseType::PROFILE, EVENT_BACKTOWORK);
                }
                else
                {;
                    current.AddMessage("WARNING ${name} is doing nothing!\n", EImageBaseType::PROFILE, EVENT_WARNING);
                }
            }
        }

        //       EVERY SHIFT processing

        // Sanity check! Don't process dead girls
        if (current.is_dead())
        {
            return;
        }
        cGirls::UseItems(current);                        // Girl uses items she has
        cGirls::UpdateAskPrice(current, true);        // Calculate the girls asking price

        //       JOB PROCESSING
        JOBS sw = current.get_job(is_night);

        // Sanity check! Don't process runaways
        if (sw == JOB_RUNAWAY)        // `J` added for .06.03.00
        {
            return;
        }

        // do their job
        // advertising and whoring are handled earlier.
        if (sw == JOB_ADVERTISING || sw == JOB_WHOREGAMBHALL || sw == JOB_WHOREBROTHEL ||
            sw == JOB_BARWHORE || sw == JOB_BARMAID || sw == JOB_WAITRESS ||
            sw == JOB_SINGER || sw == JOB_PIANO || sw == JOB_DEALER || sw == JOB_ENTERTAINMENT ||
            sw == JOB_XXXENTERTAINMENT || sw == JOB_SLEAZYBARMAID || sw == JOB_SLEAZYWAITRESS ||
            sw == JOB_BARSTRIPPER || sw == JOB_MASSEUSE || sw == JOB_BROTHELSTRIPPER || sw == JOB_PIANO || sw == JOB_PEEP)
        {
            // these jobs are already done so we skip them
        }
        else {
            g_Game->job_manager().handle_simple_job(current, is_night);
        }

        // Runaway, Depression & Drug checking
        if (runaway_check(current))
        {
            current.run_away();
            return;
        }

        //       MATRON CODE START

        // Lets try to compact multiple messages into one.
        bool matron = (num_girls_on_job(*this, JOB_MATRON, true) >= 1 || num_girls_on_job(*this, JOB_MATRON, false) >= 1);

        std::string MatronWarningMsg;
        std::string MatronMsg;

        if (current.tiredness() > 80)
        {
            if (matron)
            {
                if (current.m_PrevNightJob == 255 && current.m_PrevDayJob == 255)
                {
                    current.m_PrevDayJob = current.m_DayJob;
                    current.m_PrevNightJob = current.m_NightJob;
                    current.m_DayJob = current.m_NightJob = JOB_RESTING;
                    MatronWarningMsg += "Your matron takes ${name} off duty to rest due to her tiredness.\n";
                }
                else
                {
                    if (g_Dice.percent(70))
                    {
                        MatronMsg += "Your matron helps ${name} to relax.\n";
                        current.tiredness(-5);
                    }
                }
            }
            else if (is_night && current.m_NightJob != JOB_RESTING)
                MatronWarningMsg += "CAUTION! This girl desperately needs rest. Give her some free time\n";
        }

        if (current.happiness() < 40 && matron && g_Dice.percent(70))
        {
            MatronMsg = "Your matron helps cheer up ${name} after she feels sad.\n";
            current.happiness(5);
        }

        if (current.health() < 40)
        {
            if (matron)
            {
                if (current.m_PrevNightJob == 255 && current.m_PrevDayJob == 255)
                {
                    current.m_PrevDayJob = current.m_DayJob;
                    current.m_PrevNightJob = current.m_NightJob;
                    current.m_DayJob = current.m_NightJob = JOB_RESTING;
                    MatronWarningMsg += "${name} is taken off duty by your matron to rest due to her low health.\n";
                }
                else
                {
                    MatronMsg = "Your matron helps heal ${name}.\n";
                    current.health(5);
                }
            }
            else
            {
                MatronWarningMsg = "DANGER ${name}'s health is very low!\nShe must rest or she will die!\n";
            }
        }

        if (!MatronMsg.empty())
        {
            current.AddMessage(MatronMsg, EImageBaseType::PROFILE, is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);
            MatronMsg = "";
        }

        if (!MatronWarningMsg.empty())
        {
            current.AddMessage(MatronWarningMsg, EImageBaseType::PROFILE, EVENT_WARNING);
            MatronWarningMsg = "";
        }
    });
}
*/
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
    int matron_count = 0;
    for (int i = 0; i < 2; i++)
    {
        if(matron_on_shift(i)) {
            ++matron_count;
        }
    }
    int matron_chance = matron_count * 35;

    if (g_Dice.percent(matron_chance)) return false;    // if there is a matron 70%

    if (girl.m_DayJob == JOB_REHAB && (num_girls_on_job(*this, JOB_COUNSELOR, true) > 0 || num_girls_on_job(*this, JOB_COUNSELOR, false) > 0))
    {
        if (g_Dice.percent(70)) return false;
    }

    /*
    *    mainly here, we're interested in the chance that she might run away
    */
    if (girl.disobey_check(ACTION_GENERAL))    // check if the girl will run away
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