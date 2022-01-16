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

#ifndef WM_CIMAGELIST_H
#define WM_CIMAGELIST_H

#include <vector>
#include <string>
#include "ids.h"

struct sImageRecord {
    std::string FileName;
    EOutfitType Outfit;
    bool IsPregnant : 1;
    bool IsFallback : 1;
};

class cImageList {
    using records_vector_t = std::vector<sImageRecord>;
public:
    explicit cImageList(const std::vector<records_vector_t>& source);

    using const_iterator_t = records_vector_t::const_iterator;

    const_iterator_t get_begin(EBaseImage image_type) const;
    const_iterator_t get_end(EBaseImage image_type) const;
    struct sImageRange {
        const cImageList* Target;
        EBaseImage Image;

        const_iterator_t begin() const {
            return Target->get_begin(Image);
        }
        const_iterator_t end() const {
            return Target->get_end(Image);
        }

    };

    sImageRange iterate(EBaseImage img) const { return sImageRange{this, img}; }

private:
    records_vector_t m_FileNames;
    std::vector<int> m_Offsets;
};



#endif //WM_CIMAGELIST_H
