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
#pragma region //	Includes and Externs			//
#include "src/buildings/cBrothel.h"
#include <sstream>
#include "cGold.h"
#include "src/Game.hpp"

#pragma endregion

// `J` Job House - General
bool cJobManager::WorkRecruiter(sGirl* girl, bool Day0Night1, string& summary, cRng& rng)
{
	if (Day0Night1) return false;
    auto brothel = girl->m_Building;
#pragma region //	Job setup				//
	int actiontype = ACTION_WORKRECRUIT;
	stringstream ss; string girlName = girl->m_Realname; ss << girlName;
	int roll_a = rng.d100(), roll_b = rng.d100(), roll_c = rng.d100();
	if (girl->disobey_check(actiontype, JOB_RECRUITER))			// they refuse to work
	{
		ss << " refused to work during the " << (Day0Night1 ? "night" : "day") << " shift.";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}
	ss << " worked trying to recruit girls for you.\n \n";

	int wages = 100, tips = 0;
	int enjoy = 0, fame = 0;

	int imagetype = IMGTYPE_PROFILE;
	int msgtype = Day0Night1;

#pragma endregion
#pragma region //	Job Performance			//


	cGirls::UnequipCombat(girl);	// put that shit away, are you are trying to recruit for the military?

	int HateLove = girl->pclove() - girl->pchate();
	int findchance = 0;

	/* */if (HateLove < -80)	ss << "She hates you more then anything so she doesn't try that hard.";
	else if (HateLove < -60)	ss << "She hates you.";
	else if (HateLove < -40)	ss << "She doesn't like you.";
	else if (HateLove < -20)	ss << "She finds you to be annoying.";
	else if (HateLove <   0)	ss << "She finds you to be annoying.";
	else if (HateLove <  20)	ss << "She finds you to be decent.";
	else if (HateLove <  40)	ss << "She finds you to be a good person.";
	else if (HateLove <  60)	ss << "She finds you to be a good person.";
	else if (HateLove <  80)	ss << "She has really strong feelings for you so she trys really hard for you.";
	else						ss << "She loves you more then anything so she gives it her all.";
	ss << "\n \n";

	double jobperformance = JP_Recruiter(girl, false);
	if (jobperformance >= 245)
	{
		ss << "She must be the perfect recruiter.";
		findchance = 20;
	}
	else if (jobperformance >= 185)
	{
		ss << "She's unbelievable at this.";
		findchance = 15;
	}
	else if (jobperformance >= 135)
	{
		ss << "She's good at this job.";
		findchance = 12;
	}
	else if (jobperformance >= 85)
	{
		ss << "She made a few mistakes but overall she is okay at this.";
		findchance = 10;
	}
	else if (jobperformance >= 65)
	{
		ss << "She was nervous and made a few mistakes. She isn't that good at this.";
		findchance = 8;
	}
	else
	{
		ss << "She was nervous and constantly making mistakes. She really isn't very good at this job.";
		findchance = 4;
	}
	ss << "\n \n";

	// `J` add in player's disposition so if the girl has heard of you
	int dispmod = 0;
	/* */if (g_Game->player().disposition() >= 100)	dispmod = 3;	// "Saint"
	else if (g_Game->player().disposition() >= 80)	dispmod = 2;	// "Benevolent"
	else if (g_Game->player().disposition() >= 50)	dispmod = 1;	// "Nice"
	else if (g_Game->player().disposition() >= 10)	dispmod = 0;	// "Pleasant"
	else if (g_Game->player().disposition() >= -10)	dispmod = 0;	// "Neutral"
	else if (g_Game->player().disposition() >= -50)	dispmod = -1;	// "Not nice"
	else if (g_Game->player().disposition() >= -80)	dispmod = -2;	// "Mean"
	else /*								  */	dispmod = -3;	// "Evil"

	int findroll = (rng.d100());
	if (findroll < findchance + 10)	// `J` While out recruiting she does find someone...
	{
		int finddif = findroll - findchance;
		sGirl* newgirl = g_Game->GetRandomGirl(false, (dispmod == -3 && rng % 4 != 0));
		if (newgirl)
		{
			bool add = false;
			ss << "She finds a girl, ";
			ss << newgirl->m_Name;
			if (findroll < findchance - 5)
			{		// `J` ... and your disposition did not come up.
				add = true;
				ss << " and convinces her that she should work for you.";
			}
			else if (findroll < findchance + 5)	// `J` ... and your disposition did come up...
			{
				if (findroll < findchance + dispmod)	// `J` ... and she was recruited
				{
					add = true;
					if (dispmod > 0)
					{
						ss << "\nYour nice reputation has helped you today as she agrees to come work for you.";
					}
					else if (dispmod < 0)
					{
						ss << "\nScared of what you might to do to her if you don't come work for her she agrees to taking the job.";
					}
					else
					{
						ss << "\nThe fact that your neither good nor evil seems to have helped you today.  As the girl doesn't think your evil nor a \"pussy\" and comes to work for you.";
					}
					if (dispmod == 3)
					{
						int rollt(rng % 4);
						if (rollt == 0)	newgirl->add_trait("Optimist");
					}
					if (dispmod == -3)
					{
						int rollt(rng % 4);
						if (rollt == 0)	newgirl->add_trait("Demon");
						if (rollt == 1)	newgirl->add_trait("Fearless");
					}
				}
				else	// `J` ... and she was not recruited
				{

					if (dispmod > 0)
					{
						ss << "\nBeing so nice she is worried you will get yourself killed and someone else will take over that she won't like.  So she won't come work for you.";
					}
					else if (dispmod < 0)
					{
						ss << "\nShe fears you to much to come and work for you.";
					}
					else
					{
						ss << "\nAs you are on the fence with your reputation she doesn't know what to think about you and won't work for you.";
					}
				}
			}
			else	// `J` ... She was not recruited.
			{
				ss << " but was unable to convince her that she should work for you.";
			}
			if (add)
			{
				newgirl->m_Stats[STAT_HOUSE] = 60;
				stringstream NGmsg;
				NGmsg << newgirl->m_Realname << " was recruited by " << girlName << " to work for you.";
				newgirl->m_Events.AddMessage(NGmsg.str(), imagetype, EVENT_GANG);

                g_Game->dungeon().AddGirl(newgirl, DUNGEON_RECRUITED);
			}
		}
		else
		{
			ss << "But was unable to find anyone to join.";
		}
	}
	else
	{
		ss << "But was unable to find anyone to join.";
	}

	ss << "\n \n";

#pragma endregion
#pragma region	//	Enjoyment and Tiredness		//

	//enjoyed the work or not
	if (roll_a <= 5)
	{
		ss << "Some of the people abused her during the shift.";
		enjoy -= 1;
	}
	else if (roll_a <= 25)
	{
		ss << "She had a pleasant time working.";
		enjoy += 3;
	}
	else
	{
		ss << "Otherwise, the shift passed uneventfully.";
		enjoy += 1;
	}

#pragma endregion
#pragma region	//	Money					//


#pragma endregion
#pragma region	//	Finish the shift			//


	girl->upd_Enjoyment(actiontype, enjoy);
	girl->m_Events.AddMessage(ss.str(), imagetype, Day0Night1);
	int roll_max = (girl->charisma() + girl->service());
	roll_max /= 4;
	wages += 10 + rng%roll_max;
	// Money
	girl->m_Tips = max(0, tips);
	girl->m_Pay = max(0, wages);


	// Improve stats
	int xp = 10, libido = 1, skill = 3;

	if (girl->has_trait( "Quick Learner"))		{ skill += 1; xp += 3; }
	else if (girl->has_trait( "Slow Learner"))	{ skill -= 1; xp -= 3; }
	if (girl->has_trait( "Nymphomaniac"))			{ libido += 2; }
	if (girl->fame() < 10 && jobperformance >= 70)		{ fame += 1; }
	if (girl->fame() < 20 && jobperformance >= 100)		{ fame += 1; }
	if (girl->fame() < 40 && jobperformance >= 145)		{ fame += 1; }
	if (girl->fame() < 50 && jobperformance >= 185)		{ fame += 1; }

	girl->fame(fame);
	girl->exp(xp);
	if (rng % 2)	girl->lesbian(1);
	else			girl->intelligence(1);
	if (rng % 2)	girl->charisma(skill);
	else			girl->service(skill);
	girl->upd_temp_stat(STAT_LIBIDO, libido);

	//gain traits
	cGirls::PossiblyGainNewTrait(girl, "Charismatic", 60, actiontype, "Dealing with people all day has made " + girlName + " more Charismatic.", Day0Night1);
	cGirls::PossiblyGainNewTrait(girl, "Psychic", 80, actiontype, girlName + " has been doing this for so long it's as if she can read minds now.", Day0Night1);

	//lose traits
	cGirls::PossiblyLoseExistingTrait(girl, "Nervous", 20, actiontype, girlName + " seems to finally be getting over her shyness. She's not always so Nervous anymore.", Day0Night1);

#pragma endregion
	return false;
}

double cJobManager::JP_Recruiter(sGirl* girl, bool estimate)// not used
{
	if (girl->is_slave()) return -1000;

	int HateLove = girl->pclove() - girl->pchate();
	double jobperformance =
		(HateLove + girl->charisma());
	if (!estimate)
	{
		int t = girl->tiredness() - 80;
		if (t > 0)
			jobperformance -= (t + 2) * (t / 3);
	}
    jobperformance += girl->get_trait_modifier("work.recruiter");

	return jobperformance;
}
