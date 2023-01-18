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

#include "cBasicJob.h"
#include "xml/util.h"
#include "traits/ITraitsManager.h"
#include "character/sGirl.h"
#include "cGirls.h"
#include "CLog.h"
#include "utils/string.hpp"
#include "IGame.h"
#include "cJobManager.h"
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

void cBasicJob::update_enjoyment_of(EBasicActionType action, int change_chance) const {
    auto& shift = active_shift();
    auto& girl = active_girl();
    int old_enjoyment = girl.enjoyment(action);
    if(shift.Enjoyment > old_enjoyment + 5) {
        int delta = std::min(4, (shift.Enjoyment - old_enjoyment + 2) / 4);
        if(chance(change_chance + 5 * delta)) {
            girl.enjoyment(action, uniform(delta / 2, delta * 2));
            shift.EventMessage << "She now enjoys " << get_activity_descr(action)
                               << " a little more (" << old_enjoyment << " -> " << girl.enjoyment(action) << ").\n";
            // shift.EventType = EVENT_GOODNEWS;
        }
    } else if(shift.Enjoyment < old_enjoyment - 5) {
        int delta = std::min(4, (old_enjoyment - shift.Enjoyment) / 4);
        if(chance(change_chance + 5 * delta)) {
            girl.enjoyment(action, -uniform(delta / 2, delta * 2));
            shift.EventMessage << "She now enjoys " << get_activity_descr(action)
                               << " a little less (" << old_enjoyment << " -> " << girl.enjoyment(action) << ").\n";
            // shift.EventType = EVENT_WARNING;
        }
    }
}

void cBasicJob::apply_gains() const {
    auto& shift = active_shift();
    auto& girl = active_girl();

    m_Gains.apply(girl, shift.Performance);

    // Handle enjoyment updates
    if(active_shift().Enjoyment > active_girl().happiness() + 50) {
        active_girl().happiness(1);
    }

    // add messages for how much she enjoys the job
    if (active_shift().Enjoyment <= -66 && has_text("enjoy.hate")) {
        add_line("enjoy.hate");
    } else if (active_shift().Enjoyment <= -33) {
        if(has_text("enjoy.dislike")) { add_line("enjoy.dislike"); }
    } else if (active_shift().Enjoyment <= 33) {
        if(has_text("enjoy.neutral")) { add_line("enjoy.neutral"); }
    } else if (active_shift().Enjoyment > 66 && has_text("enjoy.love")) {
        add_line("enjoy.love");
    } else if(has_text("enjoy.like")) {
        add_line("enjoy.like");
    }

    if (g_Game->settings().get_bool(settings::USER_SHOW_NUMBERS)) {
        shift.EventMessage << "(Shift Enjoyment: " << shift.Enjoyment << ")\n";
    }

    // gain/lose happiness based on enjoyment
    if(shift.Enjoyment > girl.happiness() + 25) {
        girl.happiness(2);
    } else if(shift.Enjoyment < girl.happiness() - 33) {
        girl.happiness(-2);
    }

    if(shift.Enjoyment < -50) {
        girl.pclove(-1);
    } else if(shift.Enjoyment > 80 && girl.pclove() < 50) {
        girl.pclove(1);
    }

    if(m_Info.PrimaryAction != EBasicActionType::GENERIC) {
        if(m_Info.SecondaryAction != EBasicActionType::GENERIC) {
            update_enjoyment_of(m_Info.PrimaryAction, 25);
            update_enjoyment_of(m_Info.SecondaryAction, 15);
        } else {
            update_enjoyment_of(m_Info.PrimaryAction, 33);
        }
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
        load_from_xml_callback(*config_el);
    }
}

void cBasicJob::InitWork(sGirlShiftData& shift) {
    auto& girl = shift.girl();
    shift.Performance = GetPerformance(active_girl(), false);

    // Calculate enjoyment
    if(m_Info.PrimaryAction != EBasicActionType::GENERIC) {
        int base_value = girl.enjoyment(m_Info.PrimaryAction);
        if(m_Info.SecondaryAction != EBasicActionType::GENERIC) {
            base_value = (2 * base_value + girl.enjoyment(m_Info.SecondaryAction)) / 3;
        }
        shift.Enjoyment = (2 * base_value + (girl.happiness() - 75)) / 3;
    } else {
        shift.Enjoyment = girl.happiness() - 75;
    }

    // modifiers: unhappy when very tired
    if(girl.tiredness() > 75) {
        shift.Enjoyment -= (girl.tiredness() - 50) / 2;
    }

    // even more enjoyment reduction if working while really unhappy
    if(girl.happiness() < 20) {
        shift.Enjoyment -= 10;
    }

    shift.Enjoyment += uniform(-5, 5);

    // TODO Salary-based enjoyment
    // TODO trait-based enjoyments
    if(shift.Performance < 50) {
        shift.Enjoyment -= uniform(4, 10);
    } else if(shift.Performance < 100) {
        shift.Enjoyment -= uniform(2, 6);
    } else if(shift.Performance > 200) {
        shift.Enjoyment += uniform(2, 6);
    }

    // TODO modification of performance based on salary, obedience, and enjoyment
    // TODO modification based on whether she only does this because she is afraid of you
}

bool cBasicJob::check_refuse_action(sGirl& girl, EBasicActionType action) const {
    if (girl.disobey_check(action, job()))
    {
        add_text("refuse");
        return true;
    }
    return false;
}

void cBasicJob::add_performance_text() const {
    add_text(performance_based_lookup("work.worst", "work.bad", "work.ok", "work.good", "work.great", "work.perfect"));
    add_literal("\n\n");
}

int cBasicJob::get_performance_class(int performance) {
    if (performance >= 245) { return 5; }
    else if (performance >= 185) { return 4; }
    else if (performance >= 145) { return 3; }
    else if (performance >= 100) { return 2; }
    else if (performance >= 70) { return 1; }
    else { return 0;}
}

void cBasicJob::shift_enjoyment() const {
    add_literal("\n");
    int roll = d100();
    if (roll <= 5)
    {
        if(has_text("shift.bad")) {
            add_text("shift.bad");
        }
        active_shift().Enjoyment -= uniform(1, 4);
    }
    else if (roll <= 25)
    {
        if(has_text("shift.good")) {
            add_text("shift.good");
        }
        active_shift().Enjoyment += uniform(1, 4);
    }
    else
    {
        if(has_text("shift.neutral")) {
            add_text("shift.neutral");
        }
    }
}

void cBasicJob::on_post_shift(sGirlShiftData& shift) const {
    if (shift.Refused == ECheckWorkResult::REFUSES) {
        //brothel->m_Fame -= girl.fame();
        shift.girl().AddMessage("${name} refused to work so she made no money.", EImageBaseType::PROFILE, EVENT_SUMMARY);
    } else {
        // TODO check that there is no partial event left.
        auto& ss = shift.EventMessage;
        shift.EventImage = EImageBaseType::PROFILE;
        shift.EventType = EEventType::EVENT_SUMMARY;
        //brothel->m_Fame += girl.fame();
        auto money_data = job_manager().CalculatePay(shift);
        ss << "${name} made " << money_data.Earnings;
        if(money_data.Tips != 0) {
            ss << " and " << money_data.Tips << " in tips. ";
        } else {
            ss << " gold. ";
        }
        if (money_data.Wages > 0) ss << "You paid her a salary of " << money_data.Wages << ". ";
        ss << "In total, she got " << money_data.GirlGets << " gold and you ";
        if(money_data.PlayerGets > 0) {
            ss << "got " << money_data.PlayerGets << " gold.";
        } else {
            ss << "spent " << -money_data.PlayerGets << " gold.";
        }

        ss << "\n\n";

        apply_gains();

        generate_event();
    }
}

bool cBasicJob::disobey_check(sGirl& girl) const {
    // TODO fill in
    return false;
}
