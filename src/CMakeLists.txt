include(FetchContent)
enable_testing()

add_library(letterpress)
target_sources(letterpress PRIVATE
	letterpress/logging.cpp
	letterpress/parser.cpp
	letterpress/document/boxes.cpp
	letterpress/document/document.cpp
	letterpress/document/hyphenation.cpp
	letterpress/parser/parser.cpp
	letterpress/pdf/constants.cpp
	letterpress/pdf/font_descriptor.cpp
	letterpress/pdf/font.cpp
	letterpress/pdf/page.cpp
	letterpress/pdf/pdf.cpp
	letterpress/pdf/pdfdriver.cpp
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

add_executable(letterpressexe)
target_sources(letterpressexe PRIVATE
	main.cpp
)
target_compile_features(letterpressexe PUBLIC cxx_std_23)
target_link_libraries(letterpressexe letterpress)

##########################################################################################
# Libraries
##########################################################################################
# spdlog for logging
FetchContent_Declare(spdlog GIT_REPOSITORY https://github.com/gabime/spdlog.git GIT_TAG v1.14.1)
FetchContent_MakeAvailable(spdlog)
target_link_libraries(letterpress spdlog::spdlog $<$<BOOL:${MINGW}>:ws2_32>)

# Freetype
FetchContent_Declare(FREETYPE GIT_REPOSITORY https://gitlab.freedesktop.org/freetype/freetype.git GIT_TAG VER-2-13-2)
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

# ANTLR4
# required if linking to static library
add_definitions(-DANTLR4CPP_STATIC)
# using /MD flag for antlr4_runtime (for Visual C++ compilers only)
set(ANTLR4_WITH_STATIC_CRT OFF)
set(ANTLR_EXECUTABLE ${PROJECT_SOURCE_DIR}/thirdparty/antlr-4.13.0-complete.jar)
include(ExternalAntlr4Cpp)
target_include_directories(letterpress PUBLIC ${ANTLR4CPP_INCLUDE_DIRS})

find_package(ANTLR REQUIRED)

antlr_target(LPGrammarLexer ${CMAKE_CURRENT_LIST_DIR}/letterpress/parser/lplexer.g4 LEXER PACKAGE lp)
antlr_target(LPGrammarParser ${CMAKE_CURRENT_LIST_DIR}/letterpress/parser/lpparser.g4 PARSER
			 VISITOR
             PACKAGE lp
             DEPENDS_ANTLR LPGrammarLexer
             COMPILE_FLAGS -lib ${ANTLR_LPGrammarLexer_OUTPUT_DIR})

target_link_libraries(letterpress antlr4_static)
target_include_directories(letterpress PUBLIC ${ANTLR_LPGrammarLexer_OUTPUT_DIR} ${ANTLR_LPGrammarParser_OUTPUT_DIR})
target_sources(letterpress PUBLIC ${ANTLR_LPGrammarLexer_CXX_OUTPUTS} ${ANTLR_LPGrammarParser_CXX_OUTPUTS})

# QPDF
FetchContent_Declare(QPDF GIT_REPOSITORY https://github.com/qpdf/qpdf.git GIT_TAG v11.9.1)
FetchContent_MakeAvailable(QPDF)
target_link_libraries(letterpress libqpdf)
target_include_directories(letterpress PUBLIC ${QPDF_SOURCE_DIR}/include)

# CLI11
FetchContent_Declare(cli11 GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git GIT_TAG v2.4.2)
FetchContent_MakeAvailable(cli11)
target_link_libraries(letterpressexe CLI11::CLI11)