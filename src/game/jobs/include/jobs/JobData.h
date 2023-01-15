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
    sTraitChange(bool g, std::string trait, int amount, std::string message, EEventType e = EVENT_GOODNEWS,
                 int perf = -1000) :
            Gain(g), TraitName(std::move(trait)), Amount(amount), Message(std::move(message)), EventType(e),
            PerformanceRequirement(perf) {}
    // target change
    bool Gain;
    std::string TraitName;
    int Amount;

    // message
    std::string Message;
    EEventType EventType = EVENT_GOODNEWS;

    // Conditions
    int PerformanceRequirement = -1000;        // minimum job performance to consider the trait

    struct sAttributeCondition {
        StatSkill Attribute;
        int LowerBound;
        int UpperBound;
    };
    std::vector<sAttributeCondition> Conditions;
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
