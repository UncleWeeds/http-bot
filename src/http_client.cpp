#include "http_client.hpp"
#include <curl/curl.h>
#include <string>

/*
 High-level approach:
 - Use libcurl to perform an HTTP GET request
 - Stream the response into a std::string via a write callback
 - Set a User-Agent (required)
 - Add retry logic for transient failures (network hiccups/timeouts/etc.)
*/

static size_t write_to_string(void* contents, size_t size, size_t nmemb, void* userp) {
    const size_t total = size * nmemb;
    auto* out = static_cast<std::string*>(userp);
    out->append(static_cast<char*>(contents), total);
    return total;
}

static HttpResult fetch_once(const std::string& url, const std::string& user_agent) {
    HttpResult res;
    res.ok = false;
    res.status = 0;

    CURL* curl = curl_easy_init();
    if (!curl) {
        res.error = "curl_easy_init() failed";
        return res;
    }

    std::string body;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, user_agent.c_str());

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_to_string);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &body);

    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 20L);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10L);

    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);

    CURLcode code = curl_easy_perform(curl);
    if (code != CURLE_OK) {
        res.error = curl_easy_strerror(code);
        curl_easy_cleanup(curl);
        return res;
    }

    long http_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

    res.ok = true;
    res.status = http_code;
    res.body = std::move(body);

    curl_easy_cleanup(curl);
    return res;
}

HttpResult fetch_url(const std::string& url, const std::string& user_agent, int retries) {
    if (retries < 0) retries = 0;

    HttpResult last;
    for (int attempt = 0; attempt <= retries; ++attempt) {
        last = fetch_once(url, user_agent);

        // Success path
        if (last.ok) return last;

        // If it failed and we have attempts left, try again
        if (attempt < retries) {
            // simple retry; could add backoff, but keeping it minimal
        }
    }
    return last;
}
