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

#include "cImageLookup.h"
#include "utils/FileList.h"
#include "utils/streaming_random_selection.hpp"
#include "xml/util.h"
#include "xml/getattr.h"
#include "CLog.h"
#include <tinyxml2.h>
#include <cassert>
#include <set>
#include <regex>
#include <utility>

const std::vector<std::string>& cImageLookup::lookup_files(const std::string& base_path) {
    auto lookup = m_PathCache.find(base_path);
    if(lookup != m_PathCache.end()) {
        return lookup->second;
    } else {
        auto result = FileList(base_path.c_str(), "*.*");
        auto& target = m_PathCache[base_path];
        for(int i = 0; i < result.size(); ++i) {
            target.push_back(result[i].leaf());
        }
        return target;
    }
}

namespace {
    std::string make_pattern(const std::string& base, bool is_pregnant) {
        std::string pattern;
        if(is_pregnant) {
            return "preg" + base;
        } else {
            return base;
        }
    }
}

std::string cImageLookup::find_image_internal(const std::string& base_path, const sImageSpec& spec, int max_cost) {
    auto& files = lookup_files(base_path);

    std::minstd_rand rng(spec.Seed);

    // OK, need to go to the fallbacks. This is a BFS
    std::set<EBaseImage> visited;
    visited.insert(spec.BasicImage);

    struct QueueEntry {
        int Cost;
        sImageSpec Value;
        bool operator<(const QueueEntry& other) const {
            return Cost < other.Cost;
        }
    };

    RandomSelectorV2<std::string> selector;
    std::multiset<QueueEntry> queue;
    queue.insert({0, spec});
    g_LogFile.info("image", "Looking for image: ", get_image_name(spec.BasicImage));
    while(!queue.empty()) {
        auto it = queue.begin();
        g_LogFile.debug("image", "Look up '", get_image_name(it->Value.BasicImage), "' at cost ", it->Cost);
        if(it->Cost > max_cost) {
            break;
        }
        visited.insert(it->Value.BasicImage);
        auto found = find_image_direct(files,  it->Value, rng);
        if(found) {
            g_LogFile.debug("image", "Found a '", get_image_name(it->Value.BasicImage), "' image");
            selector.process(rng, found.value());
        }

        // insert does not invalidate iterators for set, so it is still fine
        for(auto& fbs : m_ImageTypes.at((int)it->Value.BasicImage).Fallbacks) {
            if(visited.count(fbs.NewImageType) > 0) continue;
            queue.insert({it->Cost + fbs.Cost,
                          sImageSpec{fbs.NewImageType, spec.IsPregnant, spec.IsVirgin, 0}});
        }
        if(it->Value.IsPregnant) {
            queue.insert({it->Cost + 5, sImageSpec{it->Value.BasicImage, false, it->Value.IsVirgin, 0}});
        }

        auto next = it;
        ++next;
        if(next != queue.end()) {
            // if the next trial would be less desired, and we already have found one, stop the search
            if(next->Cost > it->Cost && selector.selection()) {
                return selector.selection().value();
            }
        }
        queue.erase(it);
    }

    if(selector.selection()) {
        return selector.selection().value();
    }

    return "";
}


cImageLookup::cImageLookup(std::string def_img_path, const std::string& spec_file) :
    m_DefaultPath(std::move(def_img_path))
{
    m_ImageTypes.resize((int)EBaseImage::NUM_TYPES);

    auto doc = LoadXMLDocument(spec_file);
    auto& root = *doc->RootElement();

    for(auto& element : IterateChildElements(root, "Type")) {
        std::string base_name = GetStringAttribute(element, "Name");
        EBaseImage img = get_image_id(base_name);
        boost::optional<std::string> display_name;
        sImgTypeInfo& info = m_ImageTypes.at((int)img);
        for(auto& c : IterateChildElements(element)) {
            std::string el_name = c.Value();
            if(el_name == "Display") {
                display_name = c.GetText();
            } else if (el_name == "Pattern") {
                info.Patterns.push_back(std::string(c.GetText()) + "\\..*");
            } else if (el_name == "Fallback") {
                info.Fallbacks.push_back({get_image_id(c.GetText()),
                                          std::max(1, c.IntAttribute("Cost", 10))});

            }
        }

        if(info.Patterns.empty()) {
            info.Patterns.push_back(base_name + ".*\\..*");
        }
    }

    // check that we have every image?
}

boost::optional<std::string>
cImageLookup::find_image_direct(const std::vector<std::string>& haystack, const sImageSpec& spec, std::minstd_rand& rng) const {
    RandomSelectorV2<std::string> selector;
    for(auto& base_pattern: m_ImageTypes.at((int)spec.BasicImage).Patterns) {
        std::string pattern = make_pattern(base_pattern, spec.IsPregnant);
        std::regex re(pattern, std::regex::icase | std::regex::ECMAScript | std::regex::optimize);
        for (auto& f: haystack) {
            if (std::regex_match(f, re)) {
                selector.process(rng, f);
            }
        }
    }
    return selector.selection();
}

std::string cImageLookup::find_image(const std::string& base_path, const sImageSpec& spec) {
    auto result = find_image_internal(base_path, spec, m_CostCutoff);
    // TODO clean up this code!
    if(result.empty()) {
        // still nothing? retry with default images. Here we go to the end with fallbacks.
        result = find_image_internal(m_DefaultPath, spec, std::numeric_limits<int>::max());
        if(result.empty()) {
            return result;
        } else {
            DirPath path(m_DefaultPath.c_str());
            path << result;
            return path.str();
        }
    } else {
        DirPath path(base_path.c_str());
        path << result;
        return path.str();
    }
}
