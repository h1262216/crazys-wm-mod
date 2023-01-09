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

#ifndef WM_IGENERICJOB_H
#define WM_IGENERICJOB_H

#include "Constants.h"
#include "images/sImageSpec.h"
#include <vector>

class sGirl;
class IBuildingShift;
class cRng;
struct sGirlShiftData;

enum class EJobPhase {
    PREPARE,  //!< Preparations to make the building ready for work: Cleaning, Security, Advertisement
    PRODUCE,
    MAIN,
    LATE
};

EJobPhase get_phase_id(const std::string& name);

struct sJobInfo {
    JOBS        JobId;
    std::string Name;
    std::string ShortName;
    std::string Description;

    bool FullTime = false;
    bool FreeOnly = false;
    bool DayOnly = false;
    bool NightOnly = false;
    EJobPhase Phase = EJobPhase::MAIN;

    sImagePreset DefaultImage = EImageBaseType::PROFILE;

    std::vector<std::string> Consumes;
    std::vector<std::string> Provides;
};

struct sJobValidResult {
    bool IsValid;
    std::string Reason;
    explicit operator bool() const { return IsValid; }
};

/*!
 * \brief Base class for all girl jobs.
 * \details Here, we use the word `job` in a wide sense. Girl jobs include having free time, or receiving treatment or
 * surgery. Essentially, any activity the girls do during the day and night shifts. The `IGenericJob` interface
 * defines the interface to be used for anything that interacts with a given job.
 * Implementations of jobs should derive from `cGenericJob`, which also provides a variety of protected methods that
 * are useful for job implementations, and contains the data that is required of all job objects.
 */
class IGenericJob {
public:
    virtual ~IGenericJob() noexcept = default;

    // queries
    virtual const sJobInfo& get_info() const = 0;
    JOBS job() const { return get_info().JobId; }
    EJobPhase phase() const { return get_info().Phase; }

    /// Gets an estimate or actual value of how well the girl performs at this job
    virtual double GetPerformance(const sGirl& girl, bool estimate) const = 0;

    /// Checks whether the given girl can do this job.
    virtual sJobValidResult IsJobValid(const sGirl& girl, bool night_shift) const = 0;

    /// Handles simple pre-shift setup, before any actual jobs are run.
    /// Note: This function cannot handle any
    /// stateful job processing. Multiple `PreShift` calls for different
    /// girls might happen before the corresponding `Work` calls.
    virtual void PreShift(sGirlShiftData& shift) = 0;

    /// Lets the girl do the job
    virtual void Work(sGirlShiftData& shift) = 0;

    virtual void HandleCustomer(sGirl& girl, IBuildingShift& building, bool is_night, cRng& rng) const {};
    virtual void HandleInteraction(sGirlShiftData& interactor, sGirlShiftData& target) const {};

};

#endif //WM_IGENERICJOB_H
