#.rst:
# FindOpenNI2
# -----------
#
# Find OpenNI2 include dir, library dir and library
#
# Use this module by invoking find_package with the form::
#
#    find_package( OpenNI2 [REQUIRED] )
#
# Results for users are reported in following variables::
#
#    OpenNI2_FOUND       - Return "TRUE" when OpenNI2 found. Otherwise, Return "FALSE".
#    OpenNI2_INCLUDE_DIR - OpenNI2 include directory.
#    OpenNI2_LIBRARY_DIR - OpenNI2 library directory.
#    OpenNI2_LIBRARY     - OpenNI2 library file.
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

set(OPENNI2_SUFFIX)
if(WIN32 AND CMAKE_CL_64)
  set(OPENNI2_SUFFIX 64)
endif()

find_path(
  OpenNI2_INCLUDE_DIR 
  NAMES OpenNI.h
  PATHS "$ENV{OPENNI2_INCLUDE${OPENNI2_SUFFIX}}"
        "/usr/include"
        "/usr/local/include"
  PATH_SUFFIXES openni2
)

find_library(
  OpenNI2_LIBRARY
  NAMES OpenNI2
        libOpenNI2
  PATHS "$ENV{OPENNI2_LIB${OPENNI2_SUFFIX}}"
        "/usr/lib"
        "/usr/local/lib"
)

get_filename_component(OpenNI2_LIBRARY_DIR ${OpenNI2_LIBRARY} DIRECTORY)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(OpenNI2 DEFAULT_MSG OpenNI2_INCLUDE_DIR OpenNI2_LIBRARY_DIR OpenNI2_LIBRARY)
mark_as_advanced(OpenNI2_INCLUDE_DIR OpenNI2_LIBRARY_DIR OpenNI2_LIBRARY)
