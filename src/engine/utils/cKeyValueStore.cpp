/*
 * Copyright 2019-2023, The Pink Petal Development Team.
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

#include "utils/cKeyValueStore.h"
#include "tinyxml2.h"
#include "xml/util.h"
#include "xml/getattr.h"
#include <utility>
#include "CLog.h"
#include "utils/lookup.h"

int cKeyValueBase::get_integer(const char *name) const {
    return std::get<sIntWithBounds>(get_value(name)).value;
}

bool cKeyValueBase::get_bool(const char *name) const {
    return std::get<bool>(get_value(name));
}

const std::string& cKeyValueBase::get_str(const char* name) const {
    return std::get<std::string>(get_value(name));
}


float cKeyValueBase::get_float(const char* name) const {
    const auto& val = get_value(name);
    if(std::holds_alternative<float>(val)) {
        return std::get<float>(val);
    } else {
        return (float)std::get<sPercent>(val);
    }
}

sPercent cKeyValueBase::get_percent(const char* name) const {
    return std::get<sPercent>(get_value(name));
}


void cKeyValueBase::set_value(const char *name, bool value) {
    auto& val = get_value(name);
    if(std::holds_alternative<bool>(val)) {
        val = value;
    } else {
        throw std::logic_error("Cannot set non-bool setting to bool value");
    }
}

void cKeyValueBase::set_value(const char* name, int value) {
    auto& val = get_value(name);
    if(std::holds_alternative<sIntWithBounds>(val)) {
        std::get<sIntWithBounds>(val).assign_checked(value);
    } else {
        throw std::logic_error("Cannot set non-integer setting to integer value");
    }
}

void cKeyValueBase::set_value(const char* name, float value) {
    auto& val = get_value(name);
    if(std::holds_alternative<float>(val)) {
        val = value;
    } else {
        throw std::logic_error("Cannot set non-float setting to float value");
    }
}

void cKeyValueBase::set_value(const char* name, std::string value) {
    auto& val = get_value(name);
    if(std::holds_alternative<std::string>(val)) {
        val = std::move(value);
    } else {
        throw std::logic_error("Cannot set non-string setting to string value");
    }
}

void cKeyValueBase::set_value(const char* name, sPercent value) {
    auto& val = get_value(name);
    if(std::holds_alternative<sPercent>(val)) {
        val = std::move(value);
    } else {
        throw std::logic_error("Cannot set non-string setting to string value");
    }
}

void cKeyValueBase::load_xml(const tinyxml2::XMLElement& root) {
    bool found_entry = false;
    for(auto& element : IterateChildElements(root, m_ElementName)) {
        found_entry = true;
        try {
            auto name = GetStringAttribute(element, m_KeyName);
            auto& value = get_value(name);
            switch(value.index()) {
                case 0:         // bool
                    value = GetBoolAttribute(element, m_ValueName);
                    break;
                case 1:         // IntWithBounds
                {
                    auto& bounded = std::get<sIntWithBounds>(value);
                    bounded.assign_checked(GetIntAttribute(element, m_ValueName, bounded.Min, bounded.Max));
                }
                    break;
                case 2:         // float
                {
                    const char* val_str = GetStringAttribute(element, m_ValueName);
                    float val = std::strtof(val_str, nullptr);
                    value = val;
                    break;
                }
                case 3:         // percent
                {
                    const char* val_str = GetStringAttribute(element, m_ValueName);
                    float val = std::strtof(val_str, nullptr);
                    value = sPercent(val / 100.f);
                    break;
                }
                case 4:         // string
                default:
                    value = std::string(GetStringAttribute(element, m_ValueName));
                    break;
            }

        } catch( std::exception& error ) {
            /// TODO fix the error handling, this message also comes for additional entries
            g_LogFile.error("settings", "Could not read attribute from xml element on line ",
                            element.GetLineNum(), ": ", error.what());
            continue;
        }
    }

    if(!found_entry && root.FirstChildElement() != nullptr) {
        // if there is content, but no matching elements, chances are there is a typo/wrong name somewhere
        g_LogFile.error("xml", "Reading a value set with expected Tag <", m_ElementName, ">, did not find ",
                        "any matching data. Root element <", root.Value(), "> given on line ", root.GetLineNum());
    }
}

namespace {
    struct visitor {
        const char* AttributeName;
        tinyxml2::XMLElement &t;

        template<class T>
        void operator()(T value) const {
            t.SetAttribute(AttributeName, value);
        }

        void operator()(const std::string& value) const {
            t.SetAttribute(AttributeName, value.c_str());
        }

        void operator()(sPercent value) const {
            t.SetAttribute(AttributeName, (float)value * 100);
        }

        void operator()(sIntWithBounds value) const {
            t.SetAttribute(AttributeName, value.value);
        }

        void operator()(std::monostate value) const {
        }

        using result_type = void;
    };
}

void cKeyValueBase::save_value_xml(tinyxml2::XMLElement& target, const settings_value_t& value) const {
    auto vis = visitor{m_ValueName, target};
    std::visit(vis, value);
}

// ---------------------------------------------------------------------------------------------------------------------

const settings_value_t& cSimpleKeyValue::get_value(const char* tag) const {
    return const_cast<cSimpleKeyValue*>(this)->get_value(tag);
}

settings_value_t& cSimpleKeyValue::get_value(const char* tag) {
    auto ref_iter = m_Settings.find(tag);
    if(ref_iter == m_Settings.end()) {
        throw std::out_of_range(std::string("Key ") + tag + " not found");
    }
    auto& ref = ref_iter->second;
    if(std::holds_alternative<std::monostate>(ref.value)) {
        return get_value(ref.fallback);
    }
    return ref.value;
}

std::string cSimpleKeyValue::get_display_name(const char* name) const {
    return lookup_with_error(m_Settings, name, "Could not find key").name;
}

std::string cSimpleKeyValue::get_description(const char* name) const {
    return lookup_with_error(m_Settings, name, "Could not find key").description;
}

void cSimpleKeyValue::save_xml(tinyxml2::XMLElement& target) const {
    for(auto& s : m_Settings) {
        auto& el = PushNewElement(target, m_ElementName);
        el.SetAttribute(m_KeyName, s.first.c_str());
        save_value_xml(el, s.second.value);
    }
}

void cSimpleKeyValue::add_setting(const char* tag, const char* name, settings_value_t default_value,
                                  const char* description, const char* fallback) {
    m_Settings[tag] = sKeyValueEntry{tag, name, description, fallback, std::move(default_value)};
}

void cSimpleKeyValue::add_setting(const char* tag, const char* name, const char* default_value, const char* description,
                                  const char* fallback) {
    add_setting(tag, name, std::string(default_value), description, fallback);
}

std::vector<std::string> cSimpleKeyValue::keys() const {
    std::vector<std::string> entries;
    entries.reserve(m_Settings.size());
    for(auto& setting : m_Settings) {
        entries.push_back(setting.first);
    }
    return entries;
}

void sIntWithBounds::assign(int new_value) noexcept(true) {
    if(new_value < Min) {
        value = Min;
        g_LogFile.warning("key-value", "Assigned value ", new_value, " less than lower bound ", Min);
    } else if(new_value > Max) {
        value = Max;
        g_LogFile.warning("key-value", "Assigned value ", new_value, " larger than lower bound ", Max);
    } else {
        value = new_value;
    }
}


void sIntWithBounds::assign_checked(int new_value) noexcept(false) {
    if(new_value < Min) {
        throw std::invalid_argument("Assigned value below lower bound");
    } else if(new_value > Max) {
        throw std::invalid_argument("Assigned value above upper bound");
    } else {
        value = new_value;
    }
}
