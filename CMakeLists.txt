
set (CMAKE_EXPORT_COMPILE_COMMANDS 1)
set (CMAKE_CXX_STANDARD 17)
set (CMAKE_BUILD_TYPE Debug)
set( MAKE_CXX_FLAGS "-g -Werror -Wold-style-cast -Wpedantic" )

cmake_minimum_required (VERSION 2.8.11)
project (RPG)

set(SOURCES
    ./src/main.cpp
    ./src/game.cpp
    ./src/scene.cpp
    ./src/overworld.cpp
    ./src/combatinstance.cpp
    ./src/combatmanager.cpp
    ./src/combatmanagerfactory.cpp
    ./src/dice.cpp
    ./src/log.cpp
    ./src/inputmanager.cpp
    ./src/components/aicontroller.cpp
    ./src/components/aicombatcontroller.cpp
    ./src/creatures/creature.cpp
    ./src/creatures/human.cpp
    ./src/creatures/werewolf.cpp
    ./src/creatures/vampire.cpp
    ./src/creatures/goblin.cpp
    ./src/creatures/player.cpp
    ./src/creatures/wound.cpp
    ./src/creatures/utils.cpp
    ./src/items/item.cpp
    ./src/items/armor.cpp
    ./src/items/weapon.cpp
    ./src/items/component.cpp
    ./src/items/consumable.cpp
    ./src/items/utils.cpp
    ./src/level/level.cpp
    ./src/level/levelmanager.cpp
    ./src/level/trigger.cpp
    ./src/level/changeleveltrigger.cpp
    ./src/level/factionclearedtrigger.cpp
    ./src/level/creatureclearedtrigger.cpp  
    ./src/object/object.cpp
    ./src/object/corpseobject.cpp
    ./src/object/creatureobject.cpp
    ./src/object/playerobject.cpp
    ./src/object/humanobject.cpp
    ./src/object/selectorobject.cpp
    ./src/object/itemobject.cpp
    ./src/object/objectfactory.cpp
    ./src/object/relationmanager.cpp
    ./src/object/utils.cpp
    ./src/object/equipmentmanager.cpp
    ./src/object/campfireobject.cpp
    ./src/ui/utils.cpp
    ./src/ui/gameui.cpp
    ./src/ui/combatui.cpp
    ./src/ui/offenseui.cpp
    ./src/ui/defenseui.cpp
    ./src/ui/precombatui.cpp
    ./src/ui/preresolveui.cpp
    ./src/ui/createcharui.cpp
    ./src/ui/inventoryui.cpp
    ./src/ui/positionui.cpp
    ./src/ui/common.cpp
    ./src/ui/numberinput.cpp
    ./src/ui/tradeui.cpp
    ./src/ui/dialogueui.cpp
    ./src/ui/textinput.cpp
    ./src/ui/mainmenuui.cpp
    ./src/ui/createcharui.cpp
    ./src/ui/page.cpp
    ./src/gfxobjects/gfxlevel.cpp
    ./src/gfxobjects/gfxobject.cpp
    ./src/gfxobjects/gfxqueue.cpp
    ./src/gfxobjects/gfxselector.cpp
    ./src/gfxobjects/utils.cpp
    ./src/gameeffects/gameeffect.cpp
    )
set(DIRS
    ./src
    ./src/object
    ./src/gfxobjects
    ./src/level
    ./src/items
    ./src/creatures
    ./src/components
    ./src/gameeffects
    ./src/3rdparty
    ./src/ui
    )
add_executable(RPG ${SOURCES})

if(WIN32)
    set(SFML_DIR "C:/SFML-2.5.1/lib/cmake/SFML")
    set(SFML_ROOT "C:/SFML-2.5.1")

    set(BOOST_DIR "C:/boost_1_70_0")
    set(BOOST_INCLUDEDIR "C:/boost_1_70_0")
    set(BOOST_LIBRARYDIR "C:/boost_1_70_0/lib64-msvc-14.1")
    set(BOOST_ROOT "C:/boost_1_70_0/boost")
    set(Boost_USE_STATIC_LIBS   ON)
endif()
add_subdirectory("external/SFGUI")
set(SFGUI_DIR "./external/SFGUI")
set(SFGUI_INCLUDE_DIR "./external/SFGUI/include")
find_package(SFML REQUIRED COMPONENTS graphics window system)
find_package(Boost REQUIRED system serialization)
find_package(SFGUI REQUIRED)   
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    target_link_libraries(RPG ${Boost_LIBRARIES})
    message("\n found Boost")
else()
    message("\n could not find boost")
endif()
if(SFML_FOUND)
    include_directories(${SFML_INCLUDE_DIR})
    include_directories(${DIRS})
    target_link_libraries(RPG sfml-graphics sfml-window sfml-system)    
    message("\n found SFML")
else()
    message("\n could not find SFML")
endif()
if(SFGUI_FOUND)
    include_directories(${SFGUI_INCLUDE_DIR})
    target_link_libraries(RPG SFGUI)
    message("\n found SFGUI")
else()
    message("\n could not find SFGUI")
endif()   
