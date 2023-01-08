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

double cBasicJob::GetPerformance(const sGirl& girl, bool estimate) const {
    if(m_Info.FreeOnly && girl.is_slave()) return -1000;
    return m_PerformanceData.eval(girl, estimate);
}

cBasicJob::cBasicJob(JOBS job, std::string xml_file) :
    cGenericJob(job, std::move(xml_file)) {
}

cBasicJob::~cBasicJob() = default;

void cBasicJob::apply_gains(int performance) const {
    m_Gains.apply(active_girl(), performance);
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
    shift.Performance = GetPerformance(active_girl(), false);
}
/*
void cBasicJob::RegisterVariable(std::string name, sImagePreset& value) {
    /// TODO
    assert(false);
    m_Interface->RegisterVariable(std::move(name), value);
}
*/
ECheckWorkResult cBasicJob::SimpleRefusalCheck(sGirl& girl, Action_Types action) const {
    auto& ss = active_shift().shift_message();
    if (girl.disobey_check(action, job()))
    {
        add_text("refuse");
        girl.AddMessage(ss.str(), EImageBaseType::REFUSE, EVENT_NOWORK);
        return ECheckWorkResult::REFUSES;
    }
    return ECheckWorkResult::ACCEPTS;
}

void cBasicJob::add_performance_text() const {
    auto& ss = active_shift().shift_message();
    if (active_shift().Performance >= 245)
    {
        add_text("work.perfect");
    }
    else if (active_shift().Performance >= 185)
    {
        add_text("work.great");
    }
    else if (active_shift().Performance >= 145)
    {
        add_text("work.good");
    }
    else if (active_shift().Performance >= 100)
    {
        add_text("work.ok");
    }
    else if (active_shift().Performance >= 70)
    {
        add_text("work.bad");
    }
    else
    {
        add_text("work.worst");
    }
    ss << "\n\n";
}