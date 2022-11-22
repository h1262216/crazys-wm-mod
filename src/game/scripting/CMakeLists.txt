set(SCRIPTING_SRC
        src/cEventMapping.cpp
        src/cLuaScript.cpp
        src/cLuaState.cpp
        src/cScriptManager.cpp
        src/sLuaGirl.cpp
        src/sLuaParameter.cpp
        src/sLuaEventResult.cpp
        )

add_library(WmScriptingInterface INTERFACE)
target_include_directories(WmScriptingInterface INTERFACE include)

add_library(WmScriptingObjects OBJECT ${SCRIPTING_SRC})
target_link_libraries(WmScriptingObjects PRIVATE WMEngineObjects lua::lua WmCharacterObjects WmScriptingInterface)
target_link_libraries(WmScriptingObjects PUBLIC WmScriptingInterface)
target_include_directories(WmScriptingObjects PRIVATE include/scripting)
# TODO Fix includes!
target_include_directories(WmScriptingObjects PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)
set_target_properties(WmScriptingObjects PROPERTIES CXX_STANDARD 14)

