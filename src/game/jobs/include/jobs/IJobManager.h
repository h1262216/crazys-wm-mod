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

    virtual void setup() = 0;

    // Job-Filter data
    virtual const sJobFilter& get_filter(EJobFilter filter) const = 0;
    bool is_in_filter(EJobFilter filter, JOBS job) const;
};

#endif // WM_IJOBMANAGER_H