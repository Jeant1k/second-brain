.PHONY: build run logs stop clean requirements

# Имя сервиса и тег
IMAGE_NAME := telegram-bot-service
IMAGE_TAG := latest

# Файл с переменными окружения
ENV_FILE := .env

build:
	docker build -t $(IMAGE_NAME):$(IMAGE_TAG) .

run-docker: build
	docker run -d --rm --name $(IMAGE_NAME) \
		--env-file $(ENV_FILE) \
		-p 8085:8080 \ # Если боту нужен порт (обычно нет для polling)
		$(IMAGE_NAME):$(IMAGE_TAG)

run-local:
	python telegram_bot/main.py

logs:
	docker logs -f $(IMAGE_NAME)

stop:
	docker stop $(IMAGE_NAME) || true

clean: stop
	docker rm $(IMAGE_NAME) || true
	docker rmi $(IMAGE_NAME):$(IMAGE_TAG) || true

requirements:
	pip freeze > requirements.txt

