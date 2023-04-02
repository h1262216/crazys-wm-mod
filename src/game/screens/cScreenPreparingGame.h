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

#pragma once

#include "ScreenBase.h"
#include <mutex>
#include <future>
#include <sstream>

class cScreenPreparingGame : public screens::cPreparingGameScreenBase
{
public:
    cScreenPreparingGame();

    void init(bool back) override;
    void process() override;
    void resetScreen();
    void loadFailed();

private:
    void setup_callbacks() override;

    std::mutex m_Mutex;
    std::vector<std::string> m_NewMessages;
    std::string m_LastError;
    std::future<bool> m_AsyncLoad;

    std::stringstream m_MessagesText;

    bool NewGame(std::string name);
    bool LoadGame(const std::string& file_path);

    bool m_Loading = true;
    bool m_IsNewGame = false;
};