#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

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

    // Parse command line arguments
    CLI::App args("Simple image mathiching utility.");

    path dataset{ current_path() };
    args
      .add_option(
        "-d,--dataset", dataset, "Path to the directory containing images.")
      ->required()
      ->check(CLI::ExistingDirectory);

    path input_image_path;
    args
      .add_option("image_path", input_image_path, "Path of image to compare.")
      ->required()
      ->check(CLI::ExistingFile);

    bool output_json{ false };
    args.add_option("--json", output_json, "Output data in JSON format.");

    int matches_num{ 10 };
    args.add_option("-n,--number-of-matches",
                    matches_num,
                    "Number of matches to display, -1 for all. (default: 10)");

    CLI11_PARSE(args, argc, argv);

    // 

    SPDLOG_DEBUG("store_path={}", dataset.string());
    SPDLOG_DEBUG("input_image_path={}", input_image_path.string());
    SPDLOG_DEBUG("output_json={}", output_json);
    SPDLOG_DEBUG("matches_num={}", matches_num);

    return EXIT_SUCCESS;
}