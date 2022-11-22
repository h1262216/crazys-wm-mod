/*
 * Copyright 2019-2022, The Pink Petal Development Team.
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

#ifndef WM_ITRAITSPEC_H
#define WM_ITRAITSPEC_H

#include <string>
#include <array>
#include <vector>
#include <cassert>
#include "IKeyValueStore.h"
#include "fwd.h"

namespace traits {
    class ITraitSpec {
    public:
        /// virtual destructor
        virtual ~ITraitSpec() = default;

        /// Get the (internal) name of this trait
        virtual const std::string& name() const = 0;

        /// Get the name of this trait as it should be displayed.
        virtual const std::string& display_name() const = 0;

        /// Get the description text of this trait.
        virtual const std::string& desc() const = 0;

        /// Get the list of other traits (as their names) that should be excluded by this trait.
        virtual const std::vector<std::string>& exclusions() const = 0;

        /// Returns `true` if this trait excludes the trait with the given `trait_name`.
        virtual bool excludes(const std::string& trait_name) const = 0;

        /// Get trait-specific properties
        virtual IKeyValueStore& get_properties() const = 0;
    };
}


#endif //WM_ITRAITSPEC_H
