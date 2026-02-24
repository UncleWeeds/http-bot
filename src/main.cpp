#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <vector>

#include <curl/curl.h>
#include <nlohmann/json.hpp>

#include "json_loader.hpp"
#include "http_client.hpp"

using json = nlohmann::json;

static std::string sanitize_filename(const std::string& s) {
    std::string out;
    out.reserve(s.size());
    for (char c : s) {
        if ((c >= 'a' && c <= 'z') ||
            (c >= 'A' && c <= 'Z') ||
            (c >= '0' && c <= '9') ||
            c == '-' || c == '_') {
            out.push_back(c);
        } else {
            out.push_back('_');
        }
    }
    if (out.empty()) out = "output";
    return out;
}

static void print_usage() {
    std::cerr
        << "Usage: http_bot <input.json> [--out <output_dir>] [--print] [--retries <N>]\n"
        << "  --out <dir>     Output directory (default: output)\n"
        << "  --print         Print first 500 chars of each HTML response\n"
        << "  --retries <N>   Retry failed fetches N times (default: 2)\n";
}

int main(int argc, char** argv) {
    if (argc < 2) {
        print_usage();
        return 1;
    }

    std::string json_path = argv[1];
    std::string out_dir = "output";
    bool do_print = false;
    int retries = 2;

    for (int i = 2; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "--out") {
            if (i + 1 >= argc) {
                std::cerr << "Error: --out requires a directory\n";
                return 1;
            }
            out_dir = argv[++i];
        } else if (arg == "--print") {
            do_print = true;
        } else if (arg == "--retries") {
            if (i + 1 >= argc) {
                std::cerr << "Error: --retries requires a number\n";
                return 1;
            }
            retries = std::stoi(argv[++i]);
            if (retries < 0) retries = 0;
        } else {
            std::cerr << "Unknown option: " << arg << "\n";
            print_usage();
            return 1;
        }
    }

    // Best practice: init libcurl once per process
    curl_global_init(CURL_GLOBAL_DEFAULT);

    auto cfg = load_config(json_path);
    std::cout << "Loaded bot_name='" << cfg.bot_name << "' with "
              << cfg.requests.size() << " request(s)\n";

    if (cfg.requests.empty()) {
        std::cerr << "No requests found in JSON.\n";
        curl_global_cleanup();
        return 1;
    }

    std::string user_agent = cfg.bot_name.empty() ? "http_bot/1.0" : (cfg.bot_name + "/1.0");

    std::filesystem::create_directories(out_dir);

    json summary;
    summary["bot_name"] = cfg.bot_name;
    summary["user_agent"] = user_agent;
    summary["results"] = json::array();

    for (const auto& req : cfg.requests) {
        std::cout << "\n==> " << req.name << "\n";
        std::cout << "URL: " << req.url << "\n";

        auto result = fetch_url(req.url, user_agent, retries);

        json item;
        item["name"] = req.name;
        item["url"] = req.url;
        item["ok"] = result.ok;
        item["http_status"] = result.status;
        item["bytes"] = result.body.size();
        item["error"] = result.error;

        if (!result.ok) {
            std::cerr << "Fetch failed: " << result.error << "\n";
            summary["results"].push_back(item);
            continue;
        }

        std::cout << "HTTP: " << result.status << "\n";
        std::cout << "Bytes: " << result.body.size() << "\n";

        std::string file_name = sanitize_filename(req.name) + ".html";
        std::filesystem::path out_path = std::filesystem::path(out_dir) / file_name;

        std::ofstream out(out_path, std::ios::binary);
        if (!out.is_open()) {
            std::cerr << "Could not write to: " << out_path.string() << "\n";
            item["ok"] = false;
            item["error"] = "failed to write output file";
            summary["results"].push_back(item);
            continue;
        }

        out << result.body;
        std::cout << "Saved: " << out_path.string() << "\n";

        if (do_print) {
            const size_t n = std::min<size_t>(500, result.body.size());
            std::cout << "--- First " << n << " chars ---\n";
            std::cout << result.body.substr(0, n) << "\n";
            std::cout << "------------------------\n";
        }

        summary["results"].push_back(item);
    }

    // Write summary.json
    {
        std::filesystem::path summary_path = std::filesystem::path(out_dir) / "summary.json";
        std::ofstream s(summary_path);
        if (s.is_open()) {
            s << summary.dump(2) << "\n";
            std::cout << "\nSummary written: " << summary_path.string() << "\n";
        } else {
            std::cerr << "\nWarning: could not write summary.json\n";
        }
    }

    curl_global_cleanup();
    return 0;
}
