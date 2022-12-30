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

class sGirl;
class IBuildingShift;
class cRng;

constexpr const int NUM_JOB_VARIABLES = 5;

enum class ECheckWorkResult {
    REFUSES,
    ACCEPTS,
    IMPOSSIBLE
};

struct sGirlShiftData {
    sGirlShiftData(sGirl* girl, IBuildingShift* building, JOBS job, bool shift);

    // Keeping track of things that change over the course of shift processing.
    // Not all variables are relevant for all jobs
    JOBS Job;
    bool IsNightShift;
    ECheckWorkResult Refused;   // Whether she actually worked
    int Tips = 0;               // how much she received in tips
    int Earnings = 0;           // how much money did she make you directly
    int Wages = 0;              // how much do you pay her for the job
    int Performance = 0;        // How well did she perform this shift

    [[nodiscard]] sGirl& girl() { return *m_Girl; }
    [[nodiscard]] IBuildingShift& building() { return *m_BuildingShift; }
    [[nodiscard]] cRng& rng() { return *m_Rng; }
    [[nodiscard]] int get_var(int index) const { return m_ProcessingCache.at(index); }
    void set_var(int index, int value) { m_ProcessingCache.at(index) = value; }
private:
    sGirl* m_Girl = nullptr;
    IBuildingShift* m_BuildingShift = nullptr;
    cRng* m_Rng;
    std::array<int, NUM_JOB_VARIABLES> m_ProcessingCache;
};

#endif //WM_SGIRLSHIFTDATA_H
