project(FMF-Mallas)
cmake_minimum_required(VERSION 3.1)
set(CMAKE_SKIP_INSTALL_RULES True)

# Compiler requirements
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

FIND_PACKAGE(OpenMP)
if(OPENMP_FOUND)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

#Activate all warnings
if(CMAKE_COMPILER_IS_GNUCC)
    add_definitions ("-Wall")
    add_definitions ("-std=c++11")
endif()
if ( MSVC )
    add_definitions ("/W4")
endif()


include_directories(.)

#Search for Eigen library
set(EIGEN3_INCLUDE_DIR "${CMAKE_SOURCE_DIR}")
message("EIGEN ${EIGEN3_VERSION} found at ${EIGEN3_INCLUDE_DIR}" )
include_directories(${EIGEN3_INCLUDE_DIR})
add_definitions(-DEIGEN3_FOUND)

#Copy aux files to build directory
foreach(file mallas UVchecker.jpg isolinesTexture.png isolinesTexture2.png)
  if(NOT EXISTS ${CMAKE_BINARY_DIR}/${file} )
    message("copy " ${CMAKE_SOURCE_DIR}/${file}  " to " ${CMAKE_BINARY_DIR} )
    file(COPY ${CMAKE_SOURCE_DIR}/${file} DESTINATION ${CMAKE_BINARY_DIR} )
endif()
endforeach()

#Simple examples and tests
add_executable(meshColor meshColor.cpp vec3.hpp SimpleMesh.hpp ColorMesh.hpp)
add_executable(meshColor2 meshColor2.cpp vec3.hpp SimpleMesh.hpp TextureMesh.hpp)

#Exercises to solve
add_executable(meshStatistic meshStatistic.cpp vec3.hpp SimpleMesh.hpp)

add_executable(meshBoundary meshBoundary.cpp vec3.hpp SimpleMesh.hpp )

add_executable(meshTexture-Dense meshTexture-Dense.cpp vec3.hpp SimpleMesh.hpp TextureMesh.hpp)
add_executable(meshTexture-Sparse meshTexture-Sparse.cpp vec3.hpp SimpleMesh.hpp TextureMesh.hpp)

