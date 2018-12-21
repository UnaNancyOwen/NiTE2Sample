cmake_minimum_required( VERSION 3.6 )

# Require C++11 (or later)
set( CMAKE_CXX_STANDARD 11 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )
set( CMAKE_CXX_EXTENSIONS OFF )

# Create Project
project( Sample )
add_executable( User device.h device.cpp util.h main.cpp )

# Set StartUp Project
set_property( DIRECTORY PROPERTY VS_STARTUP_PROJECT "User" )

# Find Package
# OpenNI2/NiTE2
set( CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}" ${CMAKE_MODULE_PATH} )
find_package( OpenNI2 REQUIRED )
find_package( NiTE2 REQUIRED )

# OpenCV
set( OpenCV_DIR "C:/Program Files/opencv/build" CACHE PATH "Path to OpenCV config directory." )
find_package( OpenCV REQUIRED )

if( OpenNI2_FOUND AND NiTE2_FOUND AND OpenCV_FOUND )
  # Additional Include Directories
  include_directories( ${OpenNI2_INCLUDE_DIR} )
  include_directories( ${NiTE2_INCLUDE_DIR} )
  include_directories( ${OpenCV_INCLUDE_DIRS} )

  # Additional Dependencies
  target_link_libraries( User ${OpenNI2_LIBRARY} )
  target_link_libraries( User ${NiTE2_LIBRARY} )
  target_link_libraries( User ${OpenCV_LIBS} )

  # Post-Build Event (Copy Dependencies)
  add_custom_command( TARGET User POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${NiTE2_REDIST_DIR}/NiTE.ini ${CMAKE_CURRENT_BINARY_DIR}/NiTE.ini )
  add_custom_command( TARGET User POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${NiTE2_REDIST_DIR}/NiTE2 ${CMAKE_CURRENT_BINARY_DIR}/NiTE2 )
endif()

