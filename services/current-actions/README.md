# Current Actions Service

## 🎯 Назначение

Бэкенд-сервис `current-actions` отвечает за управление списком "Текущие действия" (Next Actions) в рамках методологии GTD. Он предоставляет API для создания, чтения, обновления и удаления активных задач, которые пользователь планирует выполнить в ближайшее время.

## 🛠️ Технологии

*   C++ (C++20)
*   Фреймворк `userver`
*   PostgreSQL (для хранения данных)
*   CMake/Make (для сборки)

## 🗃️ База данных

Сервис использует собственную схему или набор таблиц в общей базе данных PostgreSQL. Основная таблица для хранения текущих задач может выглядеть так:

*   **`current_action_tasks`**:
    *   `id` (UUID, PK)
    *   `user_id` (BIGINT, FK to users table or just identifier)
    *   `title` (TEXT, NOT NULL)
    *   `description` (TEXT, nullable)
    *   `created_at` (TIMESTAMP WITH TIME ZONE, NOT NULL)
    *   `updated_at` (TIMESTAMP WITH TIME ZONE, NOT NULL)
    *   `due_date` (TIMESTAMP WITH TIME ZONE, nullable)
    *   `status` (VARCHAR, e.g., 'active', 'completed', 'deferred', 'deleted')
    *   `gtd_list_type` (VARCHAR, const 'current_actions') // Для унификации при агрегации
    *   ... другие необходимые поля (контекст, проект и т.д.)

Скрипты для создания/миграции таблиц находятся в директории `postgresql/`.

## 🌐 API (Основные эндпоинты)

Сервис предоставляет REST-подобный HTTP API:

*   `GET /v1/tasks?user_id=<user_id>&limit=<N>&cursor=<cursor>`: Получить список активных задач пользователя (с пагинацией).
*   `POST /v1/tasks`: Создать новую задачу.
    *   Тело запроса (JSON): `{ "user_id": "...", "title": "...", "description": "..." }`
*   `GET /v1/tasks/{task_id}?user_id=<user_id>`: Получить информацию о конкретной задаче.
*   `PUT /v1/tasks/{task_id}`: Обновить задачу (например, изменить название).
    *   Тело запроса (JSON): `{ "user_id": "...", "title": "новое название" }`
*   `POST /v1/tasks/{task_id}/complete`: Пометить задачу как выполненную.
    *   Тело запроса (JSON): `{ "user_id": "..." }`
*   `POST /v1/tasks/{task_id}/defer`: Отложить задачу (переместить в `sometime-later`).
    *   Тело запроса (JSON): `{ "user_id": "..." }`
    *   *Примечание: это действие может инициировать вызов сервиса `sometime-later` или просто изменить статус и ожидать, что `telegram-bot` сделает нужные вызовы.*
*   `DELETE /v1/tasks/{task_id}?user_id=<user_id>`: Логически или физически удалить задачу.

Эндпоинты и структура ответов должны быть четко документированы (например, с использованием OpenAPI в `docs/`).

## 🚀 Сборка и запуск

Сервис предназначен для сборки и запуска в Docker-контейнере. `Dockerfile` и `Makefile` в корневой директории сервиса управляют этим процессом.

Для локальной сборки (примерно):
1.  `mkdir build-debug && cd build-debug`
2.  `cmake .. -DCMAKE_BUILD_TYPE=Debug`
3.  `make -jN`
4.  Запуск бинарного файла с указанием пути к конфигурационному файлу (например, `configs/config.yaml`).
