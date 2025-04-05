PROJECT_ROOT := /home/jean/second_brain
TEMP_DIR := /tmp/openapi-gen

# .PHONY: run-tests
# run-tests:
# 	pytest --log-cli-level=DEBUG -vv test_telegram_bot/test_telegram_bot.py


.PHONY: run-codegen
run-codegen:
	cd $(PROJECT_ROOT) && datamodel-codegen --input schemas/services/ai-proxy/api/api.yaml --output services/telegram-bot/telegram_bot/generated/ai_proxy_api.py --input-file-type openapi
	cd $(PROJECT_ROOT) && datamodel-codegen --input services/telegram-bot/docs/yaml/types.yaml --output services/telegram-bot/telegram_bot/generated/types.py --input-file-type openapi


.PHONY: run-service
run-service:
	python -m telegram_bot.main


.PHONY: install-requirements
install-requirements:
	pip install -r requirements.txt


.PHONY: cache-delete
cache-delete:
	find . -type d -name "__pycache__" | xargs rm -rf
	rm -rf test_telegram_bot/pytest_cache
