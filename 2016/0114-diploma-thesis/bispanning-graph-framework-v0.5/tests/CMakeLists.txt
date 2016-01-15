################################################################################
# tests/CMakeLists.txt
#
# CMake file for bispanner connectivity project
#
################################################################################
# Copyright (C) 2015 Timo Bingmann <tb@panthema.net>
#
# This program is free software: you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free Software
# Foundation, either version 3 of the License, or (at your option) any later
# version.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License along with
# this program.  If not, see <http://www.gnu.org/licenses/>.
###############################################################################

include_directories(${EXPAT_INCLUDE_DIRS})
include_directories(${IGRAPH_INCLUDE_DIRS})
include_directories(${EIGEN3_INCLUDE_DIRS})
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${PROJECT_SOURCE_DIR}/src)

set(ALL_LIBRARIES ${EXPAT_LIBRARIES} ${IGRAPH_LIBRARIES} ${Boost_LIBRARIES})

# macro for building test programs with correct libraries
macro(build_test TARGETNAME)

  add_executable(${TARGETNAME} ${ARGN})
  target_link_libraries(${TARGETNAME} ${ALL_LIBRARIES})

endmacro(build_test)

# macro for building test programs, without main() in gtest_main
macro(build_plain PROGNAME)

  string(REPLACE "/" "_" TESTNAME "${PROGNAME}") # replace slashes

  build_test(${TESTNAME} ${PROGNAME}.cpp ${ARGN})

endmacro(build_plain)

# macro for registering test programs: maybe prepend valgrind
macro(test_only TESTNAME)

  add_test(
    NAME ${TESTNAME}
    COMMAND ${TESTNAME} ${ARGN})

endmacro(test_only)

# macro for building and running test programs
macro(build_run_test PROGNAME)

  build_plain(${PROGNAME})

  string(REPLACE "/" "_" TESTNAME "${PROGNAME}") # replace slashes
  test_only(${TESTNAME})

endmacro(build_run_test)

################################################################################

build_run_test(test_geng)
build_run_test(test_random)
build_run_test(test_bispanning)
build_run_test(test_alg_game)

################################################################################
