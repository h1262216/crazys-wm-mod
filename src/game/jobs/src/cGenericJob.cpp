/*
 * Copyright 2020-2023, The Pink Petal Development Team.
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

#include "cGenericJob.h"
#include "TextInterface.h"
#include "cJobManager.h"

#include "IGame.h"
#include "CLog.h"
#include "cRng.h"
#include "cGirls.h"

#include "character/sGirl.h"
#include "text/repo.h"
#include "utils/string.hpp"
#include "xml/util.h"
#include "xml/getattr.h"

cGenericJob::cGenericJob(JOBS j, std::string xml_file, EJobClass job_class) :
        m_Info{j, get_job_name(j)}, m_XMLFile(std::move(xml_file)) , m_JobClass(job_class),
        m_Interface(std::make_unique<cJobTextInterface>(this))
{
}

cGenericJob::~cGenericJob() = default;

void cGenericJob::Work(sGirlShiftData& shift) {
    auto& girl = shift.girl();
    m_ActiveData = &shift;

    if(m_Info.Shift == EJobShift::FULL && girl.m_DayJob != girl.m_NightJob) {
        g_LogFile.error("jobs", "Full time job was assigned for a single shift!");
        girl.m_DayJob = job();
        girl.m_NightJob = job();
    }

    if(shift.IsNightShift) {
        girl.m_Refused_To_Work_Night = false;
    } else {
        girl.m_Refused_To_Work_Day = false;
    }

    for(auto& var : m_Variables) {
        shift.set_var(var.Index, var.DefaultValue);
    }

    shift.EventType = shift.IsNightShift ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT;
    shift.EventImage = m_Info.DefaultImage;
    shift.Wages = m_Info.BaseWages;

    if(m_Info.IsFightingJob) {
        cGirls::EquipCombat(shift.girl());
    } else {
        cGirls::UnequipCombat(shift.girl());
    }

    InitWork(shift);
    DoWork(shift);
    m_ActiveData = nullptr;
}

void cGenericJob::PreShift(sGirlShiftData& shift) {
    m_ActiveData = &shift;
    shift.Refused = ECheckWorkResult::ACCEPTS;
    on_pre_shift(shift);
    if(shift.Refused == ECheckWorkResult::IMPOSSIBLE) {
        shift.EventImage = EImageBaseType::PROFILE;
        shift.EventType = EVENT_WARNING;
        generate_event();
    } else if(shift.Refused == ECheckWorkResult::REFUSES) {
        shift.EventImage = EImageBaseType::REFUSE;
        shift.EventType = EVENT_NOWORK;
        generate_event();
    }

    m_ActiveData = nullptr;
}

void cGenericJob::PostShift(sGirlShiftData& shift) {
    m_ActiveData = &shift;
    on_post_shift(shift);
    m_ActiveData = nullptr;
}


int cGenericJob::d100() const {
    return rng().d100();
}

bool cGenericJob::chance(float percent) const {
    return rng().percent(percent);
}

int cGenericJob::uniform(int min_, int max_) const {
    int min = std::min(min_, max_);
    int max = std::max(min_, max_);
    return rng().closed_uniform(min, max);
}


sGirl& cGenericJob::active_girl() const {
    assert(m_ActiveData);
    return m_ActiveData->girl();
}

IBuildingShift& cGenericJob::active_building() const {
    assert(m_ActiveData);
    return m_ActiveData->building();
}

bool cGenericJob::is_night_shift() const {
    assert(m_ActiveData);
    return m_ActiveData->IsNightShift;
};

int cGenericJob::consume_resource(const std::string& name, int amount) const {
    return active_building().ConsumeResource(name, amount);
}

void cGenericJob::provide_resource(const std::string& name, int amount) const {
    return active_building().ProvideResource(name, amount);
}

bool cGenericJob::try_consume_resource(const std::string& name, int amount) const {
    return active_building().TryConsumeResource(name, amount);
}

void cGenericJob::provide_interaction(const std::string& name, int amount) const {
    return active_building().ProvideInteraction(name, &active_girl(), amount);
}

sGirl* cGenericJob::request_interaction(const std::string& name) const {
    return active_building().RequestInteraction(name, &active_girl());
}

bool cGenericJob::has_interaction(const std::string& name) const {
    return active_building().HasInteraction(name);
}

cRng& cGenericJob::rng() const {
    assert(m_ActiveData);
    return m_ActiveData->rng();
}

void cGenericJob::on_pre_shift(sGirlShiftData& shift) const {
    auto valid = IsJobValid(shift.girl(), shift.IsNightShift);
    if(!valid.IsValid) {
        add_literal(valid.Reason);
        shift.Refused = ECheckWorkResult::IMPOSSIBLE;
        return;
    }

    if(!CheckCanWork(shift.girl())) {
        shift.Refused = ECheckWorkResult::IMPOSSIBLE;
    } else if(CheckRefuseWork(shift.girl())) {
        shift.Refused = ECheckWorkResult::REFUSES;
    } else {
        shift.Refused = ECheckWorkResult::ACCEPTS;
    }
}

sGirlShiftData& cGenericJob::active_shift() const {
    assert(m_ActiveData);
    return *m_ActiveData;
}

void cGenericJob::OnRegisterJobManager(const cJobManager& manager) {
    assert(m_JobManager == nullptr);
    m_JobManager = &manager;
    load_job();
    setup_job();
}

sJobValidResult cGenericJob::IsJobValid(const sGirl& girl, bool night_shift) const {
    if(m_Info.FreeOnly && girl.is_slave()) {
        return {false, "Slaves cannot work as " + m_Info.Name + "!"};
    }
    if(night_shift && m_Info.Shift == EJobShift::DAY) {
        return {false, "The " + m_Info.Name + " job can only be assigned for the day shift."};
    } else if (!night_shift && m_Info.Shift == EJobShift::NIGHT) {
        return {false, "The " + m_Info.Name + " job can only be assigned for the night shift."};
    }

    return on_is_valid(girl, night_shift);
}

sJobValidResult cGenericJob::on_is_valid(const sGirl& girl, bool night_shift) const {
    return {true, {}};
}

void cGenericJob::load_job() {
    if(m_XMLFile.empty()) return;
    const char* base_element = "Job";
    const char* directory = "Jobs";
    if(m_JobClass == EJobClass::TREATMENT) {
        base_element = "Treatment";
        directory = "Treatments";
    }
    try {
        DirPath path = DirPath() << "Resources" << "Data" << directory << m_XMLFile;
        auto doc = LoadXMLDocument(path.c_str());
        auto job_data = doc->FirstChildElement(base_element);
        if(!job_data) {
            throw std::runtime_error(std::string("Job xml does not contain <") + base_element + "> element!");
        }

        // Info
        m_Info.ShortName = GetStringAttribute(*job_data, "ShortName");
        if(const auto* desc_el = job_data->FirstChildElement("Description")) {
            if(const char* description = desc_el->GetText()) {
                m_Info.Description = description;
            } else {
                g_LogFile.error("jobs", "<Description> element does not contain text. File: ", m_XMLFile);
            }
        } else {
            g_LogFile.error("jobs", "<Job> element does not contain <Description>. File: ", m_XMLFile);
        }

        // Texts
        const auto* text_el = job_data->FirstChildElement("Messages");
        if(text_el) {
            m_TextRepo = ITextRepository::create();
            m_TextRepo->load(*text_el);
            if(!m_TextRepo->verify()) {
                g_LogFile.error("jobs", "Detected some problems when loading ", path.str());
                g_Game->error("Detected some problems when loading " + path.str());
            }
        }

        // Configs
        const auto* config_el = job_data->FirstChildElement("Config");
        if(config_el) {
            std::string shift = tolower(GetDefaultedStringAttribute(*config_el, "Shift", "both"));
            if(shift == "day-only") {
                m_Info.Shift = EJobShift::DAY;
            } else if (shift == "night-only") {
                m_Info.Shift = EJobShift::NIGHT;
            } else if(shift == "full") {
                m_Info.Shift = EJobShift::FULL;
            }
            m_Info.FreeOnly = config_el->BoolAttribute("FreeOnly", false);
            m_Info.IsFightingJob = config_el->BoolAttribute("FightingJob", false);

            const char* image_preset_name = config_el->Attribute("DefaultImage");
            if(image_preset_name) {
                m_Info.DefaultImage = get_image_preset(image_preset_name);
            }

            std::string phase = GetDefaultedStringAttribute(*config_el, "Phase", "main");
            m_Info.Phase = get_phase_id(phase);
            m_Info.BaseWages = config_el->IntAttribute("BaseWages");

            // Filters
            for(auto& filter_el : IterateChildElements(*config_el, "Filter")) {
                std::string filter_name = filter_el.GetText();
                m_Info.Filters.push_back(m_JobManager->get_filter_id(filter_name));
            }
        }

        load_from_xml_internal(*job_data, path.str());
    } catch (std::exception& error) {
        g_LogFile.error("job", "Error loading job xml '", m_XMLFile, "': ", error.what());
        throw;
    }
}

void cGenericJob::add_literal(const std::string& text) const {
    active_shift().EventMessage << text;
}


const std::string& cGenericJob::get_text(const std::string& prompt) const {
    assert(m_TextRepo);
    try {
        return m_TextRepo->get_text(prompt, *m_Interface);
    } catch (const std::out_of_range& oor) {
        g_LogFile.error("job", "Trying to get missing text '", prompt, "\' in job ", m_Info.Name);
        throw;
    }
}

bool cGenericJob::has_text(const std::string& prompt) const {
    if(!m_TextRepo) return false;
    return m_TextRepo->has_text(prompt);
}

void cGenericJob::add_text(const std::string& prompt) const {
    auto& tpl = get_text(prompt);
    auto& ss = active_shift().EventMessage;
    interpolate_string(ss, tpl, [&](const std::string& var) -> std::string {
        if(var == "name") {
            return active_girl().FullName();
        } else if (var == "shift") {
            return is_night_shift() ? "night" : "day";
        } else if (m_Replacements.count(var) != 0) {
            return m_Replacements.at(var);
        }
        assert(false);
    }, rng());
}

void cGenericJob::add_line(const std::string& prompt) const {
    add_text(prompt);
    active_shift().EventMessage << "\n";
}

void cGenericJob::SetSubstitution(std::string key, std::string replace) {
    m_Replacements[std::move(key)] = std::move(replace);
}

void cGenericJob::Register(cJobManager& manager, std::unique_ptr<cGenericJob> job) {
    manager.register_job(std::move(job));
}

int cGenericJob::RegisterVariable(std::string name, int default_value) {
    int index = m_Variables.size();
    if(index >= NUM_JOB_VARIABLES) {
        throw std::runtime_error(std::string("Ran out of variables for job ") + get_job_name(job()));
    }
    sVariableData data = {std::move(name), index, default_value};
    m_Variables.push_back(data);
    return index;
}

int cGenericJob::GetVariable(int index) const {
    if(index < 0 || index >= m_Variables.size()) {
        throw std::runtime_error("Variable index is out of range.");
    }
    return active_shift().get_var(index);
}

int cGenericJob::FindVariable(const std::string& name) const {
    auto result = std::find_if(m_Variables.begin(), m_Variables.end(), [&](const auto& a){
        return a.Name == name;
    });
    if(result != m_Variables.end()) {
        return result->Index;
    }
    return -1;
}

void cGenericJob::generate_event() const {
    active_girl().AddMessage(active_shift().EventMessage.str(), active_shift().EventImage, active_shift().EventType);
    active_shift().EventMessage.str("");
}

void cGenericJob::on_post_shift(sGirlShiftData& shift) const {

}

const cJobManager& cGenericJob::job_manager() const {
    return *m_JobManager;
}
