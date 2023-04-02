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

#include "cScreenSettings.h"
#include "utils/FileList.h"
#include "CLog.h"
#include "sConfig.h"
#include "images/ids.h"

extern cConfig cfg;

void cScreenSettings::setup_callbacks()
{
    SetButtonCallback(m_RevertBtn_id, [this]() { init(false); });
    SetButtonCallback(m_OkBtn_id, [this]() {
        update_settings();
        pop_window();
    });

    auto themes = FileList("Resources/Interface", "*.xml");
    for(int i = 0; i < themes.size(); ++i) {
        auto theme = themes[i].leaf().substr(0, themes[i].leaf().length() - 4);
        if(theme == cfg.theme()) {
            AddToListBox(m_ThemeList_id, 1, theme, COLOR_EMPHASIS);
        } else {
            AddToListBox(m_ThemeList_id, 0, theme);
        }
    }
    SetSelectedItemInList(m_ThemeList_id, 1, false, true);
}

cScreenSettings::cScreenSettings() = default;

void cScreenSettings::init(bool back)
{
    Focused();
    if (!back)
    {
        std::stringstream ss;
        SetEditBoxText(m_Characters_id, cfg.characters());
        SetEditBoxText(m_Saves_id, cfg.saves());
        SetEditBoxText(m_DefaultImages_id, cfg.defaultimageloc());
        SetEditBoxText(m_Items_id, cfg.items());
        SetEditBoxText(m_WindowWidth_id, std::to_string(cfg.width()));
        SetEditBoxText(m_WindowHeight_id, std::to_string(cfg.height()));
        SetCheckBox(m_PreferDefault_id, cfg.preferdefault());
        SetCheckBox(m_Fullscreen_id, cfg.fullscreen());
        SetSelectedItemInList(m_ThemeList_id, 1, false, true);

        int style = cfg.image_style();
        SetCheckBox(m_DrawingsCb_id, style & (int)EImageStyle::DRAWING);
        SetCheckBox(m_RenderingsCb_id, style & (int)EImageStyle::RENDERING);
        SetCheckBox(m_PhotosCb_id, style & (int)EImageStyle::PHOTO);
    }
    HideWidget(m_WindowWidth_id, cfg.fullscreen());
    HideWidget(m_WindowHeight_id, cfg.fullscreen());
}

void cScreenSettings::update_settings()
{
    cfg.set_value("folders.characters", GetEditBoxText(m_Characters_id));
    cfg.set_value("folders.saves", GetEditBoxText(m_Saves_id));
    cfg.set_value("folders.default_images", GetEditBoxText(m_DefaultImages_id));
    cfg.set_value("folders.items", GetEditBoxText(m_Items_id));

    cfg.set_value("folders.prefer_defaults", IsCheckboxOn(m_PreferDefault_id));

    cfg.set_value("interface.theme", GetSelectedTextFromList(m_ThemeList_id));
    cfg.set_value("interface.fullscreen", IsCheckboxOn(m_Fullscreen_id));

    cfg.set_value("interface.height", std::stoi(GetEditBoxText(m_WindowHeight_id)));
    cfg.set_value("interface.width", std::stoi(GetEditBoxText(m_WindowWidth_id)));

    EImageStyle style = EImageStyle(0);
    if(IsCheckboxOn(m_DrawingsCb_id))
       style = combine(style, EImageStyle::DRAWING);
    if(IsCheckboxOn(m_RenderingsCb_id))
       style = combine(style, EImageStyle::RENDERING);
    if(IsCheckboxOn(m_PhotosCb_id))
       style = combine(style, EImageStyle::PHOTO);
    cfg.set_value("interface.width", std::stoi(GetEditBoxText(m_WindowWidth_id)));
    cfg.set_value("interface.image-styles", int(style));

    cfg.save();
}

std::shared_ptr<cInterfaceWindow> screens::cSettingsBase::create() {
    return std::make_shared<cScreenSettings>();
}

