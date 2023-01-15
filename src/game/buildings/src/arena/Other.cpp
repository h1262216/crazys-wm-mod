/*
 * Copyright 2023, The Pink Petal Development Team.
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

#include "cArena.h"
#include "Constants.h"
#include "IGame.h"
#include "buildings/IBuildingShift.h"
#include "character/cPlayer.h"
#include "cGirlGangFight.h"
#include "jobs/cJobManager.h"

CityGuard::CityGuard() : cSimpleJob(JOB_CITYGUARD, "ArenaCityGuard.xml") {
    CatchThiefID = RegisterVariable("CatchThief", 0);
    m_Info.PrimaryAction = EBasicActionType::FIGHTING;
}

void CityGuard::JobProcessing(sGirl& girl, sGirlShiftData& shift) const {
    auto& ss = active_shift().EventMessage;
    int roll_a = d100();
    int enjoy = 0, enjoyc = 0, sus = 0;

    int catch_thief_value = girl.agility() / 2 + uniform(0, girl.constitution() / 2);;
    shift.set_var(CatchThiefID, catch_thief_value);
    shift.Earnings = 30 + catch_thief() / 5;

    if (roll_a >= 50)
    {
        add_text("event.calm");
        sus -= 5;
    }
    else if (roll_a >= 25)
    {
        int chance_catch = std::max(5, std::min(95, 2 * catch_thief() - 50));
        if(chance(chance_catch)) {
            add_text("event.caught-thief");
            enjoy += 2;
            sus -= 10;
            shift.Earnings += 25;
        } else {
            enjoy -= 2;
            sus += 5;
            add_text("event.lost-thief");
            girl.gain_attribute(STATS::STAT_AGILITY, 0, 1, 33);
            girl.gain_attribute(STATS::STAT_CONSTITUTION, 0, 1, 66);
        }
    }
    else
    {
        auto tempgirl = g_Game->CreateRandomGirl(SpawnReason::ARENA);
        if (tempgirl)        // `J` reworked incase there are no Non-Human Random Girls
        {
            auto outcome = GirlFightsGirl(girl, *tempgirl);
            shift.EventImage = EImageBaseType::COMBAT;
            if (outcome == EFightResult::VICTORY)    // she won
            {
                enjoy += 3; enjoyc += 3;
                ss << "${name} ran into some trouble and ended up in a fight. She was able to win.";
                sus -= 20;
                shift.Earnings += 25;
                shift.Tips += 25;
            }
            else  // she lost or it was a draw
            {
                ss << "${name} ran into some trouble and ended up in a fight. She was unable to win the fight.";
                enjoy -= 1; enjoyc -= 1;
                sus += 10;
                shift.Earnings -= 20;
            }
        }
        else
        {
            g_LogFile.log(ELogLevel::ERROR, "You have no Random Girls for your girls to fight");
            ss << "There were no criminals around for her to fight.\n \n";
            ss << "(Error: You need a Random Girl to allow WorkCityGuard randomness)";
        }
    }

    g_Game->player().suspicion(sus);

    // Improve girl
    girl.upd_Enjoyment(ACTION_WORKSECURITY, enjoy);
    girl.upd_Enjoyment(ACTION_COMBAT, enjoyc);

    apply_gains(shift.Performance);
}

int CityGuard::catch_thief() const {
    return GetVariable(CatchThiefID);
}

Medic::Medic() : cSimpleJob(JOB_MEDIC, "ArenaMedic.xml") {
    m_Info.PrimaryAction = EBasicActionType::MEDICAL;
}

sJobValidResult Medic::on_is_valid(const sGirl& girl, bool night_shift) const {
    if (girl.medicine() < 33 || girl.intelligence() < 33)
    {
        return {false, "${name} does not have enough training to work as a Medic."};
    }
    return {true, {}};
}

void Medic::JobProcessing(sGirl& girl, sGirlShiftData& shift) const {
    int re_sus = performance_based_lookup(0, 0, 1, 1, 2, 2);
    if(re_sus > 0) {
        provide_interaction(ResuscitateId, re_sus);
    }
    if(is_night_shift()) {
        int surgery_amount = performance_based_lookup(0, 0, 10, 15, 25, 33);
        provide_resource(SurgeryId, surgery_amount);
        add_text("work-night-shift");
    } else {
        int surgery_amount = performance_based_lookup(5, 10, 15, 20, 30, 40);
        provide_resource(SurgeryId, surgery_amount);
        add_text("work-day-shift");
        add_performance_text();
    }
}

void Medic::HandleInteraction(sGirlShiftData& interactor, sGirlShiftData& target) const {
    int performance = interactor.Performance;
    int prevent_scars = performance - 100;
    std::stringstream ss;
    std::stringstream healing_message;
    ss << "${name} is called in to handle an emergency. "
        << target.girl().FullName() << " is heavily wounded. ";
    if(!interactor.rng().percent(prevent_scars)) {
        healing_message << interactor.girl().FullName() << " expertly treats " << target.girl().FirstName() << "'s wounds, setting her "
            << " broken bones and stitching up her gaping gashes.\n\n";
        healing_message << "After an intense 10 minutes, " << interactor.girl().FullName() << " declares her charge stable enough to be transported "
            << "from the arena floor into her surgery room.\n\n";
        healing_message << "At the end of the night, she informs you that " << target.girl().FirstName()
            << " has made it and is expected to make a full recovery.";
        target.girl().combat(-5);
        target.girl().strength(-5);
        target.girl().constitution(-5);
        target.girl().health(20);
    } else {
        healing_message << interactor.girl().FullName() << " seems to be out of her depth with the degree of " << target.girl().FirstName() << "'s wounds.\n";
        healing_message << "She manages to save " <<  target.girl().FirstName() << "'s life, but there will be Scars.";
        target.girl().gain_trait(traits::HORRIFIC_SCARS);
        const char* injury = cJobManager::get_injury_trait(target.girl());
        if(injury) {
            target.girl().gain_trait(injury);
            healing_message << "\nShe now has " << injury << ".";
        }
        target.girl().spirit(-5);
        target.girl().combat(-10);
        target.girl().strength(-10);
        target.girl().constitution(-10);
        target.girl().health(15);
    }
    target.EventMessage << healing_message.str();
    ss << healing_message.str();
    interactor.girl().tiredness(10);
    interactor.girl().exp(35);
    interactor.girl().medicine(2);
    interactor.girl().AddMessage(ss.str(), EImageBaseType::NURSE, EVENT_NIGHTSHIFT);
}

IntermissionStripper::IntermissionStripper() : cSimpleJob(JOB_INTERMISSION_SHOW, "ArenaIntermission.xml") {
    m_Info.PrimaryAction = EBasicActionType::STRIPPING;
}

void IntermissionStripper::JobProcessing(sGirl& girl, sGirlShiftData& shift) const {
    add_performance_text();

    shift_enjoyment();

    int turn_beauty = shift.Performance / 10;
    int turn_sex = std::max(0, (shift.Performance - 100) / 10);
    provide_resource(BeautyId, turn_beauty);
    provide_resource(SexualityId, turn_sex);

    // Improve stats
    HandleGains(girl, 0);
}

Recuperate::Recuperate(): cBasicJob(JOB_RECUPERATE, "ArenaRecuperate.xml") {

}

void Recuperate::DoWork(sGirlShiftData& shift) const {
    if(shift.girl().health() < 10) {
        auto* medic = request_interaction(ResuscitateId);
        if(medic) {
            add_text("medic-near-death");
            shift.girl().health(uniform(3, 8));
            shift.girl().tiredness(uniform(5, 10));
        } else {
            add_text("healing-near-death");
            shift.girl().health(uniform(2, 5));
            shift.girl().strength(-uniform(0, 2));
            shift.girl().constitution(-uniform(0, 2));
            shift.girl().tiredness(uniform(10, 15));
        }
    } else if (shift.girl().health() < 33) {
        add_text("healing-low-health");
        shift.girl().health(uniform(3, 8));
        shift.girl().tiredness(uniform(2, 5));
        int med = consume_resource(SurgeryId, 10);
        if(med > 0) {
            add_text("medics-help");
            shift.girl().health(med);
        }
    } else if (shift.girl().health() < 66) {
        add_text("healing-mid-health");
        shift.girl().health(uniform(3, 8));
        shift.girl().tiredness(uniform(2, 5));
    } else {
        if(shift.girl().tiredness() > 75) {
            add_text("resting-very-tired");
            shift.girl().tiredness(-uniform(10, 15));
        } else if(shift.girl().tiredness() > 50) {
            add_text("resting-tired");
            shift.girl().tiredness(-uniform(8, 12));
            shift.girl().health(uniform(1, 4));
        } else {
            add_text("meditate");
            shift.girl().tiredness(-uniform(8, 12));
            shift.girl().health(uniform(1, 4));
        }
        if (shift.girl().health() < 90) {
            int med = consume_resource(SurgeryId, 5);
            if(med > 0) {
                add_literal("\n\n");
                add_text("medics-help");
                shift.girl().health(med);
            }
        }
    }

    generate_event();
}
bool Recuperate::CheckRefuseWork(sGirl& girl) const {
    return false;
}

void Recuperate::on_pre_shift(sGirlShiftData& shift) const {
    cBasicJob::on_pre_shift(shift);
    if(shift.girl().health() == 100 && shift.girl().tiredness() == 0) {
        add_text("full-health");
        shift.Job = JOB_RESTING;
    }
}