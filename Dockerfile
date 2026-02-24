# -------- Build Stage --------
FROM ubuntu:24.04 AS builder

RUN apt update && apt install -y \
    build-essential \
    cmake \
    git \
    pkg-config \
    libcurl4-openssl-dev

WORKDIR /app
COPY . .

RUN cmake -S . -B build \
 && cmake --build build -j

# -------- Runtime Stage --------
FROM ubuntu:24.04

RUN apt update && apt install -y \
    libcurl4 \
 && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY --from=builder /app/build/http_bot /usr/local/bin/http_bot

ENTRYPOINT ["http_bot"]
