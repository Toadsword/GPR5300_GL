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


#SFML
set(SFML_DIR ${EXTERNAL_DIR}/SFML)
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
#gli
set(GLI_DIR ${EXTERNAL_DIR}/gli)
include_directories(${GLI_DIR})
#glm
set(GLM_DIR ${EXTERNAL_DIR}/glm)
include_directories(${GLM_DIR})
#SFML Imgui
set(SFML_IMGUI ${EXTERNAL_DIR}/imgui-sfml/)
set(IMGUI_ROOT ${EXTERNAL_DIR}/imgui/)

set(SFML_ROOT ${EXTERNAL_DIR}/SFML/)
add_subdirectory(${SFML_IMGUI})
include_directories(${IMGUI_ROOT} ${SFML_IMGUI})
LIST(APPEND SFGE_LIBRARIES
	imgui-sfml)


if(APPLE)
	set(boost_min_ver 1.50.0)
	set(boost_libs system filesystem)
	find_package(Boost ${boost_min_ver})

	if(Boost_FOUND)
		find_package(Boost ${boost_min_ver} COMPONENTS ${boost_libs})
	endif()
	include_directories(${Boost_INCLUDE_DIRS})
	LIST(APPEND SFGE_LIBRARIES
	  ${Boost_LIBRARIES})
endif(APPLE)
set(UTILITIES_DIR ${EXTERNAL_DIR}/utilities/)
include_directories(${UTILITIES_DIR})

#MISCELLANEOUS UTILITIES
if(UNIX AND NOT APPLE)
	check_include_file(uuid/uuid.h HAVE_UUID_UUID_H)
	check_library_exists(uuid uuid_generate "/lib;/usr/lib;/usr/local/lib" HAVE_UUID_LIB)

	if (HAVE_UUID_LIB)
		set(CMAKE_REQUIRED_LIBRARIES "uuid;${CMAKE_REQUIRED_LIBRARIES}")
	endif (HAVE_UUID_LIB)
endif()


set(REMOTERY_DIR ${EXTERNAL_DIR}/Remotery)
add_subdirectory(${REMOTERY_DIR})
include_directories(${REMOTERY_DIR})
LIST(APPEND SFGE_LIBRARIES Remotery)

#COMMON

#shaders
file(GLOB_RECURSE SHADERS_SRC shaders/*.glsl)
source_group("Shaders" FILES ${SHADERS_SRC})
LIST(APPEND SFGE_LIBRARIES)
if(UNIX AND NOT APPLE)
	LIST(APPEND SFGE_LIBRARIES
		stdc++fs)
ENDIF()

message("SFGE LIBRARIES : ${SFGE_LIBRARIES}")
add_library(COMMON ${SRC} ${SHADERS_SRC})
target_link_libraries(COMMON PUBLIC ${SFGE_LIBRARIES})
set_property(TARGET COMMON PROPERTY CXX_STANDARD 17)

if(APPLE)
	set_target_properties(COMMON PROPERTIES
		ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}
		ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR})
ENDIF()

#copy folder to build
add_custom_command(TARGET COMMON PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_SOURCE_DIR}/shaders ${CMAKE_BINARY_DIR}/shaders)
add_custom_command(TARGET COMMON PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_SOURCE_DIR}/data ${CMAKE_BINARY_DIR}/data)

#SFGE COURSES
SET(SFGE_COURSE_DIR ${CMAKE_SOURCE_DIR}/tests)
file(GLOB COURSE_FILES ${SFGE_COURSE_DIR}/*.cpp )
foreach(course_file ${COURSE_FILES} )
    # I used a simple string replace, to cut off .cpp.
    file(RELATIVE_PATH course_relative_path ${SFGE_COURSE_DIR} ${course_file} )
    string( REPLACE ".cpp" "" course_name ${course_relative_path} )
    add_executable(${course_name} ${SFGE_COURSE_DIR}/${course_relative_path})
    # Make sure YourLib is linked to each app
    target_link_libraries(${course_name} PUBLIC COMMON)
	set_property(TARGET ${course_name} PROPERTY CXX_STANDARD 17)
	IF(UNIX)
		set_target_properties(${course_name} PROPERTIES COMPILE_FLAGS "-save-temps -arch=native")
	ENDIF(UNIX)
	set_target_properties (${course_name} PROPERTIES
    FOLDER Courses)
  IF(APPLE)
    set_target_properties(${course_name} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR})
  ENDIF()
endforeach()
