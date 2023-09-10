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

#include "JobData.h"
#include "character/sGirl.h"
#include "xml/util.h"
#include "xml/getattr.h"
#include "utils/streaming_random_selection.hpp"
#include "cGirls.h"
#include "IGame.h"
#include "traits/ITraitsManager.h"
#include "traits/ITraitSpec.h"
#include "traits/ITraitsCollection.h"
#include "jobs.h"
#include "cGirlShift.h"

namespace settings {
    extern const char* USER_SHOW_NUMBERS;
}

float cJobPerformance::eval(const sGirl& girl, bool estimate) const {
    float performance = 0.f;
    float weights = 0.f;
    for(auto& fac : Factors) {
        float add = std::max(fac.Minimum, std::min(fac.Maximum, (float)girl.get_attribute(fac.Source)));
        add = 100.f * (add - fac.Minimum) / (fac.Maximum - fac.Minimum);
        performance += add * fac.Weight;
        weights += fac.Weight;
    }

    performance = 2 * performance / weights;
    performance += 2 * girl.level();

    if (!estimate)
    {
        float t = girl.tiredness() - 80;
        if (t > 0)
            performance -= (t + 2) * (t / 3);
        float h = 25.f - girl.health();
        if (h > 0)
            performance -= (h + 2) * (h / 3.f);
    }

    for(const auto& mod : TraitMod) {
        performance += girl.get_trait_modifier(mod.Trait.c_str()) * mod.Weight;
    }
    performance = std::max(performance, 0.f);

    return performance;
}

void cJobPerformance::load(const tinyxml2::XMLElement& source, const std::string& prefix) {
    for(const auto& stat_skill : IterateChildElements(source, "Factor")) {
        float weight = stat_skill.FloatAttribute("Weight", 1.0);
        float min = stat_skill.FloatAttribute("Min", 0);
        float max = stat_skill.FloatAttribute("Max", 100);
        if(stat_skill.Attribute("Stat")) {
            auto stat = get_stat_id(GetStringAttribute(stat_skill, "Stat"));
            Factors.push_back({stat, weight, min, max});
        } else {
            auto skill = get_skill_id(GetStringAttribute(stat_skill, "Skill"));
            Factors.push_back({skill, weight, min, max});
        }
    }

    std::string mod_name = prefix + ".performance";
    if(g_Game->traits().load_modifier(source, mod_name)) {
        TraitMod.push_back(sTraitMod{mod_name, 1.f});
    }
    for(const auto& mod : IterateChildElements(source, "Mod")) {
        TraitMod.push_back(sTraitMod{mod.GetText(), mod.FloatAttribute("Weight", 1.f)});
    }
}

void cJobGains::apply(cGirlShift& shift) const {
    auto& girl = shift.girl();

    // Improve stats
    int xp = XP, skill = Skill;
    enum LearningAbility {
        SLOW, NORMAL, QUICK
    } learner = NORMAL;

    shift.data().DebugMessage << "\nGains: ";

    if (girl.has_active_trait(traits::QUICK_LEARNER))        { xp += 3; learner = QUICK; }
    else if (girl.has_active_trait(traits::SLOW_LEARNER))    { xp -= 3; learner = SLOW; }

    int gain_xp = shift.uniform(xp/2, xp);
    girl.exp(gain_xp);
    shift.data().EventMessage << "She gained: \n" << "  " << gain_xp << " Exp.";
    if(g_Game->settings().get_bool(settings::USER_SHOW_NUMBERS)) {
        shift.data().EventMessage << " (" << girl.exp() << ")";
    }
    shift.data().EventMessage << "\n";
    shift.data().DebugMessage << " XP: [" << xp/2 << ", " << xp << "] -> " << gain_xp << "\n";

    if(!Gains.empty()) {
        // TODO something without dynamic memory allocation would be better here I think
        std::unordered_map<StatSkill, int> accumulated_gains;
        while (skill > 0) {
            RandomSelector<const sWeightedStatSkill> target;
            for (auto& factor: Gains) {
                // higher chance of selection for skills below minimum
                float weight = factor.Weight;
                if(girl.get_attribute(factor.Source) < factor.Minimum) {
                    weight *= 2;
                }
                target.process(&factor, weight);
            }

            auto gain = target.selection();
            if(gain) {
                int amount = 1;
                skill -= amount;
                int current_value = girl.get_attribute(gain->Source);

                shift.data().DebugMessage << " Selected " << get_stat_skill_name(gain->Source)
                    << ": max=" << gain->Maximum << ", val=" << current_value << "\n";

                // 66% slowdown of learning above maximum
                if( current_value > gain->Maximum && g_Dice.percent(66)) {
                    amount = 0;
                    shift.data().DebugMessage << " No gain because maximum is exceeded\n";
                }

                // for skills, quick/slow learner makes a difference
                if(gain->Source.index() == 1) {
                    if(learner == SLOW && g_Dice.percent(33)) {
                        amount = 0;
                        shift.data().DebugMessage << " No gain because Slow Learner\n";
                    } else if (learner == QUICK && g_Dice.percent(33)) {
                        amount *= 2;
                        shift.data().DebugMessage << " Double gain because Quick Learner\n";
                    }
                }

                // receive the accumulated gains
                if(amount > 0) {
                    auto found = accumulated_gains.find(gain->Source);
                    if(found != accumulated_gains.end()) {
                        found->second += amount;
                    } else {
                        accumulated_gains[gain->Source] = amount;
                    }
                }
            }
        }

        for(auto [gain, amount]: accumulated_gains) {
            int new_value = girl.update_attribute(gain, amount);
            shift.data().EventMessage << "  " << amount << " " << get_stat_skill_name(gain);
            if(g_Game->settings().get_bool(settings::USER_SHOW_NUMBERS)) {
                shift.data().EventMessage << " (" << new_value << ")";
            }
            shift.data().EventMessage << "\n";
        }
    }

    gain_traits(shift);
}

namespace {
    int lin_factor(int value, int lower, int upper) {
        if(value < lower)
            return 0;
        if(value < upper) {
            return std::max(10, (100 * (value - lower)) / (upper - lower));
        }
        return 100;
    }
}

sTraitChange::sTraitChange(bool g, std::string trait, std::string message, EEventType e) :
    Gain(g), TraitName(std::move(trait)), Message(std::move(message)), EventType(e) {
    if(EventType == EVENT_NONE) {
        EventType = Gain ? EVENT_GAIN_TRAIT : EVENT_LOSE_TRAIT;
    }
}

void cJobGains::gain_traits(cGirlShift& shift) const {
    int performance = shift.performance();
    auto& girl = shift.girl();

    for(auto& trait : TraitChanges) {
        shift.data().DebugMessage << " Trait " << trait.TraitName << "\n";
        int amount = 0;
        for(auto& change : trait.ChangeAmounts) {
            if(!g_Dice.percent(change.Chance)) {
                shift.data().DebugMessage << "  discarding (" << change.Chance << "%).\n";
                continue;
            }
            int base = change.BaseAmount;
            int factor = lin_factor(performance, change.PerformanceRequirementMin, change.PerformanceRequirementMax);

            for(auto& cond : change.Conditions) {
                int new_factor = check_condition_fraction(cond, girl);
                if(new_factor > 0 && new_factor < 10)
                    new_factor = 10;
                factor = std::min(factor, new_factor);
            }

            shift.data().DebugMessage << "  base: " << base << ", factor: " << factor << "\n";
            amount += (factor * base) / 100;
        }
        if(amount <= 0)
            continue;

        if(trait.Gain) {
            // only gain traits that are not currently blocked.
            if(!girl.raw_traits().is_trait_blocked(trait.TraitName.c_str())) {
                cGirls::PossiblyGainNewTrait(girl, trait.TraitName, amount, trait.Message, EImageBaseType::PROFILE,
                                             trait.EventType);
            }
        } else {
            cGirls::PossiblyLoseExistingTrait(girl, trait.TraitName, amount, trait.Message, EImageBaseType::PROFILE, trait.EventType);
        }
    }
}

namespace {
    sTraitChange::sChangeAmount load_change_amount(const tinyxml2::XMLElement& element) {
        sTraitChange::sChangeAmount amount;
        amount.BaseAmount = GetIntAttribute(element, "Value");
        amount.Chance = element.IntAttribute("Chance", 100);

        for(auto& cond_el : IterateChildElements(element, "TraitChangeCondition")) {
            int lower = -1;
            int upper = -1;
            if(cond_el.Attribute("Value")) {
                int val = GetIntAttribute(cond_el, "Value");
                lower = val;
                upper = val;
            } else {
                lower = GetIntAttribute(cond_el, "Lower");
                upper = GetIntAttribute(cond_el, "Upper");
            }
            std::string what = GetStringAttribute(cond_el, "What");
            if(what == "JobPerformance") {
                amount.PerformanceRequirementMin = lower;
                amount.PerformanceRequirementMax = upper;
            } else {
                amount.Conditions.push_back(sAttributeCondition{get_stat_skill_id(what), lower, upper, sAttributeCondition::AT_LEAST});
            }
        }
        return amount;
    };
}

void cJobGains::load(const tinyxml2::XMLElement& source) {
    XP = GetIntAttribute(source, "XP");
    Skill = GetIntAttribute(source, "Skill");

    auto load_trait_change = [&](const tinyxml2::XMLElement& element, bool gain) {
        std::string trait = GetStringAttribute(element, "Trait");
        auto* msg_el = element.FirstChildElement("Text");
        std::string message = (gain ? "${name} has gained the trait " : "${name} has lost the trait ") + trait;
        if(msg_el) {
            // TODO trim
            message = msg_el->GetText();
        }

        auto event_type = (EEventType)element.IntAttribute("Event", EVENT_NONE);
        TraitChanges.emplace_back(gain, trait, message, event_type);
        g_Game->traits().lookup(trait.c_str());  // Check that trait exists
        for(auto& amount_el : IterateChildElements(element, "TraitChangeAmount")) {
            TraitChanges.back().ChangeAmounts.push_back(load_change_amount(amount_el));
        }
        if(TraitChanges.back().ChangeAmounts.empty()) {
            throw std::runtime_error("Missing <Amount> for trait change element");
        }
    };

    for(const auto& trait_change : IterateChildElements(source, "GainTrait")) {
        load_trait_change(trait_change, true);
    }

    for(const auto& trait_change : IterateChildElements(source, "LoseTrait")) {
        load_trait_change(trait_change, false);
    }

    for(const auto& stat_skill : IterateChildElements(source, "Skill")) {
        float weight = stat_skill.FloatAttribute("Weight", 1.0);
        float min = stat_skill.FloatAttribute("Min", -100);
        float max = stat_skill.FloatAttribute("Max", 100);
        auto skill = get_skill_id(GetStringAttribute(stat_skill, "Name"));
        Gains.push_back({skill, weight, min, max});
    }

    for(const auto& stat_skill : IterateChildElements(source, "Stat")) {
        float weight = stat_skill.FloatAttribute("Weight", 1.0);
        float min = stat_skill.FloatAttribute("Min", -100);
        float max = stat_skill.FloatAttribute("Max", 100);
        auto skill = get_stat_id(GetStringAttribute(stat_skill, "Name"));
        Gains.push_back({skill, weight, min, max});
    }
}

bool is_refused(ECheckWorkResult result) {
    switch (result) {
        case ECheckWorkResult::REFUSE_HATE:
        case ECheckWorkResult::REFUSE_DIGNITY:
        case ECheckWorkResult::REFUSE_FEAR:
        case ECheckWorkResult::REFUSE_HORNY:
        case ECheckWorkResult::REFUSES:
            return true;
        default:
            return false;
    }
}

bool is_impossible(ECheckWorkResult result) {
    switch (result) {
        case ECheckWorkResult::IMPOSSIBLE:
        case ECheckWorkResult::INVALID:
            return true;
        default:
            return false;
    }
}