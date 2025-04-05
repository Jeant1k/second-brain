import pytest
import sys

from unittest.mock import MagicMock, patch
from fastapi.testclient import TestClient

from ai_proxy.main import app
from ai_proxy.generated.models import TaskType


ai_proxy_client = TestClient(app)

# Фикстура для мокирования YCloudML
@pytest.fixture
def mock_yandex_cloud_ml_sdk():
    with patch("ai_proxy.api.task_define.YCloudML") as mock_yandex_cloud_ml_sdk:
        # Создаем мок объекты для цепочки вызовов
        mock_instance = MagicMock()
        mock_models = MagicMock()
        mock_text_classifiers = MagicMock()
        mock_configure = MagicMock()
        mock_run = MagicMock()

        # Настраиваем цепочку вызовов
        mock_yandex_cloud_ml_sdk.return_value = mock_instance
        mock_instance.models = mock_models
        mock_models.text_classifiers.return_value = mock_text_classifiers
        mock_text_classifiers.configure.return_value = mock_configure
        
        # Сохраняем мок run для настройки в тестах
        mock_configure.run = mock_run
        
        yield mock_configure.run
