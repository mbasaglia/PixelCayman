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

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(SOURCES
document/animation.hpp
document/animation.cpp
document/document.hpp
document/document.cpp
document/document_element.hpp
document/frame.hpp
document/image.hpp
document/image.cpp
document/layer.hpp
document/layer.cpp
document/visitor.hpp
document/io.hpp
document/io.cpp
style/dockwidget_style_icon.hpp
tool/tool.hpp
ui/color_editor.cpp
ui/color_editor.hpp
ui/main_window.cpp
ui/main_window.hpp
view/graphics_item.hpp
view/graphics_widget.hpp
view/graphics_widget.cpp
main.cpp
util.hpp
settings.hpp
${CMAKE_CURRENT_BINARY_DIR}/info.cpp
)

configure_file(info.in.cpp info.cpp)

# Enable extra Qt tools
find_package(Qt5Widgets REQUIRED)
set(CMAKE_AUTOMOC ON)
#set(CMAKE_AUTOUIC ON)
#if ( CMAKE_MAJOR_VERSION LESS 3 )
    file(GLOB UI_FILES ui/*.ui)
    qt5_wrap_ui(SOURCES ${UI_FILES})
#endif()

# Executable
set(EXECUTABLE_NAME pixelcayman) # repeated otherwise kdevelop messes up
add_executable(${EXECUTABLE_NAME} ${SOURCES})

# Libraries

# Color widgets
include_directories(../submodules/ColorWidgets/include)
target_link_libraries(${EXECUTABLE_NAME} ColorWidgets-qt5)

# Extended TabWidget
include_directories(../submodules/Qt-Extended-TabWidget/src)
target_link_libraries(${EXECUTABLE_NAME} ExtendedTabWidget)


# Qt
target_link_libraries(${EXECUTABLE_NAME} Qt5::Widgets)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Install
install(TARGETS ${EXECUTABLE_NAME} RUNTIME DESTINATION bin)
