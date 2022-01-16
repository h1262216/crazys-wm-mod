/*
 * Copyright 2009, 2010, The Pink Petal Development Team.
 * The Pink Petal Devloment Team are defined as the game's coders
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

#include "ids.h"
#include "utils/lookup.h"

const std::array<const char*, (int)EBaseImage::NUM_TYPES>& get_imgtype_names() {
    static std::array<const char*, (int)EBaseImage::NUM_TYPES> names {
            "anal", "bdsm", "sex", "beast", "group", "ffm", "mmf", "orgy", "lesbian", "oral", "mast",
            "titty", "hand", "foot", "deepthroat", "eatout", "dildo", "strapon",
            "les69", "scissor", "lick", "finger", "fisting", "suckballs", "cowgirl", "revcowgirl",
            "sexdoggy", "rape", "rapebeast",
            "strip", "ecchi", "nude", "bunny", "escort", "massage", "milk", "dom", "sub",
            "maid", "sing", "wait", "card", "nurse", "piano", "music", "stagehand",
            "crystalpurifier", "director", "cameramage", "studiocrew", "security", "cook", "bake",
            "farm", "herd", "advertise",
            "sport", "study", "teacher", "combat", "magic", "bed", "rest", "bath", "swim",
            "shop", "craft", "dance",
            "birthhuman", "birthhumanmultiple", "birthbeast",
            "refuse", "death", "profile", "portrait", "jail", "torture", "formal", "presented",
            "brand", "puppygirl", "ponygirl", "catgirl"
    };
    return names;
}

const id_lookup_t<EBaseImage>& get_image_lookup() {
    static auto lookup = create_lookup_table<EBaseImage>(get_imgtype_names());
    return lookup;
}

EBaseImage get_image_id(const std::string& name) {
    return lookup_with_error(get_image_lookup(), name, "Trying to get invalid Image Type");
}

const char* get_image_name(EBaseImage image) {
    return get_imgtype_names().at((int)image);
}

const std::array<const char*, (int)EOutfitType::NUM_OUTFITS>& get_outfit_names() {
    static std::array<const char*, (int)EOutfitType::NUM_OUTFITS> names {
            "armour", "fetish", "maid", "teacher", "nurse", "formal", "schoolgirl",
            "swimwear", "sportswear", "lingerie", "farmer", "sorceress", "rags",
            "casual", "nude", "none"
    };
    return names;
}

const id_lookup_t<EOutfitType>& get_outfit_lookup() {
    static auto lookup = create_lookup_table<EOutfitType>(get_outfit_names());
    return lookup;
}

EOutfitType get_outfit_id(const std::string& name) {
    return lookup_with_error(get_outfit_lookup(), name, "Trying to get invalid Image Type");
}

const char* get_outfit_name(EOutfitType outfit) {
    return get_outfit_names().at((int)outfit);
}
