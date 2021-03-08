# Add the spdlog library
add_library(spdlog INTERFACE IMPORTED GLOBAL)
target_sources(spdlog INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/spdlog")
target_include_directories(spdlog INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/spdlog")

# Add stb library
add_library(stb INTERFACE IMPORTED GLOBAL)
target_sources(stb INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/stb")
target_include_directories(stb INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/stb")

# Add cli library
add_library(CLI11 INTERFACE IMPORTED GLOBAL)
target_sources(CLI11 INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/CLI11")
target_include_directories(CLI11 INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/CLI11")

# Add gsl-lite
add_library( gsl INTERFACE )
target_sources(gsl INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/gsl-lite")
target_include_directories(gsl INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/gsl-lite")

# Add rapidjson
add_library( RapidJSON INTERFACE )
target_sources(RapidJSON INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/rapidjson")
target_include_directories(RapidJSON INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/rapidjson")
