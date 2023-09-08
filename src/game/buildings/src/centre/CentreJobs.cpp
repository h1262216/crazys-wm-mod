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

#include "jobs/cSimpleJob.h"
#include "jobs/cJobManager.h"
#include "character/sGirl.h"
#include "character/cCustomers.h"
#include "character/predicates.h"
#include "buildings/cBuilding.h"
#include "cGirls.h"
#include "IGame.h"
#include "character/cPlayer.h"
#include "character/lust.h"

extern const char* const CounselingInteractionId;


namespace {
    class CommunityJob : public cSimpleJob {
    public:
        using cSimpleJob::cSimpleJob;
        void JobProcessing(sGirl& girl, cGirlShift& shift) const override;
        virtual void CommunityWork(cGirlShift& shift) const = 0;

        void SexEvent(cGirlShift& shift) const;
    };
    class CommunityService: public CommunityJob {
    public:
        CommunityService();
        void CommunityWork(cGirlShift& shift) const override;
    };

    class FeedPoor : public CommunityJob {
    public:
        FeedPoor();
        void CommunityWork(cGirlShift& shift) const override;
    };

    class Counselor : public cSimpleJob {
    public:
        Counselor();
        void JobProcessing(sGirl& girl, cGirlShift& shift) const override;
    };
}

void CommunityJob::JobProcessing(sGirl& girl, cGirlShift& shift) const {
    add_performance_text(shift);
    int dispo = performance_based_lookup(shift, 2, 4, 6, 8, 10, 12);

    // try and add randomness here
    if (!is_virgin(girl) && check_public_sex(girl, ESexParticipants::HETERO, SKILL_NORMALSEX, sPercent{30}, true))
    {
        SexEvent(shift);
        shift.girl().fame(1);
        dispo += 6;
    }

    if (shift.chance(30) && girl.intelligence() < 55)//didn't put a check on this one as we could use some randomness and it's an intel check... guess we can if people keep bitching
    {
        shift.add_line("event.blowjob");

        shift.building().m_Happiness += shift.uniform(60, 130);
        girl.oralsex(2);
        shift.set_image(EImagePresets::BLOWJOB);
        shift.girl().fame(1);
        dispo += 4;
    }

    // enjoyed the work or not
    shift_enjoyment(shift);

    // TODO this shouldn't be here, I think.
    if(shift.chance(25)) {
        if (girl.is_unpaid()) {
            shift.add_literal("\nThe fact that she is your slave makes people think its less of a good deed on your part.");
            dispo = (dispo + 1) / 2;
        } else if(girl.house() > 50) {
            shift.add_literal("\nThe fact that your paying this girl to do this helps people think your a better person.");
            dispo = (dispo * 3) / 2;
        }
    }

    g_Game->player().disposition(dispo);

    CommunityWork(shift);
}

void CommunityJob::SexEvent(cGirlShift& shift) const {
    auto& girl = shift.girl();

    sCustomer Cust = cJobManager::GetMiscCustomer(shift.building());

    shift.add_line("event.sex");

    if (girl.is_sex_type_allowed(SKILL_NORMALSEX) && (shift.chance(50) || !girl.is_sex_type_allowed(SKILL_ANAL)))
    {
        shift.set_image(EImageBaseType::VAGINAL);
        girl.normalsex(2);
        if (girl.lose_trait(traits::VIRGIN))
        {
            shift.add_literal("She is no longer a virgin.\n");
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

CommunityService::CommunityService() : CommunityJob(JOB_COMUNITYSERVICE, "CommunityService.xml") {

}

void CommunityService::CommunityWork(cGirlShift& shift) const {
    int help = shift.performance() / 10;        //  1 helped per 10 point of performance
    shift.data().EventMessage << "Overall, ${name} helped " << help << " people today.";
}

FeedPoor::FeedPoor() : CommunityJob(JOB_FEEDPOOR, "FeedPoor.xml") {

}

void FeedPoor::CommunityWork(cGirlShift& shift) const {
    int feed = shift.performance() / 10;        //  1 feed per 10 point of performance

    int cost = 0;
    for (int i = 0; i < feed; i++)
    {
        cost += shift.uniform(2, 5); // 2-5 gold per customer
    }
    shift.data().Cost = cost;
    shift.data().EventMessage << "Overall, ${name} fed " << feed << " people costing you " << cost << " gold.";
}


Counselor::Counselor() : cSimpleJob(JOB_COUNSELOR, "Counselor.xml") {
}

void Counselor::JobProcessing(sGirl& girl, cGirlShift& shift) const {
    int roll_a = shift.d100();
    auto& ss = shift.data().EventMessage;

    if (roll_a <= 10)       { shift.data().Enjoyment -= shift.uniform(1, 3);    ss << "The addicts hasseled her."; }
    else if (roll_a >= 90)  { shift.data().Enjoyment += shift.uniform(1, 3);    ss << "She had a pleasant time working."; }
    else                    { shift.data().Enjoyment += shift.uniform(0, 1);    ss << "Otherwise, the shift passed uneventfully."; }

    int rehabers = shift.building().num_girls_on_job(JOB_REHAB, shift.is_night_shift());
    int roll_max = (girl.spirit() + girl.intelligence()) / 4;
    shift.data().Wages += shift.uniform(10, 10 + roll_max);
    shift.data().Wages += 5 * rehabers;    // `J` pay her 5 for each patient you send to her
    shift.provide_interaction(CounselingInteractionId, 2);
}

void RegisterCentreJobs(cJobManager& mgr) {
    mgr.register_job(std::make_unique<CommunityService>());
    mgr.register_job(std::make_unique<FeedPoor>());
    mgr.register_job(std::make_unique<Counselor>());
}