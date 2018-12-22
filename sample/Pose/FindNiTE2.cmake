#.rst:
# FindNiTE2
# ---------
#
# Find NiTE2 include dir, library dir, library and redistributable dir
#
# Use this module by invoking find_package with the form::
#
#    find_package( NiTE2 [REQUIRED] )
#
# Results for users are reported in following variables::
#
#    NiTE2_FOUND       - Return "TRUE" when NiTE2 found. Otherwise, Return "FALSE".
#    NiTE2_INCLUDE_DIR - NiTE2 include directory.
#    NiTE2_LIBRARY_DIR - NiTE2 library directory.
#    NiTE2_LIBRARY     - NiTE2 library file.
#    NiTE2_REDIST_DIR  - NiTE2 redistributable directory.
#
# =============================================================================
#
# Copyright (c) 2018 Tsukasa SUGIURA
# Distributed under the MIT License.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
# The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#
# =============================================================================

set(NITE2_SUFFIX)
if(WIN32 AND CMAKE_CL_64)
  set(NITE2_SUFFIX 64)
endif()

find_path(
  NiTE2_INCLUDE_DIR
  NAMES NiTE.h
  PATHS "$ENV{NITE2_INCLUDE${NITE2_SUFFIX}}"
        "/usr/include"
        "/usr/local/include"
  PATH_SUFFIXES nite2
)

find_library(
  NiTE2_LIBRARY
  NAMES NiTE2
        libNiTE2
  PATHS "$ENV{NITE2_LIB${NITE2_SUFFIX}}"
        "$ENV{NITE2_REDIST${NITE2_SUFFIX}}"
        "/usr/lib"
        "/usr/local/lib"
)

get_filename_component(NiTE2_LIBRARY_DIR ${NiTE2_LIBRARY} DIRECTORY)

find_path(
  NiTE2_REDIST_DIR
  NAMES NiTE.ini
  PATHS "$ENV{NITE2_REDIST${NITE2_SUFFIX}}"
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(NiTE2 DEFAULT_MSG NiTE2_INCLUDE_DIR NiTE2_LIBRARY_DIR NiTE2_LIBRARY NiTE2_REDIST_DIR)
mark_as_advanced(NiTE2_INCLUDE_DIR NiTE2_LIBRARY_DIR NiTE2_LIBRARY NiTE2_REDIST_DIR)
