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

#ifndef WM_ITRAITSMANAGER_H
#define WM_ITRAITSMANAGER_H

#include <memory>
#include <vector>

#include "fwd.h"

namespace tinyxml2 {
    class XMLElement;
}

namespace traits {
    class ITraitsCollection;

    class ITraitSpec;

    // Manages and loads the traits files
    class ITraitsManager {
    public:
        virtual ~ITraitsManager() = default;

        virtual void load_xml(const tinyxml2::XMLElement& root) = 0;

        virtual void load_modifiers(const tinyxml2::XMLElement& root, std::string prefix) = 0;

        virtual bool load_modifier(const tinyxml2::XMLElement& root, std::string name) = 0;

        virtual std::unique_ptr<ITraitsCollection> create_collection() const = 0;

        virtual const ITraitSpec* lookup(const char* name) const = 0;

        virtual std::vector<std::string> get_all_traits() const = 0;

        virtual void iterate(std::function<void(const ITraitSpec&)> callback) const = 0;

        // Trait groups
        /// Gets the level within this group of the the (sole) active trait of this group in the given
        /// collection.
        virtual int get_group_level(const char* group, const ITraitsCollection&) const   = 0;

        /// Gets the trait at the given level in the group.
        virtual const ITraitSpec* get_group_at_level(const char* group, int level) const = 0;

        static std::unique_ptr<ITraitsManager> createTraitsManager();
    };
}

#endif //WM_ITRAITSMANAGER_H
