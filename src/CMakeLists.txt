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
cayman/application.cpp
cayman/application.hpp
cayman/application_init_info.cpp
cayman/data.cpp
cayman/data.hpp
cayman/message.cpp
cayman/message.hpp
cayman/settings.hpp
${CMAKE_CURRENT_BINARY_DIR}/static_info.hpp
document/animation.cpp
document/animation.hpp
document/builder.hpp
document/command/change_image.cpp
document/command/change_image.hpp
document/command/move_child_layers.cpp
document/command/move_child_layers.hpp
document/command/set_property.hpp
document/document.cpp
document/document_element.hpp
document/document.hpp
document/format_settings.hpp
document/frame.hpp
document/image.cpp
document/image.hpp
document/layer_container.cpp
document/layer_container.hpp
document/layer.cpp
document/layer.hpp
document/visitor.hpp
io/bitmap.cpp
io/bitmap.hpp
io/formats.cpp
io/formats.hpp
io/xml.cpp
io/xml.hpp
item/layer_tree.cpp
item/layer_tree.hpp
item/tree_view_accept_self.hpp
misc/color.cpp
misc/color.hpp
misc/composition_mode.cpp
misc/composition_mode.hpp
misc/draw.hpp
misc/math.hpp
misc/misc.hpp
plugin/library_plugin.cpp
plugin/library_plugin.hpp
plugin/plugin_api.cpp
plugin/plugin_api.hpp
plugin/plugin.cpp
plugin/plugin.hpp
style/dockwidget_style_icon.hpp
style/itemview_style_rowdrop.hpp
tool/brush.cpp
tool/brush.hpp
tool/eraser.hpp
tool/flood_fill.hpp
tool/flood_fill.cpp
tool/registry.hpp
tool/tool.hpp
ui/widgets/color_editor.cpp
ui/widgets/color_editor.hpp
ui/widgets/layer_widget.cpp
ui/widgets/layer_widget.hpp
ui/widgets/tool_paint_widget.cpp
ui/widgets/tool_paint_widget.hpp
ui/dialogs/main_window.cpp
ui/dialogs/main_window.hpp
ui/dialogs/main_window_p.hpp
ui/dialogs/dialog_layer_create.hpp
ui/dialogs/dialog_document_create.hpp
ui/dialogs/dialog_about.hpp
ui/dialogs/dialog_about.cpp
ui/dialogs/dialog_settings.hpp
ui/dialogs/dialog_settings.cpp
ui/menu.hpp
view/graphics_item.hpp
view/graphics_widget.cpp
view/graphics_widget.hpp
main.cpp
)

configure_file(cayman/static_info.in.hpp static_info.hpp)

# Enable extra Qt tools
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Xml REQUIRED)
set(CMAKE_AUTOMOC ON)
#set(CMAKE_AUTOUIC ON)
#if ( CMAKE_MAJOR_VERSION LESS 3 )
set(UI_FILES
ui/widgets/color_editor.ui
ui/widgets/current_color.ui
ui/widgets/layer_widget.ui
ui/widgets/tool_paint_widget.ui
ui/dialogs/main_window.ui
ui/dialogs/dialog_layer_create.ui
ui/dialogs/dialog_document_create.ui
ui/dialogs/dialog_about.ui
ui/dialogs/dialog_settings.ui
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
include_directories(../submodules/MiscWidgets/misclib/include)
target_link_libraries(${EXECUTABLE_NAME} MiscWidgets)

# Qt
target_link_libraries(${EXECUTABLE_NAME} Qt5::Widgets)
target_link_libraries(${EXECUTABLE_NAME} Qt5::Xml)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Install
install(TARGETS ${EXECUTABLE_NAME} RUNTIME DESTINATION bin)
