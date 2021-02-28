#include "CLI/Error.hpp"
#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <queue>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <vector>

#ifndef NDEBUG
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#endif
#include "spdlog/spdlog.h"

#include "CLI/CLI.hpp"

#define RAPIDJSON_HAS_STDSTRING 1
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/writer.h>

#include "image_match/csd.hpp"
#include "image_match/image.hpp"

using namespace std::filesystem;
using namespace rapidjson;

const std::vector<std::string> CSD_TYPES = { "32", "64", "128", "256" };
const std::vector<std::string> DB_FILES = { "csd_32.json",
                                            "csd_64.json",
                                            "csd_128.json",
                                            "csd_256.json" };

struct config
{
    path dataset{ current_path() };
    path input_image_path;
    int matches_num{ 10 };
    int type;
    bool quiet_mode{ false };
    bool output_json{ false };
    bool force_regenerate{ false };
};

config app;

void
load_databse(path& db_file, Document& doc)
{
    spdlog::info("Loading database {} ...", db_file.string());

    if (!is_regular_file(db_file)) {
        spdlog::info("Did not find a database file.");
        return;
    }

    std::ifstream ifs{ db_file };
    if (!ifs) {
        auto msg = "File read error! Could not read database!";
        throw std::runtime_error(msg);
    }

    IStreamWrapper isw{ ifs };
    doc.ParseStream(isw);
}

path
database_filename()
{
    std::ostringstream oss;
    oss << "csd_" << app.type << ".json";
    auto filename = absolute(app.dataset / oss.str()).lexically_normal();

    SPDLOG_DEBUG("Database filename: {}", filename.string());

    return filename;
}

std::unordered_set<std::string>
already_generated_descriptors(const Document& doc)
{
    std::unordered_set<std::string> already_generated;
    for (auto&& v : doc.GetArray()) {
        if (!v.HasMember("path") || v["path"].IsString())
            throw std::runtime_error(
              "Bad database format! \"path\" field missing or invalid!");

        SPDLOG_DEBUG("Adding: {}", v["path"].GetString());
        already_generated.insert(v["path"].GetString());
    }

    return already_generated;
}

std::vector<path>
get_new_images(const std::unordered_set<std::string>& generated)
{
    std::vector<path> image_paths;
    for (auto&& ip : image_match::get_image_paths(app.dataset)) {
        auto norm = absolute(ip).lexically_normal();
        if (generated.find(norm.string()) == generated.end()) {
            image_paths.push_back(norm);
        }
    }

    return image_paths;
}

void
generate_descriptors(const std::vector<path>& ips, Document& doc)
{
    spdlog::info("Generating descriptors...");

    Document::AllocatorType& alloc = doc.GetAllocator();
    size_t count = 0;
    size_t image_count = ips.size();
    for (auto&& ip : ips) {
        spdlog::info("({}/{}) {}", count, image_count, ip.string());

        image_match::image im{ ip };
        if (!im) {
            spdlog::warn(
              "Error reading {}! {}. Skipping.", ip.string(), im.fail_msg());
            continue;
        }

        auto descriptor =
          image_match::CSD(im, image_match::csd_from_int(app.type));

        // Add descriptor to DOM
        Value new_desc(kObjectType);
        new_desc.AddMember("path", absolute(ip).string(), alloc);
        new_desc.AddMember("descriptor", Value().SetArray(), alloc);

        for (auto&& v : descriptor.data)
            new_desc["descriptor"].PushBack(v, alloc);

        doc.PushBack(new_desc, alloc);

        ++count;
    }

    spdlog::info("Descriptors generated");
}

void
write_descriptors(const path& db_file, Document& doc)
{
    spdlog::info("Writing database...");

    std::ofstream ofs{ db_file };
    if (!ofs) {
        auto msg = "File write error! Could not write database!";
        throw std::runtime_error(msg);
    }

    OStreamWrapper osw(ofs);
    Writer<OStreamWrapper> writer(osw);
    doc.Accept(writer);

    spdlog::info("Database saved successfully.");
}

void
run_generate_subcommand()
{
    SPDLOG_DEBUG("Running generate subcommand.");

    path db_file = database_filename();

    Document descriptors;
    descriptors.SetArray();

    if (!app.force_regenerate)
        load_databse(db_file, descriptors);

    auto already_generated = already_generated_descriptors(descriptors);
    auto image_paths = get_new_images(already_generated);

    generate_descriptors(image_paths, descriptors);
    write_descriptors(db_file, descriptors);
}

bool
is_db_entry_valid(const Value& entry)
{
    if (!entry.IsObject() || !entry.HasMember("path") ||
        !entry.HasMember("descriptor"))
        return false;

    return entry["descriptor"].GetArray().Size() == app.type;
}

std::vector<std::pair<path, image_match::CSD>>
load_descriptors(const Document& doc)
{
    SPDLOG_DEBUG("Loading descriptors from databse...");

    std::vector<std::pair<path, image_match::CSD>> descriptors;
    if (!doc.IsArray())
        throw std::runtime_error("Invalid database file! (not an array)");

    auto arr = doc.GetArray();
    for (auto&& val : arr) {
        if (!is_db_entry_valid(val))
            throw std::runtime_error("Invalid database file! (invalid entry)");

        image_match::CSD::descriptor descriptor;
        for (auto&& num : val["descriptor"].GetArray())
            descriptor.push_back(num.GetFloat());

        descriptors.push_back(
          { val["path"].GetString(),
            image_match::CSD(descriptor,
                             image_match::csd_from_int(app.type)) });
    }

    return descriptors;
}

bool
is_db_file(const path& file)
{
    if (!is_regular_file(file))
        return false;

    return find(DB_FILES.begin(), DB_FILES.end(), file.filename().string()) !=
           DB_FILES.end();
}

std::vector<path>
find_database_file()
{
    SPDLOG_DEBUG("Looking for database file...");

    std::vector<path> ans;
    for (auto&& dir_entry : directory_iterator(app.dataset)) {
        if (app.type) {
            if (absolute(dir_entry.path()).lexically_normal() ==
                database_filename()) {
                ans.push_back(dir_entry.path());
                return ans;
            }
            continue;
        }

        if (is_db_file(dir_entry.path())) {
            SPDLOG_DEBUG("Found DB file {}", dir_entry.path().string());
            ans.push_back(dir_entry.path());
        }
    }

    return ans;
}

int
db_type(const path& db_file)
{
    std::istringstream iss{ db_file.filename().string().substr(4) };
    int ans;
    iss >> ans;
    return ans;
}

struct similarity_pair_less
{
    using sim_pair = std::pair<float, path>;
    bool operator()(const sim_pair& p1, const sim_pair& p2)
    {
        return p1.first < p2.first;
    }
};

void
run_match_subcommand()
{
    SPDLOG_DEBUG("Running match subcommand.");

    if (app.matches_num == 0)
        return;

    auto db_files = find_database_file();
    if (db_files.empty()) {
        throw std::runtime_error("Database file not found or invalid! Please "
                                 "run the `generate` subcommand first.");
    }

    if (db_files.size() != 1) {
        throw std::runtime_error(
          "Multiple database files found, please use the -t argument to chose "
          "the descriptor type you want to use.");
    }

    auto db_file = db_files[0];
    app.type = db_type(db_file);

    Document database;
    load_databse(db_file, database);
    auto descriptors = load_descriptors(database);

    // Load image to compare
    image_match::image im{ app.input_image_path };
    if (!im) {
        std::ostringstream msg;
        msg << "Error reading " << app.input_image_path.string() << "!\n"
            << im.fail_msg();
        throw std::runtime_error(msg.str());
    }

    spdlog::info("Generating descriptor for: {}",
                 absolute(app.input_image_path).lexically_normal().string());
    auto base_descriptor =
      image_match::CSD(im, image_match::csd_from_int(app.type));

    SPDLOG_DEBUG("base_descriptor type={}, giveth={}",
                 base_descriptor.type,
                 image_match::csd_from_int(app.type));

    // Find best match
    if (app.matches_num == -1)
        app.matches_num = descriptors.size();

    std::priority_queue<std::pair<float, path>,
                        std::vector<std::pair<float, path>>,
                        similarity_pair_less>
      matches;
    for (auto&& [p, descriptor] : descriptors) {
        auto similarity_index =
          image_match::compare(base_descriptor, descriptor);

        if (matches.size() < app.matches_num) {
            matches.push({ similarity_index, p });
        } else {
            auto index = matches.top().first;
            if (similarity_index < index) {
                matches.pop();
                matches.push({ similarity_index, p });
            }
        }
    }

    while (!matches.empty()) {
        auto [index, p] = matches.top();
        matches.pop();
        std::cout << index << '\t' << p.string() << '\n';
    }
}

std::string
check_type(const std::string& opt)
{
    if (std::find(CSD_TYPES.begin(), CSD_TYPES.end(), opt) == CSD_TYPES.end()) {
        std::ostringstream msg{};
        msg << opt << "is not a valid type.";
        throw CLI::ValidationError(msg.str());
    }

    return std::string();
}

int
main(int argc, char* argv[])
{
    // Set the default global log level to debug
    spdlog::set_level(spdlog::level::debug);

    // Parse command line arguments
    CLI::App args("Simple image mathiching utility.");
    args.require_subcommand(1);

    auto generate_sub =
      args.add_subcommand("generate", "Generate descriptor database.");
    auto match_sub =
      args.add_subcommand("match", "Match an image against database.");

    // Arguments for the generate subcommand
    generate_sub
      ->add_option(
        "type", app.type, "Type of descriptor to generate (32, 64, 128 or 256)")
      ->required(true)
      ->check(check_type);

    generate_sub
      ->add_option(
        "dataset", app.dataset, "Path to the directory containing images.")
      ->required()
      ->check(CLI::ExistingDirectory);

    generate_sub->add_flag("-f,--force-regenerate",
                           app.force_regenerate,
                           "Force regenerate all descriptors.");

    // Arguments for the match subcommand
    match_sub
      ->add_option(
        "image_path", app.input_image_path, "Path of image to compare.")
      ->required()
      ->check(CLI::ExistingFile);

    match_sub
      ->add_option(
        "dataset", app.dataset, "Path to the directory containing images.")
      ->required()
      ->check(CLI::ExistingDirectory);

    match_sub->add_option(
      "-n,--number-of-matches",
      app.matches_num,
      "Number of matches to display, -1 for all. (default: 10)");

    match_sub
      ->add_option("-t,--type",
                   app.type,
                   "Type of descriptor to generate (32, 64, 128 or 256)")
      ->check(check_type);

    // Common arguments for both commands
    generate_sub->add_flag("-q,--quiet", app.quiet_mode, "Enable quiet mode.");
    match_sub->add_flag("-q,--quiet", app.quiet_mode, "Enable quiet mode.");

    CLI11_PARSE(args, argc, argv);

    SPDLOG_DEBUG("dataset={}", app.dataset.string());
    SPDLOG_DEBUG("input_image_path={}", app.input_image_path.string());
    SPDLOG_DEBUG("output_json={}", app.output_json);
    SPDLOG_DEBUG("matches_num={}", app.matches_num);
    SPDLOG_DEBUG("quiet_mode={}", app.quiet_mode);

    if (app.quiet_mode)
        spdlog::set_level(spdlog::level::critical);

    if (*generate_sub)
        try {
            run_generate_subcommand();
        } catch (std::runtime_error e) {
            spdlog::critical(e.what());
            return EXIT_FAILURE;
        }

    if (*match_sub)
        try {
            run_match_subcommand();
        } catch (std::runtime_error e) {
            spdlog::critical(e.what());
            return EXIT_FAILURE;
        }

    return EXIT_SUCCESS;
}
