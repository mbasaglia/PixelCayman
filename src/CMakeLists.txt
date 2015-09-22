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
document/builder.hpp
document/document.hpp
document/document.cpp
document/document_element.hpp
document/frame.hpp
document/image.hpp
document/image.cpp
document/layer.hpp
document/layer.cpp
document/visitor.hpp
document/layer_container.hpp
document/layer_container.cpp
document/format_settings.hpp
document/command/change_image.hpp
document/command/change_image.cpp
document/command/move_child_layers.hpp
document/command/move_child_layers.cpp
document/command/set_property.hpp
io/formats.hpp
io/formats.cpp
io/bitmap.hpp
io/bitmap.cpp
io/xml.hpp
io/xml.cpp
style/dockwidget_style_icon.hpp
style/itemview_style_rowdrop.hpp
tool/tool.hpp
tool/brush.hpp
tool/brush.cpp
tool/eraser.hpp
tool/registry.hpp
ui/color_editor.cpp
ui/color_editor.hpp
ui/layer_widget.hpp
ui/layer_widget.cpp
ui/main_window.cpp
ui/main_window.hpp
ui/main_window_p.hpp
ui/tool_paint_widget.hpp
ui/tool_paint_widget.cpp
ui/dialog_layer.hpp
ui/menu.hpp
view/graphics_item.hpp
view/graphics_widget.hpp
view/graphics_widget.cpp
item/layer_tree.hpp
item/layer_tree.cpp
item/tree_view_accept_self.hpp
misc/composition_mode.hpp
misc/composition_mode.cpp
misc/color.hpp
misc/color.cpp
misc/misc.hpp
plugin/plugin.hpp
plugin/plugin.cpp
plugin/library_plugin.hpp
plugin/library_plugin.cpp
plugin/plugin_api.hpp
plugin/plugin_api.cpp
main.cpp
settings.hpp
data.cpp
data.hpp
draw.hpp
math.hpp
info.cpp
info.hpp
message.hpp
message.cpp
application.hpp
application.cpp
${CMAKE_CURRENT_BINARY_DIR}/static_info.hpp
)

configure_file(static_info.in.hpp static_info.hpp)

# Enable extra Qt tools
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Xml REQUIRED)
set(CMAKE_AUTOMOC ON)
#set(CMAKE_AUTOUIC ON)
#if ( CMAKE_MAJOR_VERSION LESS 3 )
set(UI_FILES
ui/color_editor.ui
ui/current_color.ui
ui/main_window.ui
ui/layer_widget.ui
ui/tool_paint_widget.ui
ui/dialog_layer.ui
)
qt5_wrap_ui(SOURCES ${UI_FILES})
#endif()

# Executable
set(EXECUTABLE_NAME pixelcayman) # repeated otherwise kdevelop messes up
add_executable(${EXECUTABLE_NAME} ${SOURCES})

# Submodules
include_directories(../submodules/ColorWidgets/include)
target_link_libraries(${EXECUTABLE_NAME} ColorWidgets-qt5)

include_directories(../submodules/Qt-Extended-TabWidget/src)
target_link_libraries(${EXECUTABLE_NAME} ExtendedTabWidget)

include_directories(../submodules/ConfirmCloseDialog/src)
target_link_libraries(${EXECUTABLE_NAME} ConfirmClose)

include_directories(../submodules/MiscWidgets/src)
target_link_libraries(${EXECUTABLE_NAME} MiscWidgets)

# Qt
target_link_libraries(${EXECUTABLE_NAME} Qt5::Widgets)
target_link_libraries(${EXECUTABLE_NAME} Qt5::Xml)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Install
install(TARGETS ${EXECUTABLE_NAME} RUNTIME DESTINATION bin)
