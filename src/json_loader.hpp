#pragma once
#include <string>
#include <vector>

struct RequestItem {
  std::string name;
  std::string url;
};

struct BotConfig {
  std::string bot_name;
  std::vector<RequestItem> requests;
};

BotConfig load_config(const std::string& json_path);
