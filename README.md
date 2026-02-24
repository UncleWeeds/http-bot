# http_bot 

A simple CLI-based HTTP request bot written in C++.  
It reads a JSON file containing a list of URLs and fetches the HTML content of each URL using **libcurl**.

This project demonstrates:

- Structured JSON parsing
- Proper HTTP client usage
- User-Agent handling
- Robust error handling
- Reproducible builds using CMake
- Optional containerized execution via Docker


## Features

- Parses input JSON using **nlohmann/json**
- Fetches HTML over HTTP/HTTPS using **libcurl**
- Sets a custom **User-Agent** (`CURLOPT_USERAGENT`)
- Follows HTTP redirects
- Supports configurable retry logic
- Saves HTML output to an output directory
- Generates a structured `summary.json` report
- Optional `--print` flag to print the first 500 characters of each response
- Docker support for reproducible builds

## Design Overview

High-level approach:

1. Parse configuration from JSON input
2. For each URL:
   - Perform HTTP GET using libcurl
   - Capture response via write callback
   - Extract HTTP status
   - Save content to file
3. Record metadata into `summary.json`

The program initializes libcurl globally once per process and performs safe cleanup on exit.

### Dependencies (Ubuntu)

```bash
sudo apt update
sudo apt install -y build-essential cmake pkg-config libcurl4-openssl-dev
```

## Build (Native)

```bash
cmake -S . -B build
cmake --build build -j
```



### Run

```bash
./build/http_bot input/example.json
```

Custom output directory:

```bash
./build/http_bot input/example.json --out output_dir
```

Print first 500 characters:

```bash
./build/http_bot input/example.json --print
```

Retry failed fetches:

```bash
./build/http_bot input/example.json --retries 2
```

---

## Run with Docker (Optional)

Build image:

```bash
docker build -t http_bot .
```

Run:

```bash
docker run --rm \
  -v $(pwd)/input:/input \
  -v $(pwd)/output:/output \
  http_bot /input/example.json --out /output --print
```

---

## Input JSON Format

```json
{
  "bot_name": "awesomeCrawler",
  "requests": [
    { "name": "Google", "url": "https://www.google.com" },
    { "name": "GitHub", "url": "https://www.github.com" }
  ]
}
```

---

## Output

- `output/<name>.html` — raw HTML response
- `output/summary.json` — metadata including:
  - name
  - URL
  - HTTP status
  - byte count
  - success flag
  - error message (if any)




