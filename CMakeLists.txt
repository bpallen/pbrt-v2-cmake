project(pbrt-v2-cmake CXX C)
cmake_minimum_required(VERSION 2.8)

# tiff seems not to be used in the renderer itself, so is excluded from this cmake

# compiler flags 
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
	# Force to always compile with W3
	if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
		string(REGEX REPLACE "/W[0-4]" "/W3" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
	else()
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3")
	endif()
	# turn on parallel build
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
	
	# fix some shit?
	add_definitions(
		-DZLIB_WINAPI
		-D_CRT_SECURE_NO_WARNINGS
	)
	
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -Wall -Werror=return-type -pedantic -std=c++11 -g")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -Wall -Werror=return-type -pedantic -std=c++11 -g")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")
endif()

# output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

# glob me some header files
file(GLOB ilmbase_hdr "src/3rdparty/ilmbase-1.0.2/*.h")
file(GLOB openexr_hdr "src/3rdparty/openexr-1.7.0/*.h")
#file(GLOB tiff_hdr "src/3rdparty/tiff-3.9.4/*.h")
file(GLOB zlib_hdr "src/3rdparty/zlib-1.2.5/*.h")
file(GLOB accelerators_hdr "src/accelerators/*.h")
file(GLOB cameras_hdr "src/cameras/*.h")
file(GLOB core_hdr "src/core/*.h" "src/core/*.hh") # *.hh is only used for the parser header
file(GLOB film_hdr "src/film/*.h")
file(GLOB filters_hdr "src/filters/*.h")
file(GLOB integrators_hdr "src/integrators/*.h")
file(GLOB lights_hdr "src/lights/*.h")
file(GLOB materials_hdr "src/materials/*.h")
file(GLOB renderers_hdr "src/renderers/*.h")
file(GLOB samplers_hdr "src/samplers/*.h")
file(GLOB shapes_hdr "src/shapes/*.h")
file(GLOB textures_hdr "src/textures/*.h")
file(GLOB volumes_hdr "src/volumes/*.h")

# and some source files
file(GLOB ilmbase_src "src/3rdparty/ilmbase-1.0.2/*.cpp")
file(GLOB openexr_src "src/3rdparty/openexr-1.7.0/*.cpp")
#file(GLOB tiff_src "src/3rdparty/tiff-3.9.4/*.c")
file(GLOB zlib_src "src/3rdparty/zlib-1.2.5/*.c")
file(GLOB accelerators_src "src/accelerators/*.cpp")
file(GLOB cameras_src "src/cameras/*.cpp")
# *.yy is the parser generator source, *.ll is the lexer
file(GLOB core_src "src/core/*.cpp" "src/core/*.c" "src/core/*.yy" "src/core/*.ll")
file(GLOB film_src "src/film/*.cpp")
file(GLOB filters_src "src/filters/*.cpp")
file(GLOB integrators_src "src/integrators/*.cpp")
file(GLOB lights_src "src/lights/*.cpp")
file(GLOB materials_src "src/materials/*.cpp")
file(GLOB renderers_src "src/renderers/*.cpp")
file(GLOB samplers_src "src/samplers/*.cpp")
file(GLOB shapes_src "src/shapes/*.cpp")
file(GLOB textures_src "src/textures/*.cpp")
file(GLOB volumes_src "src/volumes/*.cpp")

# includes
include_directories(
	"src"
	"src/core"
	"src/3rdparty/ilmbase-1.0.2"
	"src/3rdparty/openexr-1.7.0"
	"src/3rdparty/zlib-1.2.5"
)

if(WIN32)
	# for unistd.h replacement
	include_directories("src/3rdparty")
endif()

# preprocessor definitions
add_definitions(
	-DPBRT_HAS_OPENEXR
	-DPBRT_PROBES_NONE
)

# parser/lexer generator
find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)
# TODO nice way to not to require bison/flex
if(BISON_FOUND AND FLEX_FOUND)
	# the line directives produced use ABSOLUTE paths atm
	# bear this in mind if trying to make bison/flex optional
	# target to build parser cpp and header
	bison_target(
		pbrtparse 
		"${PROJECT_SOURCE_DIR}/src/core/pbrtparse.yy" 
		"${PROJECT_BINARY_DIR}/src/core/pbrtparse.cpp"
		COMPILE_FLAGS "--debug"
	)
	# target to build lexer cpp
	flex_target(
		pbrtlex
		"${PROJECT_SOURCE_DIR}/src/core/pbrtlex.ll"
		"${PROJECT_BINARY_DIR}/src/core/pbrtlex.cpp"
#		COMPILE_FLAGS "-d" # this results in debug spam of some description
	)
	# add generated parser and lexer to source list
	set(core_src ${core_src} ${BISON_pbrtparse_OUTPUT_SOURCE} ${FLEX_pbrtlex_OUTPUTS})
	set(core_hdr ${core_hdr} "${PROJECT_BINARY_DIR}/src/core/pbrtparse.hh")
	# add generated source dir to include dirs
	include_directories("${PROJECT_BINARY_DIR}/src/core")
	# add the flex dependency on the bison generated header
	add_flex_bison_dependency(pbrtlex pbrtparse)
	# copy the generated header to one of a determinate name (also the includes are like that)
	add_custom_command(
		OUTPUT "${PROJECT_BINARY_DIR}/src/core/pbrtparse.hh"
		COMMAND ${CMAKE_COMMAND} -E	copy ${BISON_pbrtparse_OUTPUT_HEADER} "${PROJECT_BINARY_DIR}/src/core/pbrtparse.hh"
		DEPENDS ${BISON_pbrtparse_OUTPUT_HEADER}
	)
endif()

# libpbrt
add_library(libpbrt
	${ilmbase_hdr} ${ilmbase_src}
	${openexr_hdr} ${openexr_src}
	${tiff_hdr} ${tiff_src}
	${zlib_hdr} ${zlib_src}
	${accelerators_hdr} ${accelerators_src}
	${cameras_hdr} ${cameras_src}
	${core_hdr} ${core_src}
	${film_hdr} ${film_src}
	${filters_hdr} ${filters_src}
	${integrators_hdr} ${integrators_src}
	${lights_hdr} ${lights_src}
	${materials_hdr} ${materials_src}
	${renderers_hdr} ${renderers_src}
	${samplers_hdr} ${samplers_src}
	${shapes_hdr} ${shapes_src}
	${textures_hdr} ${textures_src}
	${volumes_hdr} ${volumes_src}
)

# main pbrt executable
add_executable(pbrt "src/main/pbrt.cpp")
target_link_libraries(pbrt libpbrt)

# set source groups
source_group("3rdparty\\ilmbase" FILES ${ilmbase_hdr} ${ilmbase_src})
source_group("3rdparty\\openexr" FILES ${openexr_hdr} ${openexr_src})
#source_group("3rdparty\\tiff" FILES ${tiff_hdr} ${tiff_src})
source_group("3rdparty\\zlib" FILES ${zlib_hdr} ${zlib_src})
source_group("accelerators" FILES ${accelerators_hdr} ${accelerators_src})
source_group("cameras" FILES ${cameras_hdr} ${cameras_src})
source_group("core" FILES ${core_hdr} ${core_src})
source_group("film" FILES ${film_hdr} ${film_src})
source_group("filters" FILES ${filters_hdr} ${filters_src})
source_group("integrators" FILES ${integrators_hdr} ${integrators_src})
source_group("lights" FILES ${lights_hdr} ${lights_src})
source_group("materials" FILES ${materials_hdr} ${materials_src})
source_group("renderers" FILES ${renderers_hdr} ${renderers_src})
source_group("samplers" FILES ${samplers_hdr} ${samplers_src})
source_group("shapes" FILES ${shapes_hdr} ${shapes_src})
source_group("textures" FILES ${textures_hdr} ${textures_src})
source_group("volumes" FILES ${volumes_hdr} ${volumes_src})


































