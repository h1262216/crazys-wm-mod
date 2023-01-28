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
#include "cScreenSettings.h"
#include "utils/FileList.h"
#include "CLog.h"
#include "sConfig.h"
#include "images/ids.h"

extern cConfig cfg;

void cScreenSettings::set_ids()
{
    ok_id              = get_id("OkButton");
    revert_id          = get_id("RevertButton");

    // Folders
    characters_id      = get_id("Characters");
    saves_id           = get_id("Saves");
    defaultimages_id   = get_id("DefaultImages");
    items_id           = get_id("Items");
    preferdefault_id   = get_id("PreferDefault");

    theme_id           = get_id("ThemeList");
    fullscreen_id      = get_id("Fullscreen");
    width_id           = get_id("WindowWidth");
    height_id          = get_id("WindowHeight");

    drawings_id        = get_id("DrawingsCB");
    renderings_id      = get_id("RenderingsCB");
    photos_id          = get_id("PhotosCB");

    SetButtonCallback(revert_id, [this]() { init(false); });
    SetButtonCallback(ok_id, [this]() {
        update_settings();
        pop_window();
    });

    auto themes = FileList("Resources/Interface", "*.xml");
    for(int i = 0; i < themes.size(); ++i) {
        auto theme = themes[i].leaf().substr(0, themes[i].leaf().length() - 4);
        if(theme == cfg.theme()) {
            AddToListBox(theme_id, 1, theme, COLOR_EMPHASIS);
        } else {
            AddToListBox(theme_id, 0, theme);
        }
    }
    SetSelectedItemInList(theme_id, 1, false, true);
}

cScreenSettings::cScreenSettings() : cInterfaceWindowXML("settings.xml")
{
}

void cScreenSettings::init(bool back)
{
    Focused();
    if (!back)
    {
        std::stringstream ss;
        SetEditBoxText(characters_id, cfg.characters());
        SetEditBoxText(saves_id, cfg.saves());
        SetEditBoxText(defaultimages_id, cfg.defaultimageloc());
        SetEditBoxText(items_id, cfg.items());
        SetEditBoxText(width_id, std::to_string(cfg.width()));
        SetEditBoxText(height_id, std::to_string(cfg.height()));
        SetCheckBox(preferdefault_id, cfg.preferdefault());
        SetCheckBox(fullscreen_id, cfg.fullscreen());
        SetSelectedItemInList(theme_id, 1, false, true);

        int style = cfg.image_style();
        SetCheckBox(drawings_id, style & (int)EImageStyle::DRAWING);
        SetCheckBox(renderings_id, style & (int)EImageStyle::RENDERING);
        SetCheckBox(photos_id, style & (int)EImageStyle::PHOTO);
    }
    HideWidget(width_id, cfg.fullscreen());
    HideWidget(height_id, cfg.fullscreen());
}

void cScreenSettings::update_settings()
{
    cfg.set_value("folders.characters", GetEditBoxText(characters_id));
    cfg.set_value("folders.saves", GetEditBoxText(saves_id));
    cfg.set_value("folders.default_images", GetEditBoxText(defaultimages_id));
    cfg.set_value("folders.items", GetEditBoxText(items_id));

    cfg.set_value("folders.prefer_defaults", IsCheckboxOn(preferdefault_id));

    cfg.set_value("interface.theme", GetSelectedTextFromList(theme_id));
    cfg.set_value("interface.fullscreen", IsCheckboxOn(fullscreen_id));

    cfg.set_value("interface.height", std::stoi(GetEditBoxText(height_id)));
    cfg.set_value("interface.width", std::stoi(GetEditBoxText(width_id)));

    EImageStyle style = EImageStyle(0);
    if(IsCheckboxOn(drawings_id))
       style = combine(style, EImageStyle::DRAWING);
    if(IsCheckboxOn(renderings_id))
       style = combine(style, EImageStyle::RENDERING);
    if(IsCheckboxOn(photos_id))
       style = combine(style, EImageStyle::PHOTO);
    cfg.set_value("interface.width", std::stoi(GetEditBoxText(width_id)));
    cfg.set_value("interface.image-styles", int(style));

    cfg.save();
}
