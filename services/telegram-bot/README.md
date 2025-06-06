# Telegram Bot Service (Frontend)

## 🎯 Назначение

Сервис `telegram-bot` является основным пользовательским интерфейсом AI-ассистента. Он выполняет следующие функции:

1.  **Взаимодействие с пользователем:** Обработка команд и текстовых сообщений от пользователя в Telegram.
2.  **Отображение информации:** Вывод списков задач, заметок, подтверждений и другой релевантной информации пользователю.
3.  **Управление состоянием диалога:** Поддержка многошаговых сценариев взаимодействия (например, подтверждение удаления).
4.  **Интеграция с бэкенд-сервисами:** Отправка запросов к `ai-proxy` для классификации задач и к C++ бэкенд-сервисам (`current-actions`, `sometime-later`, `waiting`, `notes`, `calendar`) для управления данными.
5.  **Абстрагирование сложности GTD:** Предоставление пользователю простого и интуитивно понятного интерфейса, полностью скрывающего внутреннюю логику GTD.

## 🛠️ Технологии

*   Python
*   Библиотека `aiogram` для создания Telegram-ботов.
*   HTTP-клиент (например, `aiohttp` или `httpx`) для взаимодействия с другими микросервисами.

## ⚙️ Конфигурация

Для работы сервиса необходим токен Telegram-бота и URL-адреса других микросервисов. Они должны быть указаны в файле `.env` в корневой директории этого сервиса:

```env
# services/telegram-bot/.env
TELEGRAM_BOT_TOKEN="Ваш_Telegram_Bot_Token"
AI_PROXY_URL="http://ai-proxy:8080"
CURRENT_ACTIONS_URL="http://current-actions:8080"
SOMETIME_LATER_URL="http://sometime-later:8080"
WAITING_URL="http://waiting:8080"
NOTES_URL="http://notes:8080"
# CALENDAR_URL="http://calendar:8080" # Если бот напрямую обращается для .ics, но обычно это ссылка
```
Имена и порты сервисов должны соответствовать конфигурации в `docker-compose.yml`.

## 🤖 Основные команды и сценарии

*   **Команды:** `/start`, `/help`, `/support`.
*   **Свободный ввод текста:** Автоматическая классификация через `ai-proxy` и добавление в соответствующий список (`current-actions`, `waiting`, `notes` или обработка как `trash`).
*   **Кнопки кастомной клавиатуры:**
    *   **"Задачи"**: Взаимодействие с `current-actions` (просмотр, ✅, ✏️,  postpon, 🗑️).
    *   **"Не сейчас"**: Взаимодействие с `sometime-later` (просмотр, ✅, ✏️, ➡️, 🗑️).
    *   **"Ожидания"**: Взаимодействие с `waiting` (просмотр, ✅, ✏️, 🗑️).
    *   **"Заметки"**: Взаимодействие с `notes` (просмотр, ✏️, 🗑️).
    *   **"Выполнено"**: Агрегированный просмотр выполненных задач из `current-actions`, `sometime-later`, `waiting`.
*   **Пагинация**: Поддержка просмотра больших списков с кнопками "⬅️ Влево" / "➡️ Вправо".
*   **Подтверждение действий**: Запрос подтверждения ("Да"/"Нет") для операций удаления.

Подробное описание сценариев взаимодействия смотрите в главном `README.md` проекта.

## 🚀 Запуск

Сервис предназначен для запуска в Docker-контейнере в составе общей системы через `docker-compose.yml`.

Для локальной разработки (если не используется Docker):
1. Установите зависимости: `pip install -r requirements.txt`
2. Убедитесь, что файл `.env` настроен.
3. Запустите основной файл бота (например, `python telegram_bot/bot.py`).
