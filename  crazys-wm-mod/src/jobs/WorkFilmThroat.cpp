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
#include "src/buildings/cBrothel.h"
#include "cRng.h"
#include "src/buildings/cMovieStudio.h"
#include "src/Game.hpp"
#include <sstream>

// `J` Job Movie Studio - Actress
bool cJobManager::WorkFilmThroat(sGirl* girl, bool Day0Night1, string& summary, cRng& rng)
{
    auto brothel = dynamic_cast<sMovieStudio*>(girl->m_Building);

	int actiontype = ACTION_WORKMOVIE;
	// No film crew.. then go home	// `J` this will be taken care of in building flow, leaving it in for now
	if (brothel->num_girls_on_job(JOB_CAMERAMAGE, SHIFT_NIGHT) == 0 || brothel->num_girls_on_job(JOB_CRYSTALPURIFIER, SHIFT_NIGHT) == 0)
	{
		girl->m_Events.AddMessage("There was no crew to film the scene, so she took the day off", IMGTYPE_PROFILE, EVENT_NOWORK);
		return false;
	}

	stringstream ss;
	string girlName = girl->m_Realname;
	int wages = 50, tips = 0;
	int enjoy = 0, hate = 0, tired = 0, bonus = 0;
	double jobperformance = JP_FilmThroat(girl, false);

	cGirls::UnequipCombat(girl);	// not for actress (yet)

	int roll = rng.d100();

	if (girl->health() < 20)
	{
		ss << ("The crew refused to film a throatjob scene because ") << girlName << (" is not healthy enough.\n\"We are NOT filming snuff\".");
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}
	else if (girl->has_trait( "Mind Fucked"))
	{
		ss << ("Mind fucked ") << girlName << (" was pleased to be paraded naked in public and tortured and degraded. It is natural.\n");
		bonus += 10;
	}
	else if (girl->has_trait( "Masochist"))
	{
		ss << ("Masochist ") << girlName << (" was pleased to be tortured, used and degraded. It is her place.\n");
		bonus += 6;
	}
	else if (roll <= 10 && girl->disobey_check(ACTION_WORKMOVIE, JOB_FILMFACEFUCK))
	{
		ss << girlName << (" angrily refused to be throat-fucked on film today.");
		if (girl->is_slave())
		{
			if (g_Game->player().disposition() > 30)  // nice
			{
				ss << ("\nThough she is a slave, she was upset so you allowed her the day off.\n");
				girl->pclove(2);
				girl->spirit(1);
				girl->pchate(-1);
				girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
				return true;
			}
			else if (g_Game->player().disposition() > -30) //pragmatic
			{
				ss << (" Amused, you over-ruled her, and gave owner's consent for her. She glared at you as they dragged her away.\n");
				girl->pclove(-1);
				girl->pchate(2);
				girl->pcfear(2);
				g_Game->player().disposition(-1);
				enjoy -= 2;
			}
			else if (g_Game->player().disposition() > -30)
			{
				ss << (" Amused, you over-ruled her, and gave owner's consent.\nShe made a hell of a fuss, but you knew just the thing to shut her up.");
				girl->pclove(-4);
				girl->pchate(+5);
				girl->pcfear(+5);
				g_Game->player().disposition(-2);
				enjoy -= 6;
			}
		}
		else // not a slave
		{
			girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
			return true;
		}
	}
	else ss << girlName << " was filmed in facefucking scenes.\n \n";


	int OPTION = rng % 3;
	int OUTCOME = rng % 3;
	if (jobperformance >= 350)
	{
		switch (OPTION)
		{
		case 0:
			ss << girlName << (" knelt in front of him, opened wide, and swallowed down his whole length, bouncing back and forward until he came ");
			break;
		case 1:
			ss << girlName << (" lay on her back on the bed with her head hanging back over the edge. Gripping her breasts, he rammed his cock down her throat until cum exploded ");
			break;
		case 2:
			ss << ("The actor sat on a chair, with ") << girlName << (" knelt before him. Gripping her head, he pulled her onto his cock, rocking her back and forward on his cock until he came ");
			break;
		default:
			ss << girlName << (" did some kind of throatfuck until he came ");
			break;
		}
		switch (OUTCOME)
		{
		case 0:
			ss << ("deep down her throat");
			break;
		case 1:
			ss << ("in her mouth");
			break;
		case 2:
			ss << ("deep in her stomach");
			break;
		default:
			ss << ("somewhere");
			break;
		}
		ss << (".\n");
		bonus = 12;
		hate = 1;
		tired = 1;
	}
	else if (jobperformance >= 245)
	{
		switch (OPTION)
		{
		case 0:
			ss << girlName << (" knelt in front of him and sucked his cock down, bouncing back and forward until he came ");
			break;
		case 1:
			ss << girlName << (" lay back on the bed with her head over the edge. He fucked her throat until he came ");
			break;
		case 2:
			ss << ("The actor sat on a chair, with ") << girlName << (" before him. Gripping her head, he pulled her onto his cock, back and forth until he came ");
			break;
		default:
			ss << girlName << (" did some kind of throatfuck until he came ");
			break;
		}
		switch (OUTCOME)
		{
		case 0:
			ss << ("all over her");
			break;
		case 1:
			ss << ("in her mouth");
			break;
		case 2:
			ss << ("deep down her throat");
			break;
		default:
			ss << ("somewhere");
			break;
		}
		hate = 2;
		tired = 4;
		bonus = 6;
	}
	else if (jobperformance >= 185)
	{
		switch (OPTION)
		{
		case 0:
			ss << girlName << (" knelt down and let him facefuck her until he came ");
			break;
		case 1:
			ss << girlName << (" lay on the bed and let him fuck her throat. He carefully facefucked her until he came ");
			break;
		case 2:
			ss << ("The actor pulled ") << girlName << ("'s head onto his cock, rubbing his cock down her throat until he came ");
			break;
		default:
			ss << girlName << (" did some kind of throatfuck until he came ");
			break;
		}
		switch (OUTCOME)
		{
		case 0:
			ss << ("all over her");
			break;
		case 1:
			ss << ("in her mouth");
			break;
		case 2:
			ss << ("down her throat");
			break;
		default:
			ss << ("somewhere");
			break;
		}
		bonus = 4;
		hate = 3;
		tired = 8;
	}
	else if (jobperformance >= 145)
	{
		switch (OPTION)
		{
		case 0:
			ss << girlName << (" knelt down and get his cock down her throat. Eventually he came ");
			break;
		case 1:
			ss << girlName << (" lay on the bed and tried to take his meat in her throat. He facefucked her until he finally came ");
			break;
		case 2:
			ss << ("The actor pulled ") << girlName << ("'s head onto his cock, making her gag, until he came ");
			break;
		default:
			ss << girlName << (" did some kind of throatfuck until he came ");
			break;
		}
		switch (OUTCOME)
		{
		case 0:
			ss << ("in her face");
			break;
		case 1:
			ss << ("in her mouth");
			break;
		case 2:
			ss << ("over her face");
			break;
		default:
			ss << ("somewhere");
			break;
		}
		bonus = 2;
		hate = 5;
		tired = 10;
	}
	else if (jobperformance >= 100)
	{
		ss << ("It was a pretty awkward scene, with the actor not getting much pleasure trying to fuck her throat. ");
		ss << ("In the end he gave up and plunged his cock down her throat, making her throw up through her nose, as he came in her head.");
		bonus = 1;
		hate = 8;
		tired = 12;
	}
	else
	{
		ss << ("With her continual gagging, retching and vomittig the actor couldn't get any pleasure, and ended up wanking over her face.");
		hate = 8;
		tired = 12;
	}
	ss << ("\n");

	//Enjoyed?! No, but some will hate it less.
	if (jobperformance >= 200)
	{
		enjoy++;
		ss << "She won't say it, but you suspect she secretly gets off on the degradation.\n \n";
	}
	else if (jobperformance >= 100)
	{
		enjoy -= (1 + rng % 2);
		ss << "She's pretty raw and her makeup is everywhere.\n \n";
	}
	else
	{
		enjoy -= (2 + rng % 3);
		ss << "From the way she's coughing and gagging and has bright red eyes, you suspect this wasn't much fun for her.\n \n";
	}
	bonus = bonus + enjoy;

	int impact = rng%10;
	if (girl->has_trait( "Strong Gag Reflex"))
	{
		ss << "She was gagging and retching the whole scene, and was violently sick. She was exhausted and felt awful afterward.\n \n";
		girl->health((10 + impact));
		girl->tiredness((10 + impact + tired));
		girl->pchate((2 + hate));
		girl->pcfear((2 + hate));
	}
	else if (girl->has_trait( "Gag Reflex"))
	{
		ss << "She gagged and retched a lot. It was exhausting and left her feeling sick.\n \n";
		girl->health((2 + impact));
		girl->tiredness((5 + impact + tired));
		girl->pchate((hate));
		girl->pcfear((hate));
	}

	// remaining modifiers are in the AddScene function --PP
	int finalqual = brothel->AddScene(girl, JOB_FILMORAL, bonus);
	ss << "Her scene is valued at: " << finalqual << " gold.\n";

	girl->m_Events.AddMessage(ss.str(), IMGTYPE_ORAL, Day0Night1);

	// work out the pay between the house and the girl
	if (girl->is_slave() && !cfg.initial.slave_pay_outofpocket())
	{
		wages = 0;	// You own her so you don't have to pay her.
	}
	else
	{
		wages += finalqual * 2;
	}
	girl->m_Tips = max(0, tips);
	girl->m_Pay = max(0, wages);

	// Improve stats
	int xp = 10, skill = 3, libido = 1;

	if (girl->has_trait( "Quick Learner"))		{ skill += 1; xp += 3; }
	else if (girl->has_trait( "Slow Learner"))	{ skill -= 1; xp -= 3; }
	if (girl->has_trait( "Nymphomaniac"))			{ libido += 2; }


	girl->exp(xp);
	girl->performance(rng%skill);
	girl->oralsex(rng%skill + 1);
	girl->upd_temp_stat(STAT_LIBIDO, libido);

	girl->upd_Enjoyment(ACTION_SEX, enjoy);
	girl->upd_Enjoyment(ACTION_WORKMOVIE, enjoy);
	cGirls::PossiblyGainNewTrait(girl, "Masochist", 75, ACTION_SEX, girlName + " has turned into a Masochist from filming so many BDSM scenes.", Day0Night1);
	cGirls::PossiblyGainNewTrait(girl, "Mind Fucked", 90, ACTION_WORKMOVIE, "She has been abused so much she is now completely Mind Fucked.", Day0Night1);
	if (jobperformance > 200) cGirls::PossiblyGainNewTrait(girl, "Porn Star", 80, ACTION_WORKMOVIE, "She has performed in enough sex scenes that she has become a well known Porn Star.", Day0Night1);
	if (rng.percent(5) && (girl->happiness() > 80) && (girl->get_enjoyment(ACTION_WORKMOVIE) > 75))
		cGirls::AdjustTraitGroupGagReflex(girl, 1, true, Day0Night1);

	//lose
	if (rng.percent(5)) cGirls::PossiblyLoseExistingTrait(girl, "Iron Will", 80, ACTION_SEX, "Somewhere between having a dick in her throat, balls slapping her eyes and a camera watching her retch, " + girlName + " has lost her iron will.", Day0Night1);

	//Evil job bonus-------------------------------------------------------
	//BONUS - evil jobs damage her body, break her spirit and make her hate you

	int MrEvil = rng % 8, MrNasty = rng % 8;
	MrEvil = (MrEvil + MrNasty) / 2;				//Should come out around 3 most of the time.

	girl->confidence(-MrEvil);
	girl->spirit(-MrEvil);
	girl->dignity(-MrEvil);
	girl->pclove(-MrEvil);
	girl->pchate(MrEvil);
	girl->pcfear(MrEvil);
	g_Game->player().disposition(-MrEvil);

	//----------------------------------------------------------------------

	return false;
}

double cJobManager::JP_FilmThroat(sGirl* girl, bool estimate)
{
	double jobperformance =
		(((girl->charisma() + girl->beauty()) / 2)
		+ girl->oralsex());

	if (!estimate)
	{
		int t = girl->tiredness() - 80;
		if (t > 0)
			jobperformance -= (t + 2) * (t / 3);
	}

    jobperformance += girl->get_trait_modifier("work.film.throat");

	return jobperformance;
}
