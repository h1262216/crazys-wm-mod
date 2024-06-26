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
#ifndef __CTEXTITEM_H
#define __CTEXTITEM_H

#include "interface/cInterfaceObject.h"

class cScrollBar;

class cTextItem : public cUIWidget
{
    bool m_ForceScrollBar;    // force scrollbar display even if text fits?

public:
    cTextItem(cInterfaceWindow* parent, int ID, int x, int y, int width, int height, std::string text, int size,
              bool force_scrollbar = false, int red = -1, int green = -1, int blue = -1, bool as_table=false);

    int HeightTotal() const;
    bool HandleMouseWheel(bool down) override;

    bool IsOver(int x, int y) const override;

    void SetText(std::string text, bool as_table);
    void DrawWidget(const CGraphics& gfx) override;
    void SetHidden(bool mode) override;

    bool m_IsTable = false;
    std::string m_Text;
    std::unique_ptr<cFont> m_Font;
    cSurface m_PreRenderedText;
    int m_FontHeight;        // height of the font

    cScrollBar* m_ScrollBar;  // pointer to the associated scrollbar, if any
    int m_ScrollChange;  // scrollbar changes will update this value; translates to skipped lines of text
};

#endif
