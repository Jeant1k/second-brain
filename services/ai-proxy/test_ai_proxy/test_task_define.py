import pytest

from test_ai_proxy.contest import ai_proxy_client, mock_yandex_cloud_ml_sdk


# Тест успешного определения типа задачи
def test_define_task_success(mock_yandex_cloud_ml_sdk):
    # Настраиваем моки
    mock_yandex_cloud_ml_sdk.return_value = [
        {"label": "current_actions", "confidence": 0.9},
        {"label": "notes", "confidence": 0.05}
    ]
    
    # Выполняем запрос
    response = ai_proxy_client.post(
        "/ai-proxy/v1/task/define",
        json={"task": "Создать новый проект по разработке мобильного приложения"}
    )

    # Проверяем результат
    assert response.status_code == 200
    assert response.json() == {"task_type": "current_actions"}


# Тест для каждого возможного типа задачи
@pytest.mark.parametrize(
    "task_text,expected_type,mock_result",
    [
        (
            "алорпвлт", 
            "trash", 
            [
                {"label": "trash", "confidence": 0.9}, 
                {"label": "notes", "confidence": 0.1}
            ],
        ),
        (
            "Идея для нового проекта - создать приложение для учета финансов", 
            "notes", 
            [
                {"label": "notes", "confidence": 0.7}, 
                {"label": "trash", "confidence": 0.3}
            ],
        ),
        (
            "Коллега должен проверить отчет", 
            "waiting", 
            [
                {"label": "waiting", "confidence": 0.9}, 
                {"label": "current_actions", "confidence": 0.1}
            ],
        ),
        (
            "Подготовить презентацию в течение недели", 
            "current_actions", 
            [
                {"label": "current_actions", "confidence": 0.8}, 
                {"label": "notes", "confidence": 0.2}
            ],
        ),
    ],
    ids=[
        "Throw away old things - trash",
        "Idea for a new project - finance app - notes",
        "Waiting report from colleague - waiting",
        "Prepare presentation within a week - current_actions"
    ]
)
def test_different_task_types(mock_yandex_cloud_ml_sdk, task_text, expected_type, mock_result):
    # Настраиваем моки
    mock_yandex_cloud_ml_sdk.return_value = mock_result
    
    # Выполняем запрос
    response = ai_proxy_client.post(
        "/ai-proxy/v1/task/define",
        json={"task": task_text}
    )
    
    # Проверяем результат
    assert response.status_code == 200
    assert response.json() == {"task_type": expected_type}


# Тест с ошибкой в запросе (отсутствует обязательное поле)
def test_bad_request():
    # Выполняем запрос с пустым json
    response = ai_proxy_client.post(
        "/ai-proxy/v1/task/define",
        json={}
    )
    
    # Проверяем результат
    assert response.status_code == 400


# Тест с ошибкой модели
def test_model_exception(mock_yandex_cloud_ml_sdk):
    # Настраиваем моки для возврата исключения
    mock_yandex_cloud_ml_sdk.side_effect = ValueError("Model error")
    
    # Выполняем запрос
    response = ai_proxy_client.post(
        "/ai-proxy/v1/task/define",
        json={"task": "Какая-то задача"}
    )
    
    # Проверяем результат
    assert response.status_code == 422
    response_data = response.json()
    assert response_data["code"] == "classification_error"
    assert "Failed to classify task" in response_data["message"]


# Тест с необработанным исключением
def test_unexpected_exception(mock_yandex_cloud_ml_sdk):
    # Настраиваем моки для возврата исключения
    mock_yandex_cloud_ml_sdk.side_effect = Exception("Unexpected error")
    
    # Выполняем запрос
    response = ai_proxy_client.post(
        "/ai-proxy/v1/task/define",
        json={"task": "Какая-то задача"}
    )
    
    # Проверяем результат
    assert response.status_code == 500
    response_data = response.json()
    assert response_data["code"] == "internal_error"
    assert "Unexpected error during task classification" in response_data["message"]
