/*
 * Copyright 2022, The Pink Petal Development Team.
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

#include <sstream>
#include "cBuildingShift.h"
#include "cBuilding.h"
#include "character/cCustomers.h"
#include "IGame.h"
#include "CLog.h"
#include "jobs/cJobManager.h"
#include "cGirls.h"
#include "utils/lookup.h"

namespace settings{
    extern const char* PREG_COOL_DOWN;
}

cBuildingShift::cBuildingShift(cBuilding* building) : m_Building(building) {
    assert(m_Building);
    declare_resource(CleaningAmountId);
}

void cBuildingShift::declare_resource(const std::string& name) {
    auto result = m_ShiftResources.insert(std::make_pair(name, 0));
    assert(result.second);
}

void cBuildingShift::setup_resources() {
    g_LogFile.info("shift", "Set up resources");
    for(auto& res : m_ShiftResources) {
        res.second = 0;
    }

    for(auto& ia : m_ShiftInteractions) {
        ia.second.TotalConsumed = 0;
        ia.second.TotalProvided = 0;
        ia.second.Workers.clear();
    }
}

int cBuildingShift::GetResourceAmount(const std::string& name) const {
    return lookup_with_error(m_ShiftResources, name, "Could not find resource");
}

int cBuildingShift::ConsumeResource(const std::string& name, int amount) {
    int has = GetResourceAmount(name);
    if(has >= amount) {
        m_ShiftResources[name] -= amount;
        return amount;
    } else {
        m_ShiftResources[name] = 0;
        return has;
    }
}

bool cBuildingShift::TryConsumeResource(const std::string& name, int amount) {
    int has = GetResourceAmount(name);
    if(has >= amount) {
        m_ShiftResources[name] -= amount;
        return true;
    }
    return false;
}

void cBuildingShift::ProvideResource(const std::string& name, int amount) {
    m_ShiftResources.at(name) += amount;
}

void cBuildingShift::declare_interaction(const std::string& name) {
    auto result = m_ShiftInteractions.insert(std::make_pair(name, sInteractionData{}));
    assert(result.second);
}

void cBuildingShift::ProvideInteraction(const std::string& name, sGirl* source, int amount) {
    sInteractionData& data = m_ShiftInteractions.at(name);
    data.TotalProvided += amount;
    data.Workers.push_back(sInteractionWorker{source, amount});
}

sGirl* cBuildingShift::RequestInteraction(const std::string& name) {
    sInteractionData& data = m_ShiftInteractions.at(name);
    auto& candidates = data.Workers;
    data.TotalConsumed += 1;
    auto res = std::max_element(begin(candidates), end(candidates),
                                [](auto&& a, auto&& b){ return a.Amount < b.Amount; });
    if(res == end(candidates))  return nullptr;
    if(res->Amount == 0) return nullptr;
    res->Amount -= 1;
    return res->Worker;
}

bool cBuildingShift::HasInteraction(const std::string& name) const {
    const sInteractionData& data = m_ShiftInteractions.at(name);
    auto& candidates = data.Workers;
    return std::any_of(begin(candidates), end(candidates),[](auto&& a){ return a.Amount > 0; });
}

int cBuildingShift::NumInteractors(const std::string& name) const {
    const sInteractionData& data = m_ShiftInteractions.at(name);
    return data.Workers.size();
}

int cBuildingShift::GetInteractionProvided(const std::string& name) const {
    return m_ShiftInteractions.at(name).TotalProvided;
}

int cBuildingShift::GetInteractionConsumed(const std::string& name) const {
    return m_ShiftInteractions.at(name).TotalConsumed;
}

void cBuildingShift::TriggerInteraction(sGirl& interactor, sGirl& target) {
    auto& iad = get_girl_data(interactor);
    auto& iat = get_girl_data(target);
    g_Game->job_manager().get_job(iad.Job)->HandleInteraction(iad, iat);
}


cGold& cBuildingShift::Finance() {
    return m_Building->m_Finance;
}

void cBuildingShift::AddMessage(std::string message, EEventType event) {
    m_Building->AddMessage(std::move(message), event);
}

int cBuildingShift::Filthiness() const {
    return m_Building->filthiness();
}

void cBuildingShift::ProvideCleaning(int amount) {
    ProvideResource(CleaningAmountId, amount);
    if(m_Building->filthiness() > 0) {
        m_Building->m_Filthiness -= ConsumeResource(CleaningAmountId, m_Building->filthiness());
    }
}

void cBuildingShift::GenerateFilth(int amount) {
    amount -= ConsumeResource(CleaningAmountId, amount);
    m_Building->m_Filthiness += amount;
}

sCustomer& cBuildingShift::AttractCustomer() {
    m_Customers.push_back(g_Game->customers().CreateCustomer(*m_Building));
    return *m_Customers.back();
}

void cBuildingShift::run_shift(bool is_night) {
    g_LogFile.info("shift", "Running shift: ", is_night ? "night": "day", " for ", m_Building->name());
    begin_shift(is_night);
    g_LogFile.info("shift", "Running Prepare phase");
    update_girls(EJobPhase::PREPARE);
    m_Building->AttractCustomers(*this, is_night);
    g_LogFile.info("shift", "Running Produce phase");
    update_girls(EJobPhase::PRODUCE);
    debug_resources();
    g_LogFile.info("shift", "This shift attracted ", m_Customers.size(), " customers");
    g_LogFile.info("shift", "Running Main phase");
    update_girls(EJobPhase::MAIN);
    g_LogFile.info("shift", "Running Late phase");
    m_Building->PostMainShift(is_night);
    /// TODO handle customer-initiated jobs
    update_girls(EJobPhase::LATE);
    g_LogFile.info("shift", "Running end shift");
    end_shift(is_night);

    m_Building->EndShift(is_night);
}

void cBuildingShift::begin_shift(bool is_night) {
    setup_resources();
    m_Customers.clear();
    setup_girls(is_night);
    set_active_matron(m_Building->SetupMatron(is_night));

    g_LogFile.info("shift", "Running pre-shift handlers");
    apply_to_girls([this, is_night](sGirl& girl){
        cGirls::UseItems(girl);
        cGirls::CalculateGirlType(girl);            // update the fetish traits
        cGirls::UpdateAskPrice(girl, true);    // Calculate the girls asking price
        auto& shift = get_girl_data(girl);
        handle_resting_girl(shift);

        // call pre-shift until we've settled on a job
        auto old_job = JOB_UNSET;
        while(shift.Job != old_job) {
            old_job = shift.Job;
            g_Game->job_manager().handle_pre_shift(shift);
        }
    });
};

void cBuildingShift::handle_resting_girl(sGirlShiftData& shift) {

    auto& girl = shift.girl();
    if (girl.m_PregCooldown == g_Game->settings().get_integer(settings::PREG_COOL_DOWN))
    {
        shift.Refused = ECheckWorkResult::IMPOSSIBLE;
        shift.Job = JOB_RESTING;
        girl.AddMessage("${name} is on maternity leave.", EImageBaseType::PROFILE, EVENT_SUMMARY);
    }

    if (shift.Job != JOB_RESTING) {
        // skip anyone not resting
        return;
    }
    auto sum = EVENT_SUMMARY;
    std::stringstream ss;
    ss.str("");

    const auto& girlName = girl.FullName();
    if (m_Building->handle_resting_girl(girl, shift.IsNightShift, ActiveMatron(), ss)) {
        // nothing to do her, handle_resting_girl did all the work.
    }
    else if (girl.health() < 80 || girl.tiredness() > 20)
    {
        g_Game->job_manager().handle_main_shift(shift);
    }
    else if (ActiveMatron())    // send her back to work
    {
        int psw = shift.IsNightShift ? girl.m_PrevNightJob : girl.m_PrevDayJob;
        if (psw != JOB_RESTING && psw != 255)
        {    // if she had a previous job, put her back to work.
            if(!m_Building->handle_back_to_work(girl, ss, shift.IsNightShift)) {
                if (shift.IsNightShift == SHIFT_DAY)
                {
                    girl.m_DayJob = girl.m_PrevDayJob;
                    if (girl.m_NightJob == JOB_RESTING && girl.m_PrevNightJob != JOB_RESTING && girl.m_PrevNightJob != 255)
                        girl.m_NightJob = girl.m_PrevNightJob;
                }
                else
                {
                    if (girl.m_DayJob == JOB_RESTING && girl.m_PrevDayJob != JOB_RESTING && girl.m_PrevDayJob != 255)
                        girl.m_DayJob = girl.m_PrevDayJob;
                    girl.m_NightJob = girl.m_PrevNightJob;
                }
                ss << "The " << get_job_name(m_Building->matron_job()) << " puts " << girlName << " back to work.\n";
            }
        }
        else if (girl.m_DayJob == JOB_RESTING && girl.m_NightJob == JOB_RESTING)
        {
            m_Building->auto_assign_job(girl, ss, shift.IsNightShift);
        }
        girl.m_PrevDayJob = girl.m_PrevNightJob = JOB_UNSET;
        sum = EVENT_BACKTOWORK;
    }
    else if (girl.health() < 100 || girl.tiredness() > 0)    // if there is no matron to send her somewhere just do resting
    {
        g_Game->job_manager().handle_main_shift(shift);
    }
    else    // no one to send her back to work
    {
        ss << "WARNING " << girlName << " is doing nothing!\n";
        sum = EVENT_WARNING;
    }

    if (ss.str().length() > 0) girl.AddMessage(ss.str(), EImageBaseType::PROFILE, sum);
}

sGirlShiftData::sGirlShiftData(sGirl* girl, IBuildingShift* building, JOBS job, bool shift) :
    Job(job), IsNightShift(shift), m_Girl(girl), m_BuildingShift(building), m_Rng(&g_Dice), EventImage(EImageBaseType::PROFILE) {
    m_ProcessingCache.fill(0);
}

void cBuildingShift::setup_girls(bool is_night) {
    m_GirlShiftData.clear();
    g_LogFile.info("shift", "Set up girls");
    apply_to_girls([&](sGirl& girl){
        m_GirlShiftData.emplace(girl.GetID(),
                                sGirlShiftData{&girl, this, girl.get_job(is_night), is_night});
    });
}

void cBuildingShift::end_shift(bool is_night) {
    apply_to_girls([&](sGirl& current)
    {
        m_Building->GirlEndShift(current, is_night);
        auto& shift = get_girl_data(current);
        make_summary_message(shift);
    });
}

void cBuildingShift::update_girls(EJobPhase phase) {
    apply_to_girls(
    [&](auto& current) {
        auto& shift = get_girl_data(current);
        if(shift.Refused == ECheckWorkResult::ACCEPTS) {
            auto* job = g_Game->job_manager().get_job(shift.Job);
            if (job->phase() == phase) {
                g_Game->job_manager().handle_main_shift(shift);
            }
        }
    });
}

void cBuildingShift::apply_to_girls(const std::function<void(sGirl&)>& handler) {
    m_Building->girls().apply([&](sGirl& current)
    {
      if (current.is_dead())
          return;
      handler(current);
    });
}

void cBuildingShift::make_summary_message(sGirlShiftData& shift)
{
    // Summary Messages
    if (shift.Refused == ECheckWorkResult::REFUSES)
    {
        //brothel->m_Fame -= girl.fame();
        shift.girl().AddMessage("${name} refused to work so she made no money.", EImageBaseType::PROFILE, EVENT_SUMMARY);
    }
    // TODO handle the 'IMPOSSIBLE' case
    else
    {
        //brothel->m_Fame += girl.fame();
        std::stringstream ss;
        auto money_data = g_Game->job_manager().CalculatePay(shift);
        ss << "${name} made " << money_data.Earnings;
        if(money_data.Tips != 0) {
            ss << " and " << money_data.Tips << " in tips. ";
        } else {
            ss << " gold. ";
        }
        if (money_data.Wages > 0) ss << "You paid her a salary of " << money_data.Wages << ". ";
        ss << "In total, she got " << money_data.GirlGets << " gold and you ";
        if(money_data.PlayerGets > 0) {
            ss << "got " << money_data.PlayerGets << " gold.";
        } else {
            ss << "spent " << -money_data.PlayerGets << " gold.";
        }

        shift.girl().AddMessage(ss.str(), EImageBaseType::PROFILE, EVENT_SUMMARY);
    }
}

sGirlShiftData& cBuildingShift::get_girl_data(const sGirl& girl) {
    auto found = m_GirlShiftData.find(girl.GetID());
    if(found != m_GirlShiftData.end()) {
        return found->second;
    } else {
        BOOST_THROW_EXCEPTION(std::runtime_error("Error trying to get data for girl " + girl.FullName()));
    }
}

IBuilding* cBuildingShift::Building() {
    return m_Building;
}

void cBuildingShift::debug_resources() {
    g_LogFile.info("shift", "Resources overview: ");
    for(auto& res : m_ShiftResources) {
        g_LogFile.info("shift", res.first, ": ", res.second);
    }
}

