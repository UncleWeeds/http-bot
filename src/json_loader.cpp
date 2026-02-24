#include "json_loader.hpp"
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>

using json = nlohmann::json;

/*
 High-level approach:
 - Open the JSON file
 - Parse it into a json object
 - Extract bot_name (if present)
 - Extract requests array
 - Validate each request entry
*/

BotConfig load_config(const std::string& json_path) {
    BotConfig config;

    std::ifstream file(json_path);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open JSON file: " << json_path << "\n";
        return config;
    }

    json j;
    try {
        file >> j;
    } catch (const std::exception& e) {
        std::cerr << "Error parsing JSON: " << e.what() << "\n";
        return config;
    }

    // Extract bot_name
    if (j.contains("bot_name") && j["bot_name"].is_string()) {
        config.bot_name = j["bot_name"];
    }

    // Extract requests
    if (j.contains("requests") && j["requests"].is_array()) {
        for (const auto& item : j["requests"]) {
            if (item.contains("name") && item.contains("url") &&
                item["name"].is_string() && item["url"].is_string()) {

                RequestItem req;
                req.name = item["name"];
                req.url = item["url"];

                if (!req.url.empty()) {
                    config.requests.push_back(req);
                }
            }
        }
    }

    return config;
}
