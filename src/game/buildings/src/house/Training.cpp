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
#include "cHouse.h"

extern const char* const TrainingInteractionId;



MistressJob::MistressJob() : cBasicJob(JOB_MISTRESS, "Mistress.xml") {
    m_Info.FreeOnly = true;
    m_Info.Provides.emplace_back(TrainingInteractionId);
}

void MistressJob::DoWork(sGirlShiftData& shift) const {
    shift.Wages = 100;
    if (shift.Performance > 150) {
        provide_interaction(TrainingInteractionId, 3);
        add_text("work.good");
    } else {
        add_text("work.normal");
        provide_interaction(TrainingInteractionId, 2);
    }

    if(shift.Performance > 100) {
        auto* target_girl = shift.building().Building()->girls().get_random_girl([](const sGirl& g) {
            return g.is_slave() && g.obedience() < 50;
        });
        if (target_girl) {
            const_cast<MistressJob*>(this)->SetSubstitution("slacker", target_girl->FullName());
            add_text("slacker");
            target_girl->upd_temp_stat(STAT_OBEDIENCE, 4);
            target_girl->upd_temp_stat(STAT_PCFEAR, 2);
        }
    }

    shift.EventImage = EImageBaseType::TEACHER;
    generate_event();
}

bool MistressJob::CheckRefuseWork(sGirl& girl) const {
    return check_refuse_action(girl, EBasicActionType::SOCIAL);
}


PracticeJob::PracticeJob() : cBasicJob(JOB_TRAINING, "Training.xml") {
    m_Info.Consumes.emplace_back(TrainingInteractionId);
    m_Info.Phase = EJobPhase::LATE;
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

void PracticeJob::DoWork(sGirlShiftData& shift) const {
    auto& girl = active_girl();
    sGirl* mistress = request_interaction(TrainingInteractionId);
    const_cast<PracticeJob*>(this)->SetSubstitution("mistress", mistress ? mistress->FullName() : "The Mistress");
    girl.tiredness(2);

    shift.Wages = 20;

    if(!mistress) {
        add_text("no-mistress");
        RandomSelector<SKILLS> selector;
        SKILLS skills[] = {SKILL_NORMALSEX, SKILL_ANAL, SKILL_FOOTJOB, SKILL_HANDJOB, SKILL_LESBIAN,
                           SKILL_TITTYSEX, SKILL_ORALSEX, SKILL_PERFORMANCE, SKILL_STRIP};
        for(auto& skill : skills) {
            if(girl.get_skill(skill) < 50 && girl.is_sex_type_allowed(skill)) {
                selector.process(&skill, 50 - girl.get_skill(skill));
            }
        }

        girl.exp(5);
        shift.EventImage = EImagePresets::MASTURBATE;
        if(selector.selection()) {
            auto target = *selector.selection();
            shift.EventImage = skill_to_image(target);
            if(girl.get_skill(target) < 50) {
                girl.upd_skill(target, 1);
                shift.EventMessage << "\n" << get_skill_name(target) << " + 1";
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
        shift.EventImage = skill_to_image(target);

        const_cast<PracticeJob*>(this)->SetSubstitution("skill", get_skill_name(target));
        int my_value = girl.get_skill(target);
        int other_value = mistress->get_skill(target);
        if(my_value >= other_value) {
            add_text("lack-of-skill");
            girl.exp(5);
            // if total skill < 50, we still get some update
            if(girl.get_skill(target) < 50) {
                girl.upd_skill(target, 1);
                shift.EventMessage << "\n" << get_skill_name(target) << " + 1";
            }
            if(chance(5)) {
                mistress->upd_skill(target, 1);
            }
        } else {
            add_text(std::string("train.") + get_skill_name(target));
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
            int amount = uniform(min_gain, std::max(max_gain, min_gain));
            girl.upd_skill(target, amount);

            shift.EventMessage << "\n${name}'s " << get_skill_name(target) << " skill increased by " << amount << " points.";
        }
    }

    generate_event();
}

bool PracticeJob::CheckRefuseWork(sGirl& girl) const {
    if (disobey_check(girl))
    {
        sGirl* mistress = nullptr;
        if(girl.is_slave() && (mistress = request_interaction(TrainingInteractionId))) {
            // TODO FIX THIS
            const_cast<PracticeJob*>(this)->SetSubstitution("mistress", mistress->FullName() );
            add_text("refuse.forced");
            // smaller changes than for dungeon torture, but still we should combine the code at some point
            girl.obedience(2);
            girl.upd_temp_stat(STAT_OBEDIENCE, 5);
            girl.pcfear(1);
            girl.pclove(-1);
            girl.spirit(-1);
            girl.happiness(-2);
            girl.tiredness(2);
            girl.health(-2);

            active_shift().EventImage = EImageBaseType::TORTURE;
            active_shift().EventType = EVENT_NOWORK;

            generate_event();
        } else {
            add_text("refuse");
            active_shift().EventImage = EImageBaseType::REFUSE;
            active_shift().EventType = EVENT_NOWORK;

            generate_event();
        }
        return true;
    }
    return false;
}

TrainingJob::TrainingJob(JOBS job, std::string xml_file, const char* trait) :
    ITreatmentJob(job, std::move(xml_file)), TargetTrait(trait)
{
    TirednessId = RegisterVariable("Tiredness", 0);
    m_Info.Consumes.emplace_back(TrainingInteractionId);
}


int TrainingJob::calculate_progress(const sGirl& girl) const {
    int progress = 0;
    progress += uniform(10, 20);
    progress += girl.obedience() / 20;
    if (girl.pcfear() > 50)                progress +=uniform(0, girl.pcfear() / 20);        // She will do as she is told
    if (girl.pclove() > 50)                progress += uniform(0, girl.pclove() / 20);       // She will do what you ask

    // Negative Stats/Skills
    progress -= girl.spirit() / 25;
    if (girl.pclove() < -30)               progress -= uniform(0, -girl.pclove() / 10);      // She will not do what you want
    if (girl.happiness() < 50)             progress -= uniform(1, 5);                        // She is not feeling like it
    if (girl.health() < 50)                progress -= uniform(1, 5);                        // She is feeling sick
    if (girl.tiredness() > 50)             progress -= uniform(1, 5);                        // She is tired

    return progress;
}

void TrainingJob::ReceiveTreatment(sGirl& girl, bool is_night) const {
    auto mistress = request_interaction(TrainingInteractionId);
    if(!mistress) {
        add_text("no-mistress");
        active_shift().EventType = EVENT_WARNING;
        active_shift().EventImage = EImageBaseType::PROFILE;
        generate_event();
        return;
    }

    // Base adjustment
    tiredness() = uniform(5, 15);

    add_line("training");
    active_shift().EventType = is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT;

    HandleTraining(girl, is_night);

    girl.tiredness(tiredness());

    generate_event();
}

double TrainingJob::GetPerformance(const sGirl& girl, bool estimate) const {
    if (girl.has_active_trait(TargetTrait))    return -1000;
    return 250;
}

sJobValidResult TrainingJob::on_is_valid(const sGirl& girl, bool night_shift) const {
    if (girl.has_active_trait(TargetTrait))
    {
        return {false, get_text("is-already")};
    }
    return {true, {}};
}


void TrainingJob::CountTheDays(sGirl& girl, bool is_night, int progress) const
{
    /// if (girl.disobey_check(ACTION_WORKTRAINING, job())) progress /= 2;    // if she disobeys, half her time is wasted

    if (progress <= 0)                                // she lost time so more tired
    {
        tiredness() += uniform(5, 5-progress);
        //Enjoyment -= uniform(0, 2);
    }
    else if (progress > 33)                        // or if she trained a lot
    {
        tiredness() += uniform(progress / 4, progress / 2);
        //Enjoyment += uniform(0, 2);
    }
    else                                        // otherwise just a bit tired
    {
        tiredness() += uniform(0, progress / 3);
        //Enjoyment -= uniform(-2, 2);
    }

    girl.make_treatment_progress(progress);

    if (progress <= 0)
    {
        active_shift().EventType = EVENT_WARNING;
        OnNoProgress(girl);
    }
    else if (girl.get_treatment_progress() >= 100 && is_night)
    {
        girl.finish_treatment();
        active_shift().EventType = EVENT_GOODNEWS;
        OnComplete(girl);
        girl.FullJobReset(JOB_RESTING);
    }
    else {
        OnRegularProgress(girl, is_night);
    }
}

void TrainingJob::OnNoProgress(sGirl& girl) const {
    add_line("no-progress");
    tiredness() += uniform(5, 15);
}

void TrainingJob::OnRegularProgress(sGirl& girl, bool is_night) const {
    if (girl.get_treatment_progress() >= 100)
    {
        add_line("almost-done");
        tiredness() -= (girl.get_treatment_progress() - 100) / 2;    // her last day so she rested a bit
    }
    else {
        add_line("in-progress");
    }
}

int& TrainingJob::tiredness() const {
    return active_shift().get_var_ref(TirednessId);
}

void SoStraight::HandleTraining(sGirl& girl, bool is_night) const {
    // Positive Stats/Skills
    int progress = 0;
    progress += girl.normalsex() / 5;
    progress += girl.group() / 10;
    progress += girl.oralsex() / 20;
    progress += girl.tittysex() / 20;
    progress += girl.anal() / 20;

    progress += calculate_progress(girl);

    if (!likes_men(girl))
    {
        add_line("dislikes-men");
        progress -= girl.lesbian() / 5;                    // it is hard to change something you are good at
        tiredness() += girl.lesbian() / 10;
    }
    if (girl.has_active_trait(traits::BISEXUAL)) progress -= girl.lesbian() / 20;    // it is hard to change something you are good at

    int trait = girl.get_trait_modifier(traits::modifiers::SO_STRAIGHT);
    progress += uniform(trait / 2, trait + trait / 2);
    if (girl.has_active_trait(traits::BROKEN_WILL))        {
        add_line("broken-will");
    }

    if (!girl.is_sex_type_allowed(SKILL_NORMALSEX))      progress -= uniform(10, 30);

    CountTheDays(girl, is_night, progress);

    // Improve girl
    girl.exp( 1 + std::max(0, progress / 20) );
    girl.lesbian( uniform(-15, -2) );
    girl.normalsex( uniform(2, 15) );
    girl.group( uniform(1, 5) );
    girl.anal( uniform(1, 5) );
    girl.oralsex( uniform(1, 5) );
    girl.handjob( uniform(1, 5) );
    girl.tittysex( uniform(1, 5) );
}

void SoStraight::OnComplete(sGirl& girl) const {
    add_line("complete");
    girl.lose_trait(traits::LESBIAN);    girl.gain_trait(traits::BISEXUAL);    girl.lose_trait(traits::STRAIGHT);
}

void SoLesbian::HandleTraining(sGirl& girl, bool is_night) const {
    // Positive Stats/Skills
    int progress = 0;
    progress += girl.lesbian() / 5;
    progress += girl.group() / 20;
    progress += girl.oralsex() / 25;
    progress += calculate_progress(girl);

    if (girl.has_active_trait(traits::STRAIGHT))
    {
        add_line("dislikes-women");
        progress -= girl.normalsex() / 5;                // it is hard to change something you are good at
        tiredness() += girl.normalsex() / 10;
    }
    if (girl.has_active_trait(traits::BISEXUAL)) progress -= girl.normalsex() / 20;                    // it is hard to change something you are good at

    int trait = girl.get_trait_modifier(traits::modifiers::SO_LESBIAN);
    progress += uniform(trait / 2, trait + trait / 2);

    if (girl.has_active_trait(traits::BROKEN_WILL))    {
        add_line("broken-will");
    }

    //    if (girl.check_virginity())                {}

    if (!girl.is_sex_type_allowed(SKILL_LESBIAN))        progress -= uniform(10, 30);

    CountTheDays(girl, is_night, progress);

    // Improve girl
    girl.exp( 1 + std::max(0, progress / 20) );
    girl.lesbian( uniform(2, 15) );
    girl.normalsex( uniform(-2, -15) );
    girl.group( uniform(1, -3) );
    girl.anal( uniform(-1, -5) );
    girl.oralsex( uniform(1, -3) );
    girl.handjob( uniform(1, -3) );
    girl.tittysex( uniform(-1, -5) );
}

void SoLesbian::OnComplete(sGirl& girl) const {
    add_text("complete");
    girl.gain_trait(traits::LESBIAN);    girl.lose_trait(traits::BISEXUAL);    girl.lose_trait(traits::STRAIGHT);
}

void SoBi::HandleTraining(sGirl& girl, bool is_night) const {
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
    progress += calculate_progress(girl);

    int trait = girl.get_trait_modifier(traits::modifiers::SO_BI);
    progress += uniform(trait / 2, trait + trait / 2);
    if (girl.has_active_trait(traits::BROKEN_WILL))    {
        add_line("broken-will");
    }

    if (!girl.is_sex_type_allowed(SKILL_LESBIAN))        progress -= uniform(5, 15);
    if (!girl.is_sex_type_allowed(SKILL_NORMALSEX))      progress -= uniform(5, 15);

    CountTheDays(girl, is_night, progress);

    girl.exp( 1 + std::max(0, progress / 20) );
    girl.lesbian( uniform(1, 10) );
    girl.normalsex( uniform(1, 10) );
    girl.group( uniform(2, 15) );
    girl.anal( uniform(0, 5) );
    girl.oralsex( uniform(0, 5) );
    girl.handjob( uniform(0, 5) );
    girl.tittysex( uniform(0, 3) );
}

void SoBi::OnComplete(sGirl& girl) const {
    add_line("complete");
    girl.lose_trait(traits::LESBIAN);    girl.gain_trait(traits::BISEXUAL);    girl.lose_trait(traits::STRAIGHT);
}

void FakeOrg::HandleTraining(sGirl& girl, bool is_night) const {
    // Positive Stats/Skills
    int progress = 0;
    progress += girl.performance() / 5;
    progress += girl.group() / 20;
    progress += girl.normalsex() / 20;
    progress += girl.lesbian() / 20;
    progress += calculate_progress(girl);

    int trait = girl.get_trait_modifier(traits::modifiers::FAKE_ORGASM);
    progress += uniform(trait / 2, trait + trait / 2);
    if (girl.has_active_trait(traits::BROKEN_WILL))    { add_line("broken-will"); }

    if (!girl.is_sex_type_allowed(SKILL_NORMALSEX))      progress -= uniform(5, 15);

    CountTheDays(girl, is_night, progress);

    // Improve girl
    girl.exp( 1 + std::max(0, progress / 20) );
    girl.lesbian( uniform(0, 5) );
    girl.normalsex( uniform(0, 5) );
    girl.group( uniform(0, 5) );
    girl.anal( std::max(0, uniform(-2, 2)) );
    girl.performance( uniform(3, 15) );
    girl.confidence( uniform(-1, 5) );
    girl.constitution( std::max(0, uniform(-2, 1)) );
    girl.spirit( uniform(-5, 5) );
}

void FakeOrg::OnNoProgress(sGirl& girl) const {
    add_line("no-progress");
    tiredness() += uniform(5, 15);
}

void FakeOrg::OnComplete(sGirl& girl) const {
    add_line("complete");
    girl.lose_trait(traits::SLOW_ORGASMS);    girl.lose_trait(traits::FAST_ORGASMS);    girl.gain_trait(traits::FAKE_ORGASM_EXPERT);
}

void FakeOrg::OnRegularProgress(sGirl& girl, bool is_night) const {
    auto& ss = active_shift().EventMessage;
    int status = girl.get_treatment_progress();
    if (status >= 100)    tiredness() -= (status - 100) / 2;    // her last day so she rested a bit
    else                  ss << "Training in progress (" << status << "%).\n \n";
    if (status < 25)      ss << "She has no idea what she sounds like during sex, but it ain't orgasmic.";
    else if (status < 50) ss << "When she realizes she should finish, you can see it click in her mind and easily notice her changing things up.";
    else if (status < 75) ss << "She is still not getting into rhythm with her partner but it still seems enjoyable.";
    else                  ss << "She is almost there but you want her to practice a little more to get it perfect.";

    if (!is_night)
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
