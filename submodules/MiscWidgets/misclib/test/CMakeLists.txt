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
include(CTest)

set(Boost_FOUND ON)

# Boost
find_package (Boost COMPONENTS unit_test_framework)

#Tests
if (Boost_FOUND)

include_directories (${Boost_INCLUDE_DIRS})
add_definitions (-DBOOST_TEST_DYN_LINK)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address --coverage -O0 -fno-inline")
set(MISCLIB_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/../include/misclib")
include_directories(${MISCLIB_SOURCES}/..)

message(STATUS "Test targets for Misclib enabled")
enable_testing()

set(TEST_TARGETS "")
function(test test_name)
    add_executable(${test_name} EXCLUDE_FROM_ALL ${ARGN})
    target_link_libraries(${test_name} ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
    add_test (NAME ${test_name} COMMAND ${test_name})
    set(TEST_TARGETS ${TEST_TARGETS} ${test_name} PARENT_SCOPE)
endfunction(test)

test(misclib_test_gsl ${MISCLIB_SOURCES}/gsl.hpp gsl.cpp)

# Coverage
add_custom_target(misclib_test_all
    DEPENDS ${TEST_TARGETS}
    COMMENT "Building all tests"
)

set(TRACEFILE "${CMAKE_CURRENT_BINARY_DIR}/coverage.info")
add_custom_target(misclib_coverage
    COMMAND cd "${CMAKE_CURRENT_BINARY_DIR}"
    COMMAND ctest -V
    COMMAND lcov -c -d "${CMAKE_CURRENT_BINARY_DIR}" -b "${MISCLIB_SOURCES}" -o ${TRACEFILE} --no-external
    COMMAND genhtml ${TRACEFILE} -o "${CMAKE_CURRENT_BINARY_DIR}/lconv" -p "${MISCLIB_SOURCES}" --demangle-cpp
    DEPENDS ${TEST_TARGETS}
)

add_custom_target(misclib_coverage_view
    COMMAND xdg-open "${CMAKE_CURRENT_BINARY_DIR}/lconv/index.html"
)

endif()
