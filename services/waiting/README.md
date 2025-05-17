# Waiting For Service

## 🎯 Назначение

Бэкенд-сервис `waiting` управляет списком "Ожидание" (Waiting For) в методологии GTD. Здесь хранятся задачи или элементы, которые были делегированы другим людям или зависят от внешних событий, прежде чем пользователь сможет предпринять следующие шаги.

## 🛠️ Технологии

*   C++ (C++20)
*   Фреймворк `userver`
*   PostgreSQL (для хранения данных)
*   CMake/Make (для сборки)

## 🗃️ База данных

Основная таблица:

*   **`waiting_tasks`**:
    *   `id` (UUID, PK)
    *   `user_id` (BIGINT)
    *   `title` (TEXT, NOT NULL)
    *   `description` (TEXT, nullable) // Может содержать информацию о том, от кого/чего ожидание
    *   `created_at` (TIMESTAMP WITH TIME ZONE, NOT NULL)
    *   `updated_at` (TIMESTAMP WITH TIME ZONE, NOT NULL)
    *   `expected_by_date` (TIMESTAMP WITH TIME ZONE, nullable)
    *   `status` (VARCHAR, e.g., 'waiting', 'received', 'completed', 'deleted')
    *   `gtd_list_type` (VARCHAR, const 'waiting')
    *   ... другие поля.

Скрипты для создания/миграции таблиц находятся в директории `postgresql/`.

## 🌐 API (Основные эндпоинты)

*   `GET /v1/tasks?user_id=<user_id>&limit=<N>&cursor=<cursor>`: Получить список ожидающих задач.
*   `POST /v1/tasks`: Создать новую ожидающую задачу.
    *   Тело запроса (JSON): `{ "user_id": "...", "title": "...", "description": "..." }`
*   `GET /v1/tasks/{task_id}?user_id=<user_id>`: Получить информацию о задаче.
*   `PUT /v1/tasks/{task_id}`: Обновить задачу.
    *   Тело запроса (JSON): `{ "user_id": "...", "title": "..." }`
*   `POST /v1/tasks/{task_id}/complete`: Пометить задачу как выполненную/полученную.
    *   Тело запроса (JSON): `{ "user_id": "..." }`
*   `DELETE /v1/tasks/{task_id}?user_id=<user_id>`: Удалить задачу.

## 🚀 Сборка и запуск

Аналогично сервису `current-actions`. Управляется через `Dockerfile` и `Makefile`.
