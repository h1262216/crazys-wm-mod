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

#include "IBuilding.h"
#include "character/cGirlPool.h"

sBuildingConfig::sBuildingConfig(BuildingType type, JOBS first, JOBS last, JOBS matron, bool day_shift) :
        Type(type), FirstJob(first), LastJob(last), MatronJob(matron), HasDayShift(day_shift) {
    assert(first < last);
    assert(first <= matron);
    assert(matron <= last);
}


IBuilding::IBuilding(std::string name, sBuildingConfig config) :
        m_Config(config),
        m_Name(std::move(name)),
        m_Girls(std::make_unique<cGirlPool>()) {
}
