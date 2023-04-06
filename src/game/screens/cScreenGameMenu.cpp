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

cScreenGameMenu::cScreenGameMenu() = default;

void cScreenGameMenu::setup_callbacks()
{
    SetButtonCallback(m_QuitBtn_id, [this]() {
        input_confirm([this]() {
            pop_to_window("Main Menu");
        });
    });

    SetButtonCallback(m_SaveBtn_id, [this]() {
        SaveGame();
        push_message("Game Saved", COLOR_POSITIVE);
    });
}

void cScreenGameMenu::init(bool back)
{
    Focused();
}

std::shared_ptr<cInterfaceWindow> screens::cInGameMenuBase::create() {
    return std::make_shared<cScreenGameMenu>();
}
