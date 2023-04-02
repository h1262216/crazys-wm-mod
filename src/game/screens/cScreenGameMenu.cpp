/*
 * Copyright 2023, The Pink Petal Development Team.
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
#include "cScreenGameMenu.h"
#include "InterfaceProcesses.h"

cScreenGameMenu::cScreenGameMenu() : cInterfaceWindowXML("in_game_menu.xml")
{
}

void cScreenGameMenu::set_ids()
{
    save_id     = get_id("SaveBtn");
    quit_id     = get_id("QuitBtn");
    continue_id = get_id("ContinueBtn");

    SetButtonCallback(quit_id, [this]() {
        input_confirm([this]() {
            pop_to_window("Main Menu");
        });
    });

    SetButtonCallback(save_id, [this]() {
        SaveGame();
        push_message("Game Saved", COLOR_POSITIVE);
    });
}

void cScreenGameMenu::init(bool back)
{
    Focused();
}
