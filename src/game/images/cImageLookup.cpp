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
#include <tinyxml2.h>
#include "xml/util.h"
#include "utils/string.hpp"

namespace {
    bool parse_preg(const std::string& source) {
        if(iequals(source, "yes")) {
            return true;
        } else if (iequals(source, "no")) {
            return false;
        } else {
            g_LogFile.warning("image", "Expected 'yes' or 'no', got '", source, "'");
            throw std::invalid_argument("Expected 'yes' or 'no'");
        }
    }
}

const cImageList& cImageLookup::lookup_files(const std::string& base_path) {
    auto lookup = m_PathCache.find(base_path);
    if(lookup != m_PathCache.end()) {
        return lookup->second;
    } else {
        // Option 1: The pack as an images.xml file
        DirPath img;
        img << base_path << "images.xml";
        if(img.exists()) {
            auto doc = LoadXMLDocument(img.str());
            for(int i = 0; i < (int)EBaseImage::NUM_TYPES; ++i) {
                m_RecordsBuffer[i].clear();
            }
            for(auto& entry : IterateChildElements(*doc->RootElement(), "Image")) {
                std::string file = GetStringAttribute(entry, "File");
                std::string type = GetStringAttribute(entry, "Type");
                std::string preg = GetStringAttribute(entry, "Pregnant");
                std::string fallback = GetDefaultedStringAttribute(entry, "Fallback", "no");
                try {
                    m_RecordsBuffer[(int) get_image_id(type)].push_back(sImageRecord{file, parse_preg(preg),
                                                                                     parse_preg(fallback)});
                } catch(const std::exception& error) {
                    g_LogFile.error("image", "Error in image specification in ", img.str(), ": ", error.what());
                }
            }
        } else {
            g_LogFile.warning("image", "Image folder '", base_path, "' does not provide images.xml");
            // first get the list of all files
            auto result = FileList(base_path.c_str(), "*.*");
            m_FileNameBuffer.resize(result.size());
            for (int i = 0; i < result.size(); ++i) {
                m_FileNameBuffer[i] = result[i].leaf();
            }

            // then match the files against all patterns to sort
            for (int i = 0; i < (int) EBaseImage::NUM_TYPES; ++i) {
                m_RecordsBuffer[i].clear();
                auto& patterns = m_ImageTypes[i].Patterns;
                for (auto& pattern: patterns) {
                    std::regex base_re(pattern, std::regex::icase | std::regex::ECMAScript | std::regex::optimize);
                    std::regex preg_re("preg" + pattern,
                                       std::regex::icase | std::regex::ECMAScript | std::regex::optimize);
                    for (auto& file: m_FileNameBuffer) {
                        if (std::regex_match(file, base_re)) {
                            m_RecordsBuffer[i].push_back(sImageRecord{file, false});
                        }

                        if (std::regex_match(file, preg_re)) {
                            m_RecordsBuffer[i].push_back(sImageRecord{file, true});
                        }
                    }
                }
            }
        }

        auto inserted = m_PathCache.insert(std::make_pair(base_path, cImageList(m_RecordsBuffer)));
        return inserted.first->second;
    }
}

std::string cImageLookup::find_image_internal(const std::string& base_path, const sImageSpec& spec, int max_cost) {
    std::minstd_rand rng(spec.Seed);
    RandomSelectorV2<std::string> selector;
    auto inner_callback = [&](auto&& file, bool fallback) {
        selector.process(rng, file, fallback ? -1 : 0);
    };
    auto stop = [&](){
        return selector.selection().has_value();
    };

    find_image_internal_imp(base_path, spec, max_cost, inner_callback, stop);

    if(selector.selection()) {
        return selector.selection().value();
    }

    return "";
}

template<class T, class U>
void cImageLookup::find_image_internal_imp(const std::string& base_path, const sImageSpec& spec, int max_cost, T&& inner_callback, U&& stopping_criterion) {
    auto& haystack = lookup_files(base_path);

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

    std::multiset<QueueEntry> queue;
    queue.insert({0, spec});
    g_LogFile.info("image", "Looking for image: ", get_image_name(spec.BasicImage));
    while(!queue.empty()) {
        auto it = queue.begin();
        g_LogFile.debug("image", "  Look up '", get_image_name(it->Value.BasicImage), "' at cost ", it->Cost);
        if(it->Cost > max_cost) {
            break;
        }
        visited.insert(it->Value.BasicImage);

        for(const sImageRecord& image: haystack.iterate(it->Value.BasicImage)) {
            if(image.IsPregnant == it->Value.IsPregnant) {
                inner_callback(image.FileName, image.IsFallback);
            }
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
            if(next->Cost > it->Cost && stopping_criterion()) {
                return;
            }
        }
        queue.erase(it);
    }
}

cImageLookup::cImageLookup(std::string def_img_path, const std::string& spec_file) :
    m_DefaultPath(std::move(def_img_path))
{
    m_ImageTypes.resize((int)EBaseImage::NUM_TYPES);
    m_RecordsBuffer.resize((int)EBaseImage::NUM_TYPES);

    auto doc = LoadXMLDocument(spec_file);
    auto& root = *doc->RootElement();

    for(auto& element : IterateChildElements(root, "Type")) {
        std::string base_name = GetStringAttribute(element, "Name");
        EBaseImage img = get_image_id(base_name);
        sImgTypeInfo& info = m_ImageTypes.at((int)img);
        for(auto& c : IterateChildElements(element)) {
            std::string el_name = c.Value();
            if(el_name == "Display") {
                info.Display = c.GetText();
            } else if (el_name == "Pattern") {
                info.Patterns.push_back(std::string(c.GetText()) + "\\..*");
            } else if (el_name == "Fallback") {
                info.Fallbacks.push_back({get_image_id(c.GetText()),
                                          std::max(0, c.IntAttribute("Cost", 10))});
            }
        }

        if(info.Display.empty())
            info.Display = base_name;

        if(info.Patterns.empty()) {
            info.Patterns.push_back(base_name + ".*\\..*");
        }
    }

    // check that we have every image?
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

std::vector<std::string> cImageLookup::find_images(const std::string& base_path, sImageSpec spec, int cutoff) {
    auto& files = lookup_files(base_path);
    std::vector<std::string> result;
    auto inner_callback = [&](auto&& file, bool fallback) {
        result.push_back(file);
    };
    auto stop = [&](){
        return !result.empty();
    };
    find_image_internal_imp(base_path, spec, cutoff, inner_callback, stop);

    return result;
}

const std::string& cImageLookup::get_display_name(EBaseImage image) {
    return m_ImageTypes.at((int)image).Display;
}