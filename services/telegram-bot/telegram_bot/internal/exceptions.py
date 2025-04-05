class ServiceError(Exception):
    """Базовый класс для ошибок сервиса"""
    def __init__(self, message="Service error", code="service_error"):
        self.message = message
        self.code = code
        super().__init__(self.message)


class AIProxyError(ServiceError):
    """Ошибка при взаимодействии с сервисом AI Proxy"""
    def __init__(self, message="AI Proxy service error", code="ai_proxy_error"):
        super().__init__(message, code)


class TaskStorageError(ServiceError):
    """Ошибка при взаимодействии с сервисом хранения задач"""
    def __init__(self, message="Task storage service error", code="task_storage_error"):
        super().__init__(message, code)
