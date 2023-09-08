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

#include "jobs/cSimpleJob.h"
#include "character/sGirl.h"
#include "character/cCustomers.h"
#include "character/predicates.h"
#include "cGirls.h"
#include "buildings/cBuilding.h"
#include "IGame.h"
#include "jobs/cJobManager.h"

extern const char* const CarePointsBasicId;
extern const char* const CarePointsGoodId;
extern const char* const DoctorInteractionId;

struct DoctorJob : public cSimpleJob {
    DoctorJob();
    void JobProcessing(sGirl& girl, cGirlShift& shift) const override;
    sJobValidResult IsJobValid(const sGirl& girl, bool night_shift) const override;
};

DoctorJob::DoctorJob() : cSimpleJob(JOB_DOCTOR, "Doctor.xml") {
    m_Info.Provides.emplace_back(DoctorInteractionId);
}

sJobValidResult DoctorJob::IsJobValid(const sGirl& girl, bool night_shift) const {
    if (girl.has_active_trait(traits::AIDS)) {
        return {false, "Health laws prohibit anyone with AIDS from working in the Medical profession"};
    }

    if (girl.medicine() < 50 || girl.intelligence() < 50) {
        return {false, "${name} does not have enough training to work as a Doctor."};
    }

    return {true, ""};
}

void DoctorJob::JobProcessing(sGirl& girl, cGirlShift& shift) const {
    // this will be added to the clinic's code eventually - for now it is just used for her pay
    shift.provide_interaction(DoctorInteractionId, 1);

    // enjoyed the work or not
    int roll = shift.d100();
    if (roll <= 10)
    {
        shift.data().Enjoyment -= shift.uniform(1, 3);
        shift.data().Performance *= 0.9;
        shift.add_literal("Some of the patients abused her during the shift.\n");
    }
    else if (roll >= 90)
    {
        shift.data().Enjoyment += shift.uniform(1, 3);
        shift.data().Performance *= 1.1;
        shift.add_literal("She had a pleasant time working.\n");
    }
    else
    {
        shift.data().Enjoyment += shift.uniform(0, 1);
        shift.add_literal("Otherwise, the shift passed uneventfully.\n");
    }
    // TODO figure out external patients
}

struct NurseJob : public cSimpleJob {
    NurseJob();
    void JobProcessing(sGirl& girl, cGirlShift& shift) const override;
    sJobValidResult IsJobValid(const sGirl& girl, bool night_shift) const override;
};

sJobValidResult NurseJob::IsJobValid(const sGirl& girl, bool night_shift) const {
    if (girl.has_active_trait(traits::AIDS)) {
        return {false, "Health laws prohibit anyone with AIDS from working in the Medical profession"};
    }

    return {true, ""};
}

NurseJob::NurseJob() : cSimpleJob(JOB_NURSE, "Nurse.xml") {
    m_Info.Provides.emplace_back(CarePointsBasicId);
    m_Info.Provides.emplace_back(CarePointsGoodId);
}

void NurseJob::JobProcessing(sGirl& girl, cGirlShift& shift) const {
    int roll_a = shift.d100(), roll_b = shift.d100();
    int fame = 0;
    bool hand = false, sex = false, les = false;
    auto& ss = shift.data().EventMessage;

    // this will be added to the clinic's code eventually - for now it is just used for her pay
    int patients = 0;            // `J` how many patients the Doctor can see in a shift

    //Adding cust here for use in scripts...
    sCustomer Cust = cJobManager::GetMiscCustomer(shift.building());

    add_performance_text(shift);
    shift.data().Wages += (int)m_PerformanceToEarnings((float)shift.data().Performance);
    int basic_care = performance_based_lookup(shift, 4, 6, 8, 12, 16, 20);
    int quality_care = performance_based_lookup(shift, 0, 0, 0, 2, 6, 10);

    shift.provide_resource(CarePointsBasicId, basic_care);
    shift.provide_resource(CarePointsGoodId, quality_care);

    //try and add randomness here
    if (girl.beauty() > 85 && shift.chance(20))
    {
        shift.data().Tips += 25;
        ss << "Stunned by her beauty a customer left her a great tip.\n";
    }

    if (girl.has_active_trait(traits::CLUMSY) && shift.chance(20))
    {
        shift.data().Wages -= 15;
        ss << "Her clumsy nature caused her to spill some medicine everywhere.\n";
    }

    if (girl.has_active_trait(traits::PESSIMIST) && shift.chance(5))
    {
        if (shift.data().Performance < 125)
        {
            shift.data().Wages -= 10;
            ss << "Her pessimistic mood depressed the patients making them tip less.\n";
        }
        else
        {
            shift.data().Tips += 10;
            ss << "${name} was in a poor mood so the patients gave her a bigger tip to try and cheer her up.\n";
        }
    }

    if (girl.has_active_trait(traits::OPTIMIST) && shift.chance(20))
    {
        if (shift.data().Performance < 125)
        {
            shift.data().Wages -= 10;
            ss << "${name} was in a cheerful mood but the patients thought she needed to work more on her services.\n";
        }
        else
        {
            shift.data().Tips += 10;
            ss << "Her optimistic mood made patients cheer up increasing the amount they tip.\n";
        }
    }

    if (girl.intelligence() < 45 && shift.chance(30))//didnt put a check on this one as we could use some randomness and its an intel check... guess we can if people keep bitching
    {
        hand = true;
        ss << "An elderly fellow managed to convince ${name} that her touch can heal! She ended up giving him a hand job!\n";
    }

    if (shift.chance(30) && !is_virgin(girl) && likes_men(girl)
        && (is_sex_crazy(girl) || girl.has_active_trait(traits::BIMBO)))
    {
        if (girl.lust() > 65 && (girl.is_sex_type_allowed(SKILL_NORMALSEX) || girl.is_sex_type_allowed(SKILL_ANAL)))
        {
            shift.data().Tips += 50;
            sex = true;
            shift.data().Enjoyment += 1;
            ss << "When giving a sponge bath to one of her male patients she couldn't look away from his enormous manhood. The man took advantage and fucked her brains out!\n";
        }
        else
        {
            ss << "When giving a sponge bath to one of her male patients she couldn't look away from his enormous manhood. But she wasn't in the mood so she left.\n";
        }
    }

    if (likes_women(girl) && girl.has_active_trait(traits::AGGRESSIVE) &&
        girl.lust() > 65 && shift.chance(10))
    {
        les = true;
        shift.data().Enjoyment += 1;
        ss << "When giving a sponge bath to one of her female patients she couldn't help herself and took advantage of the situation.\n";
    }
    ss << "\n";

    //enjoyed the work or not
    if (roll_a <= 5)
    {
        shift.data().Enjoyment -= shift.uniform(1, 3);
        shift.data().Performance *= 0.9;
        ss << "Some of the patrons abused her during the shift.";
    }
    else if (roll_a <= 25)
    {
        shift.data().Enjoyment += shift.uniform(1, 3);
        shift.data().Performance *= 1.1;
        ss << "She had a pleasant time working.";
    }
    else
    {
        shift.data().Enjoyment += shift.uniform(0, 1);
        ss << "Otherwise, the shift passed uneventfully.";
    }

    if (sex)
    {
        if (girl.is_sex_type_allowed(SKILL_NORMALSEX) && (roll_b <= 50 || girl.is_sex_type_allowed(SKILL_ANAL))) //Tweak to avoid an issue when roll > 50 && anal is restricted
        {
            shift.set_image(EImageBaseType::VAGINAL);
            girl.normalsex(2);
            if (girl.lose_trait(traits::VIRGIN))
            {
                ss << "She is no longer a virgin.\n";
            }
            if (!girl.calc_pregnancy(Cust, 1.0))
            {
                g_Game->push_message(girl.FullName() + " has gotten pregnant", 0);
            }
        }
        else if (girl.is_sex_type_allowed(SKILL_ANAL))
        {
            shift.set_image(EImageBaseType::ANAL);
            girl.anal(2);
        }
        shift.building().m_Happiness += 100;
        girl.lust_release_regular();
        girl.enjoyment(EActivity::FUCKING, +3);
    }
    else if (hand)
    {
        shift.building().m_Happiness += shift.uniform(60, 130);
        girl.handjob(2);
        shift.set_image(EImageBaseType::HAND);
    }
    else if (les)
    {
        shift.building().m_Happiness += shift.uniform(30, 100);
        shift.set_image(EImagePresets::LESBIAN);
        girl.lesbian(2);
    }

    if (girl.is_unpaid())
    {
        shift.data().Performance *= 0.9;
        patients += (int)(shift.data().Performance / 5);        // `J` 1 patient per 5 point of performance
    }
    else
    {
        patients += (int)(shift.data().Performance / 5);        // `J` 1 patient per 5 point of performance
        shift.data().Wages += patients * 2;                // `J` pay her 2 for each patient you send to her
    }

    shift.generate_event();
    /* `J` this will be a placeholder until a better payment system gets done
    *  this does not take into account any of your girls in surgery
    */
    shift.data().Earnings = 0;
    for (int i = 0; i < patients; i++)
    {
        shift.data().Earnings += shift.uniform(5, 40); // 5-40 gold per customer
    }
    shift.building().m_Finance.clinic_income(shift.data().Earnings);
    ss.str("");    ss << "${name} earned " << shift.data().Earnings << " gold from taking care of " << patients << " patients.";
    girl.AddMessage(ss.str(), EImageBaseType::PROFILE, shift.is_night_shift() ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);
}

struct InternJob : public cBasicJob {
    InternJob();
    void DoWork(cGirlShift&) const override;
    double GetPerformance(const sGirl& girl, bool estimate) const override;
    sJobValidResult IsJobValid(const sGirl& girl, bool night_shift) const override;
};

InternJob::InternJob() : cBasicJob(JOB_INTERN, "Intern.xml") {
    m_Info.Provides.emplace_back(CarePointsBasicId);
}

sJobValidResult InternJob::IsJobValid(const sGirl& girl, bool night_shift) const {
    if (girl.has_active_trait(traits::AIDS)) {
        return {false, "Health laws prohibit anyone with AIDS from working in the Medical profession"};
    }

    return {true, ""};
}

double InternJob::GetPerformance(const sGirl& girl, bool estimate) const {
    if (estimate)// for third detail string
    {
        double jobperformance =
                (100 - girl.medicine()) +
                (100 - girl.intelligence()) +
                (100 - girl.charisma());

        // traits she could gain/lose
        if (girl.has_active_trait(traits::NERVOUS)) jobperformance += 20;
        if (!girl.has_active_trait(traits::CHARISMATIC)) jobperformance += 20;
        return jobperformance;
    }
    return 0.0;
}

void InternJob::DoWork(cGirlShift& shift) const {
    auto& girl = shift.girl();
    auto& ss = shift.data().EventMessage;
    auto brothel = girl.m_Building;

    shift.add_line("work");

    // less than even a bad nurse, but still something
    shift.provide_resource(CarePointsBasicId, 2);

    int enjoy = 0;                                                //
    int train = 0;                                                // main skill trained
    int tmed = girl.medicine();                                // Starting level - train = 1
    int tint = girl.intelligence();                            // Starting level - train = 2
    int tcha = girl.charisma();                                // Starting level - train = 3
    bool gaintrait = false;                                        // posibility of gaining a trait
    bool promote = false;                                        // posibility of getting promoted to Doctor or Nurse
    int skill = 0;                                                // gian for main skill trained
    int dirtyloss = brothel->m_Filthiness / 100;                // training time wasted with bad equipment
    int sgMed = 0, sgInt = 0, sgCha = 0;                        // gains per skill
    int roll_a = shift.d100();                                    // roll for main skill gain
    int roll_b = shift.d100();                                    // roll for main skill trained
    int roll_c = shift.d100();                                    // roll for enjoyment


    if (roll_a <= 5)          skill = 7;
    else if (roll_a <= 15)    skill = 6;
    else if (roll_a <= 30)    skill = 5;
    else if (roll_a <= 60)    skill = 4;
    else                      skill = 3;
    if (girl.has_active_trait(traits::QUICK_LEARNER))        { skill += 1; }
    else if (girl.has_active_trait(traits::SLOW_LEARNER))    { skill -= 1; }
    skill -= dirtyloss;
    ss << "The Clinic is ";
    if (dirtyloss <= 0) ss << "clean and tidy";
    if (dirtyloss == 1) ss << "dirty and the equipment has not been put back in its place";
    if (dirtyloss == 2) ss << "messy. The equipment is damaged and strewn about the building";
    if (dirtyloss == 3) ss << "filthy and some of the equipment is broken";
    if (dirtyloss >= 4) ss << "in complete disarray and the equipment barely usable";
    ss << ".\n \n";
    if (skill < 1) skill = 1;    // always at least 1

    do{        // `J` New method of selecting what job to do
        /* */if (roll_b < 40  && tmed < 100)    train = 1;    // medicine
        else if (roll_b < 70  && tint < 100)    train = 2;    // intelligence
        else if (roll_b < 100 && tcha < 100)    train = 3;    // charisma
        roll_b -= 10;
    } while (train == 0 && roll_b > 0);
    if (train == 0 || shift.chance(5)) gaintrait = true;
    if (train == 0 && girl.medicine() > 70 && girl.intelligence() > 70)    promote = true;

    if (train == 1) { sgMed = skill; ss << "She learns how to work with medicine better.\n"; }    else sgMed = shift.uniform(0, 2);
    if (train == 2) { sgInt = skill; ss << "She got smarter today.\n"; }                        else sgInt = shift.uniform(0, 1);
    if (train == 3) { sgCha = skill; ss << "She got more charismatic today.\n"; }                else sgCha = shift.uniform(0, 1);

    int trycount = 10;
    while (gaintrait && trycount > 0)    // `J` Try to add a trait
    {
        trycount--;
        switch (shift.uniform(0, 10))
        {
            case 0:
                if (girl.lose_trait( traits::NERVOUS))
                {
                    ss << "She seems to be getting over her Nervousness with her training.";
                    gaintrait = false;
                }
                break;
            case 1:
                if (girl.lose_trait( traits::MEEK))
                {
                    ss << "She seems to be getting over her Meekness with her training.";
                    gaintrait = false;
                }
                break;
            case 2:
                if (girl.lose_trait( traits::DEPENDENT))
                {
                    ss << "She seems to be getting over her Dependancy with her training.";
                    gaintrait = false;
                }
                break;
            case 3:
                if (girl.gain_trait( traits::CHARISMATIC))
                {
                    ss << "Dealing with patients and talking with them about their problems has made ${name} more Charismatic.";
                    gaintrait = false;
                }
                break;
            case 4:
            case 5:
            case 6:
            default:    break;    // no trait gained
        }
    }

    // Improve stats
    int xp = 5 + skill;

    if (girl.has_active_trait(traits::QUICK_LEARNER))        { xp += 2; }
    else if (girl.has_active_trait(traits::SLOW_LEARNER))    { xp -= 2; }

    int exp_start = girl.exp();
    girl.exp(shift.uniform(1, xp));

    auto get_update = [&](StatSkill target, int amount){
        int before = girl.get_attribute(target);
        int after = girl.update_attribute(target, amount);
        return after - before;
    };


    sgMed = get_update(SKILL_MEDICINE, sgMed);
    sgInt = get_update(STAT_INTELLIGENCE, sgInt);
    sgCha = get_update(STAT_CHARISMA, sgCha);
    ss << "She managed to gain:\n";
    if (sgMed > 0) { ss << sgMed << " Medicine.\n";}
    if (sgInt > 0) { ss << sgInt << " Intelligence.\n"; }
    if (sgCha > 0) { ss << sgCha << " Charisma.\n"; }
    ss << girl.exp() - exp_start << " Exp.\n";

    if(shift.chance(50)) {
        girl.progress_trait(traits::DOCTOR, 10);
    }



    //enjoyed the work or not
    if (roll_c <= 10)         { enjoy -= shift.uniform(1, 3);    ss << "Some of the patrons abused her during the shift."; }
    else if (roll_c >= 90)    { enjoy += shift.uniform(1, 3);    ss << "She had a pleasant time working."; }
    else                      { enjoy += shift.uniform(0, 1);    ss << "Otherwise, the shift passed uneventfully."; }

    shift.data().Wages = 25 + (skill * 5); // `J` Pay her more if she learns more

    if (girl.medicine() + girl.intelligence() + girl.charisma() >= 300) promote = true;
    if (promote)
    {
        std::stringstream ss;
        ss << "${name} has completed her Internship and has been promoted to ";
        if (girl.is_slave()) { ss << "Nurse.";    girl.m_DayJob = girl.m_NightJob = JOB_NURSE; }
        else                 { ss << "Doctor.";    girl.m_DayJob = girl.m_NightJob = JOB_DOCTOR; }
        girl.AddMessage(ss.str(), EImageBaseType::PROFILE, EVENT_GOODNEWS);
    }
}

void RegisterClinicJobs(cJobManager& mgr) {
    mgr.register_job(std::make_unique<DoctorJob>());
    mgr.register_job(std::make_unique<NurseJob>());
    mgr.register_job(std::make_unique<InternJob>());
}