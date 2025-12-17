FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && \
    apt-get install -y \
    build-essential \
    cmake \
    pkg-config \
    libcmocka-dev \
    git \
    valgrind && \
    rm -rf /var/lib/apt/lists/*

WORKDIR /app


COPY . .

RUN mkdir build && cd build && \
    cmake .. && \
    make

CMD ["./build/test/run_tests"]