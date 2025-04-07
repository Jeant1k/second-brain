from telegram_bot.clients.ai_proxy_client import AIProxyClient
from telegram_bot.clients.task_storage_client import TaskStorageClient
from telegram_bot.generated.ai_proxy_api import TaskType
from telegram_bot.generated.types import Task
from telegram_bot.internal.utils import get_task_type_readable_name
from telegram_bot.internal.logger import get_logger
from telegram_bot.internal.exceptions import AIProxyError, TaskStorageError

logger = get_logger()


class TaskManager:
    def __init__(self):
        """Инициализирует менеджер задач"""
        self.ai_proxy_client = AIProxyClient()
        self.task_storage_client = TaskStorageClient()

    async def define_task_type(self, task_description: str) -> tuple[TaskType, str]:
        """
        Определяет тип задачи с помощью AI Proxy
        
        Возвращает:
            tuple[TaskType, str]: Тип задачи и его человеко-читаемое название
        """
        try:
            task_type = await self.ai_proxy_client.define_task(task_description)
            readable_name = get_task_type_readable_name(task_type)
            return task_type, readable_name
        except AIProxyError as e:
            logger.error(f"Error defining task type: {e}")
            raise

    async def save_task(self, user_id: str, task_description: str, task_type: TaskType) -> bool:
        """
        Сохраняет задачу в хранилище
        
        Параметры:
            user_id: Идентификатор пользователя
            task_description: Описание задачи
            task_type: Тип задачи
            
        Возвращает:
            bool: True, если задача успешно сохранена, иначе False
        """
        try:
            task = Task(
                description=task_description,
                task_type=task_type.value,
                user_id=user_id
            )
            return await self.task_storage_client.save_task(task)
        except TaskStorageError as e:
            logger.error(f"Error saving task: {e}")
            raise

    async def get_tasks_by_type(self, user_id: str, task_type: TaskType) -> list[Task]:
        """
        Получает список задач определенного типа
        
        Параметры:
            user_id: Идентификатор пользователя
            task_type: Тип задачи
            
        Возвращает:
            list[Task]: Список задач
        """
        try:
            return await self.task_storage_client.get_tasks_by_type(user_id, task_type)
        except TaskStorageError as e:
            logger.error(f"Error getting tasks by type: {e}")
            raise
