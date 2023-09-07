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

#include "sAttributeCondition.h"
#include "character/sGirl.h"

int check_condition_fraction(const sAttributeCondition& cond, const sGirl& girl) {
    int value = girl.get_attribute(cond.Attribute);
    if(cond.Direction == sAttributeCondition::AT_LEAST) {
        if (value < cond.LowerBound)
            return 0;
        if (value < cond.UpperBound) {
            int range = cond.UpperBound - cond.LowerBound;
            int excess = value - cond.LowerBound;
            return std::max(1, (100 * excess) / range);
        }
        return 100;
    } else if(cond.Direction == sAttributeCondition::AT_MOST) {
        if (value > cond.UpperBound)
            return 0;
        if (value > cond.LowerBound) {
            int range = cond.UpperBound - cond.LowerBound;
            int excess = cond.UpperBound - value;
            return std::max(1, (100 * excess) / range);
        }
        return 100;
    }
    __builtin_unreachable();
}

bool check_condition_boolean(const sAttributeCondition& cond, const sGirl& girl) {
    int value = girl.get_attribute(cond.Attribute);
    if(cond.Direction == sAttributeCondition::AT_LEAST) {
        return value >= cond.LowerBound;
    } else if (cond.Direction == sAttributeCondition::AT_MOST) {
        return value <= cond.UpperBound;
    }
    __builtin_unreachable();
}

int check_condition_fraction(const sTraitCondition& condition, const sGirl& target) {
    return check_condition_boolean(condition, target) ? 100 : 0;
}

bool check_condition_boolean(const sTraitCondition& cond, const sGirl& girl) {
    bool has_trait = girl.has_active_trait(cond.TraitName.c_str());
    if(cond.Condition == sTraitCondition::REQUIRED) {
        return has_trait;
    } else {
        return !has_trait;
    }
}

int check_condition_fraction(const vGirlCondition& condition, const sGirl& target) {
    return std::visit([&](auto&& c){ return check_condition_fraction(c, target); }, condition);
}

bool check_condition_boolean(const vGirlCondition& condition, const sGirl& target) {
    return std::visit([&](auto&& c){ return check_condition_boolean(c, target); }, condition);
}