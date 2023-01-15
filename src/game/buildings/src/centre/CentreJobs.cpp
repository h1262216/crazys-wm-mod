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

#include "jobs/SimpleJob.h"
#include "character/sGirl.h"
#include "character/cCustomers.h"
#include "character/predicates.h"
#include "buildings/cBuilding.h"
#include "buildings/IBuildingShift.h"
#include "cGirls.h"
#include "IGame.h"
#include "character/cPlayer.h"
#include "character/lust.h"
#include "queries.h"
#include "jobs/cJobManager.h"

extern const char* const CounselingInteractionId;

namespace {
    class CommunityService: public cSimpleJob {
    public:
        CommunityService();
        void JobProcessing(sGirl& girl, sGirlShiftData& shift) const override;
    };

    class FeedPoor : public cSimpleJob {
    public:
        FeedPoor();
        void JobProcessing(sGirl& girl, sGirlShiftData& shift) const override;
    };

    class Counselor : public cSimpleJob {
    public:
        Counselor();
        void JobProcessing(sGirl& girl, sGirlShiftData& shift) const override;
    };
}

CommunityService::CommunityService() : cSimpleJob(JOB_COMUNITYSERVICE, "CommunityService.xml") {
    m_Info.BaseAction = ACTION_WORKCENTRE;
}

void CommunityService::JobProcessing(sGirl& girl, sGirlShiftData& shift) const {
    auto& ss = active_shift().EventMessage;
    bool blow = false, sex = false;
    int fame = 0;
    auto msgtype = shift.IsNightShift ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT;

    //Adding cust here for use in scripts...
    // sCustomer Cust = cJobManager::GetMiscCustomer(brothel);

    // `J` merged slave/free messages and moved actual dispo change to after
    add_performance_text();
    int changes[] = {2, 4, 5, 8, 10, 12};
    int dispo = changes[get_performance_class(shift.Performance)];

    //try and add randomness here
    if (!is_virgin(girl) && check_public_sex(girl, ESexParticipants::HETERO, SKILL_NORMALSEX, sPercent{30}, true))
    {
        sex = true;
        ss << "Her Lust got the better of her today and she decided the best way to serve her community was on her back!\n \n";
    }

    if (chance(30) && girl.intelligence() < 55)//didn't put a check on this one as we could use some randomness and it's an intel check... guess we can if people keep bitching
    {
        blow = true;
        ss << "An elderly fellow managed to convince ${name} that the best way to serve her community was on her knees. She ended up giving him a blow job!\n \n";
    }

    int roll_b = d100();
    //enjoyed the work or not
    shift_enjoyment();

    if (sex)
    {
        if (girl.is_sex_type_allowed(SKILL_NORMALSEX) && (roll_b <= 50 || girl.is_sex_type_allowed(SKILL_ANAL))) //Tweak to avoid an issue when roll > 50 && anal is restricted
        {
            girl.normalsex(2);
            shift.EventImage = EImageBaseType::VAGINAL;
            if (girl.lose_trait(traits::VIRGIN))
            {
                ss << "\nShe is no longer a virgin.\n";
            }
            /*if (!girl.calc_pregnancy(Cust, 1.0))
            {
                g_Game->push_message(girl.FullName() + " has gotten pregnant", 0);
            }*/
        }
        else if (girl.is_sex_type_allowed(SKILL_ANAL))
        {
            girl.anal(2);
            shift.EventImage = EImageBaseType::ANAL;
        }
        //brothel.m_Happiness += 100;
        girl.lust_release_regular();
        girl.upd_Enjoyment(ACTION_SEX, +3);
        fame += 1;
        dispo += 6;
    }
    else if (blow)
    {
        //brothel.m_Happiness += uniform(60, 130);
        dispo += 4;
        girl.oralsex(2);
        fame += 1;
        shift.EventImage = EImagePresets::BLOWJOB;
    }

    if (girl.is_slave())
    {
        ss << "\nThe fact that she is your slave makes people think its less of a good deed on your part.";
        shift.Wages = 0;
    }
    else
    {
        ss << "\nThe fact that your paying this girl to do this helps people think your a better person.";
        g_Game->gold().staff_wages(shift.Wages);  // wages come from you
        dispo = int(dispo*1.5);
    }

    g_Game->player().disposition(dispo);

    int help = shift.Performance / 10;        //  1 helped per 10 point of performance

    ss.str("");
    ss << "${name} helped " << help << " people today.";
    girl.AddMessage(ss.str(), EImageBaseType::PROFILE, shift.IsNightShift ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);

    // Improve stats
    HandleGains(girl, fame);
}

FeedPoor::FeedPoor() : cSimpleJob(JOB_FEEDPOOR, "FeedPoor.xml") {
    m_Info.BaseAction = ACTION_WORKCENTRE;
}

void FeedPoor::JobProcessing(sGirl& girl, sGirlShiftData& shift) const {
    auto& ss = active_shift().EventMessage;
    bool blow = false, sex = false;
    int feed = 0, fame = 0;
    int roll_b = d100();

    //Adding cust here for use in scripts...
    // sCustomer Cust = cJobManager::GetMiscCustomer(brothel);

    add_performance_text();
    int changes[] = {2, 4, 5, 8, 10, 12};
    int dispo = changes[get_performance_class(shift.Performance)];

    //try and add randomness here
    if (girl.intelligence() < 55 && chance(30))//didn't put a check on this one as we could use some randomness and its an intel check... guess we can if people keep bitching
    {
        blow = true;    ss << "An elderly fellow managed to convince ${name} that he was full and didn't need anymore food but that she did. He told her his cock gave a special treat if she would suck on it long enough. Which she did man she isn't very smart.\n \n";
    }

    if (!is_virgin(girl) && check_public_sex(girl, ESexParticipants::HETERO, SKILL_NORMALSEX, sPercent{30}, true))
    {
        sex = true;
        ss << "Her Lust got the better of her today and she decided to let them eat her pussy!  After a few minutes they started fucking her.\n";
    }

    if (girl.is_slave())
    {
        ss << "\nThe fact that she is your slave makes people think its less of a good deed on your part.";
        g_Game->player().disposition(dispo);
        shift.Wages = 0;
    }
    else
    {
        ss << "\nThe fact that your paying this girl to do this helps people think your a better person.";
        g_Game->gold().staff_wages(shift.Wages);  // wages come from you
        g_Game->player().disposition(int(dispo*1.5));
    }

    shift_enjoyment();

    if (sex)
    {
        if (girl.is_sex_type_allowed(SKILL_NORMALSEX) && (roll_b <= 50 || girl.is_sex_type_allowed(SKILL_ANAL))) //Tweak to avoid an issue when roll > 50 && anal is restricted
        {
            shift.EventImage = EImageBaseType::VAGINAL;
            girl.normalsex(2);
            if (girl.lose_trait(traits::VIRGIN))
            {
                ss << "She is no longer a virgin.\n";
            }
            /*if (!girl.calc_pregnancy(Cust, 1.0))
            {
                g_Game->push_message(girl.FullName() + " has gotten pregnant", 0);
            }*/
        }
        else if (girl.is_sex_type_allowed(SKILL_ANAL))
        {
            shift.EventImage = EImageBaseType::ANAL;
            girl.anal(2);
        }
        //brothel.m_Happiness += 100;
        girl.lust_release_regular();
        girl.upd_Enjoyment(ACTION_SEX, +3);
        fame += 1;
        dispo += 6;
    }
    else if (blow)
    {
        //brothel.m_Happiness += uniform(60, 130);
        dispo += 4;
        girl.oralsex(2);
        fame += 1;
        shift.EventImage = EImagePresets::BLOWJOB;
    }
    else
    {
        shift.EventImage = EImageBaseType::PROFILE;
    }

    feed += shift.Performance / 10;        //  1 feed per 10 point of performance

    int cost = 0;
    for (int i = 0; i < feed; i++)
    {
        cost += uniform(2, 5); // 2-5 gold per customer
    }
    shift.building().Finance().centre_costs(cost);
    std::stringstream summary;
    summary << "${name} feed " << feed << " costing you " << cost << " gold.";
    girl.AddMessage(summary.str(), EImageBaseType::PROFILE, EVENT_SUMMARY);

    HandleGains(girl, fame);
}

Counselor::Counselor() : cSimpleJob(JOB_COUNSELOR, "Counselor.xml") {
    m_Info.BaseAction = ACTION_WORKCOUNSELOR;
}

void Counselor::JobProcessing(sGirl& girl, sGirlShiftData& shift) const {
    auto& ss = active_shift().EventMessage;
    int roll_a = d100();

    if (roll_a <= 10)       { m_Enjoyment -= uniform(1, 3);    ss << "The addicts hassled her."; }
    else if (roll_a >= 90)  { m_Enjoyment += uniform(1, 3);    ss << "She had a pleasant time working."; }
    else                    { m_Enjoyment += uniform(0, 1);    ss << "Otherwise, the shift passed uneventfully."; }

    girl.AddMessage(ss.str(), EImageBaseType::TEACHER, shift.IsNightShift ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);

    int rehabers = num_girls_on_job(shift.building(), JOB_REHAB, shift.IsNightShift);
    // work out the pay between the house and the girl
    int roll_max = (girl.spirit() + girl.intelligence()) / 4;
    shift.Wages += uniform(10, 10 + roll_max);
    shift.Wages += 5 * rehabers;    // `J` pay her 5 for each patient you send to her
    provide_interaction(CounselingInteractionId, 2);

    HandleGains(girl, 0);
}

void RegisterCentreJobs(cJobManager& mgr) {
    mgr.register_job(std::make_unique<CommunityService>());
    mgr.register_job(std::make_unique<FeedPoor>());
    mgr.register_job(std::make_unique<Counselor>());
}