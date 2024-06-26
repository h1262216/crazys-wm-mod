/*
 * Copyright 2019-2022, The Pink Petal Development Team.
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

#ifndef WM_IGENERICJOB_H
#define WM_IGENERICJOB_H

#include "Constants.h"
#include "images/sImageSpec.h"
#include <vector>
#include "jobs.h"
#include "sAttributeCondition.h"

class sGirl;
class IBuildingShift;
class cRng;
struct sGirlShiftData;
class cGirlShift;

enum EJobPhase {
    //! Preparations to make the building ready for work: Cleaning, Security, Advertisement. This will happen
    //! before any customers arrive.
    PREPARE = 1,
    //! Production phase. Any job that produces some form of resource that is needed by the main job should run here
    PRODUCE = 2,
    //! Main jobs
    MAIN = 4,
    //! Any "clean-up" type processing
    LATE = 8
};

EJobPhase get_phase_id(const std::string& name);

struct sJobInfo {
    JOBS        JobId;
    std::string Name;
    std::string ShortName;
    std::string Description;
    std::string Title;          //! What's her job title

    EJobShift Shift = EJobShift::ANY;
    bool FreeOnly = false;
    bool Singleton = false;
    bool IsFightingJob = false;
    EJobPhase Phases = EJobPhase::MAIN;

    int BaseWages = 0;

    /// The primary action whose enjoyment value determines if she likes the job
    EActivity PrimaryAction = EActivity::GENERIC;
    /// The secondary action whose enjoyment value determines if she likes the job
    EActivity SecondaryAction = EActivity::GENERIC;
    /// An offset added to the enjoyment calculation
    int BaseEnjoyment = 0;
    /// A trait modifier for the enjoyment of this job
    std::string EnjoymentTraitModifier;

    sImagePreset DefaultImage = EImageBaseType::PROFILE;

    std::vector<std::string> Consumes;
    std::vector<std::string> Provides;
    std::vector<EJobFilter> Filters;
    std::vector<vGirlCondition> Conditions;
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
    EJobPhase phases() const { return get_info().Phases; }

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

    /// Handles stuff after the actual work is done, e.g. generating a summary message
    virtual void PostShift(sGirlShiftData& shift) = 0;

    virtual void HandleCustomer(sGirl& girl, IBuildingShift& building, bool is_night, cRng& rng) const {};
    virtual void HandleInteraction(sGirlShiftData& interactor, sGirlShiftData& target) const {};

    virtual bool has_text(const std::string& prompt) const = 0;
    virtual const std::string& get_text(const std::string& prompt, cGirlShift& shift) const = 0;
};

#endif //WM_IGENERICJOB_H
