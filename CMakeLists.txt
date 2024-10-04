cmake_minimum_required(VERSION 3.28)
project(Quadtree LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.x
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

add_executable(Quadtree 
    src/vec2.h
    src/circle.h
    src/quad.h
    src/Quadtree.h
    src/main.cpp)
target_link_libraries(Quadtree PRIVATE sfml-graphics)
target_compile_features(Quadtree PRIVATE cxx_std_17)

if(WIN32)
    add_custom_command(
        TARGET Quadtree
        COMMENT "Copy OpenAL DLL"
        PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SFML_SOURCE_DIR}/extlibs/bin/$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,x64,x86>/openal32.dll $<TARGET_FILE_DIR:Quadtree>
        VERBATIM)
endif()
