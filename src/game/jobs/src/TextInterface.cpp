/*
 * Copyright 2022-2023, The Pink Petal Development Team.
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

#include "TextInterface.h"
#include "IGenericJob.h"
#include "character/sGirl.h"
#include "CLog.h"
#include "utils/lookup.h"
#include "jobs/cGenericJob.h"

bool cJobTextInterface::LookupBoolean(const std::string& name) const {
    return m_Shift->girl().has_active_trait(name.c_str());
}

int cJobTextInterface::LookupNumber(const std::string& name) const {
    auto split_point = name.find(':');
    auto type = name.substr(0, split_point);
    if(type == "stat") {
        return m_Shift->girl().get_stat(get_stat_id(name.substr(split_point+1)));
    } else if(type == "skill") {
        return m_Shift->girl().get_skill(get_skill_id(name.substr(split_point+1)));
    } else if (type.size() == name.size()) {
        if(name == "Performance") {
            return m_Shift->performance();
        } else if(name == "Wages") {
            return m_Shift->data().Wages;
        } else if(name == "Tips") {
            return m_Shift->data().Tips;
        }
        auto cjob = dynamic_cast<const cGenericJob*>(m_Job);
        if(int var = cjob->FindVariable(name); var != -1) {
            return std::get<int>(m_Shift->get_variable(var));
        }
        g_LogFile.error("job", "Unknown variable ", name);
        BOOST_THROW_EXCEPTION(std::runtime_error("Unknown variable: " + name));
    } else {
        g_LogFile.error("job", "Unknown value category ", type, " of variable ", name);
        BOOST_THROW_EXCEPTION(std::runtime_error("Unknown value category: " + type));
    }
}

void cJobTextInterface::SetVariable(const std::string& name, int value) const {
    if(name == "Performance") {
        m_Shift->data().Performance = value;
    } else if(name == "Wages") {
        m_Shift->data().Wages = value;
    } else if(name == "Tips") {
        m_Shift->data().Tips = value;
    } else {
        BOOST_THROW_EXCEPTION(std::runtime_error("Unknown variable: " + name));
    }
}

void cJobTextInterface::SetVariable(const std::string& name, std::string value) const {
    //m_MappedStringValues.at(name)(value);
}

void cJobTextInterface::TriggerEvent(const std::string& name) const {
    throw std::logic_error("Event triggers are not implemented yet");
}
