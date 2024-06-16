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

#ifndef WM_SGIRLSHIFTDATA_H
#define WM_SGIRLSHIFTDATA_H

#include "Constants.h"
#include <array>
#include <sstream>
#include <variant>
#include "images/sImageSpec.h"
#include "jobs.h"

class sGirl;
class cRng;
class cBuilding;
class IBuildingShift;

constexpr const int NUM_JOB_VARIABLES = 5;

struct sGirlShiftData {
    sGirlShiftData(sGirl* girl, IBuildingShift* shift, JOBS job);

    // Keeping track of things that change over the course of shift processing.
    // Not all variables are relevant for all jobs
    JOBS Job;
    ECheckWorkResult Refused = ECheckWorkResult::ACCEPTS;   // Whether she actually worked
    int Tips = 0;               // how much she received in tips
    int Earnings = 0;           // how much money did she make you directly
    int Wages = 0;              // how much do you pay her for the job
    int Cost = 0;               // How much money she used up to support her work (e.g. for raw materials)
    int Performance = 0;        // How well did she perform this shift
    int Enjoyment = 0;          // How much did she enjoy working this job

    using JobStateVar_t = std::variant<int, std::string>;

    [[nodiscard]] sGirl& girl() { return *m_Girl; }
    [[nodiscard]] const sGirl& girl() const { return *m_Girl; }
    [[nodiscard]] IBuildingShift& shift() { return *m_Shift; }
    [[nodiscard]] cRng& rng() { return *m_Rng; }
    [[nodiscard]] JobStateVar_t get_var(int index) const { return m_ProcessingCache.at(index); }
    [[nodiscard]] JobStateVar_t& get_var_ref(int index) { return m_ProcessingCache.at(index); }
    void set_var(int index, JobStateVar_t value) { m_ProcessingCache.at(index) = std::move(value); }

    // partial message building
    std::stringstream EventMessage;
    sImagePreset EventImage;
    EEventType EventType;

    // debug message
    std::stringstream DebugMessage;
private:
    sGirl* m_Girl = nullptr;
    IBuildingShift* m_Shift = nullptr;
    cRng* m_Rng;

    std::array<JobStateVar_t, NUM_JOB_VARIABLES> m_ProcessingCache;


};

#endif //WM_SGIRLSHIFTDATA_H
