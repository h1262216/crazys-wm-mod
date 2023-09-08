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

#include "jobs/cBasicJob.h"
#include "jobs/Treatment.h"
#include "character/sGirl.h"
#include "cGirls.h"
#include "buildings/cBuilding.h"
#include "utils/streaming_random_selection.hpp"
#include "IGame.h"
#include "character/predicates.h"
#include "jobs/cJobManager.h"
#include "xml/getattr.h"

extern const char* const TrainingInteractionId;

class PracticeJob : public cBasicJob {
public:
    PracticeJob();

    void DoWork(cGirlShift& shift) const override;
    bool CheckRefuseWork(cGirlShift& girl) const override;

private:
    int mMistressId;
    int mSkillId;
};

class MistressJob : public cBasicJob {
public:
    MistressJob();

    void DoWork(cGirlShift& shift) const override;
    void on_post_shift(cGirlShift& shift) const override;
private:
    int mSlackerId;
};

class SexTrainingJob : public cTreatmentJob {
public:
    SexTrainingJob(std::string xml_file);

    double GetPerformance(const sGirl& girl, bool estimate) const override;

    virtual void HandleTraining(cGirlShift& shift) const = 0;
    virtual void OnNoProgress(cGirlShift& shift) const;
    virtual void OnRegularProgress(cGirlShift& shift) const;
    const char* specific_config_element() const override { return "SexTraining"; }
    void load_from_xml_callback(const tinyxml2::XMLElement& job_element) override;


protected:
    int calculate_progress(const sGirl& girl, cGirlShift& shift) const;
    void CountTheDays(cGirlShift& shift, int progress) const;

    void DoWork(cGirlShift& shift) const override;

    int& tiredness(cGirlShift& shift) const;

    int Enjoyment;

    std::string TargetName;

    int TirednessId;
};

class SoStraight : public SexTrainingJob {
public:
    SoStraight() : SexTrainingJob("SoStraight.xml") {
    }
    void HandleTraining(cGirlShift& shift) const override;
};

class SoLesbian : public SexTrainingJob {
public:
    SoLesbian() : SexTrainingJob("SoLesbian.xml") {
    }
    void HandleTraining(cGirlShift& shift) const override;
};

class SoBi : public SexTrainingJob {
public:
    SoBi() : SexTrainingJob("SoBi.xml") {
    }
    void HandleTraining(cGirlShift& shift) const override;
};

class FakeOrg : public SexTrainingJob {
public:
    FakeOrg() : SexTrainingJob("FakeOrgasm.xml") {
    }
    void HandleTraining(cGirlShift& shift) const override;
    void OnNoProgress(cGirlShift& shift) const override;
    void OnRegularProgress(cGirlShift& shift) const override;
};


MistressJob::MistressJob() : cBasicJob(JOB_MISTRESS, "Mistress.xml") {
    m_Info.FreeOnly = true;
    m_Info.Provides.emplace_back(TrainingInteractionId);
    mSlackerId = RegisterVariable("slacker", "");
}

void MistressJob::DoWork(cGirlShift& shift) const {
    if (shift.performance() > 150) {
        shift.provide_interaction(TrainingInteractionId, 3);
        shift.add_text("work.good");
    } else {
        shift.add_text("work.normal");
        shift.provide_interaction(TrainingInteractionId, 2);
    }

    if(shift.performance() > 100) {
        auto* target_girl = shift.building().girls().get_random_girl([](const sGirl& g) {
            return g.is_slave() && g.obedience() < 50;
        });
        if (target_girl) {
            shift.data().set_var(mSlackerId, target_girl->FullName());
            shift.add_text("slacker");
            target_girl->upd_temp_stat(STAT_OBEDIENCE, 4);
            target_girl->upd_temp_stat(STAT_PCFEAR, 2);
        }
    }

    shift.data().EventImage = EImageBaseType::TEACHER;
    shift.generate_event();
}

void MistressJob::on_post_shift(cGirlShift& shift) const {
    if (shift.data().Refused != ECheckWorkResult::ACCEPTS) {
        //brothel->m_Fame -= girl.fame();
        shift.girl().AddMessage("${name} did not work so she made no money.", EImageBaseType::PROFILE, EVENT_SUMMARY);
    } else {
        std::string tasks = shift.data().EventMessage.str();
        auto& ss = shift.data().EventMessage;
        ss.str("");
        shift.data().EventImage = EImageBaseType::PROFILE;
        shift.data().EventType = EEventType::EVENT_SUMMARY;
        auto money_data = job_manager().CalculatePay(shift.data());

        if(!tasks.empty()) {
            ss << "She trained: \n";
            ss << tasks;
            ss << "\n";
        }

        if (money_data.Wages > 0) ss << "You paid her a salary of " << money_data.Wages << ". ";
        else ss << "You did not pay her a salary. ";
        ss << "\n\n";

        apply_gains(shift);

        shift.generate_event();
    }
}

PracticeJob::PracticeJob() : cBasicJob(JOB_TRAINING, "Training.xml") {
    m_Info.Consumes.emplace_back(TrainingInteractionId);
    mMistressId = RegisterVariable("Mistress", "Mistress");
    mSkillId = RegisterVariable("Skill", "SKILL");
}

namespace {
    sImagePreset skill_to_image(SKILLS skill) {
        switch (skill) {
            case SKILL_STRIP:
                return EImageBaseType::STRIP;
            case SKILL_GROUP:
                return EImagePresets::GROUP;
            case SKILL_BDSM:
                return EImageBaseType::BDSM;
            case SKILL_PERFORMANCE:
                return EImagePresets::MASTURBATE;
            case SKILL_ANAL:
                return EImageBaseType::ANAL;
            case SKILL_NORMALSEX:
                return EImageBaseType::VAGINAL;
            case SKILL_ORALSEX:
                return EImagePresets::BLOWJOB;
            case SKILL_TITTYSEX:
                return EImageBaseType::TITTY;
            case SKILL_LESBIAN:
                return EImagePresets::LESBIAN;
            case SKILL_HANDJOB:
                return EImageBaseType::HAND;
            case SKILL_FOOTJOB:
                return EImageBaseType::FOOT;
            case SKILL_BEASTIALITY:
                return EImageBaseType::BEAST;
            default:
                return EImageBaseType::PROFILE;
        }
    }
}

void PracticeJob::DoWork(cGirlShift& shift) const {
    auto& girl = shift.girl();
    sGirl* mistress = shift.request_interaction(TrainingInteractionId);
    if(mistress)
        shift.data().set_var(mMistressId, mistress->FullName());
    girl.tiredness(2);

    shift.data().Wages = 20;

    if(!mistress) {
        shift.add_text("no-mistress");
        RandomSelector<SKILLS> selector;
        SKILLS skills[] = {SKILL_NORMALSEX, SKILL_ANAL, SKILL_FOOTJOB, SKILL_HANDJOB, SKILL_LESBIAN,
                           SKILL_TITTYSEX, SKILL_ORALSEX, SKILL_PERFORMANCE, SKILL_STRIP};
        for(auto& skill : skills) {
            if(girl.get_skill(skill) < 50 && girl.is_sex_type_allowed(skill)) {
                selector.process(&skill, 50 - girl.get_skill(skill));
            }
        }

        girl.exp(5);
        shift.data().EventImage = EImagePresets::MASTURBATE;
        if(selector.selection()) {
            auto target = *selector.selection();
            shift.data().EventImage = skill_to_image(target);
            if(girl.get_skill(target) < 50) {
                girl.upd_skill(target, 1);
                shift.data().EventMessage << "\n" << get_skill_name(target) << " + 1";
            }
        }
    } else {
        mistress->tiredness(2);

        // The mistress trains her. First, settle on a skill to train
        RandomSelector<SKILLS> selector;
        SKILLS skills[] = {SKILL_NORMALSEX, SKILL_ANAL, SKILL_BDSM, SKILL_BEASTIALITY, SKILL_FOOTJOB,
                           SKILL_HANDJOB, SKILL_LESBIAN, SKILL_TITTYSEX, SKILL_ORALSEX, SKILL_GROUP,
                           SKILL_PERFORMANCE, SKILL_STRIP};

        for(auto& skill : skills) {
            if(!girl.is_sex_type_allowed(skill))
                continue;

            int my_value = girl.get_skill(skill);
            int other_value = mistress->get_skill(skill);
            float weight = 5;       // base chance
            // is it important to train that value
            if(my_value < 20)
                weight += 5;
            if(my_value < 50)
                weight += 5;

            // can I train that value well
            if(my_value < other_value) {
                weight += float(other_value - my_value) / 2;
            }

            // don't train values close to skill cap
            int cap = g_Game->get_skill_cap(skill, girl);
            if(my_value >= cap - 5) {
                weight = 2;
            }

            selector.process(&skill, weight);
        }

        // this will always be non-NULL, because SKILL_PERFORMANCE cannot be forbidden
        SKILLS target = *selector.selection();
        shift.data().EventImage = skill_to_image(target);

        shift.data().set_var(mSkillId, get_skill_name(target));
        int my_value = girl.get_skill(target);
        int other_value = mistress->get_skill(target);
        if(my_value >= other_value) {
            shift.add_text("lack-of-skill");
            girl.exp(5);
            // if total skill < 50, we still get some update
            if(girl.get_skill(target) < 50) {
                girl.upd_skill(target, 1);
                shift.data().EventMessage << "\n" << get_skill_name(target) << " + 1";
            }
            if(shift.chance(5)) {
                mistress->upd_skill(target, 1);
            }
        } else {
            shift.add_text(std::string("train.") + get_skill_name(target));
            girl.exp(5);
            int perf = mistress->job_performance(JOB_MISTRESS, false);
            int min_gain = 1;
            int max_gain = 3;
            if(my_value < 20 && perf > 75) {
                min_gain += 1;
            }
            if(other_value > 66 && my_value < 40 && perf > 100) {
                min_gain += 1;
            }

            if(my_value < 40 && perf > 140) {
                max_gain += 1;
            }

            if (girl.has_active_trait(traits::QUICK_LEARNER))     max_gain += 1;
            else if (girl.has_active_trait(traits::SLOW_LEARNER)) max_gain -= 1;

            max_gain += std::min((other_value - my_value) / 10, 3);
            int amount = shift.uniform(min_gain, std::max(max_gain, min_gain));
            girl.upd_skill(target, amount);

            shift.data().EventMessage << "\n${name}'s " << get_skill_name(target) << " skill increased by " << amount << " points.";
        }
    }

    shift.generate_event();
}

bool PracticeJob::CheckRefuseWork(cGirlShift& shift) const {
    auto& girl = shift.girl();
    if (cBasicJob::CheckRefuseWork(shift))
    {
        sGirl* mistress = nullptr;
        if(girl.is_slave() && (mistress = shift.request_interaction(TrainingInteractionId))) {
            // TODO FIX THIS
            shift.data().set_var(mMistressId, mistress->FullName());
            shift.add_text("refuse.forced");
            // smaller changes than for dungeon torture, but still we should combine the code at some point
            girl.obedience(2);
            girl.upd_temp_stat(STAT_OBEDIENCE, 5);
            girl.pcfear(1);
            girl.pclove(-1);
            girl.spirit(-1);
            girl.happiness(-2);
            girl.tiredness(2);
            girl.health(-2);

            shift.data().EventImage = EImageBaseType::TORTURE;
            shift.data().EventType = EVENT_NOWORK;

            shift.generate_event();
        } else {
            shift.add_text("refuse");
            shift.data().EventImage = EImageBaseType::REFUSE;
            shift.data().EventType = EVENT_NOWORK;

            shift.generate_event();
        }
        return true;
    }
    return false;
}

SexTrainingJob::SexTrainingJob(std::string xml_file) :
    cTreatmentJob(std::move(xml_file))
{
    TirednessId = RegisterVariable("Tiredness", 0);
    m_Info.Consumes.emplace_back(TrainingInteractionId);
}

void SexTrainingJob::DoWork(cGirlShift& shift) const {
    auto mistress = shift.request_interaction(TrainingInteractionId);
    if(!mistress) {
        shift.add_text("no-mistress");
        shift.data().EventType = EVENT_WARNING;
        shift.data().EventImage = EImageBaseType::PROFILE;
        shift.generate_event();
        return;
    }

    // Base adjustment
    tiredness(shift) = shift.uniform(5, 15);

    shift.add_line("training");
    shift.data().EventType = shift.is_night_shift() ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT;

    HandleTraining(shift);
    // TODO Figure this out!
    {
        std::stringstream mistress_msg;
        mistress_msg << "She trained " << shift.girl().FullName() << " to be a " << TargetName << ".\n";
        mistress->exp(5);
        mistress->AddMessage(mistress_msg.str(), EImageBaseType::PROFILE, shift.is_night_shift() ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);
    }

    shift.girl().tiredness(tiredness(shift));

    shift.generate_event();
}


int SexTrainingJob::calculate_progress(const sGirl& girl, cGirlShift& shift) const {
    int progress = 0;
    progress += shift.uniform(10, 20);
    progress += girl.obedience() / 20;
    if (girl.pcfear() > 50)                progress += shift.uniform(0, girl.pcfear() / 20);        // She will do as she is told
    if (girl.pclove() > 50)                progress += shift.uniform(0, girl.pclove() / 20);       // She will do what you ask

    // Negative Stats/Skills
    progress -= girl.spirit() / 25;
    if (girl.pclove() < -30)               progress -= shift.uniform(0, -girl.pclove() / 10);      // She will not do what you want
    if (girl.happiness() < 50)             progress -= shift.uniform(1, 5);                        // She is not feeling like it
    if (girl.health() < 50)                progress -= shift.uniform(1, 5);                        // She is feeling sick
    if (girl.tiredness() > 50)             progress -= shift.uniform(1, 5);                        // She is tired

    return progress;
}

double SexTrainingJob::GetPerformance(const sGirl& girl, bool estimate) const {
    return 0;
    // TODO figure this out.
    /*if (girl.has_active_trait(TargetTrait.c_str()))    return -1000;
    return 250;*/
}


void SexTrainingJob::CountTheDays(cGirlShift& shift, int progress) const
{
    /// if (girl.disobey_check(ACTION_WORKTRAINING, job())) progress /= 2;    // if she disobeys, half her time is wasted

    if (progress <= 0)                                // she lost time so more tired
    {
        tiredness(shift) += shift.uniform(5, 5-progress);
        //Enjoyment -= shift.uniform(0, 2);
    }
    else if (progress > 33)                        // or if she trained a lot
    {
        tiredness(shift) += shift.uniform(progress / 4, progress / 2);
        //Enjoyment += shift.uniform(0, 2);
    }
    else                                        // otherwise just a bit tired
    {
        tiredness(shift) += shift.uniform(0, progress / 3);
        //Enjoyment -= shift.uniform(-2, 2);
    }

    shift.girl().make_treatment_progress(progress);

    if (progress <= 0)
    {
        shift.data().EventType = EVENT_WARNING;
        OnNoProgress(shift);
    }
    else if (shift.girl().get_treatment_progress() >= 100 && shift.is_night_shift())
    {
        shift.girl().finish_treatment();
        shift.data().EventType = EVENT_GOODNEWS;
        shift.add_line("complete");
        on_complete_treatment(shift);
        shift.girl().FullJobReset(JOB_RESTING);
    }
    else {
        OnRegularProgress(shift);
    }
}

void SexTrainingJob::OnNoProgress(cGirlShift& shift) const {
    shift.add_line("no-progress");
    tiredness(shift) += shift.uniform(5, 15);
}

void SexTrainingJob::OnRegularProgress(cGirlShift& shift) const {
    if (shift.girl().get_treatment_progress() >= 100)
    {
        shift.add_line("almost-done");
        tiredness(shift) -= (shift.girl().get_treatment_progress() - 100) / 2;    // her last day so she rested a bit
    }
    else {
        shift.add_line("in-progress");
    }
}

int& SexTrainingJob::tiredness(cGirlShift& shift) const {
    return std::get<int>(shift.data().get_var_ref(TirednessId));
}

void SexTrainingJob::load_from_xml_callback(const tinyxml2::XMLElement& element) {
    TargetName = GetStringAttribute(element, "TargetName");
}

void SoStraight::HandleTraining(cGirlShift& shift) const {
    auto& girl = shift.girl();
    // Positive Stats/Skills
    int progress = 0;
    progress += girl.normalsex() / 5;
    progress += girl.group() / 10;
    progress += girl.oralsex() / 20;
    progress += girl.tittysex() / 20;
    progress += girl.anal() / 20;

    progress += calculate_progress(girl, shift);

    if (!likes_men(girl))
    {
        shift.add_line("dislikes-men");
        progress -= girl.lesbian() / 5;                    // it is hard to change something you are good at
        tiredness(shift) += girl.lesbian() / 10;
    }
    if (girl.has_active_trait(traits::BISEXUAL)) progress -= girl.lesbian() / 20;    // it is hard to change something you are good at

    int trait = girl.get_trait_modifier(traits::modifiers::SO_STRAIGHT);
    progress += shift.uniform(trait / 2, trait + trait / 2);
    if (girl.has_active_trait(traits::BROKEN_WILL))        {
        shift.add_line("broken-will");
    }

    if (!girl.is_sex_type_allowed(SKILL_NORMALSEX))      progress -= shift.uniform(10, 30);

    CountTheDays(shift, progress);

    // Improve girl
    girl.exp( 1 + std::max(0, progress / 20) );
    girl.lesbian( shift.uniform(-15, -2) );
    girl.normalsex( shift.uniform(2, 15) );
    girl.group( shift.uniform(1, 4) );
    girl.anal( shift.uniform(1, 4) );
    girl.oralsex( shift.uniform(1, 4) );
    girl.handjob( shift.uniform(1, 4) );
    girl.tittysex( shift.uniform(1, 4) );
}

void SoLesbian::HandleTraining(cGirlShift& shift) const {
    auto& girl = shift.girl();
    // Positive Stats/Skills
    int progress = 0;
    progress += girl.lesbian() / 5;
    progress += girl.group() / 20;
    progress += girl.oralsex() / 25;
    progress += calculate_progress(girl, shift);

    if (girl.has_active_trait(traits::STRAIGHT))
    {
        shift.add_line("dislikes-women");
        progress -= girl.normalsex() / 5;                // it is hard to change something you are good at
        tiredness(shift) += girl.normalsex() / 10;
    }
    if (girl.has_active_trait(traits::BISEXUAL)) progress -= girl.normalsex() / 20;                    // it is hard to change something you are good at

    int trait = girl.get_trait_modifier(traits::modifiers::SO_LESBIAN);
    progress += shift.uniform(trait / 2, trait + trait / 2);

    if (girl.has_active_trait(traits::BROKEN_WILL))    {
        shift.add_line("broken-will");
    }

    //    if (girl.check_virginity())                {}

    if (!girl.is_sex_type_allowed(SKILL_LESBIAN))        progress -= shift.uniform(10, 30);

    CountTheDays(shift, progress);

    // Improve girl
    girl.exp( 1 + std::max(0, progress / 20) );
    girl.lesbian( shift.uniform(2, 15) );
    girl.normalsex( shift.uniform(-2, -15) );
    girl.group( shift.uniform(1, -3) );
    girl.anal( shift.uniform(-1, -5) );
    girl.oralsex( shift.uniform(1, -3) );
    girl.handjob( shift.uniform(1, -3) );
    girl.tittysex( shift.uniform(-1, -5) );
}

void SoBi::HandleTraining(cGirlShift& shift) const {
    auto& girl = shift.girl();
    int progress = 0;
    if (girl.has_active_trait(traits::STRAIGHT))
    {
        progress += girl.group() / 10;
        progress += girl.normalsex() / 20;
        progress += girl.lesbian() / 5;
        progress += girl.oralsex() / 20;
        progress += girl.tittysex() / 20;
        progress += girl.anal() / 20;
    }
    else if (girl.has_active_trait(traits::LESBIAN))
    {
        progress += girl.group() / 10;
        progress += girl.normalsex() / 5;
        progress += girl.lesbian() / 20;
        progress += girl.oralsex() / 15;
        progress += girl.tittysex() / 15;
        progress += girl.anal() / 15;
    }
    else
    {
        progress += girl.group() / 5;
        progress += girl.normalsex() / 10;
        progress += girl.lesbian() / 10;
        progress += girl.oralsex() / 20;
        progress += girl.tittysex() / 20;
        progress += girl.anal() / 20;
    }
    progress += calculate_progress(girl, shift);

    int trait = girl.get_trait_modifier(traits::modifiers::SO_BI);
    progress += shift.uniform(trait / 2, trait + trait / 2);
    if (girl.has_active_trait(traits::BROKEN_WILL))    {
        shift.add_line("broken-will");
    }

    if (!girl.is_sex_type_allowed(SKILL_LESBIAN))        progress -= shift.uniform(5, 15);
    if (!girl.is_sex_type_allowed(SKILL_NORMALSEX))      progress -= shift.uniform(5, 15);

    CountTheDays(shift, progress);

    girl.exp( 1 + std::max(0, progress / 20) );
    girl.lesbian( shift.uniform(1, 10) );
    girl.normalsex( shift.uniform(1, 10) );
    girl.group( shift.uniform(2, 15) );
    girl.anal( shift.uniform(0, 5) );
    girl.oralsex( shift.uniform(0, 5) );
    girl.handjob( shift.uniform(0, 5) );
    girl.tittysex( shift.uniform(0, 3) );
}

void FakeOrg::HandleTraining(cGirlShift& shift) const {
    auto& girl = shift.girl();

    // Positive Stats/Skills
    int progress = 0;
    progress += girl.performance() / 5;
    progress += girl.group() / 20;
    progress += girl.normalsex() / 20;
    progress += girl.lesbian() / 20;
    progress += calculate_progress(girl, shift);

    int trait = girl.get_trait_modifier(traits::modifiers::FAKE_ORGASM);
    progress += shift.uniform(trait / 2, trait + trait / 2);
    if (girl.has_active_trait(traits::BROKEN_WILL))    { shift.add_line("broken-will"); }

    if (!girl.is_sex_type_allowed(SKILL_NORMALSEX))      progress -= shift.uniform(5, 15);

    CountTheDays(shift, progress);

    // Improve girl
    girl.exp( 1 + std::max(0, progress / 20) );
    girl.lesbian( shift.uniform(0, 5) );
    girl.normalsex( shift.uniform(0, 5) );
    girl.group( shift.uniform(0, 5) );
    girl.anal( std::max(0, shift.uniform(-2, 2)) );
    girl.performance( shift.uniform(3, 15) );
    girl.confidence( shift.uniform(-1, 5) );
    girl.constitution( std::max(0, shift.uniform(-2, 1)) );
    girl.spirit( shift.uniform(-5, 5) );
}

void FakeOrg::OnNoProgress(cGirlShift& shift) const {
    shift.add_line("no-progress");
    tiredness(shift) += shift.uniform(5, 15);
}

void FakeOrg::OnRegularProgress(cGirlShift& shift) const {
    auto& ss = shift.data().EventMessage;
    int status = shift.girl().get_treatment_progress();
    if (status >= 100)    tiredness(shift) -= (status - 100) / 2;    // her last day so she rested a bit
    else                  ss << "Training in progress (" << status << "%).\n \n";
    if (status < 25)      ss << "She has no idea what she sounds like during sex, but it ain't orgasmic.";
    else if (status < 50) ss << "When she realizes she should finish, you can see it click in her mind and easily notice her changing things up.";
    else if (status < 75) ss << "She is still not getting into rhythm with her partner but it still seems enjoyable.";
    else                  ss << "She is almost there but you want her to practice a little more to get it perfect.";

    if (!shift.is_night_shift())
    {
        ss << "\nYou tell her to take a break for lunch and ";
        if (status < 50)      ss << "clear her mind, she has a lot more work to do.";
        else if (status < 75) ss << "relax, she has a bit more training to do.";
        else                  ss << "see if she can make anyone say \"I'll have what she's having\".";
    }
    else
    {
        ss << "\nThat's all for tonight, ";
        if (status < 50)    ss << "we have a lot more to do tomorrow (and probably the next few weeks).";
        else                ss << "we'll pick things up in the morning.";
    }
}

void RegisterTrainingJobs(cJobManager& mgr) {
    mgr.register_job(std::make_unique<SoStraight>());
    mgr.register_job(std::make_unique<SoLesbian>());
    mgr.register_job(std::make_unique<SoBi>());
    mgr.register_job(std::make_unique<FakeOrg>());
    mgr.register_job(std::make_unique<PracticeJob>());
    mgr.register_job(std::make_unique<MistressJob>());
}