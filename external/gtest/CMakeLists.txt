cmake_minimum_required(VERSION 2.8.12.2)

project(gtest C CXX)
include(ExternalProject)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(gtest_PREFIX "${CMAKE_CURRENT_BINARY_DIR}/external/gtest")
set(gtest_CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${gtest_prefix_DIR})

ExternalProject_Add(googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  CMAKE_ARGS -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
             -${gtest_CMAKE_ARGS}
             -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE:PATH=ReleaseLibs
             -Dgtest_force_shared_crt=ON
  INSTALL_COMMAND ""
)

ExternalProject_Get_Property(googletest source_dir)
include_directories(${source_dir}/googletest/include)
set(GTEST_INCLUDE_DIRS ${source_dir}/googletest/include PARENT_SCOPE)

ExternalProject_Get_Property(googletest binary_dir)
set(GTEST_LIBS_DIR ${binary_dir} PARENT_SCOPE)

