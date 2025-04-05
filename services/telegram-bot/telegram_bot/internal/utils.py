import os
from dotenv import load_dotenv
from telegram_bot.generated.ai_proxy_api import TaskType
from telegram_bot.internal.constants import TASK_TYPE_TO_READABLE

# Загрузка переменных окружения
load_dotenv()


def get_task_type_readable_name(task_type: TaskType) -> str:
    """Получает человеко-читаемое название типа задачи"""
    return TASK_TYPE_TO_READABLE.get(task_type, str(task_type))


def get_env_var(name: str, default=None):
    """Получает переменную окружения с возможностью указать значение по умолчанию"""
    return os.getenv(name, default)


def format_tasks_list(tasks: list, list_name: str) -> str:
    """Форматирует список задач для отображения пользователю"""
    if not tasks:
        return f"Список '{list_name}' пуст."
    
    result = f"Список '{list_name}':\n\n"
    for idx, task in enumerate(tasks, 1):
        result += f"{idx}. {task.description}\n"
    
    return result
