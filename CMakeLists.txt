# CMakeLists files in this project can
# refer to the root source directory of the project as ${HELLO_SOURCE_DIR} and
# to the root binary directory of the project as ${HELLO_BINARY_DIR}.
cmake_minimum_required (VERSION 2.8.11)
project (RPG)

set(SOURCES
    ./src/main.cpp
    ./src/game.cpp
    ./src/combatinstance.cpp
    ./src/combatmanager.cpp
    ./src/dice.cpp
    ./src/log.cpp
    ./src/creatures/creature.cpp
    ./src/creatures/human.cpp
    ./src/creatures/player.cpp
    ./src/creatures/wound.cpp
    ./src/creatures/utils.cpp
    ./src/items/item.cpp
    ./src/items/weapon.cpp
    ./src/items/component.cpp
    ./src/level/level.cpp
    ./src/object/object.cpp
    ./src/object/creatureobject.cpp
    ./src/object/playerobject.cpp
    ./src/object/humanobject.cpp
    ./src/ui/gameui.cpp
    ./src/ui/combatui.cpp
    ./src/gfxobjects/gfxlevel.cpp
    )
set(DIRS
    ./src
    ./src/object
    ./src/gfxobjects
    ./src/level
    ./src/items
    ./src/creatures
    ./src/components
    ./src/3rdparty
    ./src/ui
    )
add_executable(RPG ${SOURCES})
if(WIN32)
    set(SFML_ROOT "C:/SFML")
endif()

find_package(SFML REQUIRED COMPONENTS graphics window system)
if(SFML_FOUND)
    include_directories(${SFML_INCLUDE_DIR})
    include_directories(${DIRS})
    target_link_libraries(RPG sfml-graphics sfml-window sfml-system)    
    message("\n found SFML")
else()
    message("\n could not find SFML")
endif()
