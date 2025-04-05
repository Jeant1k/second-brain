import pytest

from test_ai_proxy.contest import ai_proxy_client, mock_yandex_cloud_ml_sdk


# Тест успешного определения типа задачи
def test_define_task_success(mock_yandex_cloud_ml_sdk):
    # Настраиваем моки
    mock_yandex_cloud_ml_sdk.return_value = [
        {"label": "project", "confidence": 0.9},
        {"label": "current_actions", "confidence": 0.05},
        {"label": "notes", "confidence": 0.05}
    ]
    
    # Выполняем запрос
    response = ai_proxy_client.post(
        "/ai-proxy/v1/task/define",
        json={"task": "Создать новый проект по разработке мобильного приложения"}
    )

    # Проверяем результат
    assert response.status_code == 200
    assert response.json() == {"task_type": "project"}


# Тест для каждого возможного типа задачи
@pytest.mark.parametrize(
    "task_text,expected_type,mock_result",
    [
        (
            "Выбросить старые вещи", 
            "trash", 
            [
                {"label": "trash", "confidence": 0.9}, 
                {"label": "someday_maybe", "confidence": 0.1}
            ],
        ),
        (
            "Когда-нибудь хорошо бы изучить испанский", 
            "someday_maybe", 
            [
                {"label": "someday_maybe", "confidence": 0.8}, 
                {"label": "notes", "confidence": 0.2}
            ],
        ),
        (
            "Напоминание изучить испанский через 3 месяца", 
            "trigger_someday_maybe", 
            [
                {"label": "trigger_someday_maybe", "confidence": 0.85}, 
                {"label": "calendar", "confidence": 0.15}
            ],
        ),
        (
            "Идея для нового проекта - создать приложение для учета финансов", 
            "notes", 
            [
                {"label": "notes", "confidence": 0.7}, 
                {"label": "project", "confidence": 0.3}
            ],
        ),
        (
            "Отправить email клиенту сегодня", 
            "do_it", 
            [
                {"label": "do_it", "confidence": 0.95}, 
                {"label": "current_actions", "confidence": 0.05}
            ],
        ),
        (
            "Поручить коллеге проверить отчет", 
            "delegate", 
            [
                {"label": "delegate", "confidence": 0.9}, 
                {"label": "do_it", "confidence": 0.1}
            ],
        ),
        (
            "Подготовить презентацию в течение недели", 
            "current_actions", 
            [
                {"label": "current_actions", "confidence": 0.8}, 
                {"label": "project", "confidence": 0.2}
            ],
        ),
        (
            "Встреча с командой в среду в 15:00", 
            "calendar", 
            [
                {"label": "calendar", "confidence": 0.95}, 
                {"label": "current_actions", "confidence": 0.05}
            ],
        ),
        (
            "Разработка нового веб-сайта компании", 
            "project", 
            [
                {"label": "project", "confidence": 0.85}, 
                {"label": "current_actions", "confidence": 0.15}
            ],
        ),
    ],
    ids=[
        "Throw away old things - trash",
        "Someday learn Spanish - someday_maybe",
        "Reminder to learn Spanish in 3 months - trigger_someday_maybe",
        "Idea for a new project - finance app - notes",
        "Send email to client today - do_it",
        "Delegate report checking to colleague - delegate",
        "Prepare presentation within a week - current_actions",
        "Team meeting on Wednesday at 15:00 - calendar",
        "Company website development - project"
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
