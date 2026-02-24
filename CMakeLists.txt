cmake_minimum_required(VERSION 3.16)
project(http_bot LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
  nlohmann_json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(nlohmann_json)

find_package(CURL REQUIRED)

add_executable(http_bot
  src/main.cpp
  src/http_client.cpp
  src/json_loader.cpp
)

target_include_directories(http_bot PRIVATE src)
target_link_libraries(http_bot PRIVATE CURL::libcurl nlohmann_json::nlohmann_json)
