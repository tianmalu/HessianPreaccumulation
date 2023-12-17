cmake_minimum_required(VERSION 3.10)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/dep/cmake")

project(etengine)

# --------------- Dependencies ---------------------------
FIND_PACKAGE(Eigen3 REQUIRED)
#FIND_PACKAGE(NAG_GBench REQUIRED)
#FIND_PACKAGE(NAG_GTest REQUIRED)
#FIND_PACKAGE(NAG_dco_cpp REQUIRED)

# --------------- Includes -------------------------------
INCLUDE_DIRECTORIES(src)
INCLUDE_DIRECTORIES(${EIGEN3_INCLUDE_DIRS})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
set(CMAKE_CXX_STANDARD 20)

ADD_SUBDIRECTORY(tests)
