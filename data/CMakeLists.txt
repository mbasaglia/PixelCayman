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

set(DATA_TARGETS "")

macro(data_file file)
    get_filename_component(directory ${file} DIRECTORY)
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${directory})
    add_custom_command(OUTPUT ${file}
        COMMAND ln -rfs -T ${CMAKE_CURRENT_SOURCE_DIR}/${file} ${file}
        DEPENDS ${file}
    )
    install(FILES ${test} DESTINATION share/${EXECUTABLE_NAME})
    list(APPEND DATA_TARGETS ${file})
endmacro()

data_file(icons/pixel-cayman-32.png)

add_custom_target(data ALL DEPENDS ${DATA_TARGETS})
