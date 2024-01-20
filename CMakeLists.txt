cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

######################################################
# READ VERSION NUMBER FROM ./include/letterpress.hpp #
######################################################
file(STRINGS "${CMAKE_CURRENT_LIST_DIR}/include/letterpress/letterpress.hpp" LETTERPRESS_VERSION_MACRO REGEX "^\tconstexpr const auto version = \"(([0-9]+\\.)*[0-9]+)\";$")
string(REGEX REPLACE "^\tconstexpr const auto version = \"(([0-9]+\\.)*[0-9]+)\"\\;$" "\\1" LETTERPRESS_VERSION ${LETTERPRESS_VERSION_MACRO})

project(letterpress VERSION ${LETTERPRESS_VERSION} LANGUAGES CXX)

message("Compiling Letterpress v${LETTERPRESS_VERSION}")


set(CMAKE_INSTALL_BINDIR "${CMAKE_CURRENT_LIST_DIR}/bin/")
add_subdirectory(src)