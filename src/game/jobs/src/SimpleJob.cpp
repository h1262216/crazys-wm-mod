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


#include "SimpleJob.h"
#include "cGirls.h"
#include "character/sGirl.h"
#include "xml/util.h"
#include "buildings/IBuildingShift.h"

void cSimpleJob::DoWork(sGirlShiftData& shift) const
{
    if(has_text("work")) {
        add_text("work");
        add_literal("\n\n");
    }

    JobProcessing(shift.girl(), shift);
    if(!shift.EventMessage.str().empty()) {
        generate_event();
    }
}

cSimpleJob::cSimpleJob(JOBS job, const char* xml) : cBasicJob(job, xml) {
    // RegisterVariable("Image", m_ImageType);
}

void cSimpleJob::HandleGains(sGirl& girl, int fame) const {
    // Update Enjoyment
    if (girl.fame() < 10 && active_shift().Performance >= 70)     { fame += 1; }
    if (girl.fame() < 20 && active_shift().Performance >= 100)    { fame += 1; }
    if (girl.fame() < 40 && active_shift().Performance >= 145)    { fame += 1; }
    if (girl.fame() < 60 && active_shift().Performance >= 185)    { fame += 1; }

    girl.fame(fame);

    apply_gains();
}

void cSimpleJob::load_from_xml_callback(const tinyxml2::XMLElement& job_element) {
    auto wages = job_element.FirstChildElement("WageFunction");
    if(wages) {
        m_PerformanceToEarnings = LoadLinearFunction(*wages, "Performance", "Wages");
    }
}

void cSimpleJob::InitWork(sGirlShiftData& shift) {
    cBasicJob::InitWork(shift);
}

bool cSimpleJob::CheckRefuseWork(sGirl& girl) const {
    return false;
    // return check_refuse_action(girl, m_Info.PrimaryAction);
}
