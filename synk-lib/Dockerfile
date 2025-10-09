FROM ubuntu:22.04

# Устанавливаем зависимости без интерактивных вопросов
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

# Создаем и переходим в директорию сборки
RUN mkdir build && cd build && \
    cmake .. && \
    make

# Запускаем тесты по умолчанию
CMD ["./build/test/run_tests"]