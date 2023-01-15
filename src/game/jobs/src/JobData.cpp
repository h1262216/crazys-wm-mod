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

#include "JobData.h"
#include "character/sGirl.h"
#include "xml/util.h"
#include "xml/getattr.h"
#include "utils/streaming_random_selection.hpp"
#include "cRng.h"
#include "cGirls.h"
#include "IGame.h"
#include "traits/ITraitsManager.h"

extern cRng g_Dice;

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

void cJobGains::apply(sGirl& girl, int performance) const {
    // Improve stats
    int xp = XP, skill = Skill;
    enum LearningAbility {
        SLOW, NORMAL, QUICK
    } learner = NORMAL;

    if (girl.has_active_trait(traits::QUICK_LEARNER))        { xp += 3; learner = QUICK; }
    else if (girl.has_active_trait(traits::SLOW_LEARNER))    { xp -= 3; learner = SLOW; }

    girl.exp(g_Dice.in_range(xp/2, xp));

    if(!Gains.empty()) {
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

                // 66% slowdown of learning above maximum
                if(girl.get_attribute(gain->Source) > gain->Maximum && g_Dice.percent(66)) {
                    amount = 0;
                }

                // for skills, quick/slow learner makes a difference
                if(gain->Source.which() == 1) {
                    if(learner == SLOW && g_Dice.percent(33)) {
                        amount = 0;
                    } else if (learner == QUICK && g_Dice.percent(33)) {
                        amount *= 2;
                    }
                }
                girl.update_attribute(gain->Source, amount);
            }
        }
    }

    gain_traits(girl, performance);
}

void cJobGains::gain_traits(sGirl& girl, int performance) const {
    for(auto& trait : TraitChanges) {
        if(trait.PerformanceRequirement > performance)
            continue;

        if(trait.Chance < 100 && !g_Dice.percent(trait.Chance))
            continue;

        if(trait.Gain) {
            cGirls::PossiblyGainNewTrait(girl, trait.TraitName, trait.Threshold, trait.Action,
                                         trait.Message, false, trait.EventType);
        } else {
            cGirls::PossiblyLoseExistingTrait(girl, trait.TraitName, trait.Threshold, trait.Action,
                                              trait.Message, false);
        }
    }
}

void cJobGains::load(const tinyxml2::XMLElement& source) {
    XP = GetIntAttribute(source, "XP");
    Skill = GetIntAttribute(source, "Skill");

    auto load_trait_change = [&](const tinyxml2::XMLElement& element, bool gain) {
        std::string trait = GetStringAttribute(element, "Trait");
        int threshold = GetIntAttribute(element, "Threshold", -100, 100);
        int performance = element.IntAttribute("MinPerformance", -1000);
        int chance = element.IntAttribute("Chance", 100);
        Action_Types action = get_action_id(GetStringAttribute(element, "Action"));
        const char* msg_text = element.GetText();
        std::string message = (gain ? "${name} has gained the trait " : "${name} has lost the trait ") + trait;
        if(msg_text) {
            // TODO trim
            message = msg_text;
        }
        EEventType event_type = (EEventType)element.IntAttribute("Event", EVENT_GOODNEWS);
        TraitChanges.push_back({gain, trait, threshold, action, message, event_type, performance, chance});
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
        float max = stat_skill.FloatAttribute("Max", -100);
        auto skill = get_skill_id(GetStringAttribute(stat_skill, "Name"));
        Gains.push_back({skill, weight, min, max});
    }

    for(const auto& stat_skill : IterateChildElements(source, "Stat")) {
        float weight = stat_skill.FloatAttribute("Weight", 1.0);
        float min = stat_skill.FloatAttribute("Min", -100);
        float max = stat_skill.FloatAttribute("Max", -100);
        auto skill = get_stat_id(GetStringAttribute(stat_skill, "Name"));
        Gains.push_back({skill, weight, min, max});
    }
}
