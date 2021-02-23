add_executable(csd csd.cpp)
target_link_libraries(csd
    PRIVATE CLI11
    PRIVATE spdlog::spdlog_header_only
    PRIVATE image
    )
