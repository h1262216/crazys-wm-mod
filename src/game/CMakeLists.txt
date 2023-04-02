add_subdirectory(character)
add_subdirectory(scripting)
add_subdirectory(buildings)
add_subdirectory(jobs)


set(SCREENS_SRC
        screens/BuildingScreenManagement.cpp
        screens/cScreenBank.cpp
        screens/cScreenBrothelManagement.cpp
        screens/cScreenBuildingSetup.cpp
        screens/cScreenDungeon.cpp
        screens/cScreenGallery.cpp
        screens/cScreenGangs.cpp
        screens/cScreenGirlDetails.cpp
        screens/cScreenHouseDetails.cpp
        screens/cScreenItemManagement.cpp
        screens/cScreenLoadGame.cpp
        screens/cScreenLoadGame.hpp
        screens/cScreenMainMenu.cpp
        screens/cScreenMayor.cpp
        screens/cScreenMovieMaker.cpp
        screens/cScreenMarketResearch.cpp
        screens/cScreenNewGame.cpp
        screens/cScreenPreparingGame.cpp
        screens/cScreenPrison.cpp
        screens/cScreenSettings.cpp
        screens/cScreenSlaveMarket.cpp
        screens/cScreenTown.cpp
        screens/cScreenTransfer.cpp
        screens/cScreenTurnSummary.cpp
        screens/cGameWindow.cpp
        screens/cScreenGameConfig.cpp
        screens/cScreenInfoTraits.cpp
        screens/cScreenGameMenu.cpp
        )

set(SRC cEvents.cpp
        cGangs.cpp
        cGangManager.cpp
        cGirlGangFight.cpp
        cGirls.cpp
        cGirlTorture.cpp
        cGold.cpp
        cInventory.cpp
        cNameList.cpp
        cObjectiveManager.cpp
        Constants.cpp
        cRandomGirl.cpp
        cRival.cpp
        cShop.cpp
        cTariff.cpp
        IGame.cpp
        gang_missions.cpp
        InterfaceGlobals.cpp
        InterfaceProcesses.cpp
        Inventory.cpp
        sStorage.cpp
        XmlMisc.cpp
        cGameSettings.cpp
        combat/combat.cpp
        combat/action.cpp
        combat/combatant.cpp
        cGame.cpp
        SavesList.cpp
        images/cImageLookup.cpp
        images/ids.cpp
        images/cImageList.cpp
        images/sImageSpec.cpp
        )

set(LUA_DOC_FILE ${RESOURCE_DIR}/Scripts/_traits.lua)

make_generated_header(
        TARGET "generate_traits_ids"
        SCRIPT "traits.py"
        SOURCE "Data/Traits"
        SOURCE_FILE_TYPE "xml"
        ARGS "${LUA_DOC_FILE}"
        OUTPUT "character/include/traits/ids.h"
        BYPRODUCTS "${LUA_DOC_FILE}")

make_generated_header(
        TARGET "generate_event_ids"
        SCRIPT "events.py"
        SOURCE "Scripts"
        SOURCE_FILE_TYPE "lua"
        OUTPUT "events.h")

add_library(WmGameObjects OBJECT ${SCREENS_SRC} ${SRC})
target_link_libraries(WmGameObjects PUBLIC WMEngineObjects pthread doctest::doctest WmScriptingObjects WmCharacterObjects WmBuildingObjects WmJobsObjects)
add_dependencies(WmGameObjects generate_event_ids)
set_target_properties(WmGameObjects PROPERTIES CXX_STANDARD 14)
target_include_directories(WmGameObjects PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

if (UNIX)
    target_compile_definitions(WmGameObjects PUBLIC LINUX=1)
endif(UNIX)

set(AllWmObjects
        $<TARGET_OBJECTS:WmGameObjects>
        $<TARGET_OBJECTS:WmScriptingObjects>
        $<TARGET_OBJECTS:WmCharacterObjects>
        $<TARGET_OBJECTS:WmBuildingObjects>
        $<TARGET_OBJECTS:WMEngineObjects>
        $<TARGET_OBJECTS:WmJobsObjects>
        )

add_executable(WhoreMaster ${AllWmObjects} main.cpp)
target_link_libraries(WhoreMaster PUBLIC WmGameObjects)

add_executable(WhoreMasterFuzz EXCLUDE_FROM_ALL ${AllWmObjects} fuzzmain.cpp )
target_link_libraries(WhoreMasterFuzz PUBLIC WmGameObjects)

add_executable(WhoreMasterTest EXCLUDE_FROM_ALL ${AllWmObjects} unittest.cpp ../engine/text/conditions_tests.cpp
        cGirlsTests.cpp
        )
target_link_libraries(WhoreMasterTest PUBLIC WmGameObjects)
target_include_directories(WhoreMasterTest PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

set_target_properties(WhoreMaster WhoreMasterFuzz WhoreMasterTest PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/../..")
