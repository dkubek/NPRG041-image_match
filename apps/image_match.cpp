#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <filesystem>

#ifndef NDEBUG
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#endif
#include "spdlog/spdlog.h"

#include "CLI/CLI.hpp"

using namespace std::filesystem;

int
main(int argc, char* argv[])
{
    // Set the default global log level to debug
    spdlog::set_level(spdlog::level::debug);

    CLI::App args("Simple image mathiching utility.");

    path store_path;
    args.add_option("-s,--store", store_path, "Path to the image store.")
      ->required()
      ->check(CLI::ExistingDirectory);


    CLI11_PARSE(args, argc, argv);

    SPDLOG_DEBUG("store_path={}", store_path.string());

    return EXIT_SUCCESS;
}
