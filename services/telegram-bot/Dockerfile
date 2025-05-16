FROM python:3.10-slim

WORKDIR /app

ENV PYTHONDONTWRITEBYTECODE 1
ENV PYTHONUNBUFFERED 1

COPY requirements.txt .
RUN pip install --no-cache-dir -r requirements.txt

COPY ./telegram_bot /app/telegram_bot
COPY .env /app/.env

# Убедитесь, что .env копируется, если он нужен внутри контейнера
# Если вы передаете переменные окружения через docker-compose, .env не обязательно копировать

CMD ["python", "-m", "telegram_bot.main"]
