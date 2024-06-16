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

#include <boost/algorithm/string/trim.hpp>
#include "cGenericJob.h"
#include "TextInterface.h"
#include "cJobManager.h"
#include "IBuildingShift.h"

#include "IGame.h"
#include "CLog.h"
#include "cRng.h"
#include "cGirls.h"

#include "buildings/cBuilding.h"

#include "character/sGirl.h"
#include "text/repo.h"
#include "utils/string.hpp"
#include "xml/util.h"
#include "xml/getattr.h"
#include "cGirlShift.h"


cGirlShift::cGirlShift(sGirlShiftData* data, const cGenericJob* job) :
    m_Data(data), m_JobClass(job)
{
    if(!m_Data)
        BOOST_THROW_EXCEPTION(std::logic_error("data is nullptr"));
    if(!m_JobClass)
        BOOST_THROW_EXCEPTION(std::logic_error("job is nullptr"));
}

sGirl& cGirlShift::girl() {
    return m_Data->girl();
}

bool cGirlShift::is_night_shift() const  {
    return m_Data->shift().IsNightShift();
}


// rng utilities
cRng& cGirlShift::rng() {
    return m_Data->rng();
}

int cGirlShift::d100() {
    return rng().d100();
}

bool cGirlShift::chance(float percent) {
    return rng().percent(percent);
}

bool cGirlShift::chance_capped(int percent) {
    if(percent < 5) return false;
    if(percent > 95) return true;
    return chance(percent);
}

int cGirlShift::uniform(int min_, int max_) {
    int min = std::min(min_, max_);
    int max = std::max(min_, max_);
    return rng().closed_uniform(min, max);
}


int cGirlShift::consume_resource(const std::string& name, int amount)  {
    return shift().ConsumeResource(name, amount);
}

void cGirlShift::provide_resource(const std::string& name, int amount) {
    return shift().ProvideResource(name, amount);
}

bool cGirlShift::try_consume_resource(const std::string& name, int amount)  {
    return shift().TryConsumeResource(name, amount);
}

void cGirlShift::provide_interaction(const std::string& name, int amount) {
    return shift().ProvideInteraction(name, &girl(), amount);
}

sGirl* cGirlShift::request_interaction(const std::string& name) {
    return shift().RequestInteraction(name);
}

bool cGirlShift::has_interaction(const std::string& name) const {
    return m_Data->shift().HasInteraction(name);
}

namespace {
    std::string make_string(int v) { return std::to_string(v); }
    std::string make_string(std::string v) { return std::move(v); }
}

void cGirlShift::add_literal(const std::string& text, LocalSubstitutions subs) {
    interpolate_string(m_Data->EventMessage, text, [&](const std::string& var) -> std::string {
        if (var == "shift") {
            return is_night_shift() ? "night" : "day";
        } else if (var == "progress") {
            return std::to_string(girl().get_treatment_progress()) + "%";
        } else if (var == "job-title") {
            return m_JobClass->m_Info.Title;
        } else if (var == "tips") {
            return std::to_string(m_Data->Tips);
        } else if (var == "wages") {
            return std::to_string(m_Data->Wages);
        } else if (var == "earnings") {
            return std::to_string(m_Data->Earnings);
        } else if (var == "cost") {
            return std::to_string(m_Data->Cost);
        } else {
            for(auto& job_var : m_JobClass->m_Variables) {
                if(job_var.Name == var) {
                    const auto& value = m_Data->get_var(job_var.Index);
                    return std::visit([](auto&& content){
                        return make_string(content);
                    }, value);
                }
            }
            for(auto& cnd: subs) {
                if(var == cnd.first) {
                    return cnd.second;
                }
            }
            return girl().GetInterpolationFor(var);
        }
        assert(false);
    }, rng());
}

void cGirlShift::add_text(const std::string& prompt, LocalSubstitutions subs) {
    auto& tpl = m_JobClass->get_text(prompt, *this);
    add_literal(tpl, std::move(subs));
}

void cGirlShift::add_line(const std::string& prompt, LocalSubstitutions subs) {
    add_text(prompt, std::move(subs));
    m_Data->EventMessage << "\n";
}

void cGirlShift::set_image(sImagePreset preset) {
    m_Data->EventImage = preset;
}

void cGirlShift::generate_event() {
    girl().AddMessage(m_Data->EventMessage.str(), m_Data->EventImage, m_Data->EventType);
    m_Data->EventMessage.str("");
}

sGirlShiftData::JobStateVar_t cGirlShift::get_variable(int index) const {
    if(index < 0 || index >= m_JobClass->m_Variables.size()) {
        throw std::runtime_error("Variable index is out of range.");
    }
    return m_Data->get_var(index);
}

void cGirlShift::set_result(ECheckWorkResult result) {
    m_Data->Refused = result;
}

int cGirlShift::performance() const {
    return m_Data->Performance;
}

int cGirlShift::enjoyment() const {
    return m_Data->Enjoyment;
}

void cGirlShift::set_variable(int id, int value) {
    m_Data->set_var(id, value);
}

sGirlShiftData& cGirlShift::data() {
    return *m_Data;
}

const sGirlShiftData& cGirlShift::data() const {
    return *m_Data;
}

IBuildingShift& cGirlShift::shift() {
    return m_Data->shift();
}

cBuilding& cGirlShift::building() {
    return *m_Data->shift().Building();
}

int cGirlShift::tips() const {
    return m_Data->Tips;
}

int cGirlShift::earnings() const {
    return m_Data->Earnings;
}

int cGirlShift::wages() const {
    return m_Data->Wages;
}

int cGirlShift::cost() const {
    return m_Data->Cost;
}

void cGirlShift::receive_tip(int min, int max) {
    int amount = max == -1 ? min : uniform(min, max);
    m_Data->Tips += amount;
}

void cGirlShift::earn_money(int min, int max)  {
    int amount = max == -1 ? min : uniform(min, max);
    m_Data->Earnings += amount;
}

void cGirlShift::incur_cost(int min, int max)  {
    int amount = max == -1 ? min : uniform(min, max);
    m_Data->Cost += amount;
}

cGenericJob::cGenericJob(JOBS j, std::string xml_file, EJobClass job_class) :
        m_Info{j, get_job_name(j)}, m_XMLFile(std::move(xml_file)) , m_JobClass(job_class)
{
}

cGenericJob::cGenericJob(std::string xml_file, cGenericJob::EJobClass job_class) :
    m_Info{JOB_UNSET}, m_XMLFile(std::move(xml_file)) , m_JobClass(job_class)
    {
}


cGenericJob::~cGenericJob() = default;

void cGenericJob::Work(sGirlShiftData& shift) {
    auto& girl = shift.girl();
    if(shift.Refused != ECheckWorkResult::ACCEPTS) {
        g_LogFile.error("jobs", "Trying to run job ", get_info().Name, " for girl ", girl.FullName(), " that is not in ACCEPT state.");
        BOOST_THROW_EXCEPTION(std::logic_error("Trying to run job that is not in ACCEPT state."));
    }
    cGirlShift wrapped{&shift, this};

    if(m_Info.Shift == EJobShift::FULL && girl.m_DayJob != girl.m_NightJob) {
        g_LogFile.error("jobs", "Full time job was assigned for a single shift!");
        girl.m_DayJob = job();
        girl.m_NightJob = job();
    }

    if(shift.shift().IsNightShift()) {
        girl.m_Refused_To_Work_Night = false;
    } else {
        girl.m_Refused_To_Work_Day = false;
    }

    for(auto& var : m_Variables) {
        shift.set_var(var.Index, var.DefaultValue);
    }

    shift.EventType = shift.shift().IsNightShift() ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT;
    shift.EventImage = m_Info.DefaultImage;
    shift.Wages = m_Info.BaseWages;
    shift.Enjoyment = m_Info.BaseEnjoyment;

    if(m_Info.IsFightingJob) {
        cGirls::EquipCombat(shift.girl());
    } else {
        cGirls::UnequipCombat(shift.girl());
    }

    InitWork(wrapped);
    DoWork(wrapped);
}

void cGenericJob::PreShift(sGirlShiftData& shift) {
    cGirlShift wrapped{&shift, this};
    wrapped.set_result(ECheckWorkResult::ACCEPTS);
    on_pre_shift(wrapped);
    if(is_impossible(shift.Refused)) {
        HandleWorkImpossible(wrapped);
    } else if(is_refused(shift.Refused)) {
        /// TODO in specific cases, this can overwrite image/event types that we set earlier.
        shift.EventImage = EImageBaseType::REFUSE;
        shift.EventType = EVENT_NOWORK;
        wrapped.generate_event();
    }
}

void cGenericJob::PostShift(sGirlShiftData& shift) {
    cGirlShift wrapped{&shift, this};
    on_post_shift(wrapped);
    if(!wrapped.data().DebugMessage.str().empty()) {
        shift.girl().AddMessage(wrapped.data().DebugMessage.str(), EImageBaseType::PROFILE, EEventType::EVENT_DEBUG);
    }
}

void cGenericJob::on_pre_shift(cGirlShift& shift) const {
    auto valid = IsJobValid(shift.girl(), shift.is_night_shift());
    if(!valid.IsValid) {
        shift.add_literal(valid.Reason);
        shift.set_result(ECheckWorkResult::INVALID);
        return;
    }

    if(!CheckCanWork(shift)) {
        shift.set_result(ECheckWorkResult::IMPOSSIBLE);
    } else if(CheckRefuseWork(shift)) {
        shift.set_result(ECheckWorkResult::REFUSES);
    } else {
        shift.set_result(ECheckWorkResult::ACCEPTS);
    }
}

void cGenericJob::OnRegisterJobManager(const cJobManager& manager) {
    assert(m_JobManager == nullptr);
    m_JobManager = &manager;
    load_job();
    setup_job();
}

namespace {
    std::string handle_failed_condition(const IGenericJob& job, const sAttributeCondition& cond) {
        if(cond.Direction == sAttributeCondition::AT_LEAST) {
            return "The " + job.get_info().Name + " job requires at least " + std::to_string(cond.LowerBound) +
                           " " + get_stat_skill_name(cond.Attribute) + ".";
        } else {
            return "The " + job.get_info().Name + " job requires at most " + std::to_string(cond.LowerBound) +
                           " " + get_stat_skill_name(cond.Attribute) + ".";
        }
    }

    std::string handle_failed_condition(const IGenericJob& job, const sTraitCondition& cond) {
        if(cond.FailMessage.empty()) {
            if(cond.Condition == sTraitCondition::REQUIRED) {
                return "The " + job.get_info().Name + " job requires the " + cond.TraitName + " trait.";
            } else {
                return "The " + job.get_info().Name + " job is not allowed for girls with the " + cond.TraitName + " trait.";
            }
        } else  {
            return cond.FailMessage;
        }
    }
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

    // check attribute conditions
    for(auto& cond : m_Info.Conditions) {
        if(!check_condition_boolean(cond, girl)) {
            return {false, std::visit([this](auto&& cond){ return handle_failed_condition(*this, cond); }, cond)};
        }
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
        // transitional code till we load all job ids from xml
        if(m_Info.JobId == JOB_UNSET) {
            m_Info.Name = GetStringAttribute(*job_data, "Name");
            m_Info.JobId = get_job_id(m_Info.Name);
        }

        m_Info.Title = GetStringAttribute(*job_data, "Title");

        if(const auto* desc_el = job_data->FirstChildElement("Description")) {
            if(const char* description = desc_el->GetText()) {
                m_Info.Description = description;
                boost::trim(m_Info.Description);
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

        // Requirements
        const auto* req_el = job_data->FirstChildElement("Requirements");
        if(req_el) {
            m_Info.FreeOnly = req_el->BoolAttribute("FreeOnly", false);
            for(auto& cond : IterateChildElements(*req_el, "Condition")) {
                if(cond.Attribute("Attribute")) {
                    std::string att_name = GetStringAttribute(cond, "Attribute");
                    if (cond.Attribute("Lower")) {
                        int value = cond.IntAttribute("Lower");
                        m_Info.Conditions.emplace_back(sAttributeCondition{get_stat_skill_id(att_name), value, value,
                                                                           sAttributeCondition::AT_LEAST});
                    } else {
                        int value = GetIntAttribute(cond, "Upper");
                        m_Info.Conditions.emplace_back(sAttributeCondition{get_stat_skill_id(att_name), value, value,
                                                                           sAttributeCondition::AT_MOST});
                    }
                } else if(cond.Attribute("RequiredTrait")) {
                    std::string trait = GetStringAttribute(cond, "RequiredTrait");
                    const char* message = cond.GetText();
                    std::string msg_tex = message ? message : "";
                    boost::trim(msg_tex);
                    m_Info.Conditions.emplace_back(sTraitCondition{std::move(trait), sTraitCondition::REQUIRED, msg_tex});
                } else if(cond.Attribute("ForbiddenTrait")) {
                    std::string trait = GetStringAttribute(cond, "ForbiddenTrait");
                    const char* message = cond.GetText();
                    std::string msg_tex = message ? message : "";
                    boost::trim(msg_tex);
                    m_Info.Conditions.emplace_back(sTraitCondition{std::move(trait), sTraitCondition::FORBIDDEN, msg_tex});
                }
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
            m_Info.IsFightingJob = config_el->BoolAttribute("FightingJob", false);

            const char* image_preset_name = config_el->Attribute("DefaultImage");
            if(image_preset_name) {
                m_Info.DefaultImage = get_image_preset(image_preset_name);
            }

            std::string phase = GetDefaultedStringAttribute(*config_el, "Phase", "main");
            std::vector<std::string> phases;
            boost::algorithm::split(phases, phase, boost::is_any_of("|"));
            m_Info.Phases = EJobPhase(0);
            for(auto& p : phases) {
                m_Info.Phases = EJobPhase(m_Info.Phases | get_phase_id(p));
            }

            if(m_Info.Phases == 0) {
                m_Info.Phases = EJobPhase::MAIN;
            }

            m_Info.BaseWages = config_el->IntAttribute("BaseWages");

            const auto* enjoy_el = config_el->FirstChildElement("Enjoyment");
            if(enjoy_el) {
                m_Info.PrimaryAction = get_activity_id(
                        GetDefaultedStringAttribute(*enjoy_el, "PrimaryAction", "Generic"));
                m_Info.SecondaryAction = get_activity_id(
                        GetDefaultedStringAttribute(*enjoy_el, "SecondaryAction", "Generic"));
                m_Info.BaseEnjoyment = enjoy_el->IntAttribute("BaseEnjoyment");
                m_Info.EnjoymentTraitModifier = GetDefaultedStringAttribute(*enjoy_el, "TraitModifier", "");
            }

            // Filters
            for(auto& filter_el : IterateChildElements(*config_el, "Filter")) {
                std::string filter_name = filter_el.GetText();
                m_Info.Filters.push_back(m_JobManager->get_filter_id(filter_name));
            }
        }

        load_from_xml_internal(*job_data, path.str());
    } catch (std::exception& error) {
        g_LogFile.error("job", "Error loading job xml '", m_XMLFile, "': ", error.what());
        throw std::runtime_error("Error loading job xml '" + m_XMLFile + "': " + error.what());
    }
}


const std::string& cGenericJob::get_text(const std::string& prompt, cGirlShift& shift) const {
    assert(m_TextRepo);
    try {
        cJobTextInterface interface{this, &shift};
        return m_TextRepo->get_text(prompt, interface);
    } catch (const std::out_of_range& oor) {
        g_LogFile.error("job", "Trying to get missing text '", prompt, "\' in job ", m_Info.Name);
        throw;
    }
}

bool cGenericJob::has_text(const std::string& prompt) const {
    if(!m_TextRepo) return false;
    return m_TextRepo->has_text(prompt);
}

void cGenericJob::Register(IJobManager& manager, std::unique_ptr<cGenericJob> job) {
    dynamic_cast<cJobManager&>(manager).register_job(std::move(job));
}

int cGenericJob::RegisterVariable(std::string name, sGirlShiftData::JobStateVar_t default_value) {
    int index = m_Variables.size();
    if(index >= NUM_JOB_VARIABLES) {
        throw std::runtime_error(std::string("Ran out of variables for job ") + get_job_name(job()));
    }
    sVariableData data = {std::move(name), index, default_value};
    m_Variables.push_back(data);
    return index;
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

void cGenericJob::on_post_shift(cGirlShift& shift) const {

}

const cJobManager& cGenericJob::job_manager() const {
    return *m_JobManager;
}

int cGenericJob::CalculateBasicEnjoyment(cGirlShift& shift) const {
    auto& girl = shift.girl();
    int enjoy = m_Info.BaseEnjoyment;
    int happy = girl.happiness();

    if(m_Info.PrimaryAction != EActivity::GENERIC) {
        int base_value = girl.enjoyment(m_Info.PrimaryAction);
        if(m_Info.SecondaryAction != EActivity::GENERIC) {
            base_value = (2 * base_value + girl.enjoyment(m_Info.SecondaryAction)) / 3;
        }
        enjoy += (2 * base_value + (girl.happiness() - 75)) / 3;
    } else {
        enjoy += happy - 75;
    }

    // modifiers: unhappy when very tired
    if(girl.tiredness() > 75) {
        enjoy -= (girl.tiredness() - 50) / 2;
    }

    // even more enjoyment reduction if working while really unhappy
    if(happy < 20) {
        enjoy -= 10;
    }

    // small happiness-based bonus in med enjoyment range
    if (happy > 80 && enjoy < 33 && enjoy > -33) {
        enjoy += 3;
    }

    // optimist or pessimist
    if(shift.chance(33)) {
        if(girl.has_active_trait(traits::OPTIMIST)) {
            enjoy += 1;
        } else if(girl.has_active_trait(traits::PESSIMIST)) {
            enjoy -= 1;
        }
    }

    if(!m_Info.EnjoymentTraitModifier.empty()) {
        enjoy += girl.get_trait_modifier(m_Info.EnjoymentTraitModifier.c_str());
    }

    enjoy += shift.uniform(-5, 5);

    // TODO Salary-based enjoyment

    return enjoy;
}

void cGenericJob::HandleWorkImpossible(cGirlShift& shift) const {
    shift.data().EventImage = EImageBaseType::PROFILE;
    shift.data().EventType = EVENT_WARNING;
    if(shift.data().EventMessage.tellp() < 1) {
        g_LogFile.warning("job", "Empty message for impossible work in job ", get_job_name(shift.data().Job));
        shift.data().EventMessage
            << "Error: This girl could not perform her job, "
            << "but the game failed to provide an adequate warning message.\n"
            << "Please report this problem to the developers.";
    }
    shift.generate_event();
}

int cGenericJob::get_setting_bool(const char* key) const {
    return g_Game->settings().get_bool(key);
}

const std::array<const char*, 4>& get_all_phases() {
    static std::array<const char*, 4> phases {
        "prepare", "produce", "main", "late"};
    return phases;
}

id_lookup_t<EJobPhase> build_phase_lookup() {
    id_lookup_t<EJobPhase> lookup;
    lookup["prepare"] = EJobPhase::PREPARE;
    lookup["produce"] = EJobPhase::PRODUCE;
    lookup["main"] = EJobPhase::MAIN;
    lookup["late"] = EJobPhase::LATE;
    return std::move(lookup);
}

const id_lookup_t<EJobPhase>& get_phase_lookup() {
    static auto lookup = build_phase_lookup();
    return lookup;
}

EJobPhase get_phase_id(const std::string& name) {
    return lookup_with_error(get_phase_lookup(), name, "Unknown Job Phase: ");
}
