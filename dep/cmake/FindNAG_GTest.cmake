# **************************************************************************** #
# This file is part of the NAG's common cmake configuration. It first searches
# for GoogleTest in the local file system and if it couldn't find it, the
# newest stable release is fetched from
# https://github.com/google/googletest.git.
#
# Copyright (c) 2022 NAG
# **************************************************************************** #

cmake_minimum_required(VERSION 3.15)

# Quick return if GBench is already imported
if(GTest_FOUND)
  return()
endif()

# Default GTest version
if(WIN32 AND CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
  set(_nag_default_gtest_tag "release-1.10.0")
else()
  set(_nag_default_gtest_tag "release-1.11.0")
endif()

set(NAG_GTest_REPOSITORY "https://github.com/google/googletest.git")
set(NAG_GTest_TAG ${_nag_default_gtest_tag} CACHE STRING
  "The gtest tag which is fetched if no local version is found.")
mark_as_advanced(NAG_GTest_TAG)
option(NAG_GTest_FORCE_FETCH "Whether to force fetch and build GTest." OFF)
mark_as_advanced(NAG_GTest_FORCE_FETCH)

if(NOT NAG_GTest_FORCE_FETCH)
  # Try to locate local GTest installation
  find_package(GTest QUIET)
endif()

# Fetch GTest if there is no local installation
if(NOT GTest_FOUND)
  # Necessary packages
  include(FetchContent)
  find_package(Git QUIET)

  # Disable gmock and gtest install
  set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
  mark_as_advanced(BUILD_GMOCK)
  set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
  mark_as_advanced(INSTALL_GTEST)
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

  # Declare GoogleTest
  FetchContent_GetProperties(googletest)
  FetchContent_Declare(
    googletest
    GIT_REPOSITORY ${NAG_GTest_REPOSITORY}
    GIT_TAG ${NAG_GTest_TAG}
    GIT_SHALLOW ON)

  # Fetch GoogleTest
  if(NOT googletest_POPULATED)
    FetchContent_Populate(googletest)
    add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})
  endif()

  # Print sha
  execute_process(COMMAND ${GIT_EXECUTABLE} rev-parse HEAD
    WORKING_DIRECTORY ${googletest_SOURCE_DIR}
    OUTPUT_VARIABLE googletest_sha)

  string(REPLACE "\n" "" googletest_sha ${googletest_sha})

  # Disable some warnings since GTest always compiles with -Werror or /WX
  if(TARGET gtest)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
      if(WIN32)
        target_compile_options(gtest PRIVATE
          "$<$<COMPILE_LANGUAGE:CXX>:/Qdiag-disable:2586>")
      else()
        target_compile_options(gtest PRIVATE
          "$<$<COMPILE_LANGUAGE:CXX>:-diag-disable:2586>")
      endif()
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "IntelLLVM")
      target_compile_options(gtest PRIVATE
        "$<$<COMPILE_LANGUAGE:CXX>:-Wno-unused-command-line-argument>")
      get_target_property(_opt_old gtest COMPILE_FLAGS)
      string(REPLACE "/utf-8" "" _opt "${_opt_old}")
      string(REPLACE "-utf-8" "" _opt "${_opt_old}")
      set_target_properties(gtest PROPERTIES COMPILE_FLAGS "${_opt}")

      target_compile_options(gtest_main PRIVATE
        "$<$<COMPILE_LANGUAGE:CXX>:-Wno-unused-command-line-argument>")
      get_target_property(_opt_old gtest_main COMPILE_FLAGS)
      string(REPLACE "/utf-8" "" _opt "${_opt_old}")
      string(REPLACE "-utf-8" "" _opt "${_opt_old}")
      set_target_properties(gtest_main PROPERTIES COMPILE_FLAGS "${_opt}")

      unset(_opt_old)
      unset(_opt)
    endif()
  endif()

  # Check if fetch was succesful
  set(GTEST_INCLUDE_DIRS "${googletest_SOURCE_DIR}/googletest/include")
  if(EXISTS "${googletest_SOURCE_DIR}" AND EXISTS "${GTEST_INCLUDE_DIRS}")
    # Pre 3.20 Targets
    if(NOT TARGET GTest::GTest)
      add_library(GTest::GTest INTERFACE IMPORTED)
      target_link_libraries(GTest::GTest INTERFACE gtest)
    endif()
    if(NOT TARGET GTest::Main)
      add_library(GTest::Main INTERFACE IMPORTED)
      target_link_libraries(GTest::GTest INTERFACE gtest_main)
    endif()

    # Post 3.20 Targets
    if(${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.20")
      if(NOT TARGET GTest::gtest)
        add_library(GTest::gtest INTERFACE IMPORTED)
        target_link_libraries(GTest::GTest INTERFACE GTest::GTest)
      endif()
      if(NOT TARGET GTest::gtest_main)
        add_library(GTest::gtest_main INTERFACE IMPORTED)
        target_link_libraries(GTest::GTest INTERFACE GTest::Main)
      endif()
    endif()

    set(GTEST_MAIN_LIBRARIES GTest::GTest)
    set(GTEST_LIBRARIES GTest::Main)
    set(GTEST_BOTH_LIBRARIES GTest::GTest GTest::Main)
    set(GTEST_FOUND 1)
  else()
    set(GTEST_FOUND 0)
  endif()
endif()

# Cleanup
unset(_nag_default_gtest_tag)
