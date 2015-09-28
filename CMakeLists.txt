#
# Copyright (C) 2015 Mattia Basaglia
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

cmake_minimum_required(VERSION 2.6)
cmake_policy(SET CMP0009 NEW)
set(NICE_NAME PixelCayman)
project(${NICE_NAME} CXX)
set(PROJECT_VERSION 0.1)
string(TOLOWER ${NICE_NAME} EXECUTABLE_NAME)
set(PROJECT_DESCRIPTION "Customizable pixel art editor and animation tool")
set(PROJECT_LONG_DESCRIPTION "A graphics editor focused on pixel art and extensibility")
set(MAINTAINER "Mattia Basaglia <mattia.basaglia@gmail.com>")
set(PROJECT_WEBSITE "https://github.com/mbasaglia/PixelCayman")
#set(PROJECT_WEBSITE "http://PixelCayman.org") # TODO: Actually make a website :-P
set(PROJECT_VERSION_LONG ${PROJECT_VERSION})

# C++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# For some reason the above flags don't really work...
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR CMAKE_COMPILER_IS_GNUCXX)
    include(CheckCXXCompilerFlag)
    check_cxx_compiler_flag(--std=c++${CMAKE_CXX_STANDARD} STD_CXX)
    if(STD_CXX)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++${CMAKE_CXX_STANDARD}")
    else()
        message(SEND_ERROR "Requires C++${CMAKE_CXX_STANDARD} or better")
    endif()
else()
    message(WARNING "Unrecognized compiler: ${CMAKE_CXX_COMPILER_ID}, make sure it supports C++${CMAKE_CXX_STANDARD}")
endif()

# Debug flags
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -pedantic -Werror")

# Enable Debug by default, can be changed with -D CMAKE_BUILD_TYPE=Release
if(CMAKE_BUILD_TYPE STREQUAL "")
    set(CMAKE_BUILD_TYPE Debug)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_BUILD_TYPE Debug)

    execute_process(COMMAND git rev-parse --short HEAD
    OUTPUT_VARIABLE GIT_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)

    if(GIT_VERSION)
        set(PROJECT_VERSION_LONG "${PROJECT_VERSION}-git-${GIT_VERSION}")
        add_custom_target(version_git ALL
            COMMAND [ "`git rev-parse --short HEAD`" != "${GIT_VERSION}" ] && ${CMAKE_COMMAND} ${CMAKE_SOURCE_DIR}
            VERBATIM
        )
    endif()
endif()

add_subdirectory(src bin)
add_subdirectory(data share/${EXECUTABLE_NAME})
add_subdirectory(plugins lib/${EXECUTABLE_NAME}/plugins)
add_subdirectory(submodules/ColorWidgets)
add_subdirectory(submodules/Qt-Extended-TabWidget)
add_subdirectory(submodules/ConfirmCloseDialog)
add_subdirectory(submodules/MiscWidgets)


file(GLOB_RECURSE ALL_SOURCES
    "src/*.cpp"
    "src/*.hpp"
    "data/plugins/*.cpp"
    "data/plugins/*.hpp"
    "submodules/*/src/*.cpp"
    "submodules/*/src/*.hpp"
    "submodules/*/include/*.hpp"
)
# Doxygen target
find_package(Doxygen)
if(DOXYGEN_FOUND)
    set(DOXYGEN_OUTPUT "\"${CMAKE_BINARY_DIR}/doc\"")

    set(DOXYGEN_INPUT "")
    file(GLOB_RECURSE DOXYGEN_INPUT_FILES
        "*.dox"
        "src/*.dox"
    )
    set(DOXYGEN_INPUT_FILES ${DOXYGEN_INPUT_FILES} ${ALL_SOURCES})
    foreach(source ${DOXYGEN_INPUT_FILES})
        set(DOXYGEN_INPUT "${DOXYGEN_INPUT} \"${source}\"")
    endforeach()

    configure_file(${CMAKE_SOURCE_DIR}/Doxyfile.in ${PROJECT_BINARY_DIR}/Doxyfile)
    add_custom_target(doc
        ${DOXYGEN_EXECUTABLE} ${PROJECT_BINARY_DIR}/Doxyfile
        DEPENDS ${PROJECT_BINARY_DIR}/Doxyfile ${DOXYGEN_INPUT_FILES}
        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
        COMMENT "Generating Doxygen Documentation" VERBATIM
    )
    add_custom_target(doc-view
        xdg-open ${DOXYGEN_OUTPUT}/html/index.html
        COMMENT "Showing Doxygen documentation"
    )
endif(DOXYGEN_FOUND)

# Translation
find_package(Qt5LinguistTools)
if(Qt5LinguistTools_FOUND)
    set(TRANSLATIONS translations/it_IT.ts)
    foreach(TRANSLATION ${TRANSLATIONS})
        set(TS_FILE "${CMAKE_CURRENT_SOURCE_DIR}/${TRANSLATION}")
        set_source_files_properties(${TS_FILE} PROPERTIES
            OUTPUT_LOCATION
            "${CMAKE_BINARY_DIR}/share/${EXECUTABLE_NAME}/translations")
    endforeach()
    file(GLOB_RECURSE UI_FILES
        "src/*.ui"
        "data/*.ui"
        "plugins/*.ui"
        "submodules/*.ui"
    )
    set(TRANSLATABLE ${ALL_SOURCES} ${UI_FILES})
    qt5_create_translation(QM_FILES ${TRANSLATIONS} ${TRANSLATABLE})
    add_custom_target(translations DEPENDS ${QM_FILES})
    # TODO: check installation
endif()

# Package generation
find_program(DPKG_PROGRAM dpkg)
if(DPKG_PROGRAM)
    execute_process(
        COMMAND ${DPKG_PROGRAM} --print-architecture
        OUTPUT_VARIABLE CPACK_DEBIAN_PACKAGE_ARCHITECTURE
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    set(CPACK_PACKAGE_FILE_NAME "${EXECUTABLE_NAME}_${PROJECT_VERSION_LONG}_${CPACK_DEBIAN_PACKAGE_ARCHITECTURE}")
endif()

include(InstallRequiredSystemLibraries)
set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_SOURCE_DIR}/COPYING)
set(CPACK_RESOURCE_FILE_README ${CMAKE_SOURCE_DIR}/README.md)
set(CPACK_PACKAGE_ICON data/icons/pixel-cayman/32x32/pixel-cayman.png)
#CPACK_CREATE_DESKTOP_LINKS
set(CPACK_PACKAGE_CONTACT ${MAINTAINER})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY ${PROJECT_DESCRIPTION})
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION_LONG})

set(CPACK_DEBIAN_PACKAGE_HOMEPAGE ${PROJECT_WEBSITE})
set(CPACK_DEBIAN_PACKAGE_SECTION graphics)
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
set(CPACK_DEBIAN_PACKAGE_DESCRIPTION "${CPACK_PACKAGE_DESCRIPTION_SUMMARY}\n ${PROJECT_LONG_DESCRIPTION}")

include(CPack)
