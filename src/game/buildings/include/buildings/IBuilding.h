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

#ifndef WM_IBUILDING_H
#define WM_IBUILDING_H

#include "Constants.h"
#include <memory>

class sGirl;
class cGirlPool;

namespace tinyxml2 {
    class XMLElement;
}

struct sCustomerConfig {
    int GroupChance = 5;
    int WomanChance = 15;
};

struct sBuildingConfig {
    sBuildingConfig(BuildingType type, JOBS first, JOBS last, JOBS matron, bool day_shift = true);

    sBuildingConfig&& spawn(SpawnReason sr, std::string event) && {
        Spawn = sr;
        SpawnEvent = std::move(event);
        return std::move(*this);
    }

    BuildingType Type;
    JOBS FirstJob;
    JOBS LastJob;
    JOBS MatronJob;
    bool HasDayShift;
    SpawnReason Spawn = SpawnReason::COUNT;
    std::string SpawnEvent;
    sCustomerConfig Customer;
};

class IBuildingShift;
class sCustomer;

class IBuilding
{
public:
    IBuilding(std::string name, sBuildingConfig config);
    virtual ~IBuilding() = default;

    IBuilding(const IBuilding&) = delete;

    virtual void auto_assign_job(sGirl& target, std::stringstream& message, bool is_night) = 0;

    // basic info functions
    const sBuildingConfig& config() const { return m_Config; };
    BuildingType type() const { return m_Config.Type; };
    const std::string& name() const { return m_Name; }
    void set_name(std::string new_name) { m_Name = std::move(new_name); }

    // saving / loading
    virtual void load_xml(const tinyxml2::XMLElement& root) = 0;
    virtual void save_xml(tinyxml2::XMLElement& root) const = 0;

    // girl management
    const cGirlPool& girls() const { return *m_Girls; }
    cGirlPool& girls() { return *m_Girls; }

    virtual void add_girl(std::shared_ptr<sGirl> girl, bool keep_job) = 0;
    virtual std::shared_ptr<sGirl> remove_girl(sGirl* girl) = 0;

    // encounters
    // this is called for when the player tries to meet a new girl at this location
    virtual bool CanEncounter() const = 0;
    virtual std::shared_ptr<sGirl> TryEncounter() = 0;

    virtual IBuildingShift& shift() const = 0;

    // building-specific variables
    virtual int get_variable(const std::string& name) const = 0;

    virtual void setup_customer(sCustomer& customer) const {};

private:
    sBuildingConfig m_Config;
    std::string m_Name;

    std::unique_ptr<cGirlPool> m_Girls;
};

#endif //WM_IBUILDING_H
