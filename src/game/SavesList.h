/*
 * Copyright 2021-2023, The Pink Petal Development Team.
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

#ifndef WM_SAVESLIST_H
#define WM_SAVESLIST_H

#include <string>
#include <functional>
#include <vector>
#include "utils/lookup.h"

class IGame;
class DirPath;

struct sSaveData {
    int WeeksPlayed;
    int Money;
    int LastSaveTime;
};


class SavesList {
public:
    void NotifySaveGame(const std::string& file_name, IGame& game);
    void BuildSaveGameList(const std::string& directory);

    void SaveXML(const std::string& target_file);
    void LoadXML(const std::string& source_file);

    static bool LoadGame(const std::string& source_file, const std::function<void(std::string)>& callback, IGame& target);

    struct sSaveEntry {
        std::string File;
        sSaveData Data;
    };
    std::vector<sSaveEntry> get_saves(const DirPath& saves_dir) const;
private:
    id_lookup_t<sSaveData> m_SaveData;
};

#endif //WM_SAVESLIST_H
