cmake_minimum_required(VERSION 3.10)
project(GPR5300)

set (CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

if(WIN32)
add_compile_definitions(NOMINMAX)
endif(WIN32)

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_VISIBILITY_PRESET hidden)

set(EXTERNAL_DIR ${CMAKE_SOURCE_DIR}/externals)

include_directories(include ${CMAKE_SOURCE_DIR}/include)

file(GLOB_RECURSE SRC src/*.cpp include/*.h)

set_property(GLOBAL PROPERTY USE_FOLDERS On)

if(MSVC)
    add_compile_options(
        $<$<CONFIG:>:/MT> #---------|
        $<$<CONFIG:Debug>:/MTd> #---|-- Statically link the runtime libraries
        $<$<CONFIG:Release>:/MT> #--|
    )
endif()

if (${CMAKE_SYSTEM_NAME} STREQUAL "Emscripten")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s USE_SDL=2 -s FULL_ES3=1 --emrun -O2 --preload-file shaders/")
  set(SDL2_LIBRARIES "-s USE_SDL=2 -s FULL_ES3=1 --emrun")
  set(CMAKE_EXECUTABLE_SUFFIX ".html")
  set(USE_SDL2 ON CACHE BOOL  "Use SDL2")
  set(USE_EMSCRIPTEN true)
  set(USE_SFML2 OFF CACHE BOOL  "Compile for SFML2")

else()
#include either SFML2 or SDL2
  set(USE_SDL2 ON CACHE BOOL  "Use SDL2")
  set(USE_EMSCRIPTEN false)
  set(USE_SFML2 OFF CACHE BOOL  "Compile for SFML2")
endif()

if(USE_SFML2)
set(USE_SDL2 OFF)
endif()

if(USE_SFML2)

	#SFML
	set(SFML_DIR "${EXTERNAL_DIR}/SFML")
	set(SFML_BUILD_EXAMPLES FALSE)
	set(BUILD_SHARED_LIBS FALSE)
	set(SFML_STATIC_LIBRARIES TRUE)
	add_subdirectory(${SFML_DIR})
	include_directories(${SFML_DIR}/include ${SFML_DIR}/src)

	LIST(APPEND SFGE_LIBRARIES
			sfml-graphics sfml-window sfml-system)

	#glew
	set(GLEW_DIR ${EXTERNAL_DIR}/glew)
	add_compile_definitions(GLEW_STATIC)
	include_directories(${GLEW_DIR}/include)
	add_subdirectory(${GLEW_DIR})
	LIST(APPEND SFGE_LIBRARIES
			glew)
endif()

if(USE_SDL2)
	if(NOT USE_EMSCRIPTEN)
		#sdl2 
		set(SDL_VERSION 2.0.9)
		set(SDL_DIR ${EXTERNAL_DIR}/SDL2-${SDL_VERSION}/)
		set(SDL_STATIC ON)
		set(SDL_SHARED OFF)
		add_subdirectory(${SDL_DIR})
		include_directories(${SDL2_DIR}/include ${SDL2_DIR}/src)
		LIST(APPEND SFGE_LIBRARIES
				SDL2-static)
		if(MSVC)
			LIST(APPEND SFGE_LIBRARIES
				SDL2main)
		endif(MSVC)
		#opengl
		set(OpenGL_GL_PREFERENCE GLVND)
		find_package(OpenGL REQUIRED)
		include_directories(${OPENGL_INCLUDE_DIR})
		LIST(APPEND SFGE_LIBRARIES
				${OPENGL_LIBRARIES})
				
		#glew
		set(GLEW_DIR ${EXTERNAL_DIR}/glew)
		add_compile_definitions(GLEW_STATIC)
		include_directories(${GLEW_DIR}/include)
		add_subdirectory(${GLEW_DIR})
		LIST(APPEND SFGE_LIBRARIES
			glew)
			
	endif()
	
endif()
#gli
set(GLI_DIR ${EXTERNAL_DIR}/gli)
IF(NOT EXISTS ${GLI_DIR})
message(FATAL_ERROR "Please do git submodule update --init --recursive")
ENDIF()
include_directories(${GLI_DIR})
#glm
set(GLM_DIR ${EXTERNAL_DIR}/glm)
IF(NOT EXISTS ${GLM_DIR})
message(FATAL_ERROR "Please do git submodule update --init --recursive")
ENDIF()
include_directories(${GLM_DIR})
#SFML Imgui

set(IMGUI_ROOT ${EXTERNAL_DIR}/imgui/)
message("IMGUI ROOT: ${IMGUI_ROOT}")
if(USE_SFML2)
	set(SFML_IMGUI ${EXTERNAL_DIR}/imgui-sfml/)
	set(SFML_ROOT ${EXTERNAL_DIR}/SFML/)
	add_subdirectory(${SFML_IMGUI})
	include_directories(${IMGUI_ROOT} ${SFML_IMGUI})
	LIST(APPEND SFGE_LIBRARIES
		imgui-sfml)
endif()

if(USE_SDL2)
	set(SDL2_IMGUI ${EXTERNAL_DIR}/imgui-sdl/)
	add_subdirectory(${SDL2_IMGUI})
	include_directories(${IMGUI_ROOT} ${SDL2_IMGUI})
	LIST(APPEND SFGE_LIBRARIES
		imgui-sdl)

endif()


set(UTILITIES_DIR ${EXTERNAL_DIR}/utilities/)
include_directories(${UTILITIES_DIR})

#MISCELLANEOUS UTILITIES
if(NOT USE_EMSCRIPTEN)
set(REMOTERY_DIR ${EXTERNAL_DIR}/Remotery)
add_subdirectory(${REMOTERY_DIR})
include_directories(${REMOTERY_DIR})
LIST(APPEND SFGE_LIBRARIES Remotery)
endif()
#COMMON
message("SFGE LIBRARIES : ${SFGE_LIBRARIES}")
add_library(COMMON ${SRC})
target_link_libraries(COMMON PUBLIC ${SFGE_LIBRARIES})
set_property(TARGET COMMON PROPERTY CXX_STANDARD 17)

if(USE_SDL2)
target_compile_definitions(COMMON PUBLIC USE_SDL2=1)
endif()
if(USE_EMSCRIPTEN)
target_compile_definitions(COMMON PUBLIC USE_EMSCRIPTEN=1)
endif()
if(USE_SFML2)
target_compile_definitions(COMMON PUBLIC USE_SFML2=1)
endif()
#copy folder to build
add_custom_command(TARGET COMMON POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_SOURCE_DIR}/shaders ${CMAKE_BINARY_DIR}/shaders)
add_custom_command(TARGET COMMON POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_SOURCE_DIR}/data ${CMAKE_BINARY_DIR}/data)
file(COPY data/ DESTINATION ${CMAKE_BINARY_DIR}/data/)
file(COPY shaders/ DESTINATION ${CMAKE_BINARY_DIR}/shaders/)
#SFGE COURSES
SET(SFGE_COURSE_DIR ${CMAKE_SOURCE_DIR}/tests)
file(GLOB COURSE_FILES ${SFGE_COURSE_DIR}/*.cpp )
foreach(course_file ${COURSE_FILES} )
    # I used a simple string replace, to cut off .cpp.
    file(RELATIVE_PATH course_relative_path ${SFGE_COURSE_DIR} ${course_file} )
    string( REPLACE ".cpp" "" course_name ${course_relative_path} )
	file(GLOB_RECURSE SHADERS_SRC shaders/${course_name}/*.vert shaders/${course_name}/*.frag)
	source_group("Shaders" FILES ${SHADERS_SRC})

    add_executable(${course_name} ${SFGE_COURSE_DIR}/${course_relative_path} ${SHADERS_SRC})
    # Make sure YourLib is linked to each app
    target_link_libraries(${course_name} PUBLIC COMMON)
	set_property(TARGET ${course_name} PROPERTY CXX_STANDARD 17)
	IF(UNIX)
		set_target_properties(${course_name} PROPERTIES COMPILE_FLAGS "-save-temps -ffast-math")
	ENDIF(UNIX)
	set_target_properties (${course_name} PROPERTIES
    FOLDER Courses)
  IF(USE_EMSCRIPTEN)
    target_compile_options(${course_name} PUBLIC --emrun)
  ENDIF()
  add_custom_command(TARGET ${course_name} POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_SOURCE_DIR}/shaders ${CMAKE_BINARY_DIR}/shaders)
add_custom_command(TARGET ${course_name} POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_SOURCE_DIR}/data ${CMAKE_BINARY_DIR}/data)
endforeach()
