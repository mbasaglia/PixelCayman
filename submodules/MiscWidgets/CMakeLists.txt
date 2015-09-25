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
set(MISC_WIDGETS_LIBRARY MiscWidgets)
set(PROJECT_VERSION "")
project(${MISC_WIDGETS_LIBRARY} CXX)

# Qt
find_package(Qt5Widgets REQUIRED)
set(CMAKE_AUTOMOC OFF)
set(CMAKE_AUTOUIC OFF)
set(CMAKE_AUTORCC OFF)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Debug
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -pedantic -Werror")

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

# Sources
include_directories("${PROJECT_SOURCE_DIR}/include")

set(SOURCES
)

set(HEADERS
src/bool_icon_delegate.hpp
src/enum_combo_delegate.hpp
src/slider_delegate.hpp
src/log_view.hpp
src/labeled_spinbox.hpp
src/size_widget.hpp
)

qt5_wrap_cpp(SOURCES ${HEADERS})

# Library
add_library(${MISC_WIDGETS_LIBRARY} ${SOURCES})
target_link_libraries(${MISC_WIDGETS_LIBRARY} Qt5::Widgets)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

# add_subdirectory(gallery)

find_package(Qt5Designer)
if(Qt5Designer_FOUND)
    add_subdirectory(designer_plugin)
endif()
