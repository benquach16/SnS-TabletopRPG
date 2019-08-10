set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
set(CMAKE_CXX_STANDARD 17)
set( CMAKE_CXX_FLAGS "-g -Werror -Wold-style-cast -Wpedantic" )

cmake_minimum_required (VERSION 2.8.11)
project (RPG)

set(SOURCES
    ./src/main.cpp
    ./src/game.cpp
    ./src/combatinstance.cpp
    ./src/combatmanager.cpp
    ./src/combatmanagerfactory.cpp
    ./src/dice.cpp
    ./src/log.cpp
    ./src/components/aicontroller.cpp
    ./src/creatures/creature.cpp
    ./src/creatures/human.cpp
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
    ./src/ui/gameui.cpp
    ./src/ui/combatui.cpp
    ./src/ui/offenseui.cpp
    ./src/ui/defenseui.cpp
    ./src/ui/precombatui.cpp
    ./src/ui/inventoryui.cpp
    ./src/ui/positionui.cpp
    ./src/ui/common.cpp
    ./src/ui/numberinput.cpp
    ./src/ui/tradeui.cpp
    ./src/ui/dialogueui.cpp
    ./src/gfxobjects/gfxlevel.cpp
    ./src/gfxobjects/gfxobject.cpp
    ./src/gfxobjects/gfxqueue.cpp
    ./src/gfxobjects/gfxselector.cpp
    ./src/gfxobjects/utils.cpp
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
    set(SFML_DIR "C:/SFML-2.5.1/lib/cmake/SFML")
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
