cmake_minimum_required(VERSION 3.11)

if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

# Setup the project
project(image_match
    VERSION 1.0
    DESCRIPTION "Image match"    # DESCRIPTION added in CMake 3.9
    LANGUAGES CXX)

#add_compile_options(-Wall -Wextra -pedantic -Werror)
add_compile_options("$<$<CONFIG:RELEASE>:-W;-Wall;-Os;-pedantic;-ffunction-sections;-fdata-sections>")
add_compile_options("$<$<CONFIG:DEBUG>:-W;-Wall;-O0;-g;-pedantic>")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(third-party)
add_subdirectory(src)
add_subdirectory(apps)
