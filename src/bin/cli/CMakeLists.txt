cmake_minimum_required(VERSION 3.16)

option(UPDATE_TRANSLATIONS "update Translation" OFF)
option(NO_PAINTER_OPERATOR "No PAINTING" OFF)
MESSAGE(STATUS "UPDATE TRANSLATIONS: ${UPDATE_TRANSLATIONS}")

project(dice)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
set(EXECUTABLE_OUTPUT_PATH bin/)

# Find the QtWidgets library
set(QT_REQUIRED_VERSION "6.2.0")

IF(NO_PAINTER_OPERATOR)
    find_package(Qt6 ${QT_REQUIRED_VERSION} CONFIG REQUIRED COMPONENTS Core Svg LinguistTools)
else()
    MESSAGE(STATUS "Compilation of operator Paint")
    find_package(Qt6 ${QT_REQUIRED_VERSION} CONFIG REQUIRED COMPONENTS Core Gui Svg LinguistTools)
    add_definitions(-DPAINTER_OP)
endif()

set(MODE "cli")

set(dice_RESOURCES diceparser.qrc)
find_package(Qt6LinguistTools)
find_package(Qt6Svg)

IF(UPDATE_TRANSLATIONS)
	MESSAGE( update Translation )
    FILE(GLOB_RECURSE translate_dice_SRCS ../*.cpp ../*.h)
    SET(translate_SRCS ${translate_dice_SRCS})
    SET(dice_TS "${CMAKE_CURRENT_SOURCE_DIR}/i18n/dice_en.ts" "${CMAKE_CURRENT_SOURCE_DIR}/i18n/dice_fr.ts")
ELSE()
	MESSAGE( NO updates for translations)
    FILE(GLOB dice_TS "${CMAKE_CURRENT_SOURCE_DIR}/i18n/*.ts")
ENDIF(UPDATE_TRANSLATIONS)

if(Qt6Core_FOUND)
    MESSAGE(status  "find" ${dice_TS} ${translate_SRCS} )
    QT_ADD_RESOURCES(dice_RESOURCES_RCC ${dice_RESOURCES})
    set(QT_PLUGINS_DIR "${Qt6Core_DIR}/../../../plugins")
    get_target_property(QT_LIBRARY_DIR Qt6::Core LOCATION)
    get_filename_component(QT_LIBRARY_DIR ${QT_LIBRARY_DIR} PATH)
endif()

SET( cli_sources
    main.cpp
    displaytoolbox.cpp
)

add_executable( dice ${cli_sources} ${dice_QM})
set(diceparser_shared_INCLUDE_DIRS "../diceparser")
link_directories(BEFORE ${CMAKE_BINARY_DIR})
IF(NO_PAINTER_OPERATOR)
    target_link_libraries(dice PUBLIC Qt6::Core Qt6::Svg PRIVATE diceparser_shared)
ELSE()
    target_link_libraries(dice PUBLIC Qt6::Core Qt6::Gui Qt6::Svg PRIVATE diceparser_shared)
ENDIF()
INSTALL_TARGETS(/bin dice)
