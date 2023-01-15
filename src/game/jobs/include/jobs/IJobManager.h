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

#ifndef WM_IJOBMANAGER_H
#define WM_IJOBMANAGER_H

#include <memory>
#include <vector>
#include "Constants.h"
#include "jobs.h"

struct sJobFilter {
    std::string Name;
    std::string Display;
    std::string Description;
    std::vector<JOBS> Contents;
};

struct sGirl;

class IJobManager {
public:
    IJobManager() = default;
    virtual ~IJobManager() = default;

    virtual const IGenericJob* get_job(JOBS job) const = 0;
    const sJobInfo& get_job_info(JOBS job) const;

    // simplified job info queries
    const std::string& get_job_name(JOBS job) const;
    const std::string& get_job_brief(JOBS job) const;
    bool is_full_time(JOBS job) const;

    /// does the pre-shift setup part of the job processing
    virtual void handle_pre_shift(sGirlShiftData& shift) = 0;
    virtual void handle_main_shift(sGirlShiftData& shift) = 0;
    virtual void handle_post_shift(sGirlShiftData& shift) = 0;

    virtual void setup() = 0;

    /// Checks if the job assignment is valid. If yes, updates `girl`s job and returns true.
    /// Otherwise, returns false.
    virtual bool assign_job(sGirl& girl, JOBS job, EJobShift shift) const = 0;

    // Job-Filter data
    virtual const sJobFilter& get_filter(EJobFilter filter) const = 0;
    bool is_in_filter(EJobFilter filter, JOBS job) const;
};

#endif // WM_IJOBMANAGER_H