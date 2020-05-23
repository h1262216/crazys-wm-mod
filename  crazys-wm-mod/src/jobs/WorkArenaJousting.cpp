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
#include "cJobManager.h"
#include "src/buildings/cBrothel.h"
#include "cCustomers.h"
#include "cRng.h"
#include "cInventory.h"
#include <sstream>
#include "cGangs.h"

bool cJobManager::WorkArenaJousting(sGirl* girl, bool Day0Night1, string& summary, cRng& rng)
{
	int actiontype = ACTION_COMBAT;
	stringstream ss; string girlName = girl->m_Realname; ss << girlName;
	if (girl->disobey_check(actiontype))			// they refuse to work
	{
		ss << " refused to work during the " << (Day0Night1 ? "night" : "day") << " shift.";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}
	ss << " worked as a jouster in the arena.\n";

	int roll = rng.d100();
	int wages = 50, tips = 0, work = 0;
	//string girls = GetNumGirlsOnJob(0, JOB_JOUSTING, Day0Night1)
	//int winner = > JP_ArenaJousting;

	//ss << girls << " competed today.";

	//CRAZY
	/*Jousting plans
	General plan is to at some point make it to where u have a jousting tourment ever week.  More girls the better
	have them place and 1st place thru 3rd get the most money.  How they place would be based off job performance.
	If they have the same lvl of jobperformance then have a random roll to see who wins.  More girls in it mean more
	people show up to watch it and would mean more money.  Not sure how to do this yet but its more or less the basic
	ideal.*/

	double jobperformance = JP_ArenaJousting(girl, false);

	if (jobperformance >= 245)
	{
		ss << " She must be perfect at this.\n \n";
		wages += 155;
	}
	else if (jobperformance >= 185)
	{
		ss << " She's unbelievable at this and is always getting praised by people for her work.\n \n";
		wages += 95;
	}
	else if (jobperformance >= 145)
	{
		ss << " She's good at this job and gets praised by the customers often.\n \n";
		wages += 55;
	}
	else if (jobperformance >= 100)
	{
		ss << " She made a few mistakes but overall she is okay at this.\n \n";
		wages += 15;
	}
	else if (jobperformance >= 70)
	{
		ss << " She was nervous and made a few mistakes. She isn't that good at this.\n \n";
		wages -= 5;
	}
	else
	{
		ss << " She was nervous and constantly making mistakes. She really isn't very good at this job.\n \n";
		wages -= 15;
	}



	//enjoyed the work or not
	if (roll <= 5)
	{
		ss << "\nSome of the patrons abused her during the shift.";
		work -= 1;
	}
	else if (roll <= 25)
	{
		ss << "\nShe had a pleasant time working.";
		work += 3;
	}
	else
	{
		ss << "\nOtherwise, the shift passed uneventfully.";
		work += 1;
	}


	girl->upd_Enjoyment(actiontype, work);
	girl->m_Events.AddMessage(ss.str(), IMGTYPE_COMBAT, Day0Night1);
	int roll_max = (girl->fame() + girl->charisma());
	roll_max /= 4;
	wages += 10 + rng%roll_max;
	girl->m_Tips = max(0, tips);
	girl->m_Pay = max(0, wages);


	// Improve stats
	int xp = 15, libido = 1, skill = 3;

	if (girl->has_trait( "Quick Learner"))		{ skill += 1; xp += 3; }
	else if (girl->has_trait( "Slow Learner"))	{ skill -= 1; xp -= 3; }
	if (girl->has_trait( "Nymphomaniac"))			{ libido += 2; }

    girl->fame(1);
    girl->exp(xp);
    girl->animalhandling(rng%skill);
    girl->combat(rng%skill+2);
	girl->upd_temp_stat(STAT_LIBIDO, libido);

	//gained


	//lose
	cGirls::PossiblyLoseExistingTrait(girl, "Nervous", 30, actiontype, girlName + " has went into battle so many times she is no longer nervous.", Day0Night1 != 0);

	return false;
}

double cJobManager::JP_ArenaJousting(sGirl* girl, bool estimate)// not used
{
	double jobperformance =
		(girl->combat() +
		(girl->intelligence() / 2) +
		(girl->animalhandling() / 2) +
		girl->level());

	if (!estimate)
	{
		int t = girl->tiredness() - 80;
		if (t > 0)
			jobperformance -= (t + 2) * (t / 3);
	}

    jobperformance += girl->get_trait_modifier("work.jousting");

	return jobperformance;
}
