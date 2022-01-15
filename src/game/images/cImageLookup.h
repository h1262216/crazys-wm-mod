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

#ifndef WM_CIMAGELOOKUP_H
#define WM_CIMAGELOOKUP_H

#include "sImageSpec.h"

#include <unordered_map>
#include <vector>
#include <boost/optional.hpp>
#include <random>
#include "cImageList.h"

class cImageLookup {
public:
    cImageLookup(std::string  def_img_path, const std::string& spec_file);
    std::string find_image(const std::string& base_path, const sImageSpec& spec);
    std::vector<std::string> find_images(const std::string& base_path, sImageSpec spec, int cutoff=0);

    struct sFallbackData {
        EBaseImage NewImageType;
        int Cost = 1;
    };
    struct sImgTypeInfo {
        std::vector<std::string> Patterns;
        std::vector<sFallbackData> Fallbacks;
    };

private:
    const cImageList& lookup_files(const std::string& base_path);
    std::string find_image_internal(const std::string& base_path, const sImageSpec& spec, int max_cost);

    template<class T, class U>
    void find_image_internal_imp(const std::string& base_path, const sImageSpec& spec, int max_cost, T&& callback, U&& stopping);
    template<class F>
    void iterate_candidates(const std::vector<sImageRecord>& haystack, const sImageSpec& spec, F&& callback);

    std::unordered_map<std::string, cImageList> m_PathCache;
    std::vector<sImgTypeInfo> m_ImageTypes;
    std::string m_DefaultPath;

    int m_CostCutoff = 25;   // if the lookup cost exceeds this value, default images will be used

    // cache variables, so we don't need to repeatedly allocate memory
    std::vector<std::string> m_FileNameBuffer;
    std::vector<std::vector<sImageRecord>> m_RecordsBuffer;
};

#endif //WM_CIMAGELOOKUP_H
