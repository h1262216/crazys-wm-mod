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

#include "IJobManager.h"
#include "IGenericJob.h"
#include <algorithm>

const sJobInfo& IJobManager::get_job_info(JOBS job) const {
    return get_job(job)->get_info();
}

const std::string& IJobManager::get_job_name(JOBS job) const {
    return get_job_info(job).Name;
}

const std::string& IJobManager::get_job_brief(JOBS job) const {
    return get_job_info(job).ShortName;
}

bool IJobManager::is_full_time(JOBS job) const {
    return get_job_info(job).FullTime;
}

bool IJobManager::is_in_filter(EJobFilter filter, JOBS job) const {
    auto& filter_contents = get_filter(filter).Contents;
    return std::count(begin(filter_contents), end(filter_contents), job) > 0;
}