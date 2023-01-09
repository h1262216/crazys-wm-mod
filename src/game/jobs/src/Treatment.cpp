/*
 * Copyright 2022, The Pink Petal Development Team.
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

#include "Treatment.h"
#include "character/sGirl.h"
#include "text/repo.h"
#include "IGame.h"
#include "CLog.h"
#include "cGirls.h"
#include "TextInterface.h"
#include "utils/string.hpp"
#include <tinyxml2.h>

ITreatmentJob::ITreatmentJob(JOBS job, std::string xml_file) : cGenericJob(job, std::move(xml_file), EJobClass::TREATMENT)
{
    m_Info.FullTime = true;
    m_Info.Phase = EJobPhase::LATE;
}

ITreatmentJob::~ITreatmentJob() = default;

void ITreatmentJob::load_from_xml_internal(const tinyxml2::XMLElement& job_data, const std::string& file_name) {
    const auto* config_el = job_data.FirstChildElement(specific_config_element());
    if(config_el) {
        load_from_xml_callback(*config_el);
    }
}

void ITreatmentJob::DoWork(sGirlShiftData& shift) const {
    cGirls::UnequipCombat(active_girl());    // not for patient
    if(active_girl().get_active_treatment() != job()) {
        active_girl().start_treatment(job());
    }
    ReceiveTreatment(active_girl(), is_night_shift());
}
