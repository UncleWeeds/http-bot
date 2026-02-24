#pragma once
#include <string>

struct HttpResult {
  bool ok{};
  long status{};
  std::string body;
  std::string error;
};

HttpResult fetch_url(const std::string& url,
                     const std::string& user_agent,
                     int retries = 2);
