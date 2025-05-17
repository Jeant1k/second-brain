# Sometime/Later Service

## 🎯 Назначение

Бэкенд-сервис `sometime-later` отвечает за управление списком "Когда-нибудь/Может быть" (Sometime/Maybe) в рамках методологии GTD. Он хранит задачи, которые не требуют немедленного внимания, но которые пользователь хотел бы рассмотреть или выполнить в будущем.

## 🛠️ Технологии

*   C++ (C++20)
*   Фреймворк `userver`
*   PostgreSQL (для хранения данных)
*   CMake/Make (для сборки)

## 🗃️ База данных

Сервис использует свою схему или набор таблиц в общей базе данных PostgreSQL. Основная таблица для хранения отложенных задач:

*   **`sometime_later_tasks`**:
    *   `id` (UUID, PK)
    *   `user_id` (BIGINT)
    *   `title` (TEXT, NOT NULL)
    *   `description` (TEXT, nullable)
    *   `created_at` (TIMESTAMP WITH TIME ZONE, NOT NULL)
    *   `updated_at` (TIMESTAMP WITH TIME ZONE, NOT NULL)
    *   `status` (VARCHAR, e.g., 'deferred', 'activated', 'completed', 'deleted')
    *   `gtd_list_type` (VARCHAR, const 'sometime_later')
    *   ... другие поля.

Скрипты для создания/миграции таблиц находятся в директории `postgresql/`.

## 🌐 API (Основные эндпоинты)

Сервис предоставляет REST-подобный HTTP API:

*   `GET /v1/tasks?user_id=<user_id>&limit=<N>&cursor=<cursor>`: Получить список отложенных задач.
*   `POST /v1/tasks`: Создать новую отложенную задачу (например, при перемещении из `current-actions`).
    *   Тело запроса (JSON): `{ "user_id": "...", "title": "...", "description": "..." }`
*   `GET /v1/tasks/{task_id}?user_id=<user_id>`: Получить информацию о задаче.
*   `PUT /v1/tasks/{task_id}`: Обновить задачу (например, изменить название).
    *   Тело запроса (JSON): `{ "user_id": "...", "title": "..." }`
*   `POST /v1/tasks/{task_id}/complete`: Пометить задачу как выполненную.
    *   Тело запроса (JSON): `{ "user_id": "..." }`
*   `POST /v1/tasks/{task_id}/activate`: Переместить задачу в список "Текущие действия" (`current-actions`).
    *   Тело запроса (JSON): `{ "user_id": "..." }`
*   `DELETE /v1/tasks/{task_id}?user_id=<user_id>`: Удалить задачу.

## 🚀 Сборка и запуск

Аналогично сервису `current-actions`. Управляется через `Dockerfile` и `Makefile`.
