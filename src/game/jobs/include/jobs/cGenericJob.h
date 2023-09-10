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

#ifndef WM_CGENERICJOB_H
#define WM_CGENERICJOB_H

#include "IGenericJob.h"

#include <unordered_map>
#include <memory>

#include "jobs/sGirlShiftData.h"
#include "jobs/cGirlShift.h"
#include "utils/lookup.h"

class ITextRepository;
class cJobTextInterface;
class cJobManager;

namespace tinyxml2 {
    class XMLElement;
}

class cGenericJob;
class cBuilding;

class cGenericJob : public IGenericJob {
    friend class cGirlShift;
public:
    enum class EJobClass {
        REGULAR_JOB, TREATMENT
    };

    explicit cGenericJob(JOBS j, std::string xml_file = {}, EJobClass job_class = EJobClass::REGULAR_JOB);
    explicit cGenericJob(std::string xml_file, EJobClass job_class = EJobClass::REGULAR_JOB);
    ~cGenericJob() override;

    const sJobInfo& get_info() const final { return m_Info; }

    void Work(sGirlShiftData& shift) final;
    void PreShift(sGirlShiftData& shift) final;
    void PostShift(sGirlShiftData& shift) final;

    sJobValidResult IsJobValid(const sGirl& girl, bool night_shift) const override;

    /// called by the job manager when the job gets registered.
    void OnRegisterJobManager(const cJobManager& manager);
    static void Register(IJobManager& manager, std::unique_ptr<cGenericJob> job);

    // text and event messages
    bool has_text(const std::string& prompt) const override;
    const std::string& get_text(const std::string& prompt, cGirlShift& shift) const override;

protected:
    [[nodiscard]] int RegisterVariable(std::string name, sGirlShiftData::JobStateVar_t default_value);
    int FindVariable(const std::string& name) const;

    virtual void on_pre_shift(cGirlShift& shift) const;
    virtual void on_post_shift(cGirlShift& shift) const;
    virtual sJobValidResult on_is_valid(const sGirl& girl, bool night_shift) const;

    const cJobManager& job_manager() const;

    virtual int CalculateBasicEnjoyment(cGirlShift& girl) const;

    // accessing global settings
    int get_setting_bool(const char* key) const;

private:
    virtual void InitWork(cGirlShift& shift) const {};
    virtual void DoWork(cGirlShift& shift) const = 0;

    /*! Checks whether the girl will work. There are two reasons why she might not:
    She could refuse, or the job could not be possible because of external
    circumstances. This function should report which reason applies.
*/
    virtual bool CheckCanWork(cGirlShift& shift) const = 0;
    virtual bool CheckRefuseWork(cGirlShift& shift) const = 0;

    virtual void HandleWorkImpossible(cGirlShift& shift) const;

    const cJobManager* m_JobManager = nullptr;

    friend class cJobTextInterface;

    /// If the job has specified an xml file, this function will load the job data from there. If no file is
    /// specified, nothing happens.
    /// Note: Since this may call virtual functions, we cannot do this in the constructor.
    /// Therefore, this is called when the job is registered to the JobManager
    void load_job();
    virtual void load_from_xml_internal(const tinyxml2::XMLElement& source, const std::string& file_name) { };
    virtual void setup_job() {}
    std::string m_XMLFile;
    EJobClass m_JobClass;

    std::unique_ptr<ITextRepository> m_TextRepo;

    struct sVariableData {
        std::string Name;
        int Index;
        sGirlShiftData::JobStateVar_t DefaultValue;
    };
    std::vector<sVariableData> m_Variables;

protected:
    sJobInfo m_Info;
};

#endif //WM_CGENERICJOB_H
