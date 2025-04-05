from pydantic import BaseModel, Field
from datetime import datetime
from typing import Optional
from telegram_bot.generated.ai_proxy_api import TaskType


class Task(BaseModel):
    """Модель задачи"""
    id: Optional[str] = None
    description: str
    task_type: str
    user_id: str
    created_at: Optional[datetime] = Field(default_factory=datetime.utcnow)
