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
#ifndef __CCHECKBOX_H
#define __CCHECKBOX_H

#include <memory>
#include <functional>
#include "interface/cInterfaceObject.h"
#include "interface/cFont.h"
#include "interface/cSurface.h"

class cCheckBox : public cUIWidget
{
public:
	cCheckBox(cInterfaceWindow* parent, int id, int x, int y, int width, int height, std::string text, int fontsize = 12, bool leftorright = false);

    bool HandleClick(int x, int y, bool press) override;

    bool GetState() const {return m_StateOn;}
    void SetState(bool on) { m_StateOn = on;}

	void DrawWidget(const CGraphics& gfx) override;

    void SetCallback(std::function<void(bool)>);

private:
    cSurface m_Image;
    cSurface m_Surface;
    cSurface m_Border;
	bool m_StateOn = false;
	cFont m_Font;
    std::function<void(bool)> m_Callback;
};

#endif
