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

#include "buildings/sGirlShiftData.h"
#include "buildings/IBuildingShift.h"

class ITextRepository;
class cJobTextInterface;
class cJobManager;

namespace tinyxml2 {
    class XMLElement;
}


class cGenericJob : public IGenericJob {
public:
    enum class EJobClass {
        REGULAR_JOB, TREATMENT
    };

    explicit cGenericJob(JOBS j, std::string xml_file = {}, EJobClass job_class = EJobClass::REGULAR_JOB);
    ~cGenericJob() override;

    const sJobInfo& get_info() const final { return m_Info; }

    void Work(sGirlShiftData& shift) final;
    void PreShift(sGirlShiftData& shift) final;

    sJobValidResult IsJobValid(const sGirl& girl, bool night_shift) const override;

    /// called by the job manager when the job gets registered.
    void OnRegisterJobManager(const cJobManager& manager);
    static void Register(cJobManager& manager, std::unique_ptr<cGenericJob> job);
protected:
    // random functions
    cRng& rng() const;
    int d100() const;
    bool chance(float percent) const;
    int uniform(int min, int max) const;

    sGirl& active_girl() const;
    IBuildingShift& active_building() const;
    bool is_night_shift() const;
    sGirlShiftData& active_shift() const;

    // text and event messages
    bool has_text(const std::string& prompt) const;
    const std::string& get_text(const std::string& prompt) const;

    void add_literal(const std::string& text) const;
    void add_text(const std::string& prompt) const;
    void add_line(const std::string& prompt) const;

    void generate_event() const;

    // resources -- just relay to the corresponding shift functions
    int consume_resource(const std::string& name, int amount) const;
    void provide_resource(const std::string& name, int amount) const;
    bool try_consume_resource(const std::string& name, int amount) const;

    //  one-on-one interactions
    void provide_interaction(const std::string& name, int amount) const;
    sGirl* request_interaction(const std::string& name) const;
    bool has_interaction(const std::string& name) const;

    void SetSubstitution(std::string key, std::string replace);

    int RegisterVariable(std::string name, int default_value);
    int GetVariable(int index) const;
    int FindVariable(const std::string& name) const;

    virtual void on_pre_shift(sGirlShiftData& shift) const;
    virtual sJobValidResult on_is_valid(const sGirl& girl, bool night_shift) const;
private:
    virtual void InitWork(sGirlShiftData& shift) {};
    virtual void DoWork(sGirlShiftData& shift) const = 0;

    /*! Checks whether the girl will work. There are two reasons why she might not:
    She could refuse, or the job could not be possible because of external
    circumstances. This function should report which reason applies.
*/
    virtual bool CheckCanWork(sGirl& girl) const = 0;
    virtual bool CheckRefuseWork(sGirl& girl) const = 0;

    sGirlShiftData* m_ActiveData;
    const cJobManager* m_JobManager = nullptr;

    friend class cJobTextInterface;

    /// If the job has specified an xml file, this function will load the job data from there. If no file is
    /// specified, nothing happens.
    /// Note: Since this may call virtual functions, we cannot do this in the constructor.
    /// Therefore, this is called when the job is registered to the JobManager
    void load_job();
    virtual void load_from_xml_internal(const tinyxml2::XMLElement& source, const std::string& file_name) { };
    std::string m_XMLFile;
    EJobClass m_JobClass;

    std::unique_ptr<ITextRepository> m_TextRepo;
    std::unique_ptr<cJobTextInterface> m_Interface;
    std::unordered_map<std::string, std::string> m_Replacements;

    struct sVariableData {
        std::string Name;
        int Index;
        int DefaultValue;
    };
    std::vector<sVariableData> m_Variables;

protected:
    sJobInfo m_Info;
};

#endif //WM_CGENERICJOB_H
