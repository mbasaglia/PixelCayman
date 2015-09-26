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

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

find_package(Qt5Widgets REQUIRED)
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
include_directories(${Qt5Widgets_INCLUDE_DIRS})
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../../src")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../../submodules/ColorWidgets/include")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../../submodules/MiscWidgets/misclib/include")

function(cayman_plugin name sources)
    add_library(${name} MODULE ${SOURCES})
    # TODO: Install plugins in lib/ not share/
    install(TARGETS ${name} DESTINATION ${DATA_INSTALL_PATH}/plugins)
endfunction()


add_subdirectory(ansi)
add_subdirectory(bitmap_io)
add_subdirectory(external_tools)
add_subdirectory(inkscape)
