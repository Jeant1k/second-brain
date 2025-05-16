# Используем официальный образ Python
FROM python:3.11-slim

# Устанавливаем переменные окружения
ENV PYTHONDONTWRITEBYTECODE 1  # Предотвращает создание .pyc файлов
ENV PYTHONUNBUFFERED 1         # Выводит логи Python сразу в stdout/stderr (лучше для Docker)

# Устанавливаем рабочую директорию в контейнере
WORKDIR /app

# Копируем файл с зависимостями
COPY requirements.txt .

# Устанавливаем зависимости
# --no-cache-dir чтобы уменьшить размер образа
# --trusted-host pypi.python.org --trusted-host files.pythonhosted.org --trusted-host pypi.org для обхода возможных проблем с SSL в некоторых сетях
RUN pip install --no-cache-dir --upgrade pip && \
    pip install --no-cache-dir -r requirements.txt \
    --trusted-host pypi.python.org \
    --trusted-host files.pythonhosted.org \
    --trusted-host pypi.org

# Копируем исходный код приложения в рабочую директорию
# Копируем папку ai_proxy в /app/ai_proxy
COPY ./ai_proxy ./ai_proxy
COPY .env .
# Если папка docs или Makefile нужны для работы приложения, их тоже можно скопировать:
# COPY ./docs ./docs
# COPY Makefile .

# Переменные окружения для приложения (могут быть переопределены в docker-compose.yml)
# HOST, PORT, DEBUG используются в вашем main.py
ENV HOST="0.0.0.0"
ENV PORT="8000"
ENV DEBUG="false"

# Открываем порт, на котором будет работать приложение внутри контейнера
EXPOSE 8000

# Команда для запуска приложения
# Ваш main.py содержит логику для uvicorn.run, включая reload=debug
# Поэтому запускаем его напрямую через python
CMD ["python", "-m", "ai_proxy.main"]
