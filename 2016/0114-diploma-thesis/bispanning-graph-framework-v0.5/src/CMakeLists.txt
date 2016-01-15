################################################################################
# src/CMakeLists.txt
#
# CMake file for bispanner connectivity project
#
################################################################################
# Copyright (C) 2013-2016 Timo Bingmann <tb@panthema.net>
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
################################################################################

include_directories(${EXPAT_INCLUDE_DIRS})
include_directories(${IGRAPH_INCLUDE_DIRS})
include_directories(${EIGEN3_INCLUDE_DIRS})
include_directories(${Boost_INCLUDE_DIRS})

add_executable(play_bispanning play_bispanning.cpp)
target_link_libraries(play_bispanning
  ${EXPAT_LIBRARIES} ${IGRAPH_LIBRARIES} ${Boost_LIBRARIES})

add_executable(matroid matroid.cpp)

################################################################################
