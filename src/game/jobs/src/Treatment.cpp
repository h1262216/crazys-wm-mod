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

#include "Treatment.h"
#include "character/sGirl.h"
#include "traits/ITraitsManager.h"
#include "text/repo.h"
#include "IGame.h"
#include "cGirls.h"
#include "TextInterface.h"
#include "utils/string.hpp"
#include <tinyxml2.h>
#include "xml/util.h"
#include "xml/getattr.h"
#include "traits/ITraitSpec.h"
#include "utils/streaming_random_selection.hpp"

cTreatmentJob::cTreatmentJob(std::string xml_file) : cGenericJob(std::move(xml_file), EJobClass::TREATMENT)
{
    m_Info.Shift = EJobShift::FULL;
    // m_Info.Phase = EJobPhase::LATE;
    m_Info.IsFightingJob = false;
}

cTreatmentJob::~cTreatmentJob() = default;

void cTreatmentJob::load_from_xml_internal(const tinyxml2::XMLElement& job_data, const std::string& file_name) {
    const auto* config_el = job_data.FirstChildElement(specific_config_element());
    if(config_el) {
        load_from_xml_callback(*config_el);
    }
    const auto* results_el = job_data.FirstChildElement("Outcomes");
    if(results_el) {
        for(auto& outcome : IterateChildElements(*results_el)) {
            m_PossibleResults.push_back(load_result_from_xml(outcome));
        }
    }
    m_PossibleResults.push_back(sTreatmentResult{sTreatmentResult::CUSTOM_EFFECT, {}, 0, ""});
}

sTreatmentResult cTreatmentJob::load_result_from_xml(const tinyxml2::XMLElement& data) {
    std::string kind = data.Name();
    sTreatmentResult result;
    if(kind == "GainTrait") {
        result.Type = sTreatmentResult::ADD_TRAIT;
    }else if(kind == "LoseTrait") {
        result.Type = sTreatmentResult::REMOVE_TRAIT;
    }else if(kind == "GainAttribute") {
        result.Type = sTreatmentResult::INCREASE_ATTRIBUTE;
    }else if(kind == "LoseAttribute") {
        result.Type = sTreatmentResult::DECREASE_ATTRIBUTE;
    } else {
        throw std::runtime_error("Invalid outcome type: " + kind);
    }

    switch (result.Type) {
        case sTreatmentResult::ADD_TRAIT:
        case sTreatmentResult::REMOVE_TRAIT:
            result.Data = g_Game->traits().lookup(GetStringAttribute(data, "Trait"));
            break;
        case sTreatmentResult::INCREASE_ATTRIBUTE:
        case sTreatmentResult::DECREASE_ATTRIBUTE:
        {
            int min = GetIntAttribute(data, "Min");
            int max = GetIntAttribute(data, "Max");
            // TODO use the attribute's actual min/max here
            int threshold = data.IntAttribute("Threshold", result.Type == sTreatmentResult::INCREASE_ATTRIBUTE ? 100 : 0);
            StatSkill target = get_stat_skill_id(GetStringAttribute(data, "Attribute"));
            result.Data = sTreatmentAttributeData{target, min, max, threshold};
        }
    }

    result.Priority = data.IntAttribute("Priority", 0);
    return result;
}

void cTreatmentJob::on_pre_shift(cGirlShift& shift) const {
    cGenericJob::on_pre_shift(shift);
    if(shift.data().Refused == ECheckWorkResult::ACCEPTS) {
        if (shift.girl().get_active_treatment() != job()) {
            shift.girl().start_treatment(job());
        }
    }
}

void cTreatmentJob::on_complete_treatment(cGirlShift& shift) const {
    RandomSelectorV2<const sTreatmentResult*> selected_effect;
    std::mt19937 rng(g_Dice % 10000);
    for(auto& effect : m_PossibleResults) {
        if(is_result_possible(effect, shift.girl())) {
            selected_effect.process(rng, &effect, effect.Priority);
        }
    }

    if(selected_effect.selection().has_value()) {
        apply_result(*selected_effect.selection().get(), shift);
        auto msg = selected_effect.selection().get()->Message;
        if(!msg.empty()) {
            shift.add_literal(msg);
            shift.data().EventMessage << "\n";
        }
    }

    if(!has_valid_effect(shift.girl())) {
        on_fully_finished_treatment(shift);
    }
}

bool cTreatmentJob::is_result_possible(const sTreatmentResult& result, const sGirl& target) const {
    switch (result.Type) {
        case sTreatmentResult::REMOVE_TRAIT:
            return target.has_active_trait(std::get<const ITraitSpec*>(result.Data)->name().c_str());
        case sTreatmentResult::ADD_TRAIT:
            return !target.has_active_trait(std::get<const ITraitSpec*>(result.Data)->name().c_str());
        case sTreatmentResult::INCREASE_ATTRIBUTE:
        {
            auto& data = std::get<sTreatmentAttributeData>(result.Data);
            int current = target.get_attribute(data.Target);
            return current < data.Threshold;
        }
        case sTreatmentResult::DECREASE_ATTRIBUTE:
        {
            auto& data = std::get<sTreatmentAttributeData>(result.Data);
            int current = target.get_attribute(data.Target);
            return current > data.Threshold;
        }
        case sTreatmentResult::CUSTOM_EFFECT:
            return check_custom_treatment(target);
    }
}

void cTreatmentJob::apply_result(const sTreatmentResult& result, cGirlShift& shift) const {
    auto& target = shift.girl();
    switch (result.Type) {
        case sTreatmentResult::REMOVE_TRAIT:
            target.gain_trait(std::get<const ITraitSpec*>(result.Data)->name().c_str());
            break;
        case sTreatmentResult::ADD_TRAIT:
            target.lose_trait(std::get<const ITraitSpec*>(result.Data)->name().c_str());
            break;
        case sTreatmentResult::INCREASE_ATTRIBUTE:
        {
            auto& data = std::get<sTreatmentAttributeData>(result.Data);
            target.gain_attribute(data.Target, data.MinAmount, data.MaxAmount, data.Threshold);
            break;
        }
        case sTreatmentResult::DECREASE_ATTRIBUTE:
        {
            auto& data = std::get<sTreatmentAttributeData>(result.Data);
            target.update_attribute(data.Target, shift.uniform(data.MinAmount, data.MaxAmount));
            break;
        }
        case sTreatmentResult::CUSTOM_EFFECT:
            handle_custom_treatment(shift);
            break;
    }
}

sJobValidResult cTreatmentJob::on_is_valid(const sGirl& girl, bool night_shift) const {
    if (!has_valid_effect(girl)) {
        // TODO fix this!
        return {false, {"IMPOSSIBRU"}};
        //return {false, get_text("no-effect-possible")};
    }
    return {true, {}};
}

bool cTreatmentJob::has_valid_effect(const sGirl& girl) const {
    return std::any_of(m_PossibleResults.begin(), m_PossibleResults.end(), [&](auto&& result){
        return is_result_possible(result, girl);
    });
}

double cTreatmentJob::GetPerformance(const sGirl& girl, bool estimate) const {
    if(has_valid_effect(girl)) {
        return 100;
    }
    return 0;
}
