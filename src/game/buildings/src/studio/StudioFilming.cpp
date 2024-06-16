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

#include "studio/StudioJobs.h"
#include "character/sGirl.h"
#include "cGirls.h"
#include "character/cPlayer.h"
#include "character/cCustomers.h"
#include "studio/cMovieStudio.h"
#include "IGame.h"
#include "CLog.h"
#include "xml/getattr.h"
#include "jobs/IBuildingShift.h"

extern const char* const DirectorInteractionId;
extern const char* const CamMageInteractionId;
extern const char* const CrystalPurifierInteractionId;
extern const char* const FluffPointsId;
extern const char* const StageHandPtsId;

bool cFilmSceneJob::CheckCanWork(cGirlShift& shift) const {
    auto& bs = shift.shift();
    auto& girl = shift.girl();
    auto brothel = dynamic_cast<sMovieStudio*>(&shift.building());
    if(!brothel) {
        g_LogFile.error("jobs", shift.girl().FullName(), " was not at the movie studio when doing movie job.");
        return false;
    }

    // other conditions in which she cannot work:
    if (girl.health() < m_MinimumHealth)
    {
        shift.add_text("crew.refuse.health");
        return false;
    }

    if(m_RefuseIfPregnant && girl.is_pregnant()) {
        shift.add_text("crew.refuse.pregnant");
        return false;
    }

    // check sex type
    if(m_Pleasure.Skill != NUM_SKILLS) {
        if(!girl.is_sex_type_allowed(m_Pleasure.Skill)) {
            girl.AddMessage("A scene of this type cannot be filmed, because you have forbidden the corresponding sex type.",
                            EImageBaseType::PROFILE, EVENT_NOWORK);
            return false;
        }
    }

    return true;
}

namespace {
    int lust_influence(const sFilmPleasureData& data, const sGirl& girl) {
        if(data.Skill == NUM_SKILLS)
            return 0;

        // calculate the percentage of the skill between SkillMin and 100
        int skill = girl.get_skill(data.Skill) - data.SkillMin;
        float skill_factor = static_cast<float>(skill) / static_cast<float>(100 - data.SkillMin);
        if(skill_factor < 0) {
            skill_factor = 0.f;
        }
        // a non-linear transformation
        // the 0.09 makes the curve a bit smoother
        skill_factor = std::sqrt(0.09f + skill_factor) / std::sqrt(1.09f);

        int lib_value = girl.lust() * (data.Factor + data.BaseValue) * skill_factor;
        return lib_value / 100 - data.BaseValue;
    }
}

bool cFilmSceneJob::CheckRefuseWork(cGirlShift& shift) const {

    disobey_check(shift);
    if(shift.data().Refused != ECheckWorkResult::ACCEPTS) {
        if(m_CanBeForced) {
            return RefusedTieUp(shift);
        } else  {
            shift.add_text("refuse");
            return true;
        }
    }
    return shift.data().Refused != ECheckWorkResult::ACCEPTS;
}

bool cFilmSceneJob::RefusedTieUp(cGirlShift& shift) const {
    auto& girl = shift.girl();
    bool is_forced = false;
    if (girl.is_slave())
    {
        if (g_Game->player().disposition() > 30)  // nice
        {
            shift.add_text("disobey.slave.nice");
            girl.pclove(1);
            girl.pcfear(-1);
            girl.pclove(1);
            girl.obedience(-1);
            return true;
        }
        else if (g_Game->player().disposition() > -30) //pragmatic
        {
            shift.add_text("disobey.slave.neutral");
            girl.pclove(-1);
            girl.pclove(-2);
            girl.pcfear(1);
            g_Game->player().disposition(-1);
            shift.data().Enjoyment -= 5;
            is_forced = true;
        }
        else
        {
            shift.add_text("disobey.slave.evil");
            girl.pclove(-4);
            girl.pclove(-5);
            girl.pcfear(+5);
            girl.spirit(-1);
            g_Game->player().disposition(-2);
            shift.data().Enjoyment -= 10;
            is_forced = true;
        }
    }
    else // not a slave
    {
        shift.add_text("disobey.free");
        return true;
    }

    if(is_forced) {
        shift.set_variable(m_IsForced_id, 1);
        girl.upd_temp_skill(SKILLS::SKILL_PERFORMANCE, -100);
        girl.upd_temp_stat(STATS::STAT_CHARISMA, -100);
        shift.data().DebugMessage << "Girl is tied. Job performance reduced from " << shift.performance() << " to ";
        shift.data().Performance = GetPerformance(girl, false) - 10;
        shift.data().DebugMessage << shift.performance() << "\n";
        girl.upd_temp_skill(SKILLS::SKILL_PERFORMANCE, 100);
        girl.upd_temp_stat(STATS::STAT_CHARISMA, 100);
    }

    return false;
}

bool cFilmSceneJob::check_is_forced(const cGirlShift& shift) const {
    return std::get<int>(shift.get_variable(m_IsForced_id));
}

void cFilmSceneJob::DoWork(cGirlShift& shift) const {
    auto brothel = dynamic_cast<sMovieStudio*>(&shift.building());

    // No film crew... then go home
    if (!shift.has_interaction(DirectorInteractionId) ||
        !shift.has_interaction(CamMageInteractionId)  ||
        !shift.has_interaction(CrystalPurifierInteractionId) )
    {
        if(shift.shift().NumInteractors(DirectorInteractionId) != 0 && shift.shift().NumInteractors(CamMageInteractionId) != 0 &&
                shift.shift().NumInteractors(CrystalPurifierInteractionId) != 0) {
            shift.add_literal("There were more scenes scheduled for filming today than you crew could handle.");
        } else {
            shift.add_literal("There was no crew to film the scene. You need at least a Director, a Camera Mage,"
                              "and a Crystal Purifier to film a scene.");
        }
        // still, we notify the building that we wanted these interactions.
        // TODO maybe have a separate function for this.
        shift.request_interaction(DirectorInteractionId);
        shift.request_interaction(CamMageInteractionId);
        shift.request_interaction(CrystalPurifierInteractionId);
        shift.set_result(ECheckWorkResult::IMPOSSIBLE);
        return;
    }

    auto& girl = shift.girl();
    assert(brothel);

    // pre-work processing
    PreFilmCallback(shift);

    int roll = shift.d100();
    if (roll <= 10) { shift.data().Enjoyment -= shift.uniform(1, 4);    }
    else if (roll >= 90) { shift.data().Enjoyment += shift.uniform(1, 4); }
    else { shift.data().Enjoyment += shift.uniform(0, 2); }

    int bonus_enjoy = 0;
    if (shift.performance() >= 200)
    {
        bonus_enjoy = shift.uniform(9, 14);
    }
    else if (shift.performance() >= 100)
    {
        int offset = (shift.performance() - 100) / 10;
        bonus_enjoy = shift.uniform(offset / 2, 2 + offset);
    }
    else
    {
        bonus_enjoy = -shift.uniform(3, 6);
    }
    shift.data().Enjoyment += bonus_enjoy;
    shift.data().DebugMessage << "  Perf base Enjoy: " << bonus_enjoy << "\n";

    shift.add_literal("\n");

    cGirls::UnequipCombat(girl);

    Narrate(shift);

    shift.data().DebugMessage << "Performance: " << shift.performance() << "\n";

    int quality = shift.performance() * 85 / 300;
    quality += girl.get_trait_modifier(traits::modifiers::MOVIE_QUALITY);

    // post-work text
    PostFilmCallback(shift);


    if(m_SexAction != SexAction::NONE) {
        if(girl.lose_trait(traits::VIRGIN)) {
            shift.add_literal("She is no longer a virgin.\n");
            quality += 10;
        }

        if(m_SexAction == SexAction::HUMAN) {
            sCustomer Cust = g_Game->GetCustomer(*brothel);
            Cust.m_Amount = 1;
            Cust.m_IsWoman = false;
            if (!girl.calc_pregnancy(Cust, 1.0)) {
                g_Game->push_message(girl.FullName() + " has gotten pregnant", 0);
            }
        } else if (m_SexAction == SexAction::MONSTER) {
            if (!girl.calc_insemination(cGirls::GetBeast(), 1.0))
                g_Game->push_message(girl.FullName() + " has gotten inseminated", 0);
        }
    }

    shift.add_literal("\n");

    // consequences of forcing are loss of iron will and potential gain of Mind Fucked
    bool is_forced = check_is_forced(shift);
    if (is_forced) {
        cGirls::PossiblyLoseExistingTrait(girl, traits::IRON_WILL, 15, "${name}'s unwilling degradation has shattered her Iron Will.", EImageBaseType::TORTURE);
        cGirls::PossiblyLoseExistingTrait(girl, traits::MIND_FUCKED, 15, "${name}'s has become Mind Fucked from the forced degradation.", EImageBaseType::TORTURE, EVENT_WARNING);
    }

    // independent of the scene description, charisma and beauty change the rating a bit
    quality += (girl.charisma() - 50) / 10;
    quality += (girl.beauty() - 50) / 10;
    quality += girl.level();

    if(girl.is_unpaid()) {
        quality = quality * 90 / 100;
    }

    try {
        auto& scene = film_scene(g_Game->movie_manager(), shift, quality, m_SceneType, is_forced);
    } catch (std::runtime_error& error)  {
        g_Game->error(error.what());
    }

    // sImageSpec spec = girl.MakeImageSpec(m_EventImage);
    // if(is_forced)  spec.IsTied = ETriValue::Yes;
    // TODO handle image spec
    // girl.AddMessage(ss.str(), spec, EVENT_DAYSHIFT);
    shift.generate_event();
}

void cFilmSceneJob::PrintPerfSceneEval(cGirlShift& shift) const {
    if (shift.performance() >= 300)
    {
        shift.add_text("work.perfect");
        shift.add_literal("\n\nIt was an excellent scene.");
    }
    else if (shift.performance() >= 220)
    {
        shift.add_text("work.great");
        shift.add_literal("\n\nIt was a great scene.");
    }
    else if (shift.performance() >= 175)
    {
        shift.add_text("work.good");
        shift.add_literal("\n\nIt was a good scene.");
    }
    else if (shift.performance() >= 135)
    {
        shift.add_text("work.ok");
        shift.add_literal("\n\nIt was an OK scene.");
        // <Text>Overall, it was an solid scene.</Text>
        // <Text>Overall, it wasn't a bad scene.</Text>
    }
    else if (shift.performance() >= 90)
    {
        shift.add_text("work.bad");
        shift.add_literal("\n\nIt was a bad scene.");
        // <Text>It wasn't a great scene.</Text>
    }
    else
    {
        shift.add_text("work.worst");
        shift.add_literal("\n\nIt was an abysmal scene.");
        // <Text>It was a poor scene.</Text>
        //  Terrible scene.
    }

    shift.add_literal("\n");
}

void cFilmSceneJob::PrintForcedSceneEval(cGirlShift& shift) const {
    shift.add_text("forced-filming");
}

cFilmSceneJob::cFilmSceneJob(JOBS job, const char* xml, SceneType scene, SexAction sex) :
    cBasicJob(job, xml), m_SceneType(scene), m_SexAction(sex) {

    m_Info.Consumes.emplace_back(DirectorInteractionId);
    m_Info.Consumes.emplace_back(CamMageInteractionId);
    m_Info.Consumes.emplace_back(CrystalPurifierInteractionId);
    m_Info.Consumes.emplace_back(FluffPointsId);
    m_Info.Consumes.emplace_back(StageHandPtsId);

    m_IsForced_id = RegisterVariable("IsForced", 0);
}

void cFilmSceneJob::load_from_xml_callback(const tinyxml2::XMLElement& job_element) {
    m_MinimumHealth = job_element.IntAttribute("MinimumHealth", -100);
    m_RefuseIfPregnant = job_element.IntAttribute("RefuseIfPregnant", 0);
    m_CanBeForced = job_element.IntAttribute("CanBeForced", 0);

    // check that we have the corresponding text elements
    if(m_CanBeForced) {
        if(!has_text("disobey.free") || !has_text("disobey.slave.nice") || !has_text("disobey.slave.neutral") ||
           !has_text("disobey.slave.evil") || !has_text("forced-filming")) {
            g_LogFile.warning("job", "Job ", get_job_name(job()), " is missing disobey texts");
        }
    }
}

void cFilmSceneJob::PostFilmCallback(cGirlShift& shift) const {
    if(has_text("post-work-event")) {
        shift.add_text("post-work-event");
    }
}

void cFilmSceneJob::PreFilmCallback(cGirlShift& shift) const {
    if(has_text("pre-work-event")) {
        shift.add_text("pre-work-event");
    }
}

void cFilmSceneJob::Narrate(cGirlShift& shift) const {
    if(check_is_forced(shift)) {
        PrintForcedSceneEval(shift);
    } else {
        PrintPerfSceneEval(shift);
    }
}

SKILLS cFilmSceneJob::GetSexType() const {
    return m_Pleasure.Skill;
}

