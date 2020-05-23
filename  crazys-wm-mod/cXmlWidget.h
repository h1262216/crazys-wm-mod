/*
 * Copyright 2009, 2010, The Pink Petal Development Team.
 * The Pink Petal Devloment Team are defined as the game's coders 
 * who meet on http://pinkpetal.org     // old site: http://pinkpetal .co.cc
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
#pragma once

#include <vector>
#include <string>

struct sXmlWidgetBase {
    std::string name;
    int x = -1;
    int y = -1;
    int w = -1;
    int h = -1;
    bool hide = false;
};

struct sXmlWidgetPart : public sXmlWidgetBase {
public:
	int         r, g, b;
	int         fontsize, bordersize;
	bool        stat, alpha, scale, events, multi, cache, leftorright;
	std::string on, off, disabled_img, type, text, file, seq;
	bool        force_scroll = false;
};

class cXmlWidget {
	std::vector<sXmlWidgetPart> list;
public:
	cXmlWidget() = default;
	int size() {	return int(list.size()); }
	sXmlWidgetPart& operator[](int i) {
		return list[i];
	}
	void add(sXmlWidgetPart &part)
	{
		list.push_back(part);
	}
};

