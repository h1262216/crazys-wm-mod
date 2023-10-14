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

#ifndef WM_JOBDATA_H
#define WM_JOBDATA_H

#include <string>
#include <vector>

#include "Constants.h"
#include "sAttributeCondition.h"

namespace tinyxml2 {
    class XMLElement;
}

class cGirlShift;
class sGirl;
using StatSkill = std::variant<STATS, SKILLS>;

struct sWeightedStatSkill {
    StatSkill Source;
    float Weight  = 1.f;
    float Minimum = -100.f;
    float Maximum = 100.f;
};

class cJobPerformance {
public:
    float eval(const sGirl& girl, bool estimate) const;
    void load(const tinyxml2::XMLElement& source, const std::string& prefix);
private:
    struct sTraitMod {
        std::string Trait;
        float Weight = 1.f;
    };

    std::vector<sTraitMod>           TraitMod;   //!< Name of the trait modifier
    std::vector<sWeightedStatSkill>  Factors;    //!< List of stat/skill influences
};

struct sTraitChange {
    // explicit constructor needed for mingw
    sTraitChange(bool g, std::string trait, std::string message, EEventType e = EVENT_NONE);
    // target change
    bool Gain;
    std::string TraitName;

    struct sChangeAmount {
        int BaseAmount;
        int PerformanceRequirementMin = -1000;
        int PerformanceRequirementMax = -1000;
        int Chance = 100;
        std::vector<sAttributeCondition> Conditions;
    };

    // message
    std::string Message;
    EEventType EventType;

    std::vector<sChangeAmount> ChangeAmounts;
};

class cJobGains {
public:
    void apply(cGirlShift& shift) const;
    void load(const tinyxml2::XMLElement& source);

    int xp() const { return XP; }
    int skill() const { return Skill; }
    const std::vector<sWeightedStatSkill>& gains() const { return Gains; }
    const  std::vector<sTraitChange>& traits() const { return TraitChanges; }
private:
    void gain_traits(cGirlShift& shift) const;

    int XP    = 0;                                      //!< Amount of experience points gained
    int Skill = 0;                                      //!< Amount of gains for stats/skills
    std::vector<sWeightedStatSkill>  Gains;             //!< List of stat/skill influences
    std::vector<sTraitChange>        TraitChanges;      //!< List of potential Trait gains/losses
};

struct sJobPleasureData {
    int Amount = 0;                             // Determines the influence of lust
    SKILLS Skill = SKILLS::NUM_SKILLS;          // Which sex skill is required
    int SkillMin = 0;                           // Minimum skill to have so it is pleasurable
    int BaseValue = 0;                          // Base value if there is no lust at all
};

struct sObedienceData {
    int MaxDignity = 1000;     /// will more likely refuse if dignity is above this value
    int Obedience  = 10;       /// The amount of obedience required for her to do the job, if there were no modifiers.
    int Fear       = -1;       /// How afraid is she of this job.
};

#endif //WM_JOBDATA_H
