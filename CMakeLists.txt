# Created by the script cgal_create_CMakeLists
# This is the CMake script for compiling a set of CGAL applications.

cmake_minimum_required(VERSION 3.1...3.15)

project( lira_pqr_mesh )


# CGAL and its components
find_package( CGAL QUIET COMPONENTS  )

if ( NOT CGAL_FOUND )

  message(STATUS "This project requires the CGAL library, and will not be compiled.")
  return()  

endif()


# Boost and its components
find_package( Boost REQUIRED )

if ( NOT Boost_FOUND )

  message(STATUS "This project requires the Boost library, and will not be compiled.")

  return()  

endif()

# include for local directory

# include for local package


# Creating entries for target: lira_pqr_mesh
# ############################

add_executable( lira_pqr_mesh  lira_pqr_mesh.cpp )

add_to_cached_list( CGAL_EXECUTABLE_TARGETS lira_pqr_mesh )

# Link the executable to CGAL and third-party libraries
target_link_libraries(lira_pqr_mesh PRIVATE CGAL::CGAL )

