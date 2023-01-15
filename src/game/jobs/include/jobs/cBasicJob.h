/*
 * Copyright 2019-2023, The Pink Petal Development Team.
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


#ifndef WM_CBASICJOB_H
#define WM_CBASICJOB_H

#include <unordered_map>

#include "cGenericJob.h"
#include "JobData.h"

class sImagePreset;

class cBasicJob : public cGenericJob {
public:
    explicit cBasicJob(JOBS job, std::string xml_file = {});
    ~cBasicJob() override;
    double GetPerformance(const sGirl& girl, bool estimate) const override;

protected:
    void apply_gains(int performance) const;

    void add_performance_text() const;

    void InitWork(sGirlShiftData& shift) override;
    // void RegisterVariable(std::string name, sImagePreset& preset);

    bool check_refuse_action(sGirl& girl, Action_Types action) const;

    bool CheckCanWork(sGirl& girl) const override {
        return true;
    }

    /// Returns a number between 0 and 5 (inclusive) that classifies the given performance, from worst (0) to perfect (5)
    static int get_performance_class(int performance);

    template<class T>
    T performance_based_lookup(T worst, T bad, T ok, T good, T great, T perfect) const {
        switch(get_performance_class(active_shift().Performance)) {
            case 0: return worst;
            case 1: return bad;
            case 2: return ok;
            case 3: return good;
            case 4: return great;
            case 5: return perfect;
            default: __builtin_unreachable();
        }
    }

    void on_post_shift(sGirlShiftData& shift) const;

private:
    cJobPerformance m_PerformanceData;
    cJobGains       m_Gains;
protected:
    // protected, so derived classes can call the base-class version
    void load_from_xml_internal(const tinyxml2::XMLElement& source, const std::string& file_name) override;
    virtual void load_from_xml_callback(const tinyxml2::XMLElement& job_element) {};
};

#endif //WM_CBASICJOB_H
