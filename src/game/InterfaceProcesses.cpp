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
#pragma region //    Includes and Externs            //
#include "CLog.h"
#include "InterfaceProcesses.h"
#include "Revision.h"
#include "utils/FileList.h"
#include "utils/DirPath.h"
#include "buildings/cBuildingManager.h"
#include "cObjectiveManager.hpp"
#include "IGame.h"
#include "cGangs.h"
#include "cInventory.h"
#include "cNameList.h"
#include "sConfig.h"
#include "SavesList.h"

extern cRng g_Dice;
extern cConfig cfg;
extern cNameList g_GirlNameList;
extern cNameList g_BoysNameList;
extern cNameList g_SurnameList;

#pragma endregion

// globals used for the interface
std::string g_ReturnText;
int g_ReturnInt = -1;
bool g_AllTogle = false;    // used on screens when wishing to apply something to all items

void LoadNames()
{// `J` load names lists
    DirPath location_N = DirPath() << "Resources" << "Data" << "RandomGirlNames.txt";
    g_GirlNameList.load(location_N);
    DirPath location_SN = DirPath() << "Resources" << "Data" << "RandomLastNames.txt";
    g_SurnameList.load(location_SN);
    // `J` Added g_BoysNameList for .06.03.00
    DirPath location_B = DirPath() << "Resources" << "Data" << "RandomBoysNames.txt";
    g_BoysNameList.load(location_B);
}

void AutoSaveGame()
{
    SaveGameXML(DirPath(DirPath::expand_path(cfg.saves()).c_str()) << "autosave.gam");
}

void SaveGame()
{
    std::string filename = g_Game->buildings().get_building(0).name();
    std::string filenamedotgam = filename + ".gam";

    auto save_dir = DirPath::expand_path(cfg.saves());
    SavesList list;
    list.BuildSaveGameList(save_dir);

    SaveGameXML(DirPath(save_dir.c_str()) << filenamedotgam);
    list.NotifySaveGame(filenamedotgam, *g_Game);
    list.SaveXML(DirPath(save_dir.c_str()) << ".saves.xml");
}

void SaveGameXML(std::string filename)
{
    g_LogFile.info("save", "Saving game to '", filename, '\'');
    tinyxml2::XMLDocument doc;
    doc.LinkEndChild(doc.NewDeclaration());
    auto* pRoot = doc.NewElement("Root");
    doc.LinkEndChild(pRoot);

    // output game version
    pRoot->SetAttribute("MajorVersion", g_MajorVersion);
    pRoot->SetAttribute("MinorVersion", g_MinorVersion);
    pRoot->SetAttribute("PatchVersion", g_PatchVersion);
    pRoot->SetAttribute("ApproxRevision", VERSION_STRING);

    //if a user mods the exe, he can tell us that here
    //by changing it to anything besides official
    pRoot->SetAttribute("ExeVersion", "official");

    g_Game->SaveGame(*pRoot);
    doc.SaveFile(filename.c_str());
}
