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

#include "cGenericJob.h"
#include <sstream>
#include <cGirls.h>
#include "character/sGirl.h"
#include "character/predicates.h"
#include "buildings/cBuilding.h"
#include "IGame.h"
#include "cInventory.h"
#include "buildings/queries.h"
#include "cJobManager.h"

class MatronJob : public cGenericJob {
public:
    MatronJob(JOBS job, const char* WorkerTitle, const char* short_name, const char* description) :
        cGenericJob(job), m_WorkerTitle(WorkerTitle) {
        m_Info.Description = description;
        m_Info.ShortName = short_name;
        m_Info.Shift = EJobShift::FULL;
        m_Info.FreeOnly = true;
    }
    double GetPerformance(const sGirl& girl, bool estimate) const override;
    void DoWork(sGirlShiftData& shift) const override;
protected:
    int MatronGains(sGirl& girl, bool Day0Night1, int conf) const;
    void HandleMatronResult(sGirl& girl, int &conf) const;
    bool CheckCanWork(sGirl& girl) const override {
        return true;
    }
    bool CheckRefuseWork(sGirl& girl) const override {
        return false;
    }
    void ApplyMatronEffect(const sGirl& girl) const;

    const char* m_WorkerTitle;
};

class BrothelMatronJob : public MatronJob {
public:
    using MatronJob::MatronJob;
    void DoWork(sGirlShiftData& shift) const override;
};

double MatronJob::GetPerformance(const sGirl& girl, bool estimate) const {
    double jobperformance = 0.0;
    if (estimate)    // for third detail string
    {
        // `J` estimate - needs work
        jobperformance = ((girl.charisma() + girl.confidence() + girl.spirit()) / 3) +
                         ((girl.service() + girl.intelligence() + girl.medicine()) / 3) +
                         girl.level(); // maybe add obedience?

        if (girl.is_slave()) jobperformance -= 1000;

        //traits, commented for now

        //good traits
        //if (girl.has_trait(traits::CHARISMATIC)) jobperformance += 20;
        //if (girl.has_trait(traits::COOL_PERSON)) jobperformance += 5;
        //if (girl.has_trait(traits::PSYCHIC)) jobperformance += 10;
        //if (girl.has_trait(traits::TEACHER)) jobperformance += 10;

        //bad traits
        //if (girl.has_trait(traits::DEPENDENT)) jobperformance -= 50;
        //if (girl.has_trait(traits::MIND_FUCKED)) jobperformance -= 50;
        //if (girl.has_trait(traits::RETARDED)) jobperformance -= 60;
        //if (girl.has_trait(traits::BIMBO)) jobperformance -= 10;
        //if (girl.has_trait(traits::SMOKER)) jobperformance -= 10;
        //if (girl.has_trait(traits::ALCOHOLIC)) jobperformance -= 25;
        //if (girl.has_trait(traits::FAIRY_DUST_ADDICT)) jobperformance -= 50;
        //if (girl.has_trait(traits::SHROUD_ADDICT)) jobperformance -= 50;
        //if (girl.has_trait(traits::VIRAS_BLOOD_ADDICT)) jobperformance -= 50;
    }
    else            // for the actual check        // not used
    {

    }

    return jobperformance;
}

void MatronJob::DoWork(sGirlShiftData& shift) const {
    auto& ss = active_shift().EventMessage;
    auto& girl = shift.girl();
    bool is_night = shift.IsNightShift;
    // DisobeyCheck is done in the building flow.
    girl.m_DayJob = girl.m_NightJob = job();    // it is a full time job

    ss << m_WorkerTitle << " ${name} ";

    int conf = 0;

    ApplyMatronEffect(girl);

    // Complications
    HandleMatronResult(girl, conf);
    girl.AddMessage(ss.str(), EImageBaseType::PROFILE, is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);

    // Improve girl
    int wages = MatronGains(girl, is_night, conf);
    shift.Wages = wages;
}

int MatronJob::MatronGains(sGirl& girl, bool Day0Night1,  int conf) const {
    int numgirls = num_girls(*girl.m_Building);
    int xp = numgirls / 10,  skill = 3;

    if (girl.has_active_trait(traits::QUICK_LEARNER))        { skill += 1; xp += 5; }
    else if (girl.has_active_trait(traits::SLOW_LEARNER))    { skill -= 1; xp -= 5; }
    int stat_sum = girl.get_skill(SKILL_SERVICE) + girl.get_stat(STAT_CHARISMA) + girl.get_stat(STAT_INTELLIGENCE) +
                   girl.get_stat(STAT_CONFIDENCE) + girl.get_skill(SKILL_MEDICINE);
    int wages = int((100.f + (stat_sum / 50.f + 1) * numgirls));

    if (conf>-1) conf += uniform(0, skill);
    girl.confidence(conf);

    girl.exp(uniform(5, 5+xp));
    girl.medicine(uniform(0, skill));
    girl.service(uniform(2, skill + 2));

    cGirls::PossiblyGainNewTrait(girl, traits::CHARISMATIC, 30, ACTION_WORKMATRON, "She has worked as a matron long enough that she has learned to be more Charismatic.", Day0Night1);
    cGirls::PossiblyGainNewTrait(girl, traits::PSYCHIC, 60, ACTION_WORKMATRON, "She has learned to handle the girls so well that you'd almost think she was Psychic.", Day0Night1);

    return std::max(0, wages);
}

void MatronJob::HandleMatronResult(sGirl& girl, int &conf) const {
    auto& ss = active_shift().EventMessage;
    int numgirls = num_girls(*girl.m_Building);
    int check = d100();
    if (check < 10 && numgirls >(girl.service() + girl.confidence()) * 3)
    {
        girl.upd_Enjoyment(ACTION_WORKMATRON, -uniform(5, 10));
        conf -= 5;
        girl.happiness(-10);
        ss << "was overwhelmed by the number of girls she was required to manage and broke down crying.";
    }
    else if (check < 10)
    {
        girl.upd_Enjoyment(ACTION_WORKMATRON, -uniform(1, 4));
        conf -= -1;
        girl.happiness(-3);
        ss << "had trouble dealing with some of the girls.";
    }
    else if (check > 90)
    {
        girl.upd_Enjoyment(ACTION_WORKMATRON, uniform(1, 4));
        conf += 1;
        girl.happiness(3);
        ss << "enjoyed helping the girls with their lives.";
    }
    else
    {
        girl.upd_Enjoyment(ACTION_WORKMATRON, -uniform(-1, 1));
        ss << "went about her day as usual.";
    }
}

void MatronJob::ApplyMatronEffect(const sGirl& girl) const {
    auto brothel = girl.m_Building;
    int change;
    int perf = GetPerformance(girl, true);
    if(perf < 70) {
        change = 1;
    } else if (perf < 120) {
        change = 2;
    } else {
        change = 3;
    }
    brothel->girls().apply([&girl, change](sGirl& g){
        // temporarily raise obedience of all other girls
        if(&g == &girl) return;
        g.upd_temp_stat(STAT_OBEDIENCE, change);
    });
}

void BrothelMatronJob::DoWork(sGirlShiftData& shift) const {
    auto& ss = active_shift().EventMessage;
    auto& girl = shift.girl();
    bool is_night = shift.IsNightShift;

    Action_Types actiontype = ACTION_WORKMATRON;
    girl.m_DayJob = girl.m_NightJob = JOB_MATRON;    // it is a full time job
    if (is_night) return;    // and is only checked once

    ss << "Matron ${name} ";

    ApplyMatronEffect(girl);

    // `J` zzzzzz - this needs to be updated for building flow
    if (girl.disobey_check(actiontype, JOB_MATRON))
    {
        ss << "refused to work during the " << (is_night ? "night" : "day") << " shift.";
        girl.AddMessage(ss.str(), EImageBaseType::PROFILE, EVENT_NOWORK);
        shift.Refused = ECheckWorkResult::REFUSES;
        return;
    }

    int conf = 0;
    int roll_b = d100();
    EImageBaseType imagetype = EImageBaseType::PROFILE;

    // Complications
    HandleMatronResult(girl, conf);

    //Events
    if (g_Game->gold().ival() > 1000 &&                                // `J` first you need to have enough money to steal
        chance(10 - girl.morality()) &&            // positive morality will rarely steal
        !chance(girl.pclove() + 20) &&                // Love will make her not want to steal
        !chance(girl.obedience()) &&                // if she fails an obedience check
        !chance(girl.pcfear()))                    // Fear may keep her from stealing
    {
        int steal = g_Game->gold().ival() / 1000;
        if (steal > 1000) steal = 1000;
        if (steal < 10) steal = 10;
/*
        if (roll_b < brothel->m_SecurityLevel)
        {
            std::stringstream warning;
            warning << "Your security spotted ${name} trying to take " << steal << " gold from the Brothel for herself.\n";
            ss << "\n" << warning.str() << "\n";
            girl.AddMessage(warning.str(), EImageBaseType::PROFILE, EVENT_WARNING);
        }
        else
        {
            g_Game->gold().misc_debit(steal);
            girl.m_Money += steal;        // goes directly into her pocket
        }
*/
    }

    if (is_addict(girl, true) && chance(girl.m_Withdrawals * 20))
    {
        int cost = 0;
        int method = 0;    // 1 = out of pocket, 2 = brothel money, 3 = sex, 4 = bj
        // 'Mute' Added so if the cost of the item changes then the gold amout will be correct
        const sInventoryItem* item = nullptr;
        std::string itemName;
        /* */if (girl.has_active_trait(traits::VIRAS_BLOOD_ADDICT))    { itemName = "Vira Blood"; }
        else if (girl.has_active_trait(traits::SHROUD_ADDICT))        { itemName = "Shroud Mushroom"; }
        else if (girl.has_active_trait(traits::FAIRY_DUST_ADDICT))    { itemName = "Fairy Dust"; }
        if (!itemName.empty())        item = g_Game->inventory_manager().GetItem(itemName);
        if (item)
        {
            cost += item->m_Cost;
            girl.add_item(item);
        }
        // 'Mute' End  Change
        if (girl.has_active_trait(traits::CUM_ADDICT))
            method = 4;
        else if (girl.has_active_trait(traits::NYMPHOMANIAC) && girl.lust() > 50)
            method = 3;
        else if (cost < girl.m_Money && chance(girl.morality()))        // pay out of pocket
            method = 1;
        else if (cost < g_Game->gold().ival() / 10 && chance(30 - girl.morality()) && !chance(girl.obedience() / 2) && !chance(girl.pcfear() / 2))
            method = 2;
        else method = uniform(3, 7);

        std::stringstream warning;
        sImagePreset warningimage = EImageBaseType::PROFILE;
        switch (method)
        {
            case 1:
                if (!chance(girl.agility()))    // you only get to know about it if she fails an agility check
                {
                    warning << "${name} bought some drugs with her money.\n";
                }
                girl.m_Money -= cost;
                break;
            case 2:
                if (chance(cost / 2))        // chance that you notice the missing money
                {
                    warning << "${name} bought some drugs with some of the brothel's money.\n";
                }
                g_Game->gold().misc_debit(cost);
                break;
            case 3:
                if (!chance(girl.agility()))    // you only get to know about it if she fails an agility check
                {
                    warning << "${name} saw a customer with drugs and offered to fuck him for some. He accepted, so she took him out of sight of security and banged him.\n";
                    warningimage = EImageBaseType::VAGINAL;
                }
                girl.normalsex(1);
                break;
            default:
                if (!chance(girl.agility()))    // you only get to know about it if she fails an agility check
                {
                    warning << "${name} saw a customer with drugs and offered to give him a blowjob for some. He accepted, so she took him out of sight of security and sucked him off.\n";
                    warningimage = EImagePresets::BLOWJOB;
                }
                girl.oralsex(1);
                break;
        }
        if (warning.str().length() > 0)
        {
            ss << "\n" << warning.str() << "\n";
            girl.AddMessage(warning.str(), warningimage, EVENT_WARNING);
        }
    }

    if (girl.has_active_trait(traits::EXHIBITIONIST))
    {
        ss << "\n \nShe hung out in the brothel wearing barely anything.";
        if (chance(50) && girl.has_active_trait(traits::HORRIFIC_SCARS))
        {
            ss << " The customers were disgusted by her horrific scars.";
            //brothel->m_Happiness -= 15;
        }
        else if (chance(50) && girl.has_active_trait(traits::SMALL_SCARS))
        {
            ss << " Some customers were disgusted by her scars.";
            //brothel->m_Happiness -= 5;
        }
        else if (chance(50) && girl.has_active_trait(traits::BRUISES))
        {
            ss << " The customers were disgusted by her bruises.";
            //brothel->m_Happiness -= 5;
        }

        if (chance(50) && girl.has_active_trait(traits::FUTANARI))
        {
            ss << " The girls and some customers couldn't stop looking at her big cock.";
            //brothel->m_Happiness += 2;
        }

        if (chance(50) && girl.breast_size() >= BreastSize::MASSIVE_MELONS)
        {
            ss << " Her enormous, heaving breasts drew a lot of attention from the customers.";
            //brothel->m_Happiness += 15;
        }
        else if (chance(50) && (girl.breast_size() >= BreastSize::BIG_BOOBS && girl.breast_size() < BreastSize::MASSIVE_MELONS))
        {
            ss << " Her big, round breasts drew a lot of attention from the customers.";
            //brothel->m_Happiness += 10;
        }
        if (chance(50) && (girl.any_active_trait({traits::DELUXE_DERRIERE, traits::GREAT_ARSE})))
        {
            ss << " The customers were hypnotized by the movements of her well shaped butt.";
            //brothel->m_Happiness += 15;
        }
        if (chance(50) && (girl.any_active_trait({traits::GREAT_FIGURE, traits::HOURGLASS_FIGURE})))
        {
            ss << " She has such a great figure that the customers couldn't stop looking at her.";
            //brothel->m_Happiness += 15;
        }
        if (chance(50) && girl.has_active_trait(traits::SEXY_AIR))
        {
            ss << " She's so sexy that the customers couldn't stop looking at her.";
            //brothel->m_Happiness += 10;
        }
        if (chance(50) && (girl.any_active_trait({traits::PIERCED_NIPPLES, traits::PIERCED_NAVEL, traits::PIERCED_NOSE})))
        {
            ss << " Her piercings catch the eye of some customers.";
            //brothel->m_Happiness += 5;
        }
        imagetype = EImageBaseType::ECCHI;
    }

    if (girl.has_active_trait(traits::OPTIMIST) && roll_b < girl.happiness() / 2) // 50% chance at best
    {
        ss << "\n \nWorking with someone as cheerful as ${name} makes everybody a bit happier.";
        // girl.m_Building->update_all_girls_stat(STAT_HAPPINESS, 1);
    }

    if (girl.has_active_trait(traits::PESSIMIST) && roll_b > 50 + girl.happiness() / 2) // 50% chance at worst
    {
        ss << "\n \nWorking with someone as pessimistic as ${name} makes everybody a little bit sadder.";
        // girl.m_Building->update_all_girls_stat(STAT_HAPPINESS, -1);
    }

    girl.AddMessage(ss.str(), imagetype, is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);

    // Improve girl
    shift.Wages = MatronGains(girl, is_night, conf);
}

void RegisterManagerJobs(cJobManager& mgr) {
    mgr.register_job(std::make_unique<BrothelMatronJob>(JOB_MATRON, "Matron", "Mtrn", "This girl will look after the other girls. Only non-slave girls can have this position and you must pay them 300 gold per week. Also, it takes up both shifts. (max 1)"));
    mgr.register_job(std::make_unique<MatronJob>(JOB_CHAIRMAN, "Clinic Chairman", "Crmn", "She will watch over the staff of the clinic"));
    mgr.register_job(std::make_unique<MatronJob>(JOB_CENTREMANAGER, "Centre Manager", "CMgr", "She will look after the girls working in the centre."));
    mgr.register_job(std::make_unique<MatronJob>(JOB_DOCTORE, "Doctore", "Dtre", "She will watch over the girls in the arena."));
    mgr.register_job(std::make_unique<MatronJob>(JOB_FARMMANGER, "Farm Manager", "FMgr", "She will watch over the farm and girls working there."));
    mgr.register_job(std::make_unique<MatronJob>(JOB_HEADGIRL, "Head Girl", "HGrl", "She takes care of the girls in your house."));
    mgr.register_job(std::make_unique<MatronJob>(JOB_EXECUTIVE, "Executive", "Exec", "She takes care of the girls in your studio."));
}
