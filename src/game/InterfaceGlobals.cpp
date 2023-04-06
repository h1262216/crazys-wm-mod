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

#include <chrono>
#include "interface/sColor.h"
#include "interface/cWindowManager.h"
#include "screens/cScreenItemManagement.h"
#include "screens/cScreenGirlDetails.h"
#include "screens/cScreenMainMenu.h"
#include "screens/cScreenLoadGame.hpp"
#include "screens/cScreenBrothelManagement.h"
#include "widgets/cScreenGetInput.h"
#include "screens/cScreenGallery.h"
#include "screens/cScreenTurnSummary.h"
#include "screens/cScreenGameConfig.h"
#include <tinyxml2.h>
#include "CLog.h"
#include "xml/util.h"
#include "sConfig.h"
#include "interface/cTheme.h"
#include "screens/ScreenBase.h"

cScreenGetInput*          g_GetInput          = nullptr;

template<class T, class... Args>
T* load_window(const char* name, bool nonav /* = false*/, Args&&... args);

template<class T>
T* load_window(const char* name)
{
  return load_window<T>(name, false);
}

template<class T>
void load_window_easy(bool nonav)
{
    g_LogFile.log(ELogLevel::DEBUG, "Loading Window '", typeid(T).name(), '\'');
    auto window = T::create();
    if (!nonav) {
        register_global_nav_keys(*window);
    }
    window_manager().add_window(std::move(window));
}

void LoadInterface(const cConfig& cfg)
{
    using namespace screens;
    std::stringstream ss;
    std::string image; std::string text; std::string file;
    std::ifstream incol;

    g_LogFile.debug("interface", "Loading screens");
    auto start_time = std::chrono::steady_clock::now();

    // `J` Bookmark - Loading the screens
    load_window_easy<screens::cPreparingGameScreenBase>(true);
    load_window<cScreenMainMenu>("Main Menu", true, cfg.saves());
    load_window_easy<screens::cNewGameBase>(true);
    load_window<cScreenLoadGame>("Load Game", true, cfg.saves());
    load_window_easy<screens::cSettingsBase>(true);

    load_window<cScreenBrothelManagement>("Brothel Management");
    load_window_easy<cScreenGirlDetails>(false);
    load_window_easy<screens::cGangsScreenBase>(false);
    load_window<cScreenItemManagement>("Item Management");
    load_window<cMovieScreen>("Movie Screen");
    load_window_easy<cTransferGirlsBase>(false);
    load_window<cScreenTurnSummary>("Turn Summary");
    load_window<cScreenGallery>("Gallery");
    g_GetInput = load_window<cScreenGetInput>("GetInput");

    load_window<CBuildingManagementScreenDispatch>("Girl Management");
    load_window<CBuildingScreenDispatch>("Building Management");
    load_window<cScreenGirlManagement>("Girl Management Brothel");
    load_window<cScreenClinicManagement>("Girl Management Clinic");
    load_window<cScreenStudioManagement>("Girl Management Studio");
    load_window<cScreenArenaManagement>("Girl Management Arena");
    load_window<cScreenCentreManagement>("Girl Management Centre");
    load_window<cScreenHouseManagement>("Girl Management House");
    load_window<cScreenFarmManagement>("Girl Management Farm");
    load_window<cScreenClinic>("Clinic Screen");
    load_window<cScreenCentre>("Centre Screen");
    load_window<cScreenArena>("Arena Screen");
    load_window<cScreenHouse>("Player House");
    load_window<cScreenFarm>("Farm Screen");
    load_window_easy<screens::cDungeonScreenBase>(false);
    load_window_easy<screens::cTownScreenBase>(false);
    load_window_easy<screens::cSlavemarketScreenBase>(false);
    load_window_easy<screens::cBuildingSetupScreenBase>(false);
    load_window_easy<screens::cMayorScreenBase>(false);
    load_window_easy<screens::cBankScreenBase>(false);
    load_window_easy<screens::cPrisonScreenBase>(false);
    load_window_easy<screens::cPlayerOfficeScreenBase>(false);
    load_window_easy<screens::cMovieMakerScreenBase>(false);
    load_window_easy<screens::cMovieMarketScreenBase>(false);
    load_window_easy<screens::cInfoTraitsScreenBase>(false);
    load_window_easy<screens::cInGameMenuBase>(true);
    load_window<cScreenGameConfig>("GameSetup", false, false);
    load_window<cScreenGameConfig>("UserSettings", false, true);

    auto duration = std::chrono::steady_clock::now() - start_time;
    g_LogFile.debug("interface", "Loaded screens in ", std::chrono::duration_cast<std::chrono::milliseconds>(duration).count(), "ms");
}

template<class T, class... Args>
T* load_window(const char* name, bool nonav, Args&&... args)
{
    g_LogFile.log(ELogLevel::DEBUG, "Loading Window '", name, '\'');
    auto window = std::make_shared<T>(std::forward<Args>(args)...);
    auto result = window.get();
    if (!nonav) {
        register_global_nav_keys(*window);
    }
    window->set_name(name);
    window_manager().add_window(std::move(window));
    return result;
}


void register_global_nav_keys(cInterfaceWindow& window) {
    window.AddKeyCallback(SDLK_F1, []() {
        window_manager().PopToWindow("Brothel Management");
        window_manager().push("Girl Management");
    });

    window.AddKeyCallback(SDLK_F2, []() {
        window_manager().PopToWindow("Brothel Management");
        window_manager().push("Gangs");
    });

    window.AddKeyCallback(SDLK_F3, []() {
        window_manager().PopToWindow("Brothel Management");
        window_manager().push("Dungeon");
    });

    window.AddKeyCallback(SDLK_F4, []() {
        window_manager().PopToWindow("Brothel Management");
        window_manager().push("Town");
        window_manager().push("Slave Market");
    });

    window.AddKeyCallback(SDLK_F5, []() {
        window_manager().PopToWindow("Brothel Management");
        window_manager().push("Item Management");
    });

    window.AddKeyCallback(SDLK_F6, []() {
        window_manager().PopToWindow("Brothel Management");
        window_manager().push("Transfer Screen");
    });

    window.AddKeyCallback(SDLK_F7, []() {
        window_manager().PopToWindow("Brothel Management");
        window_manager().push("Prison");
    });

    window.AddKeyCallback(SDLK_F8, []() {
        window_manager().PopToWindow("Brothel Management");
        window_manager().push("Town");
        window_manager().push("Mayor");
    });
/*
    window.AddKeyCallback(SDLK_F9, []() {
        window_manager().PopToWindow("Brothel Management");
        window_manager().push("Town");
        window_manager().push("Bank");
    });
*/
    window.AddKeyCallback(SDLK_F10, []() {
        window_manager().PopToWindow("Brothel Management");
        window_manager().push("Town");
    });

    window.AddKeyCallback(SDLK_F11, []() {
        window_manager().PopToWindow("Brothel Management");
        window_manager().push("Turn Summary");
    });
    /*
    1-7: Select Brothel
    Tab: Cycle Brothel (Shitf: fwd / back)
    Esc: Back
    F5 Studio
    e/F6 Arena
    F7 Centre
    c/F8 Clinic
    F9 Farm
    p/i Inventory
    F12 House
    m Mayor
    b Bank
    u Building Setup
    */
}
