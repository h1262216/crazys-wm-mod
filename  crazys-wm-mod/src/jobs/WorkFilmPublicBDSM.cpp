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
#include "cCustomers.h"
#include "src/Game.hpp"
#include <sstream>

// Job Movie Studio - Hardcore -
bool cJobManager::WorkFilmPublicBDSM(sGirl* girl, bool Day0Night1, string& summary, cRng& rng)
{
    auto brothel = dynamic_cast<sMovieStudio*>(girl->m_Building);

	int actiontype = ACTION_WORKMOVIE;
	//taken care of in building flow, leaving it in for robustness
	if (brothel->num_girls_on_job(JOB_CAMERAMAGE, SHIFT_NIGHT) == 0 || brothel->num_girls_on_job(JOB_CRYSTALPURIFIER, SHIFT_NIGHT) == 0)
	{
		girl->m_Events.AddMessage("There was no crew to film the scene. She was glad for the day off.", IMGTYPE_PROFILE, EVENT_NOWORK);
		return false;
	}

	stringstream ss;
	string girlName = girl->m_Realname;
	int wages = 50, tips = 0;
	int enjoy = 0;
	double jobperformance = JP_FilmPublicBDSM(girl, false);
	int bonus = 0;

	//JOB
	enum { BYMAN = 1, BYBEAST };
	int fucked = 0, impact = 0;
	bool throat = false, hard = false;


	cGirls::UnequipCombat(girl);	// not for actress (yet)
	int roll = rng.d100();
	if (girl->health() < 50)
	{
		ss << ("The crew refused to film a Public Torture scene with ") << girlName << (" because she is not healthy enough.\n\"We are NOT filming snuff.\"");
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}
	else if (girl->is_pregnant())
	{
		ss << ("The crew refused to film a Public Torture scene with ") << girlName << (" due to her pregnancy.");
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}
	else if (girl->has_trait( "Mind Fucked"))
	{
		ss << ("Mind fucked ") << girlName << (" was pleased to be paraded naked in public and tortured and degraded. It is natural.");
		bonus += 10;
	}
	else if (girl->has_trait( "Masochist"))
	{
		ss << ("Masochist ") << girlName << (" was pleased to be tortured and degraded. It is her place.");
		bonus += 6;
	}
	else if (roll <= 10 && girl->disobey_check(ACTION_WORKMOVIE, JOB_FILMPUBLICBDSM))
	{
		ss << girlName << (" refused to have any part in this");
		if (girl->is_slave())
		{
			if (g_Game->player().disposition() > 30)  // nice
			{
				ss << (" \"monstrous\" scene. She was clearly horrified at the thought so you allowed her the day off.");
				girl->pclove(2);
				girl->pchate(-1);
				girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
				return true;
			}
			else if (g_Game->player().disposition() > -30) //pragmatic
			{
				ss << (" \"monstrous\" scene. She was clearly horrified so you had your men drug her before stripping her down for action.");
				girl->pclove(-1);
				girl->pchate(2);
				girl->pcfear(2);
				g_Game->player().disposition(-1);
				enjoy -= 2;
			}
			else if (g_Game->player().disposition() > -30)
			{
				ss << (" \"monstrous\" scene.\nShe was clearly horrified so you had your men strip her, drag her outside and ");
				/**/ if (girl->has_trait( "Pierced Clit"))		ss << ("whip her clittoral piercing");
				else if (girl->has_trait( "Pierced Nipples"))		ss << ("whip her nipple piercings");
				else ss << ("whip some humility into her");
				ss << (" ahead of the scene.");
				girl->pclove(-4);
				girl->pchate(+5);
				girl->pcfear(+5);
				g_Game->player().disposition(-2);
				enjoy -= 6;
			}
		}
		else // not a slave
		{
			ss << " \"monstrous\" scene today and left.";
			girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
			return true;
		}
	}
	else ss << girlName << " was taken for filming in public bondage and torture scenes.";

	ss << "\n \n";

	//SCENERY
	ss << "A crowd gathered to watch as " << girlName;

	//SCENARIOS
	int NUMSCENARIOS = 9;
	roll = rng % NUMSCENARIOS;

	switch (roll)
	{
	case 0:
		ss << (" was hogtied naked on the floor outside ") << g_Game->buildings().get_building(rng%g_Game->buildings().num_buildings()).name() << (" brothel");
		break;
	case 1:
		ss << (" was stripped and locked in the public stocks in the city square");
		break;
	case 2:
		ss << (" was stripped and strapped over a barrel in the market");
		break;
	case 3:
		ss << (", shackled and sent into the streets in only a maid hat, was quickly surrounded");
		break;
	case 4:
		ss << (" was dragged naked from the studio and tied to a tree");
		break;
	case 5:
		ss << (" was stripped naked and ");
		if (girl->has_trait( "Pierced Clit") || girl->has_trait( "Pierced Nipples") || girl->has_trait( "Pierced Tongue")
			|| girl->has_trait( "Pierced Navel") || girl->has_trait( "Pierced Nose")) ss << ("tied tightly in place through the rings in her piercings");
		else
		{
			ss << ("had her arms taped-together behind her back, exposing her ");
			if (girl->has_trait( "Perky Nipples")) ss << ("perky nipples");
			else if (girl->has_trait( "Puffy Nipples")) ss << ("puffy nipples");
			else if (girl->has_trait( "Abundant Lactation") || girl->has_trait( "Cow Tits")) ss << ("large, milky breasts");
			else if (girl->has_trait( "Flat Chest") || girl->has_trait( "Petite Breasts") || girl->has_trait( "Small Boobs")) ss << ("tiny breasts");
			else if (girl->has_trait( "Busty Boobs") || girl->has_trait( "Big Boobs") || girl->has_trait( "Giant Juggs")) ss << ("large breasts");
			else if (girl->has_trait( "Massive Melons") || girl->has_trait( "Abnormally Large Boobs") || girl->has_trait( "Titanic Tits")) ss << ("huge breasts");
			else ss << ("breasts");
		}
		break;
	case 6:
		ss << (" was tied to a fence and had her clothes ripped off");
		break;
	case 7:
		ss << (" was suspended in the air by chains");
		break;
	case 8:
		ss << (" was rendered helpless and given over to the mob");
		break;
	default:
		ss << (" was tied impossibly in the dirt");
		break;
	}
	ss << (". The crowd jeered at her ");

	//Actions
	int NUMACTIONS = 10;
	roll = rng % NUMACTIONS;

	switch (roll)
	{
	case 0:
		if (girl->has_trait( "Strong Gag Reflex") || girl->has_trait( "Gag Reflex"))
		{
			ss << ("gags, retches and gasps");
		}
		else ss << ("expressions");
		ss << (" as guys competed to shove their dicks down her throat.");
		girl->oralsex(1);
		throat = true;
		impact = 5;
		break;
	case 1:
		ss << ("yelps as a gang of teenagers slapped her tits and spanked her ass with sticks.");
		girl->bdsm(1);
		impact = 1;
		break;
	case 2:
		ss << ("cries as she was double-fisted by three angry elves.");
		girl->anal(1);
		impact = 5;
		break;
	case 3:
		ss << ("gasps and squeals as a cackling old witch claiming to be her 'fairy godmother' mounted her with a thick double-dildo.");
		girl->lesbian(1);
		impact = 3;
		break;
	case 4:
		ss << ("cries as a demented priest shoved 'Candles of Cleansing' in her 'unclean places', chanting prayers as he lit them.");
		girl->bdsm(1);
		impact = 8;
		break;
	case 5:
		ss << ("screams for help as she was fucked by a pack of wild beasts.");
		girl->beastiality(1);
		fucked = BYBEAST;
		hard = true;
		impact = 8;
		break;
	case 6:
		ss << ("struggles as a pair of ");
		if /*  */(girl->beauty() > 80)		ss << ("ugly whores, jealous of her looks");
		else if (girl->age() < 25)			ss << ("old whores, jealous of her youth");
		else if (girl->intelligence() > 75)	ss << ("dumb whores, jealous of her brains");
		else if (girl->is_free())									ss << ("slave whores, jealous of her freedom");
		else													ss << ("sadistic rival whores");
		ss << (" spit-roasted her with some shockingly large strap-ons.");
		girl->lesbian(1);
		impact = 5;
		break;
	case 7:
		ss << ("cries as a large gang of ");
		if /* */(girl->has_trait( "Tsundere") || girl->has_trait( "Yandere"))	ss << ("losers she's rejected over the years");
		else if ((girl->has_trait( "Lolita") || girl->has_trait( "Flat Chest") || girl->has_trait( "Petite Breasts") || girl->has_trait( "Small Boobs"))
			&& (girl->age() < 21)) ss << ("probable pedos");
		else if (girl->has_trait( "Queen") || girl->has_trait( "Princess")) ss << ("her former subjects");
		else if (girl->has_trait( "Idol") || (girl->fame() > 75))	ss << ("men from her fan-club");
		else if (girl->has_trait( "Teacher")) ss << ("students from her class");
		else if (girl->m_NumCusts > 100) ss << ("her former customers");
		else if (girl->age() < 21) ss << ("guys she went to school with");
		else ss << ("tramps, gypsies and homeless");
		ss << (" took this chance to spank her, grope her and fill her cunt with cum.");
		girl->normalsex(1);
		fucked = BYMAN;
		impact = 3;
		break;
	case 8:
		ss << ("screams as the police captain stubbed out a cigar on her asshole, once he and his men were 'done with that'.");
		girl->bdsm(2);
		fucked = BYMAN;
		hard = true;
		impact = 10;
		break;
	case 9:
		ss << ("agonised screams as a passing battle-mage tested out flame spells on her naked form.");
		girl->bdsm(2);
		hard = true;
		impact = 7;
		break;
	default:
		ss << ("puzzled expression as something impossible happened.");
		break;
	}
	ss << "\nYour film crew rescued her once they had enough footage.\n \n";

	if (jobperformance >= 350)
	{
		ss << ("It was an outstanding scene, and definitely boosted her fame and resliience, even if it did wear her out a lot.");
		bonus = 12;
		girl->fame(5);
		girl->constitution(3);
		girl->tiredness(10 + impact);
	}
	else if (jobperformance >= 245)
	{
		ss << ("It was a great scene and should win her some fans. She looks wrecked now though.");
		bonus = 8;
		girl->fame(2);
		girl->constitution(2);
		girl->tiredness(15 + impact);
	}
	else if (jobperformance >= 185)
	{
		ss << ("It was a good scene, but not the best.");
		bonus = 4;
		girl->fame(1);
		girl->tiredness(15 + impact);
	}
	else if (jobperformance >= 145)
	{
		ss << ("It was okay, but something about the scene didn't work.");
		bonus = 2;
		girl->tiredness(15 + impact);
	}
	else if (jobperformance >= 100)
	{
		ss << ("It wasn't a great public scene.");
		bonus = 1;
		girl->tiredness(20 + impact);
	}
	else
	{
		ss << ("Sadly, the scene really didn't work. Even the onlookers weren't looking.");
		girl->fame(-1);
		girl->tiredness(20 + impact);
	}
	ss << ("\n");

	//Enjoyed? If she's deranged, she'd should have enjoyed it.
	if (girl->has_trait( "Mind Fucked"))
	{
		enjoy += 16;
		ss << ("Being completely mind fucked, ") << girlName << (" actually gets off on this.\n");
	}
	else if (girl->has_trait( "Masochist"))
	{
		enjoy += 10;
		ss << girlName << (" enjoys this. It's what she deserves.\n");
	}
	else if (girl->has_trait( "Broken Will") || girl->has_trait( "Dependant"))
	{
		enjoy += 5;
		ss << girlName << (" accepts this. It is Master's will.\n");
	}
	else if (girl->has_trait( "Iron Will") || girl->has_trait( "Fearless"))
	{
		enjoy -= 5;
		ss << girlName << (" endures in stoic silence, determined not to let you see her suffer.\n");
	}
	else if (girl->has_trait( "Nymphomaniac"))
	{
		enjoy += 2;
		ss << girlName << (" doesn't much like the pain, but enjoys getting this much sex and attention.\n");
	}
	//Feedback enjoyment
	if (enjoy > 10)
	{
		ss << "She won't say it, but you suspect she secretly gets off on the attention, sin and degradation.\n \n";
	}
	else if (enjoy > 0)
	{
		ss << "She's only a little traumatised.\n \n";
	}
	else
	{
		ss << "From the way she's still crouched, rocking in a corner and silently weeping... you figure she didn't enjoy this.\n \n";
	}

	bonus = bonus + enjoy;

	if (fucked == BYMAN || fucked == BYBEAST)
	{
		if (girl->check_virginity())
		{
			ss << "Somewhere in all this, she lost her virginity.\n";
			girl->lose_virginity();
			bonus += 5;
		}
		sCustomer Cust = g_Game->GetCustomer(*brothel);
		Cust.m_Amount = 1;
		if (fucked == BYMAN)
		{

			if (!girl->calc_pregnancy(Cust, false, 0.75))
			{
				g_Game->push_message(girl->m_Realname + " has gotten pregnant.", 0);
				ss << "And she's now pregnant.\nCongratulations!\n";
			}
		}
		else
		{
			if (!girl->calc_insemination(*cGirls::GetBeast(), false, 0.75))
			{
				g_Game->push_message(girl->m_Realname + " has been inseminated.", 0);
				ss << "And she's been inseminated by a beast.\nCongratulations!\n";
			}
		}
	}

	//BSIN: Chaned .AddScene fn to use JOB_FILMxxxxx instead of SKILL as it makes much more sense.
	//I'm guessing this function was made before that Constant was added.
	int finalqual = brothel->AddScene(girl, JOB_FILMPUBLICBDSM, bonus);
	ss << "Her scene is valued at: " << finalqual << " gold.\n";
	girl->m_Events.AddMessage(ss.str(), IMGTYPE_BDSM, Day0Night1);

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

	//Hate
	girl->pchate((impact - enjoy));
	girl->pcfear((impact - enjoy));
	//Health
	girl->health((4 * impact));


	girl->exp(xp);
	girl->performance(rng%skill);
	girl->bdsm(rng%skill + 1);
	girl->obedience(impact / 2);

	girl->upd_Enjoyment(ACTION_SEX, enjoy);
	girl->upd_Enjoyment(ACTION_WORKMOVIE, enjoy);
	//gain
	cGirls::PossiblyGainNewTrait(girl, "Masochist", 75, ACTION_SEX, girlName + " has turned into a Masochist from filming so many BDSM scenes.", Day0Night1);
	cGirls::PossiblyGainNewTrait(girl, "Porn Star", 80, ACTION_WORKMOVIE, "She has performed in enough sex scenes that she has become a well known Porn Star.", Day0Night1);
	if (hard)
	{
		cGirls::PossiblyGainNewTrait(girl, "Mind Fucked", 65, ACTION_SEX, girlName + " was pushed too far in her public torture film and is now completely Mind Fucked.", Day0Night1);
		cGirls::GirlInjured(girl, 5);
	}
	//lose
	if (hard) cGirls::PossiblyLoseExistingTrait(girl, "Iron Will", 80, ACTION_SEX, girlName + "'s public degradation has shattered her iron will.", Day0Night1);


	//Evil job bonus-------------------------------------------------------
	//BONUS - evil jobs damage her body, break her spirit and make her hate you

	int MrEvil = rng % 8, MrNasty = rng % 8;
	MrEvil = (MrEvil+ MrNasty) / 2;				//Should come out around 3 most of the time.

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

double cJobManager::JP_FilmPublicBDSM(sGirl* girl, bool estimate)
{
	double jobperformance =
		(girl->charisma() + girl->beauty() + girl->dignity());

	if (!estimate)
	{
		int t = girl->tiredness() - 80;
		if (t > 0)
			jobperformance -= (t + 2) * (t / 3);
	}

    if (girl->check_virginity())                        jobperformance += 20;   //

    jobperformance += girl->get_trait_modifier("work.film.publicbdsm");

    return jobperformance;
}
