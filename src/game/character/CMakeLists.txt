set(CHARACTER_SRC
        src/cCustomers.cpp
        src/cPlayer.cpp
        src/ICharacter.cpp
        src/sAttribute.cpp
        src/cSkillCap.cpp
        src/sGirl.cpp
        src/traits/cTraitsCollection.cpp
        src/traits/cTraitsManager.cpp
        src/traits/cTraitSpec.cpp
        src/predicates.cpp
        src/pregnancy.cpp
        src/lust.cpp
        src/cGirlPool.cpp
        )

add_library(WmCharacterInterface INTERFACE)
target_include_directories(WmCharacterInterface INTERFACE include)

add_library(WmCharacterObjects OBJECT ${CHARACTER_SRC})
target_link_libraries(WmCharacterObjects PRIVATE WMEngineObjects lua::lua WmScriptingInterface WmBuildingObjects)
target_link_libraries(WmCharacterObjects PUBLIC WmCharacterInterface)
target_include_directories(WmCharacterObjects PRIVATE include/character include/traits)
# TODO Fix includes!
target_include_directories(WmCharacterObjects PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)
set_target_properties(WmCharacterObjects PROPERTIES CXX_STANDARD 14)
add_dependencies(WmCharacterObjects generate_traits_ids)
