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

#include "cGameWindow.h"
#include <mutex>
#include <future>
#include <sstream>

class cScreenPreparingGame : public cGameWindow
{
private:
    int cancel_id;

    int text1_id;
    int text2_id;
    int text3_id;

    void set_ids() override;

    std::mutex m_Mutex;
    std::vector<std::string> m_NewMessages;
    std::string m_LastError;
    std::future<bool> m_AsyncLoad;

    std::stringstream m_MessagesText;

    bool NewGame(std::string name);
    bool LoadGame(const std::string& file_path);

    bool m_Loading = true;
public:
    cScreenPreparingGame();

    void init(bool back) override;
    void process() override;
    void resetScreen();
    void loadFailed();
};