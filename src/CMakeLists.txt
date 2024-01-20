include(FetchContent)
enable_testing()

add_executable(letterpress)
target_sources(letterpress PRIVATE
	main.cpp
	letterpress/logging.cpp
	letterpress/parser.cpp
	letterpress/document/document.cpp
	letterpress/pdf/constants.cpp
	letterpress/pdf/font_descriptor.cpp
	letterpress/pdf/font.cpp
	letterpress/pdf/page.cpp
	letterpress/pdf/pdf.cpp
	letterpress/pdf/utils/content_stream_writer.cpp
	letterpress/pdf/utils/file_content_provider.cpp
	letterpress/pdf/utils/fontfile.cpp
	letterpress/pdf/utils/page_content_stream.cpp
	letterpress/pdf/utils/resources.cpp
	letterpress/scriptengine/context.cpp
	letterpress/scriptengine/module.cpp
	letterpress/scriptengine/scriptengine.cpp
)

target_compile_features(letterpress PUBLIC cxx_std_23)
target_include_directories(letterpress PUBLIC ${CMAKE_CURRENT_LIST_DIR}/../include)

##########################################################################################
# Libraries
##########################################################################################
# spdlog for logging
FetchContent_Declare(spdlog GIT_REPOSITORY https://github.com/gabime/spdlog.git GIT_TAG v1.11.0)
FetchContent_MakeAvailable(spdlog)
target_link_libraries(letterpress spdlog::spdlog $<$<BOOL:${MINGW}>:ws2_32>)

# Freetype
FetchContent_Declare(FREETYPE GIT_REPOSITORY https://gitlab.freedesktop.org/freetype/freetype.git GIT_TAG VER-2-12-1)
FetchContent_MakeAvailable(FREETYPE)
target_compile_definitions(freetype PRIVATE FT_CONFIG_OPTION_ERROR_STRINGS)
target_link_libraries(letterpress freetype)
target_include_directories(letterpress PUBLIC ${FREETYPE_SOURCE_DIR}/include)

# AngelScript
FetchContent_Declare(
	angelscript
	URL https://www.angelcode.com/angelscript/sdk/files/angelscript_2.36.0.zip
	TLS_VERIFY true
)
FetchContent_MakeAvailable(angelscript)
add_subdirectory(${angelscript_SOURCE_DIR}/angelscript/projects/cmake ${angelscript_BINARY_DIR})
message(${ANGELSCRIPT_LIBRARY_NAME})
target_link_libraries(letterpress ${ANGELSCRIPT_LIBRARY_NAME})
target_include_directories(letterpress PUBLIC ${angelscript_SOURCE_DIR}/angelscript/include)
# AngelScript's string add on
target_sources(letterpress PRIVATE
	${angelscript_SOURCE_DIR}/add_on/scriptbuilder/scriptbuilder.cpp
#	${angelscript_SOURCE_DIR}/add_on/scriptstdstring/scriptstdstring_utils.cpp
	${angelscript_SOURCE_DIR}/add_on/scriptstdstring/scriptstdstring.cpp
)
target_include_directories(letterpress PUBLIC ${angelscript_SOURCE_DIR}/add_on)


# Bison & Flex
# find_package(BISON 2.1.3)
# if(BISON_FOUND)
# 	BISON_TARGET(parser letterpress/parser/parser.yy ${CMAKE_CURRENT_BINARY_DIR}/parser_out/parser.cpp
#				DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/parser_out/parser.hpp)
# else()
# 	message(FATAL_ERROR "Bison not found -- please install bison")
# endif()
# find_package(FLEX)
# if(FLEX_FOUND)
# 	FLEX_TARGET(scanner letterpress/parser/lexer.ll ${CMAKE_CURRENT_BINARY_DIR}/parser_out/lexer.cpp
#				DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/parser_out/lexer.hpp)
# else()
# 	message(FATAL_ERROR "Flex not found -- please install flex")
# endif()
# ADD_FLEX_BISON_DEPENDENCY(scanner parser)
# target_include_directories(letterpress PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/parser_out)
# target_sources(letterpress PUBLIC ${BISON_parser_OUTPUTS} ${FLEX_scanner_OUTPUTS})
# target_link_libraries(letterpress ${FLEX_LIBRARIES})

# ANTLR4
set(ANTLR4CPP_JAR_LOCATION ${PROJECT_SOURCE_DIR}/thirdparty/antlr-4.13.0-complete.jar)
include(../cmake/ExternalAntlr4Cpp.cmake)
message(STATUS "Found antlr4cpp libs: ${ANTLR4CPP_LIBS} and includes: ${ANTLR4CPP_INCLUDE_DIRS} ")
target_include_directories(letterpress PUBLIC ${ANTLR4CPP_INCLUDE_DIRS})
target_link_directories(letterpress PUBLIC ${ANTLR4CPP_LIBS})
target_link_libraries(letterpress antlr4-runtime)
antlr4cpp_process_grammar(letterpress lp ${CMAKE_CURRENT_LIST_DIR}/letterpress/parser/lplexer.g4 ${CMAKE_CURRENT_LIST_DIR}/letterpress/parser/lpparser.g4)
# antlr4cpp_process_grammar(letterpress lp ${CMAKE_CURRENT_LIST_DIR}/letterpress/parser/lpparser.g4)
add_dependencies(letterpress antlr4cpp antlr4cpp_generation_lp)
target_include_directories(letterpress PUBLIC ${antlr4cpp_include_dirs_lp})
target_sources(letterpress PUBLIC ${antlr4cpp_src_files_lp})

# download qpdf cmake project and run (https://qpdf.readthedocs.io/en/stable/installation.html#installation-and-packaging)
# cmake -S . -B build -DCMAKE_BUILD_TYPE=RelWithDebInfo
# cmake --build build --parallel --target libqpdf libqpdf_static
# cmake --install build --component lib
# cmake --install build --component dev
#find_package(qpdf REQUIRED)
#target_link_libraries(letterpress qpdf::libqpdf)

FetchContent_Declare(QPDF GIT_REPOSITORY https://github.com/qpdf/qpdf.git GIT_TAG v11.6.0)
FetchContent_MakeAvailable(QPDF)
target_link_libraries(letterpress libqpdf)
target_include_directories(letterpress PUBLIC ${QPDF_SOURCE_DIR}/include)

# CLI11
FetchContent_Declare(cli11 GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git GIT_TAG v2.3.2)
FetchContent_MakeAvailable(cli11)
target_link_libraries(letterpress CLI11::CLI11)