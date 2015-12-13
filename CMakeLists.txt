cmake_minimum_required(VERSION 2.8.12.2)
SET(PROJECT_NAME ComplexTerrain)

#set project
project(${PROJECT_NAME})

#variables for directories
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

set(PROJECT_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
set(PROJECT_CXX_DIR ${PROJECT_SOURCE_DIR}/src)
set(PROJECT_EXT_DIR ${PROJECT_SOURCE_DIR}/external)
set(PROJECT_TEST_DIR ${PROJECT_SOURCE_DIR}/test)

if( CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR )
    message( FATAL_ERROR "Please make a build directory" )
endif()
if( CMAKE_SOURCE_DIR MATCHES " " )
  message( "Your Source Directory contains spaces. If you experience problems when compiling, this can be the cause." )
endif()
if( CMAKE_BINARY_DIR MATCHES " " )
  message( "Your Build Directory contains spaces. If you experience problems when compiling, this can be the cause." )
endif()

#compiler flags
SET(CMAKE_CXX_FLAGS "-std=c++11")

if(CMAKE_COMPILER_IS_GNUCXX)
add_definitions(
  -g
  -Wall
#TODO: What are there macros?
# answer: TW is AntTweakBar GUI
# -DTW_STATIC
# -DTW_NO_LIB_PRAGMA
# -DTW_NO_DIRECTD3D
# answer: linking GLEW statically
 -DGLEW_STATIC
# answer: removes warnings regarding unsecure methods
 -D_CRT_SECURE_NO_WARNINGS
)
endif()

#git is used to download glm
find_package(Git REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
FIND_PACKAGE(PkgConfig REQUIRED )
pkg_search_module(GLFW REQUIRED glfw3 )

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
#fix all opengl libs
set(OPENGL_LIBRARY
  ${OPENGL_LIBRARY}
  -lGL -lGLU -lXrandr -lXext -lX11 -lrt -lXxf86vm
  ${CMAKE_DL_LIBS}
)
#add unix macros
add_definitions(-D_UNIX)
endif()

#Get external dependencies
add_subdirectory(
  ${PROJECT_EXT_DIR}
)

set(ext_includes
  ${GLM_INCLUDE_DIR}
  ${GLEW_INCLUDE_DIR}
  ${GLFW_INCLUDE_DIR}
)

include_directories(
        ${ext_includes}
        ${PROJECT_INCLUDE_DIR}
)

set(EXT_DEPS
    glm
    pthread
    googletest
    gtest
)



set(ALL_LIBS
  ${OPENGL_LIBRARY}
  ${GLFW_STATIC_LIBRARIES}
  ${GLEW_LIBRARIES}
)

add_subdirectory(${PROJECT_CXX_DIR})
add_executable(${PROJECT_NAME} ${MAIN_SOURCE})
#add_dependencies(${PROJECT_NAME} ${EXT_DEPS})
target_link_libraries(${PROJECT_NAME} ${ALL_LIBS})
####
# Testing
##

enable_testing()

set(PROJECT_TEST_NAME ${PROJECT_NAME}_test)

include_directories(${GTEST_INCLUDE_DIRS})

add_subdirectory(${PROJECT_SOURCE_DIR}/test)

add_executable(${PROJECT_TEST_NAME} ${TEST_SRC_FILES})
add_dependencies(${PROJECT_TEST_NAME} ${EXT_DEPS})
target_link_libraries(${PROJECT_TEST_NAME} ${ALL_LIBS})

set(Suffix ".a")
set(Pthread "-pthread")
target_link_libraries(${PROJECT_TEST_NAME}
       debug ${GTEST_LIBS_DIR}/googlemock/gtest/DebugLibs/${CMAKE_FIND_LIBRARY_PREFIXES}gtest${Suffix}
       optimized ${GTEST_LIBS_DIR}/googlemock/gtest/ReleaseLibs/${CMAKE_FIND_LIBRARY_PREFIXES}gtest${Suffix}
)

target_link_libraries(${PROJECT_TEST_NAME}
       debug ${GTEST_LIBS_DIR}/googlemock/gtest/DebugLibs/${CMAKE_FIND_LIBRARY_PREFIXES}gtest_main${Suffix}
       optimized ${GTEST_LIBS_DIR}/googlemock/gtest/ReleaseLibs/${CMAKE_FIND_LIBRARY_PREFIXES}gtest_main${Suffix}
       ${Pthread}
)

add_test(test ${PROJECT_TEST_NAME})

