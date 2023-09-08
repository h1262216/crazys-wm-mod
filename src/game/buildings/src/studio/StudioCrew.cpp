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


#include <sstream>
#include <buildings/studio/cMovieStudio.h>
#include "buildings/cBuilding.h"
#include "buildings/queries.h"
#include "cGirls.h"
#include "jobs/IGenericJob.h"
#include "studio/StudioJobs.h"

extern const char* const FluffPointsId;
extern const char* const DirectorInteractionId;
extern const char* const CamMageInteractionId;
extern const char* const CrystalPurifierInteractionId;
extern const char* const StageHandPtsId;

class cJobCameraMage : public cCrewJob {
public:
    cJobCameraMage();
    void HandleUpdate(cGirlShift& shift) const override {
        shift.provide_interaction(CamMageInteractionId, 3);
    }
};

class cJobCrystalPurifier : public cCrewJob {
public:
    cJobCrystalPurifier();
    void HandleUpdate(cGirlShift& shift) const override {
        shift.provide_interaction(CrystalPurifierInteractionId, 3);
    }
};

class cJobFluffer : public cCrewJob {
public:
    cJobFluffer();
    void HandleUpdate(cGirlShift& shift) const override;

};

class cJobDirector : public cCrewJob {
public:
    cJobDirector();
    void HandleUpdate(cGirlShift& shift) const override {
        shift.provide_interaction(DirectorInteractionId, 3);
    };

};

bool cCrewJob::CheckCanWork(cGirlShift& shift) const {
    if (shift.building().num_girls_on_job(JOB_CAMERAMAGE, SHIFT_NIGHT) == 0 || shift.building().num_girls_on_job(JOB_CRYSTALPURIFIER, SHIFT_NIGHT) == 0)
    {
        shift.add_text("no-crew");
        return false;
    }
    else if (GetNumberActresses(shift.building()) < 1)
    {
        shift.add_text("no-actress");
        return false;
    }
    return true;
}

void cCrewJob::JobProcessing(sGirl& girl, cGirlShift& shift) const {
    // slave girls not being paid for a job that normally you would pay directly for do less work
    int performance = shift.performance();
    if (girl.is_unpaid())
    {
        performance *= 0.9;
    }
    else    // work out the pay between the house and the girl
    {
        // `J` zzzzzz - need to change pay so it better reflects how well she filmed the films
        int roll_max = performance;
        roll_max /= 4;
        shift.data().Wages += shift.uniform(10, 10 + roll_max);
    }

    if (performance >= 166)
    {
        shift.add_text("work.perfect");
    }
    else if (performance >= 133)
    {
        shift.add_text("work.great");
    }
    else if (performance >= 100)
    {
        shift.add_text("work.good");
    }
    else if (performance >= 66)
    {
        shift.add_text("work.ok");
    }
    else if (performance >= 33)
    {
        shift.add_text("work.bad");
    }
    else
    {
        shift.add_text("work.worst");
    }

    HandleUpdate(shift);
}


cJobCameraMage::cJobCameraMage() : cCrewJob(JOB_CAMERAMAGE, "CameraMage.xml") {
    m_Info.Provides.emplace_back(CamMageInteractionId);
}

cJobCrystalPurifier::cJobCrystalPurifier() : cCrewJob(JOB_CRYSTALPURIFIER, "CrystalPurifier.xml") {
    m_Info.Provides.emplace_back(CrystalPurifierInteractionId);
}

cJobFluffer::cJobFluffer() : cCrewJob(JOB_FLUFFER, "Fluffer.xml") {
    m_Info.Provides.emplace_back(FluffPointsId);
}

void cJobFluffer::HandleUpdate(cGirlShift& shift) const {
    shift.provide_resource(FluffPointsId, shift.performance());
}

cJobDirector::cJobDirector() : cCrewJob(JOB_DIRECTOR, "Director.xml") {
    m_Info.Provides.emplace_back(DirectorInteractionId);
}

class cJobStageHand : public cSimpleJob {
public:
    cJobStageHand() : cSimpleJob(JOB_STAGEHAND, "StageHand.xml") {
        m_Info.Provides.emplace_back(StageHandPtsId);
    };
    void JobProcessing(sGirl& girl, cGirlShift& shift) const override;
    double GetPerformance(const sGirl& girl, bool estimate) const override;
};

void cJobStageHand::JobProcessing(sGirl& girl, cGirlShift& shift) const {
    auto brothel = dynamic_cast<sMovieStudio*>(girl.m_Building);
    int enjoyc = 0, enjoym = 0;
    bool filming = true;
    int roll_a = shift.d100();
    auto& ss = shift.data().EventMessage;

    // `J` - jobperformance and CleanAmt need to be worked out specially for this job.
    double jobperformance = 0;
    double CleanAmt = ((girl.service() / 10.0) + 5) * 5;
    CleanAmt += girl.get_trait_modifier(traits::modifiers::WORK_STAGEHAND_CLEAN_AMOUNT);
    jobperformance += girl.get_trait_modifier(traits::modifiers::WORK_STAGEHAND_PERFORMANCE);

    if (brothel->num_girls_on_job(JOB_CAMERAMAGE, SHIFT_NIGHT) == 0 ||
        brothel->num_girls_on_job(JOB_CRYSTALPURIFIER, SHIFT_NIGHT) == 0 ||
        GetNumberActresses(*brothel) < 1)
    {
        ss << "There were no scenes being filmed, so she just cleaned the set.\n \n";
        filming = false;
        shift.set_image(EImageBaseType::MAID);
    }

    if (roll_a <= 10)
    {
        enjoyc -= shift.uniform(1, 3); if (filming) enjoym -= shift.uniform(1, 3);
        CleanAmt *= 0.8;
        ss << "She did not like working in the studio today.";
    }
    else if (roll_a >= 90)
    {
        enjoyc += shift.uniform(1, 3); if (filming) enjoym += shift.uniform(1, 3);
        CleanAmt *= 1.1;
        ss << "She had a great time working today.";
    }
    else
    {
        enjoyc += std::max(0, shift.uniform(-1, 2)); if (filming) enjoym += std::max(0, shift.uniform(-1, 2));
        ss << "Otherwise, the shift passed uneventfully.";
    }
    jobperformance += enjoyc + enjoym;
    ss << "\n \n";

    CleanAmt = std::min((int)CleanAmt, brothel->m_Filthiness);

    if (filming)
    {
        jobperformance += (girl.crafting() / 5) + (girl.constitution() / 10) + (girl.service() / 10);
        jobperformance += girl.level();
        jobperformance += shift.uniform(-1, 3);    // should add a -1 to +3 random element --PP

        // Cleaning reduces the points remaining for actual stage hand work
        jobperformance -= CleanAmt / 2;
        if(jobperformance < 0 && CleanAmt > 0) {
            ss << "Your studio was so messy that ${name} spent the entire shift cleaning up, and had no time to "
                  "assist in movie production. She improved the cleanliness rating by " << (int)CleanAmt << ".";
            jobperformance = 0;
        } else {
            ss << "She assisted the crew in movie production and provided " << (int)jobperformance << " stage hand points.";
        }
    }


    // slave girls not being paid for a job that normally you would pay directly for do less work
    if (girl.is_unpaid())
    {
        CleanAmt *= 0.9;
        shift.data().Wages = 0;
    }
    else if (filming)
    {
        shift.data().Wages += int(CleanAmt + jobperformance);
    }
    else
    {
        shift.data().Wages += int(CleanAmt);
    }

    if (!filming && brothel->m_Filthiness < CleanAmt / 2) // `J` needs more variation
    {
        ss << "\n \n${name} finished her cleaning early so she hung out around the Studio a bit.";
        girl.happiness(shift.uniform(1, 3));
    }

    shift.provide_resource(StageHandPtsId, int(jobperformance));
    brothel->m_Filthiness = std::max(0, brothel->m_Filthiness - int(CleanAmt));

    if (filming) girl.enjoyment(EActivity::CRAFTING, enjoym);
    girl.enjoyment(EActivity::SERVICE, enjoyc);
    // Gain Traits
    cGirls::PossiblyGainNewTrait(girl, "Maid", girl.service() / 9, "${name} has cleaned enough that she could work professionally as a Maid anywhere.");
    cGirls::PossiblyLoseExistingTrait(girl, traits::CLUMSY, 30, "It took her spilling hundreds of buckets, and just as many reprimands, but ${name} has finally stopped being so Clumsy.");
}

double cJobStageHand::GetPerformance(const sGirl& girl, bool estimate) const {
    //SIN - standardizing job performance calc per J's instructs
    double jobperformance =
            //main stat - first 100
            girl.service() +
            //secondary stats - second 100
            ((girl.morality() + girl.obedience() + girl.agility()) / 3) +
            //add level
            girl.level();

    //tiredness penalty
    if (!estimate)
    {
        int t = girl.tiredness() - 80;
        if (t > 0)
            jobperformance -= (t + 2) * (t / 3);
    }

    jobperformance += girl.get_trait_modifier(traits::modifiers::WORK_CLEANING);

    return jobperformance;
}


void RegisterFilmCrewJobs(cJobManager& mgr) {
    mgr.register_job(std::make_unique<cJobCameraMage>());
    mgr.register_job(std::make_unique<cJobFluffer>());
    mgr.register_job(std::make_unique<cJobCrystalPurifier>());
    mgr.register_job(std::make_unique<cJobDirector>());
    mgr.register_job(std::make_unique<cJobStageHand>());
}