/*
 * Copyright 2021-2023, The Pink Petal Development Team.
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

#ifndef WM_LOOKUP_H
#define WM_LOOKUP_H

#include <unordered_map>
#include "CLog.h"
#include <cassert>
#include "string.hpp"

struct DefaultPolicy {
    static constexpr const char* error_channel() { return "lookup"; }
    static constexpr const char* default_message() { return "Could not find key"; }
};

template<class Value, class Policy=DefaultPolicy>
class id_lookup_t : public std::unordered_map<std::string, Value, sCaseInsensitiveHash, sCaseInsensitiveEqual> {
public:
    using base_t = std::unordered_map<std::string, Value, sCaseInsensitiveHash, sCaseInsensitiveEqual>;
    using Key = std::string;
    using base_t::base_t;

    Value& at( const Key& key, const char* message = nullptr ) {
        auto iter = this->find(key);
        if(iter != this->end()) {
            return iter->second;
        } else {
            g_LogFile.error(Policy::error_channel(), error_message(message), ": '", key, "'");
            throw std::out_of_range(std::string(error_message(message)) + ": '" + std::string(key) + "'");
        }
    }

    const Value& at( const Key& key, const char* message = nullptr ) const {
        auto iter = this->find(key);
        if(iter != this->end()) {
            return iter->second;
        } else {
            g_LogFile.error(Policy::error_channel(), error_message(message), ": '", key, "'");
            throw std::out_of_range(std::string(error_message(message)) + ": '" + std::string(key) + "'");
        }
    }

private:
    const char* error_message(const char* arg_message) const {
        return arg_message ? arg_message : Policy::default_message();
    }
};


template<class T, std::size_t N>
inline id_lookup_t<T> create_lookup_table(const std::array<const char*, N>& names) {
    id_lookup_t<T> lookup;
    for(std::size_t i = 0; i < N; ++i) {
        auto inserted = lookup.insert(std::make_pair(names[i], static_cast<T>(i)));
        assert(inserted.second);
    }
    return std::move(lookup);
}

template<class Map, class Key>
inline auto&& lookup_with_error(Map&& map, const Key& name, const char* error_msg, const char* error_channel = nullptr) {
    auto found = map.find(name);
    if(found == map.end()) {
        g_LogFile.error(error_channel ? error_channel : "lookup", error_msg, ": '", name, "'");
        throw std::out_of_range(std::string(error_msg) + ": '" + std::string(name) + "'");
    } else {
        return found->second;
    }
}

#endif //WM_LOOKUP_H
