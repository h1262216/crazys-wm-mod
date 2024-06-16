set(JOBS_SRC
        src/IGenericJob.cpp
        src/cGenericJob.cpp
        src/cJobManager.cpp
        src/IJobManager.cpp
        src/cBasicJob.cpp
        src/Cleaning.cpp
        src/cCrafting.cpp
        src/JobData.cpp
        src/cMatronJob.cpp
        src/cSimpleJob.cpp
        src/TextInterface.cpp
        src/WorkFreetime.cpp
        src/SpecialJobs.cpp
        src/Treatment.cpp
        src/sAttributeCondition.cpp
        src/cBuildingShift.cpp
        )

add_library(WmJobsInterface INTERFACE)
target_include_directories(WmJobsInterface INTERFACE include)

add_library(WmJobsObjects OBJECT ${JOBS_SRC})
target_link_libraries(WmJobsObjects PRIVATE WMEngineObjects lua::lua WmScriptingInterface WmCharacterInterface WmBuildingInterface)
target_link_libraries(WmJobsObjects PUBLIC WmJobsInterface)
target_include_directories(WmJobsObjects PRIVATE include/jobs src)
# TODO Fix includes!
target_include_directories(WmJobsObjects PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)
