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


#include "ClinicJobs.h"
#include "cGirls.h"
#include "character/sGirl.h"
#include "buildings/cBuilding.h"
#include "IGame.h"
#include <sstream>
#include <utility>
#include "character/predicates.h"
#include "text/repo.h"
#include "CLog.h"
#include "xml/getattr.h"
#include "xml/util.h"
#include "queries.h"

namespace settings {
    extern const char* PREG_COOL_DOWN;
}

IMedicalJob::IMedicalJob(std::string xml_file) : cTreatmentJob(std::move(xml_file)) {
    m_Info.Consumes.emplace_back(DoctorInteractionId);
    m_Info.Consumes.emplace_back(CarePointsBasicId);
    m_Info.Consumes.emplace_back(CarePointsGoodId);
}

void IMedicalJob::HandleWorkImpossible(cGirlShift& shift) const {
    if(shift.data().Refused == ECheckWorkResult::INVALID) {
        shift.girl().FullJobReset(JOB_RESTING);
        shift.add_literal(" She was sent to the waiting room.");
    } else {
        shift.data().Job = JOB_RESTING;
    }
    shift.data().EventType = EVENT_WARNING;
    shift.data().EventImage = EImageBaseType::PROFILE;
    shift.generate_event();
}

void SurgeryJob::load_from_xml_callback(const tinyxml2::XMLElement& job_element) {
    m_SurgeryData.Duration = GetIntAttribute(job_element, "Duration");
}

void SurgeryJob::DoWork(cGirlShift& shift) const {
    shift.data().EventType = shift.is_night_shift() ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT;
    auto& girl = shift.girl();

    auto doctor = shift.request_interaction(DoctorInteractionId);
    if (!doctor && !shift.is_night_shift()) {
        shift.add_text("surgery.lack-of-doctors");
        shift.data().EventType = EVENT_WARNING;
        shift.data().EventImage = EImageBaseType::PROFILE;
        shift.generate_event();
        shift.data().Refused = ECheckWorkResult::IMPOSSIBLE;
        return;
    }

    shift.add_line("surgery");
    shift.add_literal("\n");

    // update progress
    if (!shift.is_night_shift())    // the Doctor works on her during the day
    {
        if(girl.health() < 50) {
            int bp = shift.uniform(4, 7);
            if(shift.try_consume_resource(CarePointsBasicId, bp)) {
                int req_gp = shift.uniform(2, 4);
                if(shift.try_consume_resource(CarePointsGoodId, req_gp)) {
                    shift.add_text("too-sick.quality-nurses");
                    girl.health( shift.uniform(5, 10) );
                    girl.happiness(1);
                } else {
                    shift.add_text("too-sick.basic-nurses");
                    girl.health( shift.uniform(3, 5) );
                }
            } else {
                shift.add_text("too-sick.no-nurses");
                girl.health( shift.uniform(1, 3) );
                girl.make_treatment_progress( -shift.uniform(2, 8) );
            }
            girl.tiredness(-shift.uniform(5, 10));
            if(girl.strength() > 50) {
                girl.strength(-shift.uniform(0, 2));
            }
            shift.generate_event();
            return;
        }

        girl.make_treatment_progress(g_Dice.closed_uniform(90, 110) / m_SurgeryData.Duration);
        // TODO this is an interaction, fix it up
        nursing_effect(shift);
        doctor->AddMessage(girl.Interpolate(get_text("doctor.work", shift)), EImageBaseType::NURSE, EEventType::EVENT_DAYSHIFT);
        doctor->exp(5);
    } else    // and if there are nurses on duty, they take care of her at night
    {
        int req_gp = shift.uniform(3, 6);
        if(shift.try_consume_resource(CarePointsGoodId, req_gp)) {
            shift.consume_resource(CarePointsBasicId, req_gp);
            girl.make_treatment_progress(g_Dice.closed_uniform(3, 8));
            shift.add_text("recovery.great");
            girl.health( g_Dice.closed_uniform(25, 50) / m_SurgeryData.Duration );
        } else {
            if(!nursing_effect(shift) && shift.chance(50)) {
                girl.make_treatment_progress(-g_Dice.closed_uniform(5, 10));
                shift.add_text("recovery.complications");
            }
        }
    }

    additional_progress(shift.girl());

    // process progress
    if (girl.get_treatment_progress() < 100 && !shift.is_night_shift()) {
        shift.add_literal("\nThe operation is in progress (${progress}).\n");
    } else if(girl.get_treatment_progress() >= 100 && shift.is_night_shift()) {
        shift.data().EventType = EVENT_GOODNEWS;
        girl.finish_treatment();
        shift.add_literal("\n");
        shift.add_line("surgery.success");
        on_complete_treatment(shift);
        success(girl);
    }

    shift.generate_event();

    // Improve girl
    if (girl.has_active_trait(traits::MASOCHIST)) girl.lust(1);

    if (shift.chance(10.f)) {
        // `J` she watched what the doctors and nurses were doing
        girl.gain_attribute(SKILL_MEDICINE, 0, 2, 15);
    }
}

void SurgeryJob::on_fully_finished_treatment(cGirlShift& shift) const {
    shift.add_literal("\n\nShe has been released from the Clinic.");
    shift.girl().FullJobReset(JOB_RESTING);
}

bool SurgeryJob::nursing_effect(cGirlShift& shift) const {
    auto& girl = shift.girl();
    int health_dmg = shift.uniform(4, 7);
    health_dmg -= shift.consume_resource(CarePointsBasicId, health_dmg);

    if(health_dmg > 0) {
        if(girl.gain_trait(traits::SMALL_SCARS, 2)) {
            shift.add_text("surgery.lack-of-nurses.scars");
            girl.health(-10);
            girl.tiredness(10);
            girl.happiness(-10);
            girl.strength(-shift.uniform(0, 5));
            girl.spirit(-2);
        } else {
            shift.add_text("surgery.lack-of-nurses.health");
            girl.health(-health_dmg);
            girl.tiredness(2 * health_dmg);
            girl.happiness(-5);
            girl.beauty(shift.chance(20) ? -1 : 0);
        }
        return false;
    }

    return true;
}

// ---------------------------------------------------------------------------------------------------------------------

BreastReduction::BreastReduction() : SurgeryJob("BreastReduction.xml") {
}

bool BreastReduction::check_custom_treatment(const sGirl& target) const {
    return target.breast_size() > 1;
}

void BreastReduction::handle_custom_treatment(cGirlShift& shift) const {
    shift.data().EventMessage << cGirls::AdjustTraitGroupBreastSize(shift.girl(), -1, false) << "\n\n";
}

BoobJob::BoobJob() : SurgeryJob("BoobJob.xml") {
}

bool BoobJob::check_custom_treatment(const sGirl& target) const {
    return target.breast_size() < 10;
}

void BoobJob::handle_custom_treatment(cGirlShift& shift) const {
    shift.data().EventMessage << cGirls::AdjustTraitGroupBreastSize(shift.girl(), 1, false) << "\n\n";
}

FaceLift::FaceLift() : SurgeryJob("FaceLift.xml") {
}

bool FaceLift::check_custom_treatment(const sGirl& target) const {
    return target.age() > 21;
}

void FaceLift::handle_custom_treatment(cGirlShift& shift) const {
    auto& target = shift.girl();
    target.beauty(shift.rng().bell(4, 10));
    target.charisma(shift.rng().bell(-1, 1));
    target.age(shift.rng().bell(-2, -1));
    if (target.age() <= 18) target.set_stat(STAT_AGE, 18);
}

TubesTied::TubesTied(): SurgeryJob("TubesTied.xml") {
}

sJobValidResult TubesTied::on_is_valid(const sGirl& girl, bool night) const {
    if (girl.is_pregnant()) {
        return {false, "${name} is pregnant.\nShe must either have her baby or get an abortion before she can get her Tubes Tied."};
    }
    return SurgeryJob::on_is_valid(girl, night);
}

Fertility::Fertility(): SurgeryJob("Fertility.xml") {
}

sJobValidResult Fertility::on_is_valid(const sGirl& girl, bool is_night) const {
    if (girl.is_pregnant()) {
        return {false, "${name} is pregnant.\n"
                       "She must either have her baby or get an abortion before she can get receive any more fertility treatments."};
    }

    return SurgeryJob::on_is_valid(girl, is_night);
}

bool Fertility::check_custom_treatment(const sGirl& target) const  {
    return !target.has_active_trait(traits::BROODMOTHER);
}

void Fertility::handle_custom_treatment(cGirlShift& shift) const {
    shift.data().EventMessage << cGirls::AdjustTraitGroupFertility(shift.girl(), 1, false);
}

CureDiseases::CureDiseases() : SurgeryJob("CureDiseases.xml") {
}

void CureDiseases::additional_progress(sGirl& girl) const {
    girl.make_treatment_progress(girl.constitution() / 10 + girl.get_trait_modifier(traits::modifiers::DISEASE_RECOVERY));
}

Abortion::Abortion() : IMedicalJob("Abortion.xml") {
}

namespace
{
    void process_happiness(std::stringstream& ss, sGirl& girl, int happy, int& love, std::array<const char*, 7> messages) {
        if (happy < -50){
            ss << messages[0];
            love -= 10;
            girl.add_temporary_trait(traits::PESSIMIST, 20);
        } else if (happy < -25) {
            ss << messages[1];
            love -= 5;
        } else if (happy < -5){
            ss << messages[2];
        } else if (happy < 10) {
            ss << messages[3];
        } else if (happy < 25) {
            ss << messages[4];
        } else if (happy < 50) {
            ss << messages[5];
        } else {
            ss << messages[6];
            love += 5;
            girl.add_temporary_trait(traits::OPTIMIST, 20);
        }

        ss << "\n";
    }
}

void Abortion::DoWork(cGirlShift& shift) const {
    sGirl* doctor = shift.request_interaction(DoctorInteractionId);
    auto& girl = shift.girl();
    if (!doctor) {
        shift.add_literal("There is no doctor available to perform ${name}'s abortion!");
        shift.data().Refused = ECheckWorkResult::IMPOSSIBLE;
        shift.data().EventType = EVENT_WARNING;
        shift.data().EventImage = EImageBaseType::PROFILE;
        shift.generate_event();
        return;    // not refusing
    }
    shift.add_literal("${name} is in the Clinic to get an abortion.\n\n");

    shift.data().EventType = shift.is_night_shift() ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT;

    if (!shift.is_night_shift())    // the Doctor works on her during the day
    {
        girl.make_treatment_progress(33);
    }
    else    // and if there are nurses on duty, they take care of her at night
    {
        if (shift.try_consume_resource(CarePointsBasicId, 5))
        {
            girl.make_treatment_progress(25);
            girl.happiness(5);
            girl.mana(5);
        }
    }

    if (girl.get_treatment_progress() >= 100 && shift.is_night_shift())
    {
        shift.add_literal("The girl had an abortion.\n");
        shift.data().EventType = EVENT_GOODNEWS;

        // `J` first set the base stat modifiers
        int happy = -10, health = -20, mana = -20, spirit = -5, love = -10;

        if (shift.try_consume_resource(CarePointsBasicId, 3))
        {
            shift.add_literal("The Nurse tried to keep her healthy and happy during her recovery.\n");
            // `J` then adjust if a nurse helps her through it
            happy += 10;    health += 10;    mana += 10;    spirit += 5;    love += 2;
        }
        else
        {
            shift.add_literal("She is sad and has lost some health during the operation.\n");
        }

        happy += girl.get_trait_modifier(traits::modifiers::PREG_ABORT_HAPPY);
        love += girl.get_trait_modifier(traits::modifiers::PREG_ABORT_LOVE);
        spirit += girl.get_trait_modifier(traits::modifiers::PREG_ABORT_SPIRIT);

        // `J` next, check traits
        if (girl.has_active_trait(traits::FRAGILE))        // natural adj
        {
            health -= 5;
        }
        if (girl.has_active_trait(traits::TOUGH))        // natural adj
        {
            health += 5;
        }

        // `J` finally see what type of pregnancy it is and get her reaction to the abortion.
        if (girl.has_status(STATUS_PREGNANT))
        {
            process_happiness(shift.data().EventMessage, girl, happy, love,
                              {"She is very distraught about the loss of her baby.",
                               "She is distraught about the loss of her baby.",
                               "She is sad about the loss of her baby.",
                               "She accepts that she is not ready to have her baby.",
                               "She is glad she is not going to have her baby.",
                               "She is happy she is not going to have her baby.",
                               "She is overjoyed not to be forced to carry her baby."
                              });
        }
        else if (girl.has_status(STATUS_PREGNANT_BY_PLAYER))
        {
            // `J` adjust her happiness by her hate-love for you
            happy -= (girl.pclove() + love) / 2;
            if (girl.has_active_trait(traits::YOUR_WIFE))// "Why?"
            {
                happy -= 20;    spirit -= 1;    love -= 3;
            }

            process_happiness(shift.data().EventMessage, girl, happy, love,
                              {"She is very distraught about the loss of your baby.",
                               "She is distraught about the loss of your baby.",
                               "She is sad about the loss of your baby.",
                               "She accepts that she is not ready to have your baby.",
                               "She is glad she is not going to have your baby.",
                               "She is happy she is not going to have your baby.",
                               "She is overjoyed not to be forced to carry your hellspawn."
                              });
        }
        else if (girl.has_status(STATUS_INSEMINATED))
        {
            // `J` Some traits would react differently to non-human pregnancies.
            happy += girl.get_trait_modifier(traits::modifiers::INSEMINATED_ABORT_HAPPY);
            love += girl.get_trait_modifier(traits::modifiers::INSEMINATED_ABORT_LOVE);

            if (girl.has_active_trait(traits::ANGEL))        // "DEAR GOD, WHAT WAS THAT THING?"
            {
                spirit -= 5;    mana -= 5;
            }

            process_happiness(shift.data().EventMessage, girl, happy, love,
                              {"She is very distraught about the loss of the creature growing inside her.",
                               "She is distraught about the loss of the creature growing inside her.",
                               "She is sad about the loss of the creature growing inside her.",
                               "She accepts that she is not ready to bring a strange creature into this world.",
                               "She is glad she is not going to have to carry that strange creature inside her.",
                               "She is happy she is not going to have to carry that strange creature inside her.",
                               "She is overjoyed not to be forced to carry that hellspawn anymore."
                              });
        }
        // `J` now apply all the stat changes and finalize the transaction
        girl.happiness(happy);
        girl.health(health);
        girl.mana(mana);
        girl.spirit(spirit);
        girl.pclove(love);

        girl.m_ChildrenCount[CHILD09_ABORTIONS]++;
        girl.clear_pregnancy();
        girl.m_PregCooldown = g_Game->settings().get_integer(settings::PREG_COOL_DOWN);
        girl.FullJobReset(JOB_RESTING);
        girl.finish_treatment();
    }
    else
    {
        shift.add_literal("The abortion is in progress (1 day remaining).");
    }

    shift.generate_event();

    girl.libido(-2);
    girl.lust_turn_off(10);
}

double Abortion::GetPerformance(const sGirl& girl, bool estimate) const {
    if (!girl.is_pregnant())              return -1000;    // X - not needed
    if (girl.carrying_players_child())    return 1;        // E - its yours
    if (girl.carrying_monster())          return 150;      // B - Beast
    return 100;                                            // C - customer's child
}

Healing::Healing() : cGenericJob(JOB_GETHEALING, "Healing.xml", EJobClass::TREATMENT) {
}

double Healing::GetPerformance(const sGirl& girl, bool estimate) const {
    return 100;
}

void Healing::DoWork(cGirlShift& shift) const {
    auto& girl = shift.girl();
    sGirl* doctor = shift.request_interaction(DoctorInteractionId);

    // `J` base recovery copied free time recovery
    int health = 10 + (girl.constitution() / 10);
    int tiredness = shift.uniform(10, 30);    // build up as positive, then apply as negative
    int happy = shift.uniform(10, 20);
    int mana = 5 + (girl.magic() / 5);

    if (doctor)
    {
        shift.data().EventMessage << " Doctor " << doctor->FullName() << " takes care of her.";
        if (girl.has_active_trait(traits::HALF_CONSTRUCT) || girl.has_active_trait(traits::CONSTRUCT)) {
            health += 20;    // Less healing for constructs
        } else {
            health += 30;
        }

        if(shift.try_consume_resource(CarePointsBasicId, 3)) {
            health += 10;
        }
    } else {
        if(shift.try_consume_resource(CarePointsBasicId, 3)) {
            shift.data().EventMessage << "You don't have enough doctors on duty, so ${name} is just cared for by the nurses.";
            health += 10;
        } else {
            shift.data().EventMessage << "You don't have sufficient Doctors or Nurses on duty so ${name} just rests in her hospital bed.";
            happy -= 5;
        }
    }

    girl.upd_base_stat(STAT_HEALTH, health, false);
    girl.upd_base_stat(STAT_TIREDNESS, -tiredness, false);
    girl.happiness(happy);
    girl.mana(mana);

    // send her to the waiting room when she is healthy
    if (girl.health() > 90 && girl.tiredness() < 10)
    {
        shift.data().EventMessage << "\n \nShe has been released from the Clinic.";
        girl.m_DayJob = JOB_RESTING;
        girl.m_NightJob = JOB_RESTING;
    }

    shift.data().EventType = shift.is_night_shift() ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT;
    shift.data().EventImage = EImageBaseType::PROFILE;
    shift.generate_event();
}
