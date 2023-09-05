/*
 * Copyright 2009-2023, The Pink Petal Development Team.
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

#include "include/widgets/cButton.h"
#include "interface/cSurface.h"
#include "interface/CGraphics.h"

cButton::~cButton() = default;

void cButton::DrawWidget(const CGraphics& gfx)
{
    if (m_CurrImage && *m_CurrImage)
    {
        m_CurrImage->DrawSurface(m_XPos, m_YPos);
    }
}

cButton::cButton(cInterfaceWindow* parent, const std::string& OffImage, const std::string& DisabledImage,
        const std::string& OnImage, int ID, int x, int y, int width, int height):
    cUIWidget(ID, x, y, width, height, parent)
{
    SetImages(OffImage, DisabledImage, OnImage);
    m_CurrImage = &m_OffImage;
}

void cButton::SetImages(const std::string& base_name) {
    std::string on = base_name + "On.png";
    std::string off = base_name + "Off.png";
    std::string disabled = base_name + "Disabled.png";
    SetImages(off, disabled, on);
}


void cButton::SetImages(const std::string& OffImage, const std::string& DisabledImage, const std::string& OnImage) {
    m_OffImage = LoadUIImage("Buttons", OffImage, m_Width, m_Height);

    if (!DisabledImage.empty()) {
        m_DisabledImage = LoadUIImage("Buttons", DisabledImage, m_Width, m_Height);
    }

    if (!OnImage.empty()) {
        m_OnImage = LoadUIImage("Buttons", OnImage, m_Width, m_Height);
    } else {
        m_OnImage = m_OffImage;
    }
}

void cButton::SetDisabled(bool disable)
{
    cUIWidget::SetDisabled(disable);
    m_CurrImage = disable ? &m_DisabledImage : &m_OffImage;
}

void cButton::SetCallback(std::function<void()> cb)
{
    m_Callback = std::move(cb);
}

bool cButton::HandleKeyPress(SDL_Keysym key)
{
    if(key.sym != m_HotKey) return false;
    if(m_Callback)
        m_Callback();
    return true;
}

void cButton::SetHotKey(SDL_Keycode key)
{
    m_HotKey = key;
}

bool cButton::HandleClick(int x, int y, bool press)
{
    if(press)        return false;
    if(m_Callback)   m_Callback();
    return true;
}

void cButton::HandleMouseMove(bool over, int x, int y)
{
    m_CurrImage = over ? &m_OnImage : &m_OffImage;
}

