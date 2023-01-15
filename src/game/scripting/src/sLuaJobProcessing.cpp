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

#include "sLuaJobProcessing.h"
#include "lua.hpp"
#include "cLuaState.h"
#include "jobs/sGirlShiftData.h"
#include "IGame.h"
#include "jobs/IGenericJob.h"
#include "jobs/IJobManager.h"

using namespace scripting;

namespace {
    template <int sGirlShiftData::*Ptr>
    int GetterSetter(lua_State *L) {
        auto& shift = sLuaShiftData::check_type(L, 1);
        if(lua_gettop(L) == 1) {
            int target = lua_tointeger(L, 1);
            shift.*Ptr = target;
            return 0;
        } else {
            lua_pushinteger(L, shift.*Ptr);
            return 1;
        }
    }
}

void sLuaShiftData::init(lua_State* L) {
    luaL_newmetatable(L, "wm.ShiftData");

    cLuaState state{L};
    // metatable.__index = metatable
    lua_pushstring(L, "__index");
    lua_pushvalue(L, -2);
    lua_settable(L, -3);

    // fill in the metatable
    luaL_Reg methods[] = {
            {"tips", GetterSetter<&sGirlShiftData::Tips>},
            {"earnings", GetterSetter<&sGirlShiftData::Earnings>},
            {"wages", GetterSetter<&sGirlShiftData::Wages>},
            {"performance", GetterSetter<&sGirlShiftData::Performance>},
            {"night_shift", is_night_shift},
            {"has_text", has_text},
            {"get_text", get_text},

            // {"__gc", sLuaGirl::finalize},*/
            {nullptr, nullptr}
    };
    luaL_setfuncs(L, methods, 0);

    // clean up the stack -- remove the new meta-table
    lua_pop(L, 1);
}

void sLuaShiftData::create(lua_State* L, sGirlShiftData* shift) {
    size_t nbytes = sizeof(sGirlShiftData*);
    auto a = (sGirlShiftData**)lua_newuserdata(L, nbytes);
    *a = shift;

    luaL_getmetatable(L, "wm.ShiftData");
    lua_setmetatable(L, -2);
}


int sLuaShiftData::is_night_shift(lua_State* state) {
    auto& shift = sLuaShiftData::check_type(state, 1);
    lua_pushboolean(state, shift.IsNightShift);
    return 1;
}

sGirlShiftData& sLuaShiftData::check_type(lua_State* L, int index) {
    void *ud = luaL_checkudata(L, index, "wm.ShiftData");
    luaL_argcheck(L, ud != nullptr, index, "`ShiftData' expected");
    auto& gp = *(sGirlShiftData**)ud;
    luaL_argcheck(L, gp != nullptr, index, "shift data pointer is null");
    return *gp;
}

int sLuaShiftData::has_text(lua_State* state) {
    sGirlShiftData& data = check_type(state, 1);
    const char* query = lua_tostring(state, 2);
    auto job = g_Game->job_manager().get_job(data.Job);
    lua_pushboolean(state, job->has_text(query));
    return 1;
}

int sLuaShiftData::get_text(lua_State* state) {
    sGirlShiftData& data = check_type(state, 1);
    const char* query = lua_tostring(state, 2);
    auto job = g_Game->job_manager().get_job(data.Job);
    lua_pushstring(state, job->get_text(query).c_str());
    return 1;
}
