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

#include "queries.h"
#include "cBuilding.h"
#include "cBuildingShift.h"

int num_girls(const IBuilding& building) {
    return building.girls().num();
}

int num_girls_on_job(const IBuilding& building, JOBS jobID, int is_night) {
    return building.girls().count(HasJob(jobID, is_night));
}

sGirl* random_girl_on_job(const IBuilding& building, JOBS job, bool at_night) {
    return const_cast<sGirl*>(building.girls().get_random_girl(HasJob(job, at_night)));
}

bool is_valid_job(const IBuilding& building, JOBS job) {
    if(job == JOB_RESTING) return true;
    return building.config().FirstJob <= job && job <= building.config().LastJob;
}

bool DoctorNeeded(IBuilding& building) {
    return !(num_girls_on_job(building, JOB_DOCTOR, 0) > 0 ||
             num_girls_on_job(building, JOB_GETHEALING, 0) +
             num_girls_on_job(building, JOB_GETABORT, 0) +
             num_girls_on_job(building, JOB_COSMETICSURGERY, 0) +
             num_girls_on_job(building, JOB_LIPO, 0) +
             num_girls_on_job(building, JOB_BREASTREDUCTION, 0) +
             num_girls_on_job(building, JOB_BOOBJOB, 0) +
             num_girls_on_job(building, JOB_VAGINAREJUV, 0) +
             num_girls_on_job(building, JOB_TUBESTIED, 0) +
             num_girls_on_job(building, JOB_FERTILITY, 0) +
             num_girls_on_job(building, JOB_FACELIFT, 0) +
             num_girls_on_job(building, JOB_ASSJOB, 0) < 1);
}

int GetNumberPatients(IBuilding& building, bool Day0Night1)    // `J` added, if there is a doctor already on duty or there is no one needing surgery, return false
{
    return (num_girls_on_job(building, JOB_GETHEALING, Day0Night1) +
            num_girls_on_job(building, JOB_GETABORT, Day0Night1) +
            num_girls_on_job(building, JOB_COSMETICSURGERY, Day0Night1) +
            num_girls_on_job(building, JOB_LIPO, Day0Night1) +
            num_girls_on_job(building, JOB_BREASTREDUCTION, Day0Night1) +
            num_girls_on_job(building, JOB_BOOBJOB, Day0Night1) +
            num_girls_on_job(building, JOB_VAGINAREJUV, Day0Night1) +
            num_girls_on_job(building, JOB_FACELIFT, Day0Night1) +
            num_girls_on_job(building, JOB_ASSJOB, Day0Night1) +
            num_girls_on_job(building, JOB_TUBESTIED, Day0Night1) +
            num_girls_on_job(building, JOB_FERTILITY, Day0Night1));
}


int GetNumberActresses(const IBuilding& building)
{
    // `J` When adding new Studio Scenes, search for "J-Add-New-Scenes"  :  found in >> cMovieStudio.cpp > Num_Actress
    int actresses = 0;
    for (int i = JOB_FILMACTION; i < JOB_FILMRANDOM + 1; i++)
    {
        actresses += num_girls_on_job(building, JOBS(i), 1);
    }

    return actresses;
}

bool is_Actress_Job(int testjob)
{
    // `J` When adding new Studio Scenes, search for "J-Add-New-Scenes"  :  found in >>cMovieStudio.cpp > is_Actress_Job
    return (testjob > JOB_STAGEHAND && testjob <= JOB_FILMRANDOM);
}

bool CrewNeeded(const IBuilding& building)    // `J` added, if CM and CP both on duty or there are no actresses, return false
{
    // `J` When adding new Studio Scenes, search for "J-Add-New-Scenes"  :  found in >> cMovieStudio.cpp > CrewNeeded
    if ((num_girls_on_job(building, JOB_CAMERAMAGE, 1) > 0 &&
         num_girls_on_job(building, JOB_CRYSTALPURIFIER, 1) > 0) ||
            GetNumberActresses(building) < 1)
        return false;    // a CM or CP is not Needed
    return true;    // Otherwise a CM or CP is Needed
}

int Num_Patients(const cBuilding& building, bool at_night)
{
    return num_girls_on_job(building, JOB_REHAB, at_night) +
            num_girls_on_job(building, JOB_ANGER, at_night) +
            num_girls_on_job(building, JOB_EXTHERAPY, at_night) +
            num_girls_on_job(building, JOB_THERAPY, at_night);
}

cBuilding& cast_building(IBuilding& b) { return dynamic_cast<cBuilding&>(b); }

int free_rooms(const IBuilding& building) {
    return dynamic_cast<const cBuilding&>(building).free_rooms();
}

int num_girls_on_job(const IBuildingShift& building, JOBS jobID, int is_night) {
    return num_girls_on_job(building, jobID, is_night);
}

