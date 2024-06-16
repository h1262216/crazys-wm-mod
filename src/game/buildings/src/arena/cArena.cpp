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

#include "cArena.h"
#include "cGangs.h"
#include "buildings/cBuilding.h"
#include "IGame.h"
#include "sStorage.h"
#include "jobs/cJobManager.h"
#include "events.h"
#include "cGirls.h"

extern cRng             g_Dice;

// // ----- Strut sArena Create / destroy
sArena::sArena() : cBuilding(BuildingType::ARENA, "Arena")
{
    m_FirstJob = JOB_FIGHTBEASTS;
    m_LastJob = JOB_GROUNDSKEEPER;
    m_MatronJob = JOB_DOCTORE;
    m_MeetGirlData.Spawn = SpawnReason::ARENA;
    m_MeetGirlData.Event = events::GIRL_MEET_ARENA;
}

sArena::~sArena() = default;

void sArena::auto_assign_job(sGirl& target, std::stringstream& message, bool is_night)
{
    // shortcut
    std::stringstream& ss = message;

    ss << "The Doctore assigns " << target.FullName() << " to ";

    // need at least 1 guard and 1 cleaner (because guards must be free, they get assigned first)
    if (target.is_free() && num_girls_on_job(JOB_CITYGUARD, is_night) < 1)
    {
        target.m_DayJob = target.m_NightJob = JOB_CITYGUARD;
        ss << "work helping the city guard.";
    }
    else if (num_girls_on_job(JOB_GROUNDSKEEPER, is_night) < 1)
    {
        target.m_DayJob = target.m_NightJob = JOB_GROUNDSKEEPER;
        ss << "work cleaning the arena.";
    }
    else if (num_girls_on_job(JOB_BLACKSMITH, is_night) < 1)
    {
        target.m_DayJob = target.m_NightJob = JOB_BLACKSMITH;
        ss << "work making weapons and armor.";
    }
    else if (num_girls_on_job(JOB_COBBLER, is_night) < 1)
    {
        target.m_DayJob = target.m_NightJob = JOB_COBBLER;
        ss << "work making shoes and leather items.";
    }
    else if (num_girls_on_job(JOB_JEWELER, is_night) < 1)
    {
        target.m_DayJob = target.m_NightJob = JOB_JEWELER;
        ss << "work making jewelery.";
    }

        // next assign more guards and cleaners if there are a lot of girls to choose from
    else if (target.is_free() && num_girls_on_job(JOB_CITYGUARD, is_night) < num_girls() / 20)
    {
        target.m_DayJob = target.m_NightJob = JOB_CITYGUARD;
        ss << "work helping the city guard.";
    }
    else if (num_girls_on_job(JOB_GROUNDSKEEPER, is_night) < num_girls() / 20)
    {
        target.m_DayJob = target.m_NightJob = JOB_GROUNDSKEEPER;
        ss << "work cleaning the arena.";
    }
    else if (num_girls_on_job(JOB_BLACKSMITH, is_night) < num_girls() / 20)
    {
        target.m_DayJob = target.m_NightJob = JOB_BLACKSMITH;
        ss << "work making weapons and armor.";
    }
    else if (num_girls_on_job(JOB_COBBLER, is_night) < num_girls() / 20)
    {
        target.m_DayJob = target.m_NightJob = JOB_COBBLER;
        ss << "work making shoes and leather items.";
    }
    else if (num_girls_on_job(JOB_JEWELER, is_night) < num_girls() / 20)
    {
        target.m_DayJob = target.m_NightJob = JOB_JEWELER;
        ss << "work making jewelery.";
    }

        // Assign fighters - 50+ combat

        /*    Only fight beasts if there are 10 or more available
        *        and 1 girl per 10 beasts so they don't get depleted too fast.
        *    You can manually assign more if you want but I prefer to save beasts for the brothel
        *        until each building has their own beast supply.
        *    The farm will supply them when more work gets done to it
        */
    else if (target.combat() > 60 && g_Game->storage().beasts() >= 10 &&
            num_girls_on_job(JOB_FIGHTBEASTS, is_night) < g_Game->storage().beasts() / 10)
    {
        target.m_DayJob = target.m_NightJob = JOB_FIGHTBEASTS;
        ss << "work fighting beast in the arena.";
    }
        // if there are not enough beasts, have the girls fight other girls
    else if (target.combat() > 60 && num_girls_on_job(JOB_FIGHTARENAGIRLS, is_night) < 1)
    {
        target.m_DayJob = target.m_NightJob = JOB_FIGHTARENAGIRLS;
        ss << "work fighting other girls in the arena.";
    }

    else    // assign anyone else to Training
    {
        target.m_DayJob = target.m_NightJob = JOB_FIGHTTRAIN;
        ss << "train for the arena.";
    }
}

std::string sArena::meet_no_luck() const {
    return g_Dice.select_text(
            {
                "Your father once called this 'talent spotting' - "
                "and looking these girls over you see no talent for "
                "anything.",
                "The city is quiet and no one shows up.",
                "Married. Married. Bodyguard. Already works for you. Married. "
                "Hideous. Not a woman. Married. Escorted. Married... "
                "Might as well go home, there's nothing happening out here.",
                "It's not a bad life, if you can get paid for hanging around "
                "on street corners and eyeing up the pretty girls. Not a "
                "single decent prospect in the bunch of them, mind. "
                "Every silver lining has a cloud...",
                "You've walked and walked and walked, and the prettiest "
                "thing you've seen all day turned out not to be female. "
                "It's time to go home...",
                "When the weather is bad, the hunting is good. Get them cold "
                "and wet enough and girls too proud to spread their legs "
                "suddenly can't get their knickers off fast enough, if the job "
                "only comes with room and board. The down side is that you "
                "spend far too much time walking in the rain when everyone "
                "sane is warm inside. Time to head home for a mug of cocoa "
                "and a nice hot trollop.",
                "There's a bit of skirt over there with a lovely "
                "figure, and had a face that was pretty, ninety "
                "years ago. Over yonder, a sweet young thing frolicking "
                "through the marketplace. She's being ever so daring, "
                "spending her daddy's gold, and hasn't yet realised "
                "that there's a dozen of her daddy's goons keeping "
                "a discreet eye on her.  It's like that everywhere "
                "today. Maybe tomorrow will be better."
            }
            );
}