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

#include "jobs/Treatment.h"
#include "jobs/cJobManager.h"
#include "cGirls.h"
#include "character/sGirl.h"
#include "buildings/cBuilding.h"
#include <sstream>
#include <utility>
#include "character/predicates.h"
#include "traits/ITraitsManager.h"
#include "IGame.h"
#include "cGirlGangFight.h"
#include "buildings/cBuildingManager.h"
#include "xml/getattr.h"
#include "xml/util.h"
#include "cCentre.h"

extern const char* const CounselingInteractionId;


class TherapyJob : public cTreatmentJob {
public:
    explicit TherapyJob(std::string xml_file) : cTreatmentJob(std::move(xml_file)) {
    }

    void DoWork(cGirlShift& shift) const override;
protected:
    // common data
    std::string TreatmentName;     //!< Therapy or Rehab
    int         Duration;          //!< How many days until finished.
    int         BasicFightChance;  //!< Chance to fight with the counselor
    int         SuccessBonus;      //!< Bonus for enjoyment and happiness once therapy was a success

    bool CheckCanWork(cGirlShift& girl) const override;
    void HandleWorkImpossible(cGirlShift& shift) const override;

    virtual void FightEvent(cGirlShift& shift, sGirl& counselor) const;
    virtual void OnFinish(sGirl& girl) const {}

    const char* specific_config_element() const override { return "Therapy"; }
    void load_from_xml_callback(const tinyxml2::XMLElement& job_element) override;
    double GetPerformance(const sGirl& girl, bool estimate) const override;
};

struct Rehab : public TherapyJob {
    using TherapyJob::TherapyJob;
    void OnFinish(sGirl& girl) const override;
};

struct AngerManagement : public TherapyJob {
    using TherapyJob::TherapyJob;
    void FightEvent(cGirlShift& shift, sGirl& counselor) const override;
};


void TherapyJob::load_from_xml_callback(const tinyxml2::XMLElement& job_element) {
    Duration = GetIntAttribute(job_element, "Duration");
    BasicFightChance = GetIntAttribute(job_element, "FightChance");
    TreatmentName = GetStringAttribute(job_element, "Title");
    SuccessBonus = GetIntAttribute(job_element, "SuccessBonus");
}

void TherapyJob::DoWork(cGirlShift& shift) const {
    sGirl* counselor = shift.request_interaction(CounselingInteractionId);
    auto& girl = shift.girl();

    // TODO disobey check based on happiness, tiredness, and obedience
    if (shift.chance(BasicFightChance))    // `J` - yes, OR, not and.
    {
        FightEvent(shift, *counselor);
    }
    shift.add_line("therapy"),
            shift.add_literal("\n");

    int enjoy = 0;
    auto msgtype = shift.is_night_shift() ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT;

    if (!shift.is_night_shift()) {
        girl.make_treatment_progress(shift.uniform(90, 110) / Duration);
    }

    girl.happiness(shift.uniform(-20, 10));
    girl.spirit(shift.uniform(-5, 5));

    if (girl.health() < 1)
    {
        shift.add_text("death");
        msgtype = EVENT_DANGER;
    }

    if (girl.get_treatment_progress() >= 100 && shift.is_night_shift())
    {
        enjoy += shift.uniform(0, SuccessBonus);
        girl.enjoyment(EActivity::SOCIAL, shift.uniform(-2, 4));    // `J` She may want to help others with their problems
        girl.happiness(shift.uniform(0, SuccessBonus));

        shift.data().EventMessage << "The " << TreatmentName << " is a success.\n";
        msgtype = EVENT_GOODNEWS;

        on_complete_treatment(shift);

        OnFinish(girl);

        if (has_valid_effect(girl))
        {
            shift.data().EventMessage << "\n";
            shift.add_text("continue-therapy");
        }
        else // get out of therapy
        {
            shift.data().EventMessage << "\n";
            shift.add_text("release");
            girl.FullJobReset(JOB_RESTING);
        }
    }
    else
    {
        shift.data().EventMessage << "The " << TreatmentName << " is in progress (${progress}).";
    }

    // Improve girl
    shift.set_image(EImageBaseType::PROFILE);
    shift.data().EventType = msgtype;
    shift.generate_event();
}

void TherapyJob::FightEvent(cGirlShift& shift, sGirl& counselor) const {
    shift.add_literal("${name} fought with her counselor and did not make any progress this week.");
    shift.data().EventImage = EImageBaseType::REFUSE;
    shift.data().EventType = EVENT_NOWORK;
    shift.generate_event();
    if (shift.is_night_shift()) {
        shift.girl().make_treatment_progress(-shift.uniform(10, 20));
    }
}

double TherapyJob::GetPerformance(const sGirl& girl, bool estimate) const {
    if(!has_valid_effect(girl)) return -1000;
    return 1000;
}

bool TherapyJob::CheckCanWork(cGirlShift& shift) const {
    if (!shift.has_interaction(CounselingInteractionId))
    {
        shift.add_text("no-counselor");
        shift.data().EventType = EVENT_WARNING;
        shift.data().EventImage = EImageBaseType::PROFILE;
        shift.generate_event();
        return false;
    }

    return true;
}

void TherapyJob::HandleWorkImpossible(cGirlShift& shift) const {
    shift.add_literal(" She was sent to the waiting room.\n");
    shift.data().EventType = EVENT_WARNING;
    shift.data().EventImage = EImageBaseType::PROFILE;
    shift.generate_event();
    shift.girl().FullJobReset(JOB_RESTING);
}

void AngerManagement::FightEvent(cGirlShift& shift, sGirl& counselor) const {
    auto& girl = shift.girl();
    girl.make_treatment_progress(-shift.uniform(10, 20));
    if (shift.chance(10))
    {
        bool runaway = false;
        shift.data().EventMessage << "\n \n${name} fought hard with her counselor " << counselor.FullName();
        /// TODO Gangs and Security
        auto winner = GirlFightsGirl(counselor, girl);
        if (winner != EFightResult::VICTORY)    // the patient won
        {
            shift.data().EventMessage << " and won.\n \n";
            girl.enjoyment(EActivity::FIGHTING, 5);
            counselor.enjoyment(EActivity::SOCIAL, -5);
            counselor.enjoyment(EActivity::FIGHTING, -2);

            if (shift.chance(10))    // and ran away
            {
                std::stringstream smess;
                smess << girl.FullName() << " fought with her counselor and ran away.\nSend your goons after her to attempt recapture.\nShe will escape for good after 6 weeks.\n";
                g_Game->push_message(smess.str(), COLOR_WARNING);
                girl.run_away();
                return;
            }
        }
        else    // the counselor won
        {
            shift.add_literal(" and lost.\n \n");
            girl.enjoyment(EActivity::SOCIAL, -1);
            girl.enjoyment(EActivity::FIGHTING, -5);
            counselor.enjoyment(EActivity::SOCIAL, -2);
            counselor.enjoyment(EActivity::FIGHTING, 2);
        }
        std::stringstream ssc;
        ssc << "${name} had to defend herself from " << girl.FullName() << " who she was counseling.\n";
        counselor.AddMessage(ssc.str(), EImageBaseType::COMBAT, EVENT_WARNING);
    }
    else
    {
        shift.add_literal("${name} fought with her counselor and did not make any progress this week.");
    }
    shift.data().EventType = EVENT_NOWORK;
    shift.data().EventImage = EImageBaseType::PROFILE;
    shift.generate_event();
}

void Rehab::OnFinish(sGirl& girl) const {
    girl.add_temporary_trait(traits::FORMER_ADDICT, 40);
}

void RegisterTherapyJobs(cJobManager& mgr) {
    mgr.register_job(std::make_unique<AngerManagement>("AngerManagement.xml"));
    mgr.register_job(std::make_unique<TherapyJob>("ExtremeTherapy.xml"));
    mgr.register_job(std::make_unique<TherapyJob>("Therapy.xml"));
    mgr.register_job(std::make_unique<Rehab>("Rehab.xml"));
}
