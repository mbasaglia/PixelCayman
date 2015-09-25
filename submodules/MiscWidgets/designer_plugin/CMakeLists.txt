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

cmake_minimum_required(VERSION 2.6)
set(MISC_WIDGETS_PLUGIN MiscWidgetsPlugin)
set(PROJECT_VERSION "")
project(${MISC_WIDGETS_PLUGIN} CXX)

# Qt
find_package(Qt5Designer REQUIRED)
set(CMAKE_AUTOMOC OFF)
set(CMAKE_AUTOUIC OFF)
set(CMAKE_AUTORCC OFF)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
include_directories(${Qt5Designer_INCLUDE_DIRS})

# Sources
set(SOURCES
plugin_collection.cpp
labeled_spin_box_plugin.cpp
log_view_plugin.cpp
size_widget_plugin.cpp
# add new sources above this line
)

set(HEADERS
plugin_collection.hpp
labeled_spin_box_plugin.hpp
log_view_plugin.hpp
size_widget_plugin.hpp
# add new headers above this line
)

qt5_wrap_cpp(SOURCES ${HEADERS})

# Library
add_library(${MISC_WIDGETS_PLUGIN} MODULE EXCLUDE_FROM_ALL ${SOURCES})
target_link_libraries(${MISC_WIDGETS_PLUGIN} Qt5::Designer)
target_link_libraries(${MISC_WIDGETS_PLUGIN} ${MISC_WIDGETS_LIBRARY})

# install
get_target_property(QT_QMAKE_EXECUTABLE Qt5::qmake LOCATION)
execute_process(COMMAND ${QT_QMAKE_EXECUTABLE} -query QT_INSTALL_PLUGINS
    OUTPUT_VARIABLE QT_INSTALL_PLUGINS OUTPUT_STRIP_TRAILING_WHITESPACE)
# install(TARGETS ${MISC_WIDGETS_PLUGIN} DESTINATION ${QT_INSTALL_PLUGINS}/designer OPTIONAL)

execute_process(COMMAND ${QT_QMAKE_EXECUTABLE} -query QT_INSTALL_LIBS
    OUTPUT_VARIABLE QT_INSTALL_LIBS OUTPUT_STRIP_TRAILING_WHITESPACE)
# install(TARGETS ${MISC_WIDGETS_PLUGIN} DESTINATION ${QT_INSTALL_LIBS}/qtcreator/plugins OPTIONAL)

add_custom_target(${MISC_WIDGETS_PLUGIN}_install
    COMMAND cp $<TARGET_FILE:${MISC_WIDGETS_PLUGIN}> ${QT_INSTALL_PLUGINS}/designer
    COMMAND cp $<TARGET_FILE:${MISC_WIDGETS_PLUGIN}> ${QT_INSTALL_LIBS}/qtcreator/plugins
    DEPENDS ${MISC_WIDGETS_PLUGIN}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
