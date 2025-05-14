# Stage 1: Build the service
# Используем тот же базовый образ, что и в вашем Makefile для консистентности
# Вы можете зафиксировать тег, если хотите, например, ubuntu-24.04-userver:v1.2.3
ARG USERVER_IMAGE_TAG=latest
FROM ghcr.io/userver-framework/ubuntu-24.04-userver:${USERVER_IMAGE_TAG} as builder

ARG PROJECT_NAME=current-actions
# Аргумент для количества потоков сборки, по умолчанию 4
ARG NPROCS_BUILD_ARG=4
ENV NPROCS=${NPROCS_BUILD_ARG}

# ccache для ускорения повторных сборок (userver-образы обычно его содержат)
ENV CCACHE_DIR=/.ccache
ENV CCACHE_MAXSIZE=2G
# Если ccache не установлен в базовом образе, можно добавить:
# RUN apt-get update && apt-get install -y ccache

WORKDIR /opt/service

# Копируем исходный код проекта
COPY . .

# Создаем директорию для сборки и конфигурируем CMake для release
# Предполагается, что у вас есть CMakePresets.json с пресетом 'release'
# или CMakeLists.txt корректно настроен для сборки без явного пресета
RUN cmake --preset release -B build-release -S .

# Собираем проект (только таргет самого сервиса)
RUN cmake --build build-release -j ${NPROCS} --target ${PROJECT_NAME}

# Устанавливаем собранный сервис и его конфигурации в промежуточную директорию
# CMAKE_INSTALL_PREFIX определяет, куда будут установлены артефакты
# Компонент ${PROJECT_NAME} должен быть определен в вашем CMakeLists.txt для install()
RUN cmake --install build-release --prefix /opt/service/install --component ${PROJECT_NAME}

# Stage 2: Create the runtime image
FROM ghcr.io/userver-framework/ubuntu-24.04-userver:${USERVER_IMAGE_TAG} as runtime

ARG PROJECT_NAME=current-actions
ENV APP_DIR=/opt/${PROJECT_NAME}
ENV CONFIG_DIR=${APP_DIR}/configs
# Директории для логов и данных, если сервис их пишет в файлы (рекомендуется логировать в stdout/stderr в Docker)
ENV LOG_DIR=/var/log/${PROJECT_NAME}
ENV DATA_DIR=/var/lib/${PROJECT_NAME}

# Создаем необходимые директории и пользователя без привилегий для запуска сервиса
RUN mkdir -p ${APP_DIR}/bin ${CONFIG_DIR} ${LOG_DIR} ${DATA_DIR} && \
    useradd --system --home-dir ${APP_DIR} --no-create-home --shell /usr/sbin/nologin appuser && \
    chown -R appuser:appuser ${APP_DIR} ${LOG_DIR} ${DATA_DIR}

WORKDIR ${APP_DIR}

# Копируем исполняемый файл из builder stage
COPY --from=builder /opt/service/install/bin/${PROJECT_NAME} ./bin/
# Копируем статический конфиг, установленный CMake
# Путь /etc/${PROJECT_NAME}/static_config.yaml является стандартным для userver при install()
COPY --from=builder /opt/service/install/etc/${PROJECT_NAME}/static_config.yaml ${CONFIG_DIR}/
# Копируем config_vars.docker.yaml как основной config_vars.yaml для контейнера
COPY configs/config_vars.docker.yaml ${CONFIG_DIR}/config_vars.yaml

# Если ваш сервис устанавливает какие-то дополнительные ресурсы (например, SQL миграции, шаблоны и т.д.)
# через `install(DIRECTORY ...)` в CMake, они могут оказаться в /opt/service/install/share/${PROJECT_NAME}
# COPY --from=builder /opt/service/install/share/${PROJECT_NAME}/ ./share/

USER appuser

# Порт, который будет слушать сервис (из config_vars.docker.yaml)
EXPOSE 8080

# Переменные окружения для путей к конфигам (userver может их подхватывать)
ENV USERVER_CONFIG_PATH=${CONFIG_DIR}/static_config.yaml
ENV USERVER_CONFIG_VARS_PATH=${CONFIG_DIR}/config_vars.yaml

# Команда для запуска сервиса
# Сервис будет искать конфиги по путям, указанным в переменных окружения,
# или по путям, переданным через аргументы командной строки.
CMD ["./bin/current-actions"]
