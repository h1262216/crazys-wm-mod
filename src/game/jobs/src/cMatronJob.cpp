/*
 * Copyright 2009-2023, The Pink Petal Development Team.
 * The Pink Petal Development Team are defined as the game's coders
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

#include "cMatronJob.h"
#include "character/sGirl.h"
#include "character/predicates.h"
#include "buildings/cBuilding.h"
#include "IGame.h"
#include "cInventory.h"
#include "jobs/cJobManager.h"

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

void MatronJob::DoWork(cGirlShift& shift) const {
    auto& ss = shift.data().EventMessage;
    auto& girl = shift.girl();
    bool is_night = shift.is_night_shift();

    ss << m_WorkerTitle << " ${name} ";

    ApplyMatronEffect(shift);

    // Complications
    HandleMatronResult(shift);
    girl.AddMessage(ss.str(), EImageBaseType::PROFILE, is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);

    // Improve girl
    MatronGains(shift);

    shift.generate_event();
}

void MatronJob::MatronGains(cGirlShift& shift) const {
    auto& girl = shift.girl();
    int numgirls = shift.building().num_girls();
    int xp = numgirls / 10;
    girl.exp(xp);

    int stat_sum = girl.get_skill(SKILL_SERVICE) + girl.get_stat(STAT_CHARISMA) + girl.get_stat(STAT_INTELLIGENCE) +
                   girl.get_stat(STAT_CONFIDENCE) + girl.get_skill(SKILL_MEDICINE);
    shift.data().Wages += std::max(0, (stat_sum / 50 + 1) * numgirls);
}

void MatronJob::HandleMatronResult(cGirlShift& shift) const {
    auto& girl = shift.girl();
    auto& ss = shift.data().EventMessage;
    int numgirls = shift.building().num_girls();
    int check = shift.d100();
    if (check < 10 && numgirls >(girl.service() + girl.confidence()) * 3)
    {
        shift.data().Enjoyment -= shift.uniform(5, 10);
        girl.happiness(-10);
        girl.confidence(-5);
        ss << "was overwhelmed by the number of girls she was required to manage and broke down crying.";
    }
    else if (check < 10)
    {
        shift.data().Enjoyment -= shift.uniform(1, 4);
        girl.happiness(-3);
        girl.confidence(-1);
        ss << "had trouble dealing with some of the girls.";
    }
    else if (check > 90)
    {
        shift.data().Enjoyment += shift.uniform(1, 4);
        girl.happiness(3);
        ss << "enjoyed helping the girls with their lives.";
    }
    else
    {
        shift.data().Enjoyment -= shift.uniform(-2, 2);
        ss << "went about her day as usual.";
    }
}

void MatronJob::ApplyMatronEffect(cGirlShift& shift) const {
    auto& brothel = shift.building();
    auto& girl = shift.girl();
    int change;
    int perf = GetPerformance(girl, true);
    if(perf < 70) {
        change = 1;
    } else if (perf < 120) {
        change = 2;
    } else {
        change = 3;
    }
    brothel.girls().apply([&girl, change](sGirl& g){
        // temporarily raise obedience of all other girls
        if(&g == &girl) return;
        g.upd_temp_stat(STAT_OBEDIENCE, change);
    });
}

MatronJob::MatronJob(JOBS job, const char* xml_file, const char* worker_title) : cBasicJob(job, xml_file), m_WorkerTitle(worker_title) {
}

class BrothelMatronJob : public MatronJob {
public:
    using MatronJob::MatronJob;
    void DoWork(cGirlShift& shift) const override;
};


void BrothelMatronJob::DoWork(cGirlShift& shift) const {
    auto& ss = shift.data().EventMessage;
    auto& girl = shift.girl();
    bool is_night = shift.is_night_shift();
    auto& brothel = shift.building();

    girl.m_DayJob = girl.m_NightJob = JOB_MATRON;    // it is a full time job
    if (is_night) return;    // and is only checked once

    ss << "Matron ${name} ";

    ApplyMatronEffect(shift);

    int roll_b = shift.d100();
    EImageBaseType imagetype = EImageBaseType::PROFILE;

    // Complications
    HandleMatronResult(shift);

    //Events
    if (g_Game->gold().ival() > 1000 &&                                // `J` first you need to have enough money to steal
        shift.chance(10 - girl.morality()) &&            // positive morality will rarely steal
        !shift.chance(girl.pclove() + 20) &&                // Love will make her not want to steal
        !shift.chance(girl.obedience()) &&                // if she fails an obedience check
        !shift.chance(girl.pcfear()))                    // Fear may keep her from stealing
    {
        int steal = g_Game->gold().ival() / 1000;
        if (steal > 1000) steal = 1000;
        if (steal < 10) steal = 10;

        if (roll_b < brothel.m_SecurityLevel)
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

    }

    if (is_addict(girl, true) && shift.chance(girl.m_Withdrawals * 20))
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
        else if (cost < girl.m_Money && shift.chance(girl.morality()))        // pay out of pocket
            method = 1;
        else if (cost < g_Game->gold().ival() / 10 && shift.chance(30 - girl.morality()) && !shift.chance(girl.obedience() / 2) && !shift.chance(girl.pcfear() / 2))
            method = 2;
        else method = shift.uniform(3, 7);

        std::stringstream warning;
        sImagePreset warningimage = EImageBaseType::PROFILE;
        switch (method)
        {
            case 1:
                if (!shift.chance(girl.agility()))    // you only get to know about it if she fails an agility check
                {
                    warning << "${name} bought some drugs with her money.\n";
                }
                girl.m_Money -= cost;
                break;
            case 2:
                if (shift.chance(cost / 2))        // chance that you notice the missing money
                {
                    warning << "${name} bought some drugs with some of the brothel's money.\n";
                }
                g_Game->gold().misc_debit(cost);
                break;
            case 3:
                if (!shift.chance(girl.agility()))    // you only get to know about it if she fails an agility check
                {
                    warning << "${name} saw a customer with drugs and offered to fuck him for some. He accepted, so she took him out of sight of security and banged him.\n";
                    warningimage = EImageBaseType::VAGINAL;
                }
                girl.normalsex(1);
                break;
            default:
                if (!shift.chance(girl.agility()))    // you only get to know about it if she fails an agility check
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
        if (shift.chance(50) && girl.has_active_trait(traits::HORRIFIC_SCARS))
        {
            ss << " The customers were disgusted by her horrific scars.";
            //brothel.m_Happiness -= 15;
        }
        else if (shift.chance(50) && girl.has_active_trait(traits::SMALL_SCARS))
        {
            ss << " Some customers were disgusted by her scars.";
            brothel.m_Happiness -= 5;
        }
        else if (shift.chance(50) && girl.has_active_trait(traits::BRUISES))
        {
            ss << " The customers were disgusted by her bruises.";
            brothel.m_Happiness -= 5;
        }

        if (shift.chance(50) && girl.has_active_trait(traits::FUTANARI))
        {
            ss << " The girls and some customers couldn't stop looking at her big cock.";
            brothel.m_Happiness += 2;
        }

        if (shift.chance(50) && girl.breast_size() >= BreastSize::MASSIVE_MELONS)
        {
            ss << " Her enormous, heaving breasts drew a lot of attention from the customers.";
            brothel.m_Happiness += 15;
        }
        else if (shift.chance(50) && (girl.breast_size() >= BreastSize::BIG_BOOBS && girl.breast_size() < BreastSize::MASSIVE_MELONS))
        {
            ss << " Her big, round breasts drew a lot of attention from the customers.";
            brothel.m_Happiness += 10;
        }
        if (shift.chance(50) && (girl.any_active_trait({traits::DELUXE_DERRIERE, traits::GREAT_ARSE})))
        {
            ss << " The customers were hypnotized by the movements of her well shaped butt.";
            brothel.m_Happiness += 15;
        }
        if (shift.chance(50) && (girl.any_active_trait({traits::GREAT_FIGURE, traits::HOURGLASS_FIGURE})))
        {
            ss << " She has such a great figure that the customers couldn't stop looking at her.";
            brothel.m_Happiness += 15;
        }
        if (shift.chance(50) && girl.has_active_trait(traits::SEXY_AIR))
        {
            ss << " She's so sexy that the customers couldn't stop looking at her.";
            //brothel.m_Happiness += 10;
        }
        if (shift.chance(50) && (girl.any_active_trait({traits::PIERCED_NIPPLES, traits::PIERCED_NAVEL, traits::PIERCED_NOSE})))
        {
            ss << " Her piercings catch the eye of some customers.";
            brothel.m_Happiness += 5;
        }
        imagetype = EImageBaseType::ECCHI;
    }

    if (girl.has_active_trait(traits::OPTIMIST) && roll_b < girl.happiness() / 2) // 50% chance at best
    {
        ss << "\n \nWorking with someone as cheerful as ${name} makes everybody a bit happier.";
        brothel.update_all_girls_stat(STAT_HAPPINESS, 1);
    }

    if (girl.has_active_trait(traits::PESSIMIST) && roll_b > 50 + girl.happiness() / 2) // 50% chance at worst
    {
        ss << "\n \nWorking with someone as pessimistic as ${name} makes everybody a little bit sadder.";
        brothel.update_all_girls_stat(STAT_HAPPINESS, -1);
    }

    girl.AddMessage(ss.str(), imagetype, is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);
}

void RegisterManagerJobs(cJobManager& mgr) {
    mgr.register_job(std::make_unique<BrothelMatronJob>(JOB_MATRON, "Matron.xml", "Matron"));
    mgr.register_job(std::make_unique<MatronJob>(JOB_CHAIRMAN, "ClinicChairman.xml", "Clinic Chairman"));
    mgr.register_job(std::make_unique<MatronJob>(JOB_CENTREMANAGER, "CentreManager.xml", "Centre Manager"));
    mgr.register_job(std::make_unique<MatronJob>(JOB_DOCTORE, "ArenaDoctore.xml", "Doctore"));
    mgr.register_job(std::make_unique<MatronJob>(JOB_FARMMANGER, "FarmManager.xml", "Farm Manager"));
    mgr.register_job(std::make_unique<MatronJob>(JOB_HEADGIRL, "HeadGirl.xml", "Head Girl"));
    mgr.register_job(std::make_unique<MatronJob>(JOB_EXECUTIVE, "Executive.xml", "Executive"));
}
