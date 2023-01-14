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
#include "jobs/IJobManager.h"
#include "events.h"
#include "queries.h"
#include "cGirls.h"
#include "cBuildingShift.h"
#include "character/cCustomers.h"
#include "utils/streaming_random_selection.hpp"

extern cRng             g_Dice;

extern const char* const DrawVisitorsId = "Visitors";
extern const char* const FightsFameId = "FightFame";
extern const char* const BrutalityId = "Brutality";
extern const char* const SexualityId = "Sexuality";
extern const char* const CombatId = "Combat";
extern const char* const BeautyId = "Beauty";
extern const char* const ArenaFightId = "ArenaFight";
extern const char* const ResuscitateId = "Resuscitate";
extern const char* const SurgeryId = "Surgery";

namespace settings {
    extern const char* USER_SHOW_NUMBERS;
}

namespace {
    sBuildingConfig ArenaConfig() {
        return sBuildingConfig{BuildingType::ARENA, JOB_FIGHTBEASTS,
                               JOB_GROUNDSKEEPER, JOB_DOCTORE}
                .spawn(SpawnReason::ARENA, events::GIRL_MEET_ARENA);
    }
}

// // ----- Strut sArena Create / destroy
sArena::sArena() : cBuilding("Arena", ArenaConfig())
{
    m_Shift->declare_resource(DrawVisitorsId);
    m_Shift->declare_resource(FightsFameId);
    m_Shift->declare_resource(BrutalityId);
    m_Shift->declare_resource(SexualityId);
    m_Shift->declare_resource(CombatId);
    m_Shift->declare_resource(BeautyId);
    m_Shift->declare_resource(ArenaFightId);
    m_Shift->declare_resource(SurgeryId);
    m_Shift->declare_interaction(ResuscitateId);
    declare_variable("Fame", &m_Reputation);
}

sArena::~sArena() = default;


void sArena::auto_assign_job(sGirl& target, std::stringstream& message, bool is_night)
{
    // shortcut
    std::stringstream& ss = message;

    ss << "The Doctore assigns " << target.FullName() << " to ";

    // need at least 1 guard and 1 cleaner (because guards must be free, they get assigned first)
    if (target.is_free() && num_girls_on_job(*this, JOB_CITYGUARD, is_night) < 1)
    {
        target.m_DayJob = target.m_NightJob = JOB_CITYGUARD;
        ss << "work helping the city guard.";
    }
    else if (num_girls_on_job(*this, JOB_GROUNDSKEEPER, is_night) < 1)
    {
        target.m_DayJob = target.m_NightJob = JOB_GROUNDSKEEPER;
        ss << "work cleaning the arena.";
    }
    else if (num_girls_on_job(*this, JOB_BLACKSMITH, is_night) < 1)
    {
        target.m_DayJob = target.m_NightJob = JOB_BLACKSMITH;
        ss << "work making weapons and armor.";
    }
    else if (num_girls_on_job(*this, JOB_COBBLER, is_night) < 1)
    {
        target.m_DayJob = target.m_NightJob = JOB_COBBLER;
        ss << "work making shoes and leather items.";
    }
    else if (num_girls_on_job(*this, JOB_JEWELER, is_night) < 1)
    {
        target.m_DayJob = target.m_NightJob = JOB_JEWELER;
        ss << "work making jewelery.";
    }

        // next assign more guards and cleaners if there are a lot of girls to choose from
    else if (target.is_free() && num_girls_on_job(*this, JOB_CITYGUARD, is_night) < num_girls() / 20)
    {
        target.m_DayJob = target.m_NightJob = JOB_CITYGUARD;
        ss << "work helping the city guard.";
    }
    else if (num_girls_on_job(*this, JOB_GROUNDSKEEPER, is_night) < num_girls() / 20)
    {
        target.m_DayJob = target.m_NightJob = JOB_GROUNDSKEEPER;
        ss << "work cleaning the arena.";
    }
    else if (num_girls_on_job(*this, JOB_BLACKSMITH, is_night) < num_girls() / 20)
    {
        target.m_DayJob = target.m_NightJob = JOB_BLACKSMITH;
        ss << "work making weapons and armor.";
    }
    else if (num_girls_on_job(*this, JOB_COBBLER, is_night) < num_girls() / 20)
    {
        target.m_DayJob = target.m_NightJob = JOB_COBBLER;
        ss << "work making shoes and leather items.";
    }
    else if (num_girls_on_job(*this, JOB_JEWELER, is_night) < num_girls() / 20)
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
            num_girls_on_job(*this, JOB_FIGHTBEASTS, is_night) < g_Game->storage().beasts() / 10)
    {
        target.m_DayJob = target.m_NightJob = JOB_FIGHTBEASTS;
        ss << "work fighting beast in the arena.";
    }
        // if there are not enough beasts, have the girls fight other girls
    else if (target.combat() > 60 && num_girls_on_job(*this, JOB_FIGHTARENAGIRLS, is_night) < 1)
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

void sArena::AttractCustomers(IBuildingShift& shift, bool is_night) {
    if(!is_night) return;

    int direct_visitor_bonus = shift.GetResourceAmount(DrawVisitorsId);
    int num_viewers = 2 * (int)std::round(std::log(1 + direct_visitor_bonus));
    num_viewers += g_Dice.closed_uniform(2, 10);

    // Fame-based viewers
    int fame_viewers = 2 * std::sqrt(1 + m_Reputation);
    num_viewers += g_Dice.closed_uniform(fame_viewers / 2, fame_viewers);
    int has_viewers = 0;

    while(has_viewers < num_viewers)
    {
        auto& cust = shift.AttractCustomer();
        cust.m_Amount = g_Dice.closed_uniform(1, 10);
        has_viewers += cust.m_Amount;
    }
}

void sArena::EndShift(bool is_night) {
    if(!is_night) return;
    int entrance_fees = 0;
    bool show_numbers = g_Game->settings().get_bool(settings::USER_SHOW_NUMBERS);
    int num_viewers = 0;
    for(auto& cust : m_Shift->customers()) {
        entrance_fees += 5 * cust->m_Amount;
        num_viewers += cust->m_Amount;
    }
    m_TotalCustomers += num_viewers;
    std::stringstream ss;
    ss << "Today, your Arena drew in " << num_viewers << " customers, who paid " << entrance_fees << " in entrance fees.\n";

    int f_fame = m_Shift->GetResourceAmount(FightsFameId);
    int t_fame = std::sqrt(10 + m_Reputation) + g_Dice.closed_uniform(5, 10);
    int d_fame = f_fame - t_fame;
    if(d_fame < -3) {
        int loss = std::min(m_Reputation, -d_fame / 3);
        ss << "The fights organised today";
        if(show_numbers)
            ss << " (" << f_fame << " entertainment)";
        ss << "were sub-par for what the spectators where expecting (";
        ss << t_fame << ") for an Arena with a reputation of " << m_Reputation << ". ";
        ss << "Consequently, your Arena lost " << loss << " reputation points";
        m_Reputation -= loss;
    } else if (d_fame > 5) {
        m_TotalCustomers += m_Shift->customers().size();
        int gain = std::sqrt(d_fame / 2);
        ss << "The fights organised today provided more entertainment";
        if(show_numbers)
            ss << " (" << f_fame << ")";
        ss << "than what spectators are expecting (";
        ss << t_fame << ") for an Arena with a reputation of " << m_Reputation << ". ";
        ss << "Consequently, your Arena gained " << gain << " reputation points.";
        m_Reputation += gain;
    }

    int num_fights = m_Shift->GetResourceAmount(ArenaFightId);
    auto get_normalized = [&](auto&& id)-> int {
        int value =  m_Shift->GetResourceAmount(id);
        return std::log(1 + value) + value / (1+num_fights);
    };

    int f_brutal = get_normalized(BrutalityId);
    int f_sexual = get_normalized(SexualityId);
    int f_combat = get_normalized(CombatId);
    int f_beauty = get_normalized(BeautyId);

    ss << "\nBrutality: " << f_brutal;
    ss << "\nSexuality: " << f_sexual;
    ss << "\nCombat:    " << f_combat;
    ss << "\nBeauty:    " << f_beauty;

    for(auto& cust : m_Shift->customers()) {
        
    }


    AddMessage(ss.str(), EEventType::EVENT_BUILDING);
}

void sArena::setup_customer(sCustomer& customer) const {
    /*
    RandomSelectorV2<Goals> selector;
    std::mt19937 rng;
    rng.seed(g_Dice.random(10000));
    // Wants to see exciting fighting
    selector.process(rng, GOAL_FIGHT, 0, 30);
    // Wants to see naked skin
    selector.process(rng, GOAL_STRIPSHOW, 0, 10);
    // Wants to see blood and gore
    selector.process(rng, GOAL_RAPE, 0, 10);
    // Wants to see sex
    selector.process(rng, GOAL_SEX, 0, 10);
     */
}

void RegisterArenaJobs(cJobManager& mgr) {
    cGenericJob::Register(mgr, std::make_unique<CityGuard>());
    cGenericJob::Register(mgr, std::make_unique<Medic>());
    cGenericJob::Register(mgr, std::make_unique<FightBeasts>());
    cGenericJob::Register(mgr, std::make_unique<FightGirls>());
    cGenericJob::Register(mgr, std::make_unique<FightTraining>());
    cGenericJob::Register(mgr, std::make_unique<IntermissionStripper>());
    cGenericJob::Register(mgr, std::make_unique<Recuperate>());

    cGenericJob::Register(mgr, std::make_unique<cBlacksmithJob>());
    cGenericJob::Register(mgr, std::make_unique<cCobblerJob>());
    cGenericJob::Register(mgr, std::make_unique<cJewelerJob>());
}
