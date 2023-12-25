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

#ifndef WM_CGIRLSHIFT_H
#define WM_CGIRLSHIFT_H

#include "sGirlShiftData.h"

class cGenericJob;

class cGirlShift {
public:
    cGirlShift(sGirlShiftData* data, const cGenericJob* job);

    sGirlShiftData& data();
    [[nodiscard]] const sGirlShiftData& data() const;
    [[nodiscard]] sGirl& girl();
    [[nodiscard]] cBuilding& building();
    [[nodiscard]] bool is_night_shift() const;

    // rng utilities
    cRng& rng();
    int d100();
    bool chance(float percent);
    bool chance_capped(int percent);
    int uniform(int min, int max);

    // variables
    void set_variable(int id, int value);
    [[nodiscard]] sGirlShiftData::JobStateVar_t get_variable(int index) const;

    [[nodiscard]] int performance() const;
    [[nodiscard]] int enjoyment() const;

    // resources
    int consume_resource(const std::string& name, int amount);
    void provide_resource(const std::string& name, int amount);
    bool try_consume_resource(const std::string& name, int amount);

    // one-on-one interactions
    void provide_interaction(const std::string& name, int amount);
    sGirl* request_interaction(const std::string& name);
    [[nodiscard]] bool has_interaction(const std::string& name) const;

    // message generation
    using LocalSubstitutions = std::initializer_list<std::pair<std::string, std::string>>;
    void add_literal(const std::string& text, LocalSubstitutions subs={});
    void add_text(const std::string& prompt, LocalSubstitutions subs={});
    void add_line(const std::string& prompt, LocalSubstitutions subs={});

    void set_image(sImagePreset preset);
    void generate_event();

    // other
    void set_result(ECheckWorkResult result);
private:
    sGirlShiftData* m_Data;
    const cGenericJob* m_JobClass;
};

#endif //WM_CGIRLSHIFT_H
