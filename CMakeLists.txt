cmake_minimum_required(VERSION 3.18)

# Общие настройки
function(configure_target target)
    # Установка опций компилятора
    if(MSVC)
        target_compile_options(${target} PUBLIC /W4)
        if(CMAKE_BUILD_TYPE STREQUAL "Release")
            target_compile_options(${target} PUBLIC /WX)
        endif()
    else()
        target_compile_options(${target} PUBLIC -Wall -Wextra -Wpedantic)
        if(CMAKE_BUILD_TYPE STREQUAL "Release")
            target_compile_options(${target} PUBLIC -Werror -flto)
        endif()
    endif()

    # Запуск статического анализа
    if(PVS_CHECK)
        pvs_studio_add_target(TARGET analyze ALL
                          OUTPUT
                          FORMAT tasklist
                          ANALYZE ${target}
                          MODE GA:1,2,3;OP:1,2,3
                          LOG PVS_LOG )
    endif()
endfunction()

project(cde_labs VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Запись данных о сборке
if(EXISTS "${CMAKE_SOURCE_DIR}/.git")
    execute_process(
        COMMAND git log -1 --format=%h
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
        OUTPUT_VARIABLE GIT_HASH
        OUTPUT_STRIP_TRAILING_WHITESPACE)
    execute_process(
        COMMAND git branch --show-current
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
        OUTPUT_VARIABLE GIT_BRANCH
        OUTPUT_STRIP_TRAILING_WHITESPACE)
    string(CONCAT GIT_DATA "git: " ${GIT_BRANCH} "/" ${GIT_HASH})
else()
    set(GIT_DATA "git: unknown")
endif()
configure_file(src/build.h.in ${cde_labs_SOURCE_DIR}/src/build.h)

# Выбор работ
option(PVS_CHECK "Perform static analysis with PVS-Studio" ON)
option(BUILD_LAB1 "Build lab no.1" ON)

if(PVS_CHECK)
    include(FetchContent)
    FetchContent_Declare(PVS_CMakeModule
        GIT_REPOSITORY "https://github.com/viva64/pvs-studio-cmake-module.git"
        GIT_TAG        "master")
    FetchContent_MakeAvailable(PVS_CMakeModule)
    include("${pvs_cmakemodule_SOURCE_DIR}/PVS-Studio.cmake")
endif()

if(BUILD_LAB1)
    add_executable(lab1 src/lab1/main.cpp src/build.h)
    configure_target(lab1)
endif()