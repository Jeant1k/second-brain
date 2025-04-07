import httpx
import json
from telegram_bot.generated.types import Task
from telegram_bot.generated.ai_proxy_api import TaskType
from telegram_bot.internal.constants import (
    TASK_STORAGE_PATH, 
    TASK_STORAGE_BY_TYPE_PATH
)
from telegram_bot.internal.exceptions import TaskStorageError
from telegram_bot.internal.utils import get_env_var
from telegram_bot.internal.logger import get_logger

logger = get_logger()


class TaskStorageClient:
    def __init__(self):
        """Инициализирует клиент хранилища задач"""
        self.base_url = get_env_var("TASK_STORAGE_SERVICE_URL", "http://localhost:8080")
        self.save_task_url = f"{self.base_url}{TASK_STORAGE_PATH}"
        self.get_tasks_by_type_url = f"{self.base_url}{TASK_STORAGE_BY_TYPE_PATH}"

    async def save_task(self, task: Task) -> bool:
        """
        Сохраняет задачу в хранилище
        
        Параметры:
            task: Задача для сохранения
            
        Возвращает:
            bool: True если задача успешно сохранена
            
        Вызывает:
            TaskStorageError: Если произошла ошибка при сохранении
        """
        # В MVP используем заглушку
        logger.info(f"Simulating task save: {task.dict()}")
        return True

        # Реальный код для подключения к сервису
        # try:
        #     async with httpx.AsyncClient() as client:
        #         response = await client.post(
        #             self.save_task_url,
        #             json=task.dict(),
        #             timeout=10.0
        #         )
        #         
        #         if response.status_code != 200:
        #             error_data = response.json()
        #             message = error_data.get("message", "Unknown error")
        #             code = error_data.get("code", "unknown_error")
        #             logger.error(f"Task storage error: {message}")
        #             raise TaskStorageError(message=message, code=code)
        #         
        #         return True
        #         
        # except httpx.HTTPError as e:
        #     logger.error(f"HTTP error when saving task: {str(e)}")
        #     raise TaskStorageError(message=f"Connection error: {str(e)}")
        # except Exception as e:
        #     logger.error(f"Unexpected error when saving task: {str(e)}")
        #     raise TaskStorageError(message="Unexpected error occurred")

    async def get_tasks_by_type(self, user_id: str, task_type: TaskType) -> list[Task]:
        """
        Получает список задач определенного типа
        
        Параметры:
            user_id: Идентификатор пользователя
            task_type: Тип задачи
            
        Возвращает:
            list[Task]: Список задач
            
        Вызывает:
            TaskStorageError: Если произошла ошибка при получении списка задач
        """

        # В MVP используем заглушку
        logger.info(f"Simulating get tasks by type: user_id={user_id}, task_type={task_type}")
        
        # Генерируем тестовые данные для каждого типа
        if task_type == TaskType.completed:
            return [
                Task(description="Завершил отчет по проекту", task_type=task_type.value, user_id=user_id),
                Task(description="Ответил на все письма", task_type=task_type.value, user_id=user_id)
            ]
        
        # Для остальных типов возвращаем список с 1-2 задачами
        example_tasks = {
            TaskType.trash: ["Старая заметка про идею", "Устаревшая информация"],
            TaskType.someday_maybe: ["Изучить новый язык программирования", "Прочитать книгу о GTD"],
            TaskType.notes: ["Идея для приложения: помощник по планированию", "Мысли о структуре проекта"],
            TaskType.do_it: ["Отправить email", "Позвонить клиенту"],
            TaskType.delegate: ["Ожидаю ответа от команды", "Жду документы от бухгалтерии"],
            TaskType.current_actions: ["Разработка MVP", "Тестирование интерфейса"],
            TaskType.calendar: ["Встреча в пятницу", "Дедлайн по проекту 20 мая"],
            TaskType.project: ["Дипломный проект", "Разработка бота SECOND BRAIN"]
        }
        
        tasks = example_tasks.get(task_type, ["Пример задачи"])
        return [Task(description=task, task_type=task_type.value, user_id=user_id) for task in tasks]

        # Реальный код для подключения к сервису
        # try:
        #     params = {"user_id": user_id, "task_type": task_type}
        #     async with httpx.AsyncClient() as client:
        #         response = await client.get(
        #             self.get_tasks_by_type_url,
        #             params=params,
        #             timeout=10.0
        #         )
        #         
        #         if response.status_code != 200:
        #             error_data = response.json()
        #             message = error_data.get("message", "Unknown error")
        #             code = error_data.get("code", "unknown_error")
        #             logger.error(f"Task storage error: {message}")
        #             raise TaskStorageError(message=message, code=code)
        #         
        #         tasks_data = response.json()
        #         return [Task(**task_data) for task_data in tasks_data]
        #         
        # except httpx.HTTPError as e:
        #     logger.error(f"HTTP error when getting tasks: {str(e)}")
        #     raise TaskStorageError(message=f"Connection error: {str(e)}")
        # except Exception as e:
        #     logger.error(f"Unexpected error when getting tasks: {str(e)}")
        #     raise TaskStorageError(message="Unexpected error occurred")
