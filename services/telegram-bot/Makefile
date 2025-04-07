PROJECT_ROOT := /home/jean/second_brain

# .PHONY: run-tests
# run-tests:
# 	pytest --log-cli-level=DEBUG -vv test_telegram_bot/test_telegram_bot.py


.PHONY: run-codegen
run-codegen:
	datamodel-codegen --input $(PROJECT_ROOT)/schemas/services/ai-proxy/api/api.yaml --output $(PROJECT_ROOT)/services/telegram-bot/telegram_bot/generated/ai_proxy_api.py --input-file-type openapi
	datamodel-codegen --input $(PROJECT_ROOT)/services/telegram-bot/docs/yaml/types.yaml --output $(PROJECT_ROOT)/services/telegram-bot/telegram_bot/generated/types.py --input-file-type openapi


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
