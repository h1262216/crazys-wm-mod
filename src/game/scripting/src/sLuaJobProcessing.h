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

#ifndef WM_SLUAJOBPROCESSING_H
#define WM_SLUAJOBPROCESSING_H

struct lua_State;
class sGirlShiftData;

namespace scripting {
    struct sLuaShiftData {
        static void init(lua_State* L);
        static void create(lua_State* state, sGirlShiftData* girl);
        static sGirlShiftData& check_type(lua_State* L, int index);

        static int is_night_shift(lua_State* state);

        // text functions
        static int has_text(lua_State* state);
        static int get_text(lua_State* state);
    };
}

#endif //WM_SLUAJOBPROCESSING_H
