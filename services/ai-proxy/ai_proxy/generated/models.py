# generated by datamodel-codegen:
#   filename:  api.yaml
#   timestamp: 2025-05-16T17:29:30+00:00

from __future__ import annotations

from enum import Enum

from pydantic import BaseModel, Extra, Field


class V1TaskDefineRequest(BaseModel):
    class Config:
        extra = Extra.forbid

    task: str = Field(..., description='Описание задачи')


class TaskType(Enum):
    trash = 'trash'
    current_actions = 'current_actions'
    waiting = 'waiting'
    notes = 'notes'


class Error(BaseModel):
    class Config:
        extra = Extra.forbid

    message: str = Field(..., description='Сообщение об ошибке')
    code: str = Field(..., description='Код ошибки')


class V1TaskDefineResponse(BaseModel):
    class Config:
        extra = Extra.forbid

    task_type: TaskType
