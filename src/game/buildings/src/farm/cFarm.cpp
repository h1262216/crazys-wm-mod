/*
* Copyright 2009, 2010, The Pink Petal Development Team.
* The Pink Petal Devloment Team are defined as the game's coders
* who meet on http://pinkpetal.org     // old site: http://pinkpetal .co.cc
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

#include <sstream>

#include "cFarm.h"
#include "cGangs.h"
#include "IGame.h"
#include "jobs/cJobManager.h"

extern cRng             g_Dice;

// // ----- Strut sFarm Create / destroy
sFarm::sFarm() : cBuilding(BuildingType::FARM, "Farm")
{
    m_FirstJob = JOB_FARMMANGER;
    m_LastJob = JOB_MAKEPOTIONS;
    m_MatronJob = JOB_FARMMANGER;
}

sFarm::~sFarm()    = default;


void sFarm::auto_assign_job(sGirl& target, std::stringstream& message, bool is_night)
{
    // if they have no job at all, assign them a job
    message << "The Farm Manager assigns " << target.FullName() << " to ";
    // `J` zzzzzz need to add this in
    message << "do nothing because this part of the code has not been added yet.";
    /*
    JOB_MARKETER        // free girl, only one

    JOB_VETERINARIAN
    JOB_FARMHAND
    JOB_RESEARCH

    JOB_FARMER
    JOB_GARDENER
    JOB_SHEPHERD
    JOB_RANCHER
    JOB_CATACOMBRANCHER
    JOB_BEASTCAPTURE
    JOB_MILKER
    JOB_MILK

    JOB_BUTCHER
    JOB_BAKER
    JOB_BREWER
    JOB_MAKEITEM
    JOB_MAKEPOTIONS
    //*/
}
