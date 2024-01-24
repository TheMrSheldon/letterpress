cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)

option(BUILD_TESTS "Build tests" ON)

######################################################
# READ VERSION NUMBER FROM ./include/letterpress.hpp #
######################################################
file(STRINGS "${CMAKE_CURRENT_LIST_DIR}/include/letterpress/letterpress.hpp" LETTERPRESS_VERSION_MACRO REGEX "^\tconstexpr const auto version = \"(([0-9]+\\.)*[0-9]+)\";$")
string(REGEX REPLACE "^\tconstexpr const auto version = \"(([0-9]+\\.)*[0-9]+)\"\\;$" "\\1" LETTERPRESS_VERSION ${LETTERPRESS_VERSION_MACRO})

project(letterpress VERSION ${LETTERPRESS_VERSION} LANGUAGES CXX)

message("Compiling Letterpress v${LETTERPRESS_VERSION}")


set(CMAKE_INSTALL_BINDIR "${CMAKE_CURRENT_LIST_DIR}/bin/")
add_subdirectory(src)

##########################################################################################
# Testing
##########################################################################################
# Testing only available if this is the main app
# Emergency override MODERN_CMAKE_BUILD_TESTING provided as well
if((CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME OR MODERN_CMAKE_BUILD_TESTING) AND BUILD_TESTS)
	enable_testing()
	add_subdirectory(tests)
endif()