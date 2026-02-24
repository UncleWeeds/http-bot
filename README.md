# http_bot (Phase 1 - CVMFS evaluation)

A simple CLI "HTTP request bot" written in C++.
It reads a JSON file containing a list of URLs and downloads the HTML content of each URL using libcurl.

## Features
- Parses input JSON (nlohmann/json)
- Fetches HTML over HTTP/HTTPS (libcurl)
- Sets a User-Agent (required)
- Follows redirects
- Saves output HTML files to an output directory
- Writes a `summary.json` report
- Optional `--print` flag to print the first 500 characters of each response
- Optional retry support for failed fetches

## Notes on robots.txt
This tool fetches only the exact URLs provided in the JSON input.
It does not crawl additional links. If extended into a crawler (fetching other pages beyond the provided list),
the bot should check and respect the site's `robots.txt` rules before requesting paths.

## Dependencies (Ubuntu)
```bash
sudo apt update
sudo apt install -y build-essential cmake pkg-config libcurl4-openssl-dev
