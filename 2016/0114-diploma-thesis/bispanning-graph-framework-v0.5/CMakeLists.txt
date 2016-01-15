################################################################################
# CMakeLists.txt
#
# CMake file for bispanner connectivity project
#
################################################################################
# Copyright (C) 2013 Timo Bingmann <tb@panthema.net>
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

project(bispanning)

cmake_minimum_required(VERSION 2.8)

# disallow in-source builds

if("${PROJECT_SOURCE_DIR}" STREQUAL "${PROJECT_BINARY_DIR}")
   message(SEND_ERROR "In-source builds are not allowed.")
endif("${PROJECT_SOURCE_DIR}" STREQUAL "${PROJECT_BINARY_DIR}")

# default to Debug building for single-config generators
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message("Defaulting CMAKE_BUILD_TYPE to Debug")
  set(CMAKE_BUILD_TYPE "Debug")
endif()

# enable warnings

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W -Wall -Wno-unused-local-typedefs")

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -g -march=native")

# enable C++11 and more compiler features

include(CheckCXXCompilerFlag)
check_cxx_compiler_flag(-std=c++11 CXX_HAS_STD_CXX11)
if(CXX_HAS_STD_CXX11)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
else()
  check_cxx_compiler_flag(-std=c++0x CXX_HAS_STD_CXX0X)
  if(CXX_HAS_STD_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
  else()
    message(SEND_ERROR "Could not detect C++11 or C++0x features.")
  endif()
endif()

# enable testing

enable_testing()

# use expat

include(FindEXPAT)
find_package(EXPAT REQUIRED)

# Use Boost.Regex

find_package(Boost 1.46.0 REQUIRED COMPONENTS regex program_options)

# use OpenMP

include(FindOpenMP)

if(OPENMP_FOUND)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
else()
  message(SEND_ERROR "OpenMP not found.")
endif()

# find igraph

find_path(IGRAPH_INCLUDE_DIRS igraph/igraph.h)
find_library(IGRAPH_LIBRARIES NAMES igraph)

include(FindPackageHandleStandardArgs)
set(IGRAPH_FIND_REQUIRED 1)
find_package_handle_standard_args(IGRAPH DEFAULT_MSG
  IGRAPH_INCLUDE_DIRS
  IGRAPH_LIBRARIES
  )

# find Eigen3

if(OFF)
  find_path(EIGEN3_INCLUDE_DIRS Eigen
    PATH_SUFFIXES eigen3)

  include(FindPackageHandleStandardArgs)
  set(EIGEN3_FIND_REQUIRED 1)
  find_package_handle_standard_args(EIGEN3 DEFAULT_MSG
    EIGEN3_INCLUDE_DIRS
    )
endif()

# descend into source

add_subdirectory(src)
add_subdirectory(tests)

################################################################################
