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

#ifndef WM_TREATMENT_H
#define WM_TREATMENT_H

#include "cGenericJob.h"
#include <variant>

namespace traits {
    class ITraitSpec;
}

struct sTreatmentAttributeData {
    StatSkill Target;
    int MinAmount;
    int MaxAmount;
    int Threshold;
};

struct sTreatmentResult {
    enum EType {
        REMOVE_TRAIT,
        ADD_TRAIT,
        INCREASE_ATTRIBUTE,
        DECREASE_ATTRIBUTE,
        CUSTOM_EFFECT
    };
    EType Type;
    std::variant<const traits::ITraitSpec*, sTreatmentAttributeData> Data;
    int Priority;
    std::string Message;
};

class cTreatmentJob : public cGenericJob {
public:
    cTreatmentJob(std::string xml_file);
    ~cTreatmentJob() override;

protected:
    void on_complete_treatment(cGirlShift& shift) const;
    virtual void on_fully_finished_treatment(cGirlShift& shift) const {}

    bool is_result_possible(const sTreatmentResult& result, const sGirl& target) const;
    void apply_result(const sTreatmentResult& result, cGirlShift& shift) const;
    static sTreatmentResult load_result_from_xml(const tinyxml2::XMLElement& data);

    sJobValidResult on_is_valid(const sGirl& girl, bool night_shift) const override;

    bool has_valid_effect(const sGirl& girl) const;

    double GetPerformance(const sGirl& girl, bool estimate) const override;

private:
    bool CheckCanWork(cGirlShift& girl) const override { return true; }
    bool CheckRefuseWork(cGirlShift& girl) const override { return false; };

    void on_pre_shift(cGirlShift& shift) const override;

    virtual const char* specific_config_element() const { return nullptr; }
    void load_from_xml_internal(const tinyxml2::XMLElement& job_data, const std::string& file_name) override;

    // For custom treatments
    virtual bool check_custom_treatment(const sGirl& girl) const { return false; }
    virtual void handle_custom_treatment(cGirlShift& shift) const { }

    // For reading the custom data in the <Treatment> tag
    virtual void load_from_xml_callback(const tinyxml2::XMLElement& job_element) {};

    std::vector<sTreatmentResult> m_PossibleResults;
};

#endif //WM_TREATMENT_H
