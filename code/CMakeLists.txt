cmake_minimum_required(VERSION 3.25.0)
project(age VERSION 0.1.0)

include(CTest)
enable_testing()
# Windows Build
if(MSVC)
    if("${CMAKE_SIZEOF_VOID_P}" STREQUAL "8")
        add_executable(w64 w64_build.cpp ../externals/glad/src/glad.c)
    else()
     message("Not supported.")
    endif()

#TODO Add others
    # TODO: Find a way to identify windows platform but still use other compilers.
    target_compile_definitions(w64 PRIVATE _CRT_SECURE_NO_WARNINGS=1)
    target_compile_options(w64 PRIVATE -MTd -nologo -fp:fast -GR -EHa -EHsc -Zo -W4 -FC -Z7 -wd4100 -wd5246 -wd4201)
    target_include_directories(w64 PRIVATE ../externals/glad/include)
endif()

# DLL Build
add_library(game SHARED game_build.cpp)
target_compile_options(game PRIVATE -DLL -MTd -nologo -fp:fast -GR -EHa -EHsc -Zo -W4 -FC -Z7 -wd4100 -wd5246 -wd4201)

set_property(
    TARGET game
    PROPERTY PDB_NAME "game-$([System.DateTime]::Now.ToString(\"HH_mm_ss_fff\"))"
)
target_link_options(game PRIVATE -EXPORT:GameInit -EXPORT:GameUpdate -EXPORT:GameShutdown -incremental:no)
add_custom_command(
        TARGET game
        PRE_BUILD
        COMMAND del Debug\\game-*.pdb > NUL 2> NUL)
add_custom_command(
    TARGET game
    PRE_BUILD
    COMMAND echo lock > Debug\\pdb.lock)
add_custom_command(
    TARGET game
    POST_BUILD
    COMMAND del Debug\\pdb.lock)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
