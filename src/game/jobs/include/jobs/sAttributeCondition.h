/*
 * Copyright 2023, The Pink Petal Development Team.
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

#ifndef WM_S_ATTRIBUTE_CONDITION_H
#define WM_S_ATTRIBUTE_CONDITION_H

#include "Constants.h"
#include <variant>

class sGirl;

struct sAttributeCondition {
    enum EDirection : bool {
        AT_LEAST,
        AT_MOST,
    };
    StatSkill Attribute;
    int LowerBound;
    int UpperBound;
    EDirection Direction = EDirection::AT_LEAST;
};

struct sTraitCondition {
    std::string TraitName;
    enum ECondition : bool {
        REQUIRED,
        FORBIDDEN
    };
    ECondition Condition;
    std::string FailMessage;
};

int check_condition_fraction(const sAttributeCondition& condition, const sGirl& target);
bool check_condition_boolean(const sAttributeCondition& condition, const sGirl& target);

int check_condition_fraction(const sTraitCondition& condition, const sGirl& target);
bool check_condition_boolean(const sTraitCondition& condition, const sGirl& target);

using vGirlCondition = std::variant<sAttributeCondition, sTraitCondition>;

int check_condition_fraction(const vGirlCondition& condition, const sGirl& target);
bool check_condition_boolean(const vGirlCondition& condition, const sGirl& target);

#endif // WM_S_ATTRIBUTE_CONDITION_H