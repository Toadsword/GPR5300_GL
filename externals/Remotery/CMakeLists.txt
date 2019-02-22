project(Remotery)


include_directories(.)

add_compile_definitions(RMT_USE_OPENGL)

if(APPLE)
    add_library(Remotery STATIC Remotery.c RemoteryMetal.mm)
else()
    add_library(Remotery STATIC Remotery.c)
    if(UNIX)
        target_link_libraries(Remotery dl m pthread)
    endif(UNIX)
endif(APPLE)