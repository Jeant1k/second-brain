# ==== STAGE 1: Build ======
FROM ghcr.io/userver-framework/ubuntu-24.04-userver:latest AS builder

WORKDIR /build

COPY . .

# Можно установить дополнительные пакеты, если нужно (пример с clang-format, вдруг пригодится)
RUN apt-get update && \
    apt-get install -y clang-format && \
    rm -rf /var/lib/apt/lists/*

# Собираем проект (prod сборку)
RUN cmake --preset release -B build-release -S .
RUN cmake --build build-release -j $(nproc)

# ==== STAGE 2: Runner =====
FROM ghcr.io/userver-framework/ubuntu-24.04-userver:latest AS runner

# Добавим system user для запуска приложения
RUN groupadd -r serviceuser && useradd -r -g serviceuser serviceuser

WORKDIR /app

# Копируем собранный бинарник и конфиги
COPY --from=builder /build/build-release/notes /app/notes
COPY configs/static_config.yaml /app/static_config.yaml
COPY configs/config_vars.docker.yaml /app/config_vars.yaml

# Если нужны какие-то другие файлы - копируй по аналогии

USER serviceuser

EXPOSE 8080

# Запуск по умолчанию
CMD ["./notes", "--config", "static_config.yaml", "--config_vars", "config_vars.yaml"]
