# **************************************************************************** #
# This file is part of the NAG's common cmake configuration. It first searches
# for GoogleBenchmark in the local file system and if it couldn't find it, the
# newest stable release is fetched from
# https://github.com/google/benchmark.git.
#
# Copyright (c) 2022 NAG
# **************************************************************************** #

cmake_minimum_required(VERSION 3.15)

# Quick return if GBench is already imported
if(GBench_FOUND)
  return()
endif()

# Quick return if GBench::GBench is already imported, e.g. if a subproject
# fetches GBench as well.
if(TARGET GBench::GBench)
  return()
endif()

set(NAG_GBench_REPOSITORY "https://github.com/google/benchmark.git")
set(NAG_GBench_TAG "v1.6.1" CACHE STRING
  "The gbench tag which is fetched if no local version is found.")
mark_as_advanced(NAG_GBench_TAG)
option(NAG_GBench_FORCE_FETCH "Whether to force fetch and build GTest." OFF)
mark_as_advanced(NAG_GBench_FORCE_FETCH)

if(NOT NAG_GBench_FORCE_FETCH)
  # TODO: Local search
endif()

# Fetch GBench if there is no local installation
if(NOT GBench_FOUND)
  # Necessary packages
  include(FetchContent)
  find_package(Git QUIET)

  # Set configuration and mark everything as advanced
  set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
  mark_as_advanced(BENCHMARK_ENABLE_TESTING)
  set(BENCHMARK_ENABLE_EXCEPTIONS ON CACHE BOOL "" FORCE)
  mark_as_advanced(BENCHMARK_ENABLE_EXCEPTIONS)
  set(BENCHMARK_ENABLE_LTO OFF CACHE BOOL "" FORCE)
  mark_as_advanced(BENCHMARK_ENABLE_LTO)
  set(BENCHMARK_USE_LIBCXX OFF CACHE BOOL "" FORCE)
  mark_as_advanced(BENCHMARK_USE_LIBCXX)
  set(BENCHMARK_ENABLE_WERROR OFF CACHE BOOL "" FORCE)
  mark_as_advanced(BENCHMARK_ENABLE_WERROR)
  set(BENCHMARK_FORCE_WERROR OFF CACHE BOOL "" FORCE)
  mark_as_advanced(BENCHMARK_FORCE_WERROR)
  set(BENCHMARK_ENABLE_INSTALL OFF CACHE BOOL "" FORCE)
  mark_as_advanced(BENCHMARK_ENABLE_INSTALL)
  set(BENCHMARK_ENABLE_DOXYGEN OFF CACHE BOOL "" FORCE)
  mark_as_advanced(BENCHMARK_ENABLE_DOXYGEN)
  set(BENCHMARK_INSTALL_DOCS OFF CACHE BOOL "" FORCE)
  mark_as_advanced(BENCHMARK_INSTALL_DOCS)
  set(BENCHMARK_DOWNLOAD_DEPENDENCIES OFF CACHE BOOL "" FORCE)
  mark_as_advanced(BENCHMARK_DOWNLOAD_DEPENDENCIES)
  set(BENCHMARK_ENABLE_GTEST_TESTS OFF CACHE BOOL "" FORCE)
  mark_as_advanced(BENCHMARK_ENABLE_GTEST_TESTS)
  set(BENCHMARK_USE_BUNDLED_GTEST ON CACHE BOOL "" FORCE)
  mark_as_advanced(BENCHMARK_USE_BUNDLED_GTEST)
  set(BENCHMARK_ENABLE_LIBPFM OFF CACHE BOOL "" FORCE)
  mark_as_advanced(BENCHMARK_ENABLE_LIBPFM)
  set(BENCHMARK_BUILD_32_BITS OFF CACHE BOOL "" FORCE)
  mark_as_advanced(BENCHMARK_BUILD_32_BITS)
  set(BENCHMARK_ENABLE_ASSEMBLY_TESTS OFF CACHE BOOL "" FORCE)
  mark_as_advanced(BENCHMARK_ENABLE_ASSEMBLY_TESTS)

  # Declare GoogleBenchmark
  FetchContent_GetProperties(gbenchmark)
  FetchContent_Declare(
    gbenchmark
    GIT_REPOSITORY ${NAG_GBench_REPOSITORY}
    GIT_TAG ${NAG_GBench_TAG}
    GIT_SHALLOW ON)

  # Fetch GoogleBenchmark
  if(NOT gbenchmark_POPULATED)
    FetchContent_Populate(gbenchmark)
    add_subdirectory(${gbenchmark_SOURCE_DIR} ${gbenchmark_BINARY_DIR})
  endif()

  # Print sha
  execute_process(COMMAND ${GIT_EXECUTABLE} rev-parse HEAD
    WORKING_DIRECTORY ${gbenchmark_SOURCE_DIR}
    OUTPUT_VARIABLE gbenchmark_sha)

  string(REPLACE "\n" "" gbenchmark_sha ${gbenchmark_sha})

  # Check if fetch was succesful
  set(GBench_INCLUDE_DIRS "${gbenchmark_SOURCE_DIR}/include")
  if(EXISTS "${gbenchmark_SOURCE_DIR}" AND EXISTS "${GBench_INCLUDE_DIRS}")
    if(NOT TARGET GBench::GBench)
      add_library(GBench::GBench INTERFACE IMPORTED)
      target_link_libraries(GBench::GBench INTERFACE benchmark::benchmark)
      set_target_properties(GBench::GBench
        PROPERTIES INTERFACE_LINK_DIRECTORIES "${GBench_INCLUDE_DIRS}")
    endif()

    set(GBench_FOUND 1)
  else()
    set(GBench_FOUND 0)
  endif()
else()
endif()
