# Notes Service

## 🎯 Назначение

Бэкенд-сервис `notes` предназначен для управления заметками и справочной информацией пользователя, которая не является непосредственно задачами, но может быть полезна для их выполнения или общего ведения дел (в GTD это часть "Справочные материалы").

## 🛠️ Технологии

*   C++ (C++20)
*   Фреймворк `userver`
*   PostgreSQL (для хранения данных)
*   CMake/Make (для сборки)

## 🗃️ База данных

Основная таблица:

*   **`notes`**:
    *   `id` (UUID, PK)
    *   `user_id` (BIGINT)
    *   `title` (TEXT, NOT NULL) // Может быть первой строкой заметки или отдельным заголовком
    *   `content` (TEXT, NOT NULL)
    *   `created_at` (TIMESTAMP WITH TIME ZONE, NOT NULL)
    *   `updated_at` (TIMESTAMP WITH TIME ZONE, NOT NULL)
    *   `gtd_list_type` (VARCHAR, const 'notes')
    *   ... другие поля (теги, категория и т.д.).

Скрипты для создания/миграции таблиц находятся в директории `postgresql/`.

## 🌐 API (Основные эндпоинты)

*   `GET /v1/notes?user_id=<user_id>&limit=<N>&cursor=<cursor>`: Получить список заметок.
*   `POST /v1/notes`: Создать новую заметку.
    *   Тело запроса (JSON): `{ "user_id": "...", "title": "...", "content": "..." }`
*   `GET /v1/notes/{note_id}?user_id=<user_id>`: Получить заметку.
*   `PUT /v1/notes/{note_id}`: Обновить заметку.
    *   Тело запроса (JSON): `{ "user_id": "...", "title": "...", "content": "..." }`
*   `DELETE /v1/notes/{note_id}?user_id=<user_id>`: Удалить заметку.

## 🚀 Сборка и запуск

Аналогично сервису `current-actions`. Управляется через `Dockerfile` и `Makefile`.
