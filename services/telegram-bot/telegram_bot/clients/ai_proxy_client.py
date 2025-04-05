import httpx
from telegram_bot.generated.ai_proxy_api import V1TaskDefineRequest, TaskType
from telegram_bot.internal.constants import AI_PROXY_TASK_DEFINE_PATH
from telegram_bot.internal.exceptions import AIProxyError
from telegram_bot.internal.utils import get_env_var
from telegram_bot.internal.logger import get_logger

logger = get_logger()


class AIProxyClient:
    def __init__(self):
        """Инициализирует клиент AI Proxy"""
        self.base_url = get_env_var("AI_PROXY_SERVICE_URL", "http://localhost:8080")
        self.define_task_url = f"{self.base_url}{AI_PROXY_TASK_DEFINE_PATH}"

    async def define_task(self, task_description: str) -> TaskType:
        """
        Отправляет запрос к AI Proxy для определения типа задачи
        
        Параметры:
            task_description: Описание задачи
            
        Возвращает:
            TaskType: Тип задачи
            
        Вызывает:
            AIProxyError: Если произошла ошибка при определении типа задачи
        """
        request_data = V1TaskDefineRequest(task=task_description)
        
        try:
            async with httpx.AsyncClient() as client:
                response = await client.post(
                    self.define_task_url,
                    json=request_data.dict(),
                    timeout=10.0
                )
                
                if response.status_code != 200:
                    error_data = response.json()
                    message = error_data.get("message", "Unknown error")
                    code = error_data.get("code", "unknown_error")
                    logger.error(f"AI Proxy error: {message}")
                    raise AIProxyError(message=message, code=code)
                
                result = response.json()
                return TaskType(result["task_type"])
                
        except httpx.HTTPError as e:
            logger.error(f"HTTP error when defining task: {str(e)}")
            raise AIProxyError(message=f"Connection error: {str(e)}")
        except Exception as e:
            logger.error(f"Unexpected error when defining task: {str(e)}")
            raise AIProxyError(message="Unexpected error occurred")
