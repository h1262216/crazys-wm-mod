set(JOBS_SRC
        src/IGenericJob.cpp
        src/cJobManager.cpp
        src/BasicJob.cpp
        src/Cleaning.cpp
        src/Crafting.cpp
        src/JobData.cpp
        src/Matron.cpp
        src/SimpleJob.cpp
        src/TextInterface.cpp
        src/WorkFreetime.cpp
        src/Treatment.cpp
        )

add_library(WmJobsInterface INTERFACE)
target_include_directories(WmJobsInterface INTERFACE include)

add_library(WmJobsObjects OBJECT ${JOBS_SRC})
target_link_libraries(WmJobsObjects PRIVATE WMEngineObjects lua::lua WmScriptingInterface WmCharacterInterface WmBuildingInterface)
target_link_libraries(WmJobsObjects PUBLIC WmJobsInterface)
target_include_directories(WmJobsObjects PRIVATE include/jobs src)
# TODO Fix includes!
target_include_directories(WmJobsObjects PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)
