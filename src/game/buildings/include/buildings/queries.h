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

#pragma once

#ifndef CRAZYS_WM_MOD_QUERIES_HPP
#define CRAZYS_WM_MOD_QUERIES_HPP


#include "Constants.h"

class cBuilding;
class IBuilding;
class IBuildingShift;
class sGirl;


int num_girls(const IBuilding& building);
int num_girls_on_job(const IBuilding& building, JOBS jobID, int is_night);
[[deprecated]] int num_girls_on_job(const IBuildingShift& building, JOBS jobID, int is_night);
sGirl* random_girl_on_job(const IBuilding& building, JOBS job, bool at_night);

int free_rooms(const IBuilding& building);

bool DoctorNeeded(IBuilding& building);
int GetNumberPatients(IBuilding& building, bool Day0Night1);
bool CrewNeeded(const IBuilding& building);
bool is_Actress_Job(int testjob);
int GetNumberActresses(const IBuilding& building);
int Num_Patients(const cBuilding& building, bool is_night);

bool is_valid_job(const IBuilding& building, JOBS job);

[[deprecated]] cBuilding& cast_building(IBuilding& b);

#endif //CRAZYS_WM_MOD_QUERIES_HPP
