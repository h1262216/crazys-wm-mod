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

#include "cImageList.h"

auto cImageList::get_begin(EBaseImage image_type) const -> const_iterator_t {
    return m_FileNames.cbegin() + m_Offsets.at((int)image_type);
}

auto cImageList::get_end(EBaseImage image_type) const -> const_iterator_t {
    return m_FileNames.cbegin() + m_Offsets.at((int)image_type + 1);
}

cImageList::cImageList(const std::vector<records_vector_t>& source) {
    m_Offsets.resize(source.size() + 1);
    m_Offsets[0] = 0;
    // first determine the offsets and the size of the allocation we need to make
    auto offset = m_Offsets.begin();
    for(auto& data : source) {
        int old = *offset;
        *(++offset) = old + static_cast<int>(data.size());
    }

    // then do the actual insertion
    m_FileNames.reserve(m_Offsets.back());
    for(auto& data : source) {
        std::copy(std::begin(data), std::end(data), std::back_inserter(m_FileNames));
    }
}
