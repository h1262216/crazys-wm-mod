/*
 * Copyright 2023, The Pink Petal Development Team.
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

#ifndef WM_JOBS_H
#define WM_JOBS_H

// forward declarations
class IGenericJob;
class IJobManager;
class sGirlShiftData;
struct sJobInfo;

// enums
enum class EJobShift {
    DAY, NIGHT, FULL, ANY
};

enum class ECheckWorkResult {
    REFUSES,
    REFUSE_FEAR,
    REFUSE_DIGNITY,
    REFUSE_HATE,
    REFUSE_HORNY,
    ACCEPTS,
    IMPOSSIBLE,
    INVALID
};

bool is_refused(ECheckWorkResult);
bool is_impossible(ECheckWorkResult);

/// TODO figure out where to place this declaration
int sigmoid(int value, int threshold, int width, int amount);

#endif //WM_JOBS_H
