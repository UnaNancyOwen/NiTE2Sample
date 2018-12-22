cmake_minimum_required( VERSION 3.6 )

# Require C++11 (or later)
set( CMAKE_CXX_STANDARD 11 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )
set( CMAKE_CXX_EXTENSIONS OFF )

# Create Project
project( Sample )
add_executable( Skeleton device.h device.cpp util.h main.cpp )

# Set StartUp Project
set_property( DIRECTORY PROPERTY VS_STARTUP_PROJECT "Skeleton" )

# Find Package
# OpenNI2/NiTE2
set( CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}" ${CMAKE_MODULE_PATH} )
find_package( OpenNI2 REQUIRED )
find_package( NiTE2 REQUIRED )

# OpenCV
set( OpenCV_DIR "C:/Program Files/opencv/build" CACHE PATH "Path to OpenCV config directory." )
find_package( OpenCV REQUIRED )

# OpenMP
find_package( OpenMP )

if( OpenNI2_FOUND AND NiTE2_FOUND AND OpenCV_FOUND )
  # Additional Include Directories
  include_directories( ${OpenNI2_INCLUDE_DIR} )
  include_directories( ${NiTE2_INCLUDE_DIR} )
  include_directories( ${OpenCV_INCLUDE_DIRS} )

  # Additional Dependencies
  target_link_libraries( Skeleton ${OpenNI2_LIBRARY} )
  target_link_libraries( Skeleton ${NiTE2_LIBRARY} )
  target_link_libraries( Skeleton ${OpenCV_LIBS} )

  # Post-Build Event (Copy Dependencies)
  add_custom_command( TARGET Skeleton POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${NiTE2_REDIST_DIR}/NiTE.ini ${CMAKE_CURRENT_BINARY_DIR}/NiTE.ini )
  add_custom_command( TARGET Skeleton POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${NiTE2_REDIST_DIR}/NiTE2 ${CMAKE_CURRENT_BINARY_DIR}/NiTE2 )
endif()

if( OpenMP_FOUND )
  set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}" )
  set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}" )
endif()
