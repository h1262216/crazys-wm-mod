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
bool cJobManager::WorkFilmAnal(sGirl* girl, bool Day0Night1, string& summary, cRng& rng)
{
    auto brothel = dynamic_cast<sMovieStudio*>(girl->m_Building);

	int actiontype = ACTION_WORKMOVIE;
	stringstream ss;	string girlName = girl->m_Realname;
	// No film crew.. then go home	// `J` this will be taken care of in building flow, leaving it in for now
	if (brothel->num_girls_on_job(JOB_CAMERAMAGE, SHIFT_NIGHT) == 0 || brothel->num_girls_on_job(JOB_CRYSTALPURIFIER, SHIFT_NIGHT) == 0)
	{
		ss << "There was no crew to film the scene, so " << girlName << " took the day off";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return false;
	}

	int wages = 50;
	int tips = 0;
	int enjoy = 0;
	int finalqual = 0;
	int jobperformance = 0;
	int bonus = 0;

	cGirls::UnequipCombat(girl);	// not for actress (yet)

	ss << girlName << " worked as an actress filming anal scenes.\n \n";

	int roll = rng.d100();
	if (roll <= 10 && (girl->disobey_check(ACTION_WORKMOVIE, JOB_FILMANAL) || girl->disobey_check(ACTION_SEX, JOB_FILMANAL)))
	{
		ss << "She refused to do anal on film today.\n";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}
	else if (roll <= 10) { enjoy -= rng % 3 + 1;	ss << "She didn't like having a cock up her ass today.\n \n"; }
	else if (roll >= 90) { enjoy += rng % 3 + 1;	ss << "She loved having her ass pounded today.\n \n"; }
	else /*            */{ enjoy += rng % 2;		ss << "She had a pleasant day letting her co-star slip his cock into her butt.\n \n";
	}
	jobperformance = enjoy * 2;

	if (girl->check_virginity())
	{
		jobperformance += 20;
		ss << "She is a virgin.\n";
	}

	// remaining modifiers are in the AddScene function --PP
	finalqual = brothel->AddScene(girl, JOB_FILMANAL, bonus);
	ss << "Her scene is valued at: " << finalqual << " gold.";

	girl->m_Events.AddMessage(ss.str(), IMGTYPE_ANAL, Day0Night1);

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
	int xp = 10, skill = 3;

	if (girl->has_trait( "Quick Learner"))		{ skill += 1; xp += 3; }
	else if (girl->has_trait( "Slow Learner"))	{ skill -= 1; xp -= 3; }

	girl->exp(xp);
	girl->performance(rng%skill);
	girl->anal(rng%skill + 1);

	girl->upd_Enjoyment(ACTION_SEX, enjoy);
	girl->upd_Enjoyment(ACTION_WORKMOVIE, enjoy);
	//gain
	cGirls::PossiblyGainNewTrait(girl, "Fake Orgasm Expert", 50, ACTION_SEX, "She has become quite the faker.", Day0Night1);
	cGirls::PossiblyGainNewTrait(girl, "Slut", 80, ACTION_SEX, girlName + " has turned into quite a slut.", Day0Night1, EVENT_WARNING);
	cGirls::PossiblyGainNewTrait(girl, "Porn Star", 80, ACTION_WORKMOVIE, "She has performed in enough sex scenes that she has become a well known Porn Star.", Day0Night1);
	//lose

	return false;
}

double cJobManager::JP_FilmAnal(sGirl* girl, bool estimate)// not used
{
	return 0;
}
