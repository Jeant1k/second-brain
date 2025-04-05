.PHONY: run-tests
run-tests:
	pytest --log-cli-level=DEBUG -vv test_ai_proxy/test_task_define.py


.PHONY: run-codegen
run-codegen:
	datamodel-codegen --input docs/yaml/api/api.yaml --output ai_proxy/generated/models.py --input-file-type openapi


.PHONY: run-service
run-service:
	python -m ai_proxy.main


.PHONY: install-requirements
install-requirements:
	pip install -r requirements.txt


.PHONY: cache-delete
cache-delete:
	find . -type d -name "__pycache__" | xargs rm -rf
	rm -rf test_ai_proxy/pytest_cache
