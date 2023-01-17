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

#include <boost/variant.hpp>
#include "Constants.h"

namespace tinyxml2 {
    class XMLElement;
}

class sGirl;
using StatSkill = boost::variant<STATS, SKILLS>;

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

    struct sAttributeCondition {
        StatSkill Attribute;
        int LowerBound;
        int UpperBound;
    };

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
    void apply(sGirl& target, int performance) const;
    void load(const tinyxml2::XMLElement& source);
private:
    void gain_traits(sGirl& girl, int performance) const;

    int XP;                                             //!< Amount of experience points gained
    int Skill;                                          //!< Amount of gains for stats/skills
    std::vector<sWeightedStatSkill>  Gains;             //!< List of stat/skill influences
    std::vector<sTraitChange>        TraitChanges;      //!< List of potential Trait gains/losses
};

#endif //WM_JOBDATA_H
