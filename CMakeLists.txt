CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(timescales)

ADD_EXECUTABLE(unittests tests.cpp 
                         tests_labelers.cpp 
                         tests_scales.cpp 
                         tests_scale_simple.cpp 
                         tests_scale_with_timezone.cpp
                         tests_scale_derived.cpp
                         tests_holidays.cpp
                         tests_scale_holidays.cpp
                         tests_timescale.cpp
              )

FIND_PACKAGE(Boost REQUIRED COMPONENTS unit_test_framework date_time system filesystem)

LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})
TARGET_LINK_LIBRARIES(unittests ${Boost_LIBRARIES})
SET_PROPERTY(TARGET unittests PROPERTY COMPILE_DEFINITIONS BOOST_TEST_DYN_LINK)

SET(CMAKE_CXX_FLAGS "-std=c++0x -Wall --coverage -O0 -g -DBOOST_MPL_CFG_NO_PREPROCESSED_HEADERS")

MESSAGE(STATUS "  Build Type: " "${CMAKE_BUILD_TYPE}")
MESSAGE(STATUS "  C++ flags: " "${CMAKE_CXX_FLAGS}")
