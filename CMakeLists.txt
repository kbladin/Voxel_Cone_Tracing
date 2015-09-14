# Some functions require a certain version of cmake
cmake_minimum_required(VERSION 2.8)

# Create the project
set(PROJECT_NAME Voxelization)
project(${PROJECT_NAME})

# Add the external module path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake/Modules)

# Find external packages
find_package(OPENGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(GLFW3 REQUIRED)
find_package(ASSIMP REQUIRED)

#find_package(ANTTWEAKBAR REQUIRED)

if(APPLE)
  # Find external libraries
  find_library(OPENGL_FRAMEWORK OpenGL)
  find_library(COCOA_FRAMEWORK Cocoa)
endif(APPLE)

# Add include directories 
include_directories(${GLEW_INCLUDE_DIR})
include_directories(${GLFW_INCLUDE_DIRS})
include_directories(${ASSIMP_INCLUDE_DIRs})
#include_directories(${ANT_TWEAK_BAR_INCLUDE_PATH})

# Our own include directory
set(COMMON_INCLUDES ${PROJECT_SOURCE_DIR}/include)
include_directories(${COMMON_INCLUDES})

set(CMAKE_CXX_FLAGS "-g")

# Creating our own library
file(GLOB Voxelization_lib_SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp)
file(GLOB Voxelization_lib_HEADERS ${PROJECT_SOURCE_DIR}/include/*.h)
# We do not want main.cpp in the library
file(GLOB to_remove ${PROJECT_SOURCE_DIR}/src/main.cpp)
list(REMOVE_ITEM Voxelization_lib_SOURCES ${to_remove})
list(REMOVE_ITEM Voxelization_lib_HEADERS ${to_remove})
add_library(Voxelization_lib ${Voxelization_lib_SOURCES} ${Voxelization_lib_HEADERS})

# Creating our executable
add_executable(Voxelization src/main.cpp)

# C++11 compatability
set_target_properties(Voxelization PROPERTIES COMPILE_FLAGS "-std=c++11")

# Link all the libraries (including our own)
target_link_libraries(Voxelization Voxelization_lib ${ASSIMP_LIBRARIES} ${OPENGL_LIBRARIES} ${OPENGL_glu_LIBRARY} ${GLEW_LIBRARY} ${GLFW_LIBRARIES})

######### OPENGL LIBRARY (ONLY FOR OSX) #############
if(APPLE)
  target_link_libraries(Voxelization ${OPENGL_FRAMEWORK} ${COCOA_FRAMEWORK})
endif(APPLE)
