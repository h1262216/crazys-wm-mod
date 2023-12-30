/*
 * Copyright 2019-2023, The Pink Petal Development Team.
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

#include "cBasicJob.h"
#include "xml/util.h"
#include "traits/ITraitsManager.h"
#include "character/sGirl.h"
#include "cGirls.h"
#include "CLog.h"
#include "utils/string.hpp"
#include "IGame.h"
#include "cJobManager.h"
#include "character/lust.h"
#include "buildings/cBuilding.h"
#include "combat/combat.h"
#include "character/cCustomers.h"
#include "xml/getattr.h"

namespace settings {
    extern const char* USER_SHOW_NUMBERS;
}

double cBasicJob::GetPerformance(const sGirl& girl, bool estimate) const {
    if(get_info().FreeOnly && girl.is_slave()) return -1000;
    return m_PerformanceData.eval(girl, estimate);
}

cBasicJob::cBasicJob(JOBS job, std::string xml_file) :
    cGenericJob(job, std::move(xml_file)) {
}

cBasicJob::~cBasicJob() = default;

void cBasicJob::update_enjoyment_of(cGirlShift& shift, EActivity action, int change_chance) const {
    auto& girl = shift.girl();
    int old_enjoyment = girl.enjoyment(action);
    int enjoyment = shift.data().Enjoyment;
    shift.data().DebugMessage << "Enjoyment of " << get_activity_name(action) << ": " << enjoyment << " [" << old_enjoyment << "]\n";
    if(enjoyment > old_enjoyment + 5) {
        int delta = std::min(4, (enjoyment - old_enjoyment + 2) / 4);
        change_chance += 5 * delta;
        shift.data().DebugMessage << " Chance to increase: " << change_chance << "%, range " << delta/2 << " - " << delta * 2 << "\n";
        if(shift.chance(change_chance)) {
            std::stringstream msg;
            girl.enjoyment(action, shift.uniform(delta / 2, delta * 2));
            if(get_setting_bool(settings::USER_SHOW_NUMBERS)) {
                msg << "${name} had fun working today (" << enjoyment
                    << "), and now enjoys " << get_activity_descr(action)
                    << " a little more (" << old_enjoyment << " -> " << girl.enjoyment(action) << ").\n";
            } else {
                msg << "${name} had fun working today, and now enjoys " << get_activity_descr(action)
                    << " a little more.\n";
            }
            girl.AddMessage(msg.str(), EImageBaseType::PROFILE, EEventType::EVENT_GOODNEWS);
        }
    } else if(shift.data().Enjoyment < old_enjoyment - 5) {
        int delta = std::min(4, (old_enjoyment - enjoyment) / 4);
        change_chance += 5 * delta;
        shift.data().DebugMessage << " Chance to decrease: " << change_chance << "%, range " << delta/2 << " - " << delta * 2 << "\n";
        if(shift.chance(change_chance)) {
            girl.enjoyment(action, -shift.uniform(delta / 2, delta * 2));
            std::stringstream msg;
            if(get_setting_bool(settings::USER_SHOW_NUMBERS)) {
                msg << "${name} disliked working today (" << enjoyment
                    << "), and now enjoys " << get_activity_descr(action)
                    << " a little less (" << old_enjoyment << " -> " << girl.enjoyment(action) << ").\n";
            } else {
                msg << "${name} disliked working today, and now enjoys " << get_activity_descr(action)
                    << " a little less.\n";
            }
            girl.AddMessage(msg.str(), EImageBaseType::PROFILE, EEventType::EVENT_WARNING);
        }
    }
}

void cBasicJob::apply_gains(cGirlShift& shift) const {
    auto& girl = shift.girl();

    m_Gains.apply(shift);

    // add messages for how much she enjoys the job
    if (shift.data().Enjoyment <= -66 && has_text("enjoy.hate")) {
        shift.add_line("enjoy.hate");
    } else if (shift.data().Enjoyment <= -33) {
        if(has_text("enjoy.dislike")) { shift.add_line("enjoy.dislike"); }
    } else if (shift.data().Enjoyment <= 33) {
        if(has_text("enjoy.neutral")) { shift.add_line("enjoy.neutral"); }
    } else if (shift.data().Enjoyment > 66 && has_text("enjoy.love")) {
        shift.add_line("enjoy.love");
    } else if(has_text("enjoy.like")) {
        shift.add_line("enjoy.like");
    }

    if (get_setting_bool(settings::USER_SHOW_NUMBERS)) {
        shift.data().EventMessage << "(Shift Enjoyment: " << shift.data().Enjoyment << ")\n";
    }

    // gain/lose happiness based on enjoyment
    if(shift.data().Enjoyment > girl.happiness() + 25) {
        if(shift.data().Enjoyment > girl.happiness() + 50) {
            shift.data().DebugMessage << " +3 happy due to enjoyment.\n";
            girl.happiness(3);
        } else {
            shift.data().DebugMessage << " +2 happy due to enjoyment.\n";
            girl.happiness(2);
        }
    } else if(shift.data().Enjoyment < girl.happiness() - 33) {
        shift.data().DebugMessage << " -2 happy due to enjoyment.\n";
        girl.happiness(-2);
    }

    if(shift.data().Enjoyment < -50) {
        shift.data().DebugMessage << " -1 love due to enjoyment.\n";
        girl.pclove(-1);
    } else if(shift.data().Enjoyment > 80 && girl.pclove() < 50) {
        shift.data().DebugMessage << " +1 love due to enjoyment.\n";
        girl.pclove(1);
    }

    if(m_Info.PrimaryAction != EActivity::GENERIC) {
        if(m_Info.SecondaryAction != EActivity::GENERIC) {
            update_enjoyment_of(shift, m_Info.PrimaryAction, 25);
            update_enjoyment_of(shift, m_Info.SecondaryAction, 5);
        } else {
            update_enjoyment_of(shift, m_Info.PrimaryAction, 33);
        }
    } else {
        shift.data().DebugMessage << " No primary action, so no enjoyment based updates.\n";
    }
}

void cBasicJob::load_from_xml_internal(const tinyxml2::XMLElement& job_data, const std::string& file_name) {
    std::string prefix = "job." + file_name;

    // Performance Criteria
    const auto* performance_el = job_data.FirstChildElement("Performance");
    if (performance_el) {
        m_PerformanceData.load(*performance_el, prefix);
    }

    // Gains
    const auto* gains_el = job_data.FirstChildElement("Gains");
    if (gains_el) {
        m_Gains.load(*gains_el);
    }

    // Modifiers
    const auto* modifiers_el = job_data.FirstChildElement("Modifiers");
    if (modifiers_el) {
        // TODO automatically prefix with the jobs name, and allow for loading "local" modifiers
        // which start with .
        g_Game->traits().load_modifiers(*modifiers_el, prefix);
    }

    const auto* config_el = job_data.FirstChildElement("Config");
    if (config_el) {
        const auto* ob_el = config_el->FirstChildElement("Obedience");
        if(ob_el) {
            m_Obedience.Fear = ob_el->IntAttribute("Fear", -1);
            m_Obedience.MaxDignity = ob_el->IntAttribute("Dignity", 100);
            m_Obedience.Obedience = ob_el->IntAttribute("Difficulty", 10);
        }

        const auto* li_el = config_el->FirstChildElement("Pleasure");
        if(li_el) {
            m_Pleasure.Amount = GetIntAttribute(*li_el, "Amount");
            m_Pleasure.SkillMin = li_el->IntAttribute("SkillMin", 0);
            m_Pleasure.BaseValue = GetIntAttribute(*li_el, "BaseValue");
            m_Pleasure.Skill = get_skill_id(GetStringAttribute(*li_el, "Skill"));
        }

        load_from_xml_callback(*config_el);
    }
}

void cBasicJob::InitWork(cGirlShift& shift) const {
    auto& girl = shift.girl();
    shift.data().Performance = GetPerformance(girl, false);
    shift.data().Enjoyment = CalculateBasicEnjoyment(shift);

    shift.data().DebugMessage <<  "Initial Performance: " <<  shift.data().Performance  << "\n";
    shift.data().DebugMessage <<  "Initial Enjoyment: " <<  shift.data().Enjoyment << "\n";

    // TODO Salary-based enjoyment
    int delta_enjoy = 0;
    if(shift.performance() < 50) {
        delta_enjoy -= shift.uniform(4, 10);
    } else if(shift.performance() < 100) {
        delta_enjoy -= shift.uniform(2, 6);
    } else if(shift.performance() > 200) {
        delta_enjoy += shift.uniform(2, 6);
    }

    shift.data().Enjoyment += delta_enjoy;
    // TODO modification of performance based on salary, obedience, and enjoyment
    // TODO modification based on whether she only does this because she is afraid of you
}

bool cBasicJob::check_refuse_action(cGirlShift& shift, EActivity action) const {
    if (shift.girl().disobey_check(action, job()))
    {
        shift.add_text("refuse");
        return true;
    }
    return false;
}

void cBasicJob::add_performance_text(cGirlShift& shift) const {
    shift.add_text(performance_based_lookup(
            shift, "work.worst", "work.bad", "work.ok", "work.good", "work.great", "work.perfect")
            );
    shift.add_literal("\n\n");
}

int cBasicJob::get_performance_class(int performance) {
    if (performance >= 245) { return 5; }
    else if (performance >= 185) { return 4; }
    else if (performance >= 145) { return 3; }
    else if (performance >= 100) { return 2; }
    else if (performance >= 70) { return 1; }
    else { return 0;}
}

void cBasicJob::shift_enjoyment(cGirlShift& shift, bool security) const {
    shift.add_literal("\n");
    int roll = shift.d100();
    if(security) {
        roll -= std::min(10, shift.building().m_SecurityLevel / 10);
        if (roll <= 8 && shift.chance(50)) {
            //roll += shift.consume_resource(SecurityAmountId, 9 - roll);
        }
    }

    if (roll <= 8) {
        if(has_text("shift.bad")) {
            shift.add_text("shift.bad");
        }
        shift.data().Enjoyment -= shift.uniform(1, 4);
        if(security) {
            //shift.provide_resource(TroubleAmountId, 5);
        }
    } else if (roll <= 80) {
        if(has_text("shift.neutral")) {
            shift.add_text("shift.neutral");
        }
    } else {
        if(has_text("shift.good")) {
            shift.add_text("shift.good");
        }
        shift.data().Enjoyment += shift.uniform(1, 4);
    }
}

void cBasicJob::on_post_shift(cGirlShift& shift) const {
    if (shift.data().Refused != ECheckWorkResult::ACCEPTS) {
        //brothel->m_Fame -= girl.fame();
        shift.girl().AddMessage("${name} did not work so she made no money.", EImageBaseType::PROFILE, EVENT_SUMMARY);
    } else {
        make_summary_message(shift);
        apply_gains(shift);
        shift.generate_event();
    }
}


void cBasicJob::make_summary_message(cGirlShift& shift) const {
// TODO check that there is no partial event left.
    auto& ss = shift.data().EventMessage;
    shift.data().EventImage = EImageBaseType::PROFILE;
    shift.data().EventType = EEventType::EVENT_SUMMARY;
    //brothel->m_Fame += girl.fame();
    auto money_data = job_manager().CalculatePay(shift.data());

    if(money_data.Earnings > 0 || money_data.Tips > 0) {
        ss << "${name} made " << money_data.Earnings;
        if (money_data.Tips != 0) {
            ss << " and " << money_data.Tips << " in tips. ";
        } else {
            ss << " gold. ";
        }
    }

    if(shift.data().Cost > 0) {
        ss << "She spent " << shift.data().Cost << " gold on supplies and ingredients. ";
    }
    if (money_data.Wages > 0) ss << "You paid her a salary of " << money_data.Wages << ". ";
    ss << "In total, she got " << money_data.GirlGets << " gold and you ";
    if(money_data.PlayerGets > 0) {
        ss << "got " << money_data.PlayerGets << " gold.";
    } else {
        ss << "spent " << -money_data.PlayerGets << " gold.";
    }

    ss << "\n\n";
}


int lust_influence(const sJobPleasureData& data, const sGirl& girl) {
    if(data.Skill == NUM_SKILLS)
        return 0;

    // calculate the percentage of the skill between SkillMin and 100
    int skill = girl.get_skill(data.Skill) - data.SkillMin;
    float skill_factor = static_cast<float>(skill) / static_cast<float>(100 - data.SkillMin);
    if(skill_factor < 0) {
        skill_factor = 0.f;
    }
    // a non-linear transformation
    // the 0.1 makes the curve a bit smoother
    skill_factor = std::sqrt(0.1f + skill_factor) / std::sqrt(1.1f);

    int lib_value = girl.lust() * (data.Amount + data.BaseValue) * skill_factor;
    return lib_value / 100 - data.BaseValue;
}

int sigmoid(int value, int threshold, int width, int amount) {
    auto v = static_cast<float>(value - threshold) / static_cast<float>(width);
    return static_cast<int>(static_cast<float>(amount) / (std::exp(-v) + 1.f));
}

float chance_to_disobey(const sDisobeyData& data) {
    float chance_to_obey = 1.f;
    chance_to_obey *= (100 - data.Fear) / 100.f;
    chance_to_obey *= (100 - data.Dignity) / 100.f;
    chance_to_obey *= (100 - data.Hate) / 100.f;
    chance_to_obey *= (100 - data.Rebel) / 100.f;
    return 1.f - chance_to_obey;
}


sDisobeyData cBasicJob::calculate_disobey_chance(cGirlShift& shift) const {
    auto& girl = shift.girl();
    sDisobeyData disobey;
    int obedience = girl.obedience();
    int love = girl.pclove();
    int lust = lust_influence(m_Pleasure, girl);
    disobey.Lust = lust;

    // obedience-based offset to checks
    int obed_offset = std::max(0, obedience - 30) / 7;
    obed_offset += std::max(0, obedience - 60) / 4;

    int obd_fear = m_Obedience.Fear;
    if(m_Info.PrimaryAction == EActivity::SOCIAL) {
        obd_fear += 5 - girl.confidence() / 10;
        if(girl.any_active_trait({traits::SHY, traits::NERVOUS})) {
            obd_fear += 15;
        }
    } else if(m_Info.PrimaryAction == EActivity::TEASING) {
        if(girl.any_active_trait({traits::SHY, traits::NERVOUS})) {
            obd_fear += 5 + obd_fear / 3;
        }
    } else if(m_Info.PrimaryAction == EActivity::FIGHTING) {
        obd_fear += 10 - girl.combat() / 10 - (girl.strength() + girl.constitution()) / 20;
    }

    // Is she afraid of this job?
    if(obd_fear > 0) {
        int trust_offset = std::max(0, girl.pclove() - 50) / 5;
        int job_fear = obd_fear - trust_offset - obed_offset - lust;
        int fear_chance = sigmoid(job_fear, girl.pcfear(), 15, 100);

        if(girl.any_active_trait({traits::FEARLESS, traits::BROKEN_WILL})) {
            fear_chance /= 2;
        }
        disobey.Fear = fear_chance;
        obedience -= fear_chance / 10;
    }

    int obd_dignity = m_Obedience.MaxDignity;

    // Is she too dignified to do this
    if(obd_dignity < 100) {
        int love_offset = std::max(0, girl.pclove() - 50) / 5;
        int fear_offset = std::max(0, girl.pcfear() - 50) / 5;
        int dignity = girl.dignity() - obed_offset - love_offset - fear_offset - lust / 3;
        if(girl.has_active_trait(traits::CHASTE)) {
            dignity += 10;
        }
        int dig_chance = sigmoid(dignity, obd_dignity, 15, 100);
        if(girl.any_active_trait({traits::SLUT, traits::WHORE, traits::OPEN_MINDED})) {
            dig_chance /= 2;
        }
        disobey.Dignity = dig_chance;
        obedience -= dig_chance / 10;
    }

    // does she just want to spite you?
    if(love < 0) {
        int hate = (-love) * (girl.spirit() + 25) / 100 - lust / 3;
        int hate_chance = sigmoid(hate, obedience, 25, 75);
        disobey.Hate = hate_chance;
        obedience -= hate_chance / 10;
    } else {
        obedience += std::max(0, girl.pclove() - 50) / 5;
    }

    int enjoy = CalculateBasicEnjoyment(shift) + lust / 2;
    if(girl.happiness() < 33) {
        obedience -= 10;
    }
    if(girl.tiredness() > 80) {
        obedience -= 10;
    }
    disobey.Rebel = sigmoid(m_Obedience.Obedience, obedience + enjoy, 20, 80);
    if(girl.obedience() < 10)
        disobey.Rebel += 10 - girl.obedience();
    return disobey;
}

void cBasicJob::disobey_check(cGirlShift& shift) const {

    auto& girl = shift.girl();
    auto chances = calculate_disobey_chance(shift);

    if(get_setting_bool(settings::USER_SHOW_NUMBERS)) {
        shift.data().DebugMessage << "Obedience:\n" << " Fear: " << chances.Fear << "% [" << m_Obedience.Fear << "]\n";
        shift.data().DebugMessage << " Dignity: " << chances.Dignity << "% [" << girl.dignity() << " / " << m_Obedience.MaxDignity << "]\n";
        shift.data().DebugMessage << " Hate: " << chances.Hate << "%\n";
        shift.data().DebugMessage << " Rebel: " << chances.Rebel << "% [" << m_Obedience.Obedience << "]\n";
        shift.data().DebugMessage << " Lust: " << chances.Lust << "\n";
    }

    if(shift.chance_capped(chances.Fear)) {
        shift.set_result(ECheckWorkResult::REFUSE_FEAR);
        if(shift.chance(10)) {
            girl.obedience(-1);
            girl.pclove(-2);
        }
    } else if(shift.chance_capped(chances.Dignity)) {
        shift.set_result(ECheckWorkResult::REFUSE_DIGNITY);
        if(shift.chance(10)) {
            girl.dignity(-1);
            girl.obedience(-1);
            girl.pclove(-2);
        }
        girl.happiness(-1);
    } else if(shift.chance_capped(chances.Hate)) {
        shift.set_result(ECheckWorkResult::REFUSE_HATE);
        if(shift.chance(10)) {
            girl.pclove(1);
            girl.obedience(-2);
        }
        girl.happiness(1);
    } else if (shift.chance_capped(chances.Rebel)) {
        shift.set_result(ECheckWorkResult::REFUSES);
        if(shift.chance(10)) {
            girl.obedience(-1);
            girl.pclove(-1);
        }
    } else if(m_Info.PrimaryAction != EActivity::FUCKING && m_Info.SecondaryAction != EActivity::FUCKING
        && girl.lust() > 80 && will_masturbate(girl, true, sPercent(0.33f))) {
        // Her lust got the better of her
        shift.set_result(ECheckWorkResult::REFUSE_HORNY);
        if(shift.chance(10)) {
            girl.obedience(-1);
        }

    } else {
        // If she was very likely to disobey, but did not, she is getting used to being obedient.
        if (shift.chance_capped(chances.Rebel)) {
            girl.gain_attribute(STAT_OBEDIENCE, 1, 3, 33);
        }
        // doing things against her wishes uses up pc-love
        if (shift.chance_capped(chances.Hate + chances.Dignity + chances.Fear)) {
            girl.pclove(-1);
        }
        shift.set_result(ECheckWorkResult::ACCEPTS);
    }
}

bool cBasicJob::CheckRefuseWork(cGirlShift& shift) const {
    disobey_check(shift);

    auto refuse_with_text = [&](const char* text, const char* fallback){
        if(has_text(text)) {
            shift.add_text(text);
        } else if (has_text("refuse")) {
            shift.add_text("refuse");
        } else {
            shift.add_literal(fallback);
        }
    };

    switch(shift.data().Refused) {
        case ECheckWorkResult::REFUSE_FEAR:
            refuse_with_text("refuse.fear", "${name} refused to work as a ${job-title} to${shift} because she is afraid of this job.");
            break;
        case ECheckWorkResult::REFUSE_DIGNITY:
            refuse_with_text("refuse.dignity", "${name} refused to work as a ${job-title} to${shift} because she thinks this job is beneath her.");
            break;
        case ECheckWorkResult::REFUSE_HATE:
            refuse_with_text("refuse.hate", "${name} refused to do what you ordered and work as a ${job-title} to${shift} because she hates you.");
            break;
        case ECheckWorkResult::REFUSE_HORNY:
            refuse_with_text("refuse.horny", "${name}'s lust got the better of her to${shift}, and she masturbated instead of working as a ${job-title} .");
            shift.girl().lust_release_regular();
            shift.set_image(EImagePresets::MASTURBATE);
            break;
        case ECheckWorkResult::REFUSES:
            refuse_with_text("refuse.generic", "${name} refused to work as a ${job-title} to${shift}.");
            break;
        default:
            break;
    }

    return shift.data().Refused != ECheckWorkResult::ACCEPTS;
}

/*
void cBasicJob::customer_attempts_violence() const {
    if(chance_capped(sigmoid(active_shift().building().TroubleLevel(), 100, 20, 20))) {
        // bad event initiated
        // first possibility: dissuade by mere presence
        if(chance(50) && try_consume_resource(SecurityAmountId, 25)) {
            add_text("security.dissuaded");
        } else if(auto SecGuard = request_interaction(SecurityInteractionId)) {
            // security guard helps
            // Most of the rest of this is a copy-paste from customer_rape
            // TODO figure out customer handling here
            sCustomer Attacker;
            Combat combat(ECombatObjective::CAPTURE, ECombatObjective::KILL);
            combat.add_combatant(ECombatSide::ATTACKER, Attacker);
            combat.add_combatant(ECombatSide::DEFENDER, *SecGuard);
            auto result = combat.run(10);

            SecGuard->exp(10);
            if (result == ECombatResult::DEFEAT)  // Security guard wins
            {
                cJobManager::sec_guard_wins_1vs1(active_girl(), SecGuard);
            } else if(result == ECombatResult::DRAW) {
                SecGuard->gain_attribute(SKILL_COMBAT, 1, 2, 50);
                SecGuard->gain_attribute(STAT_CONFIDENCE, 1, 2, 33);
            } else {

            }

        } else {
            // no help
        }
    }
}
 */