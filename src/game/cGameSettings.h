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

#ifndef WM_CGAMESETTINGS_H
#define WM_CGAMESETTINGS_H

#include <string>
#include <vector>
#include "utils/cKeyValueStore.h"


struct sKeyValueEntry {
    const char* tag;
    const char* name;
    const char* description;
    settings_value_t value;
};

/*
 * A specialized key-value store that attaches a human-readable name and description to each entry.
 */
class cGameSettings : public cKeyValueBase {
public:
    cGameSettings();
    const settings_value_t & get_value(const char* name) const override;
    std::string get_display_name(const char* name) const override;
    std::string get_description(const char* name) const override;

    // list all settings
    std::vector<const sKeyValueEntry*> list_all_settings() const;

    std::vector<std::string> keys() const override;

    // io
    void save_xml(tinyxml2::XMLElement& target) const override;
private:
    settings_value_t & get_value(const char* name) override;
    const sKeyValueEntry& get_entry(const char* name) const;
    void add_setting(const char* tag, const char* name, const char* desc, settings_value_t default_value);
    id_lookup_t<sKeyValueEntry> m_Settings;
};

#endif //WM_CGAMESETTINGS_H
