cmake_minimum_required(VERSION 3.10)

if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

# Setup the project
project(image_match
    VERSION 1.0
    DESCRIPTION "Image match"    # DESCRIPTION added in CMake 3.9
    LANGUAGES CXX)

add_compile_options(-Wall -Wextra -pedantic)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Add cmake dir to path
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

# Download all submodules
find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
    option(GIT_SUBMODULE "Check submodules during build" ON)
    if(GIT_SUBMODULE)
        message(STATUS "Submodule update")
        execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                        RESULT_VARIABLE GIT_SUBMOD_RESULT)
        if(NOT GIT_SUBMOD_RESULT EQUAL "0")
            message(FATAL_ERROR
                "git submodule update --init failed \
                 with ${GIT_SUBMOD_RESULT}, please checkout submodules"
                 )
        endif()
    endif()
endif()

# Check submodules exist
if(NOT EXISTS "${PROJECT_SOURCE_DIR}/third-party/spdlog/CMakeLists.txt")
    message(FATAL_ERROR
        "The spdlog submodule was not downloaded! \
        GIT_SUBMODULE was turned off or failed. \
        Please update submodules and try again.")
endif()

if(NOT EXISTS "${PROJECT_SOURCE_DIR}/third-party/stb/stb_image.h")
    message(FATAL_ERROR
        "The stb submodule was not downloaded! \
        GIT_SUBMODULE was turned off or failed. \
        Please update submodules and try again.")
endif()

if(NOT EXISTS "${PROJECT_SOURCE_DIR}/third-party/cli11/CMakeLists.txt")
    message(FATAL_ERROR
        "The CLI11 submodule was not downloaded! \
        GIT_SUBMODULE was turned off or failed. \
        Please update submodules and try again.")
endif()


# Add the spdlog library
add_subdirectory(third-party/spdlog)
if(NOT TARGET spdlog)
    # Stand-alone build
    find_package(spdlog REQUIRED)
endif()

# Add stb library
if(NOT TARGET stb)
    add_library(stb INTERFACE IMPORTED GLOBAL)
    target_sources(stb INTERFACE
        "${CMAKE_CURRENT_SOURCE_DIR}/third-party/stb/stb_image.h")
    target_include_directories(stb INTERFACE
        "${CMAKE_CURRENT_SOURCE_DIR}/third-party/stb")
endif()

# Add cli library
add_subdirectory(third-party/cli11)
if(NOT TARGET CLI11)
    add_library(CLI11 INTERFACE IMPORTED GLOBAL)
    target_sources(CLI11 INTERFACE
        "${CMAKE_CURRENT_SOURCE_DIR}/third-party/cli11/include")
    target_include_directories(cli11 INTERFACE
        "${CMAKE_CURRENT_SOURCE_DIR}/third-party/cli11/include")
endif()


add_subdirectory(src)
add_subdirectory(apps)
