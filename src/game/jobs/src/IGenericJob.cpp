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

#include "IGenericJob.h"
#include <functional>
#include <boost/throw_exception.hpp>
#include "CLog.h"
#include "cRng.h"
#include "character/sGirl.h"
#include "buildings/cBuilding.h"
#include "buildings/IBuildingShift.h"
#include "xml/util.h"
#include "xml/getattr.h"
#include "utils/lookup.h"

class sBrothel;

const std::array<const char*, 4>& get_all_phases() {
    static std::array<const char*, 4> phases {
        "prepare", "produce", "main", "late"};
    return phases;
}

const id_lookup_t<EJobPhase>& get_phase_lookup() {
    static auto lookup = create_lookup_table<EJobPhase>(get_all_phases());
    return lookup;
}

EJobPhase get_phase_id(const std::string& name) {
    return lookup_with_error(get_phase_lookup(), name, "Unknown Job Phase: ");
}

/*
class cJobWrapper: public IGenericJob {
public:
    cJobWrapper(JOBS j, std::function<sWorkJobResult(sGirl&, bool, cRng&)> w, std::function<double(const sGirl&, bool)> p,
                std::string brief, std::string desc) :
            IGenericJob(j), m_Work(std::move(w)), m_Perf(std::move(p)) {
        m_Info.ShortName = std::move(brief);
        m_Info.Description = std::move(desc);
    }

    cJobWrapper& full_time() { m_Info.FullTime = true; return *this; };
    cJobWrapper& free_only() { m_Info.FreeOnly = true; return *this; };

    eCheckWorkResult CheckWork(sGirl& girl, IBuildingShift& building, bool is_night) override {
        if(!is_job_valid(girl)) {
            return ECheckWorkResult::IMPOSSIBLE;
        }
        return ECheckWorkResult::ACCEPTS;
    }
private:
    double GetPerformance(const sGirl& girl, bool estimate) const override { return m_Perf(girl, estimate); }
    sWorkJobResult DoWork(sGirl& girl, bool is_night) override {
        return m_Work(girl, is_night, rng());
    }

    std::function<sWorkJobResult(sGirl&, bool, cRng&)> m_Work;
    std::function<double(const sGirl&, bool)> m_Perf;
};


// `J` When modifying Jobs, search for "J-Change-Jobs"  :  found in >> cJobManager.h > class cJobManager

#define DECL_JOB(Fn) sWorkJobResult Work##Fn(sGirl& girl, bool Day0Night1, cRng& rng); \
double JP_##Fn(const sGirl& girl, bool estimate)

// - General
DECL_JOB(Freetime);
DECL_JOB(Torturer);

// house
DECL_JOB(PersonalBedWarmer);

namespace {
    sWorkJobResult WorkNullJob(sGirl&, bool, cRng&) { return {false}; }
    double JP_NullJob(const sGirl&, bool) { return 0.0; }
}

#define REGISTER_JOB_MANUAL(J, Wf, Pf, Brief, Desc)                                     \
    [&]() -> auto& {                                                                    \
    auto new_job = std::make_unique<cJobWrapper>(J, Work##Wf, JP_##Pf, Brief, Desc);    \
    auto ptr = new_job.get();                                                           \
    mgr.register_job(std::move(new_job));                                               \
    return *ptr;                                                                        \
    }()
#define REGISTER_JOB(J, Fn, Brief, Desc) REGISTER_JOB_MANUAL(J, Fn, Fn, Brief, Desc)


void RegisterWrappedJobs(cJobManager& mgr) {
    REGISTER_JOB(JOB_RESTING, Freetime, "TOff", "She will take some time off, maybe do some shopping or walk around town. If the girl is unhappy she may try to escape.");
    REGISTER_JOB(JOB_TORTURER, Torturer, "Trtr", "She will torture the prisoners in addition to your tortures, she will also look after them to ensure they don't die. (max 1 for all brothels)").full_time().free_only();

    // house
    REGISTER_JOB(JOB_PERSONALBEDWARMER, PersonalBedWarmer, "BdWm", "She will stay in your bed at night with you.");

    // Some pseudo-jobs
    REGISTER_JOB(JOB_INDUNGEON, NullJob, "", "She is languishing in the dungeon.");
    REGISTER_JOB(JOB_RUNAWAY, NullJob, "", "She has escaped.");
}
*/