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


#include "cSimpleJob.h"
#include "cGirls.h"
#include "character/sGirl.h"
#include "xml/util.h"

void cSimpleJob::DoWork(cGirlShift& shift) const
{
    if(has_text("work")) {
        shift.add_text("work");
        shift.add_literal("\n\n");
    }

    JobProcessing(shift.girl(), shift);
    if(!shift.data().EventMessage.str().empty()) {
        shift.generate_event();
    }
}

cSimpleJob::cSimpleJob(JOBS job, const char* xml) : cBasicJob(job, xml) {
    // RegisterVariable("Image", m_ImageType);
}

void cSimpleJob::gain_fame(cGirlShift& shift, int fame) const {
    if (shift.girl().fame() < 10 && shift.data().Performance >= 70)     { fame += 1; }
    if (shift.girl().fame() < 20 && shift.data().Performance >= 100)    { fame += 1; }
    if (shift.girl().fame() < 40 && shift.data().Performance >= 145)    { fame += 1; }
    if (shift.girl().fame() < 60 && shift.data().Performance >= 185)    { fame += 1; }
    shift.girl().fame(fame);
}

void cSimpleJob::load_from_xml_callback(const tinyxml2::XMLElement& job_element) {
    auto wages = job_element.FirstChildElement("WageFunction");
    if(wages) {
        m_PerformanceToEarnings = LoadLinearFunction(*wages, "Performance", "Wages");
    }
}
