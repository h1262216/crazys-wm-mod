set(BUILDING_SRC
        src/cDungeon.cpp
        src/cBuildingManager.cpp
        src/cBuildingShift.cpp
        src/queries.cpp
        src/IBuilding.cpp
        src/cBuilding.cpp
        src/arena/ArenaJobs.cpp
        src/arena/cArena.cpp
        #src/brothel/BrothelJobs.cpp
        #src/brothel/Escort.cpp
        #src/brothel/Whore.cpp
        #src/brothel/WorkTorturer.cpp
        src/brothel/cBrothel.cpp
        src/centre/cCentre.cpp
        src/centre/CentreJobs.cpp
        #src/centre/Therapy.cpp
        src/clinic/cClinic.cpp
        #src/clinic/ClinicJobs.cpp
        #src/clinic/Surgery.cpp
        src/farm/cFarm.cpp
        #src/farm/FarmJobs.cpp
        #src/farm/WorkFarmPonyGirl.cpp
        src/house/cHouse.cpp
        #src/house/House.cpp
        #src/house/Training.cpp
        #src/house/WorkPersonalBedWarmer.cpp
        src/studio/cMovieStudio.cpp
        src/studio/data.cpp
        src/studio/filming.cpp
        src/studio/manager.cpp
        #src/studio/StudioCrew.cpp
        #src/studio/StudioFilming.cpp
        #src/studio/StudioOther.cpp
        #src/studio/StudioScenes.cpp
        include/buildings/sGirlShiftData.h)

add_library(WmBuildingInterface INTERFACE)
target_include_directories(WmBuildingInterface INTERFACE include)

add_library(WmBuildingObjects OBJECT ${BUILDING_SRC})
target_link_libraries(WmBuildingObjects PRIVATE WMEngineObjects lua::lua WmScriptingInterface WmCharacterInterface WmJobsInterface)
target_link_libraries(WmBuildingObjects PUBLIC WmBuildingInterface)
target_include_directories(WmBuildingObjects PRIVATE include/buildings src)
# TODO Fix includes!
target_include_directories(WmBuildingObjects PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)
set_target_properties(WmBuildingObjects PROPERTIES CXX_STANDARD 14)