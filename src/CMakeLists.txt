include(FetchContent)
enable_testing()

add_library(letterpress)
target_sources(letterpress
	PRIVATE
		letterpress/logging.cpp
		letterpress/parser.cpp
		letterpress/document/boxes.cpp
		letterpress/document/document.cpp
		letterpress/document/hyphenation.cpp
		letterpress/parser/lpparser.cpp
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
		letterpress/scriptengine/method.cpp
		letterpress/scriptengine/module.cpp
		letterpress/scriptengine/object.cpp
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
set(BUILD_SHARED_LIBS OFF)

# spdlog for logging
CPMAddPackage(URI "gh:gabime/spdlog@1.14.1"
	EXCLUDE_FROM_ALL YES
	# OPTIONS "SPDLOG_USE_STD_FORMAT ON"
)
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
	URL https://www.angelcode.com/angelscript/sdk/files/angelscript_2.37.0.zip
	TLS_VERIFY true
)
FetchContent_MakeAvailable(angelscript)
add_subdirectory(${angelscript_SOURCE_DIR}/angelscript/projects/cmake ${angelscript_BINARY_DIR})
target_link_libraries(letterpress ${ANGELSCRIPT_LIBRARY_NAME})
target_include_directories(letterpress PUBLIC ${angelscript_SOURCE_DIR}/angelscript/include)
# AngelScript's string add on
target_sources(letterpress PRIVATE
	${angelscript_SOURCE_DIR}/add_on/scriptbuilder/scriptbuilder.cpp
#	${angelscript_SOURCE_DIR}/add_on/scriptstdstring/scriptstdstring_utils.cpp
	${angelscript_SOURCE_DIR}/add_on/scriptstdstring/scriptstdstring.cpp
	${angelscript_SOURCE_DIR}/add_on/scriptdictionary/scriptdictionary.cpp
	${angelscript_SOURCE_DIR}/add_on/scriptarray/scriptarray.cpp  # Required by scriptdictionary
)
target_include_directories(letterpress PUBLIC ${angelscript_SOURCE_DIR}/add_on)

# QPDF
CPMAddPackage(URI "gh:qpdf/qpdf@11.9.1" EXCLUDE_FROM_ALL YES)
target_link_libraries(letterpress libqpdf)
target_include_directories(letterpress PUBLIC ${qpdf_SOURCE_DIR}/include)

# CLI11
CPMAddPackage(URI "gh:CLIUtils/CLI11@2.4.2" EXCLUDE_FROM_ALL YES)
target_link_libraries(letterpressexe CLI11::CLI11)