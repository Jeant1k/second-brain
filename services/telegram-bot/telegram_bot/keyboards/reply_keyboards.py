from telegram import InlineKeyboardButton, InlineKeyboardMarkup, ReplyKeyboardMarkup
from telegram_bot.internal.constants import (
    CURRENT_ACTIONS_BTN,
    PROJECTS_BTN,
    WAITING_BTN,
    COMPLETED_BTN,
    SOMEDAY_BTN,
    NOTES_BTN,
    TRASH_BTN,
    YES_BTN,
    NO_BTN,
    TASK_TYPE_TO_READABLE,
)
from telegram_bot.generated.ai_proxy_api import TaskType


def get_main_keyboard() -> ReplyKeyboardMarkup:
    """Создает основную клавиатуру с кнопками для списков задач"""
    keyboard = [
        [CURRENT_ACTIONS_BTN, WAITING_BTN],
        [SOMEDAY_BTN, COMPLETED_BTN],
        [PROJECTS_BTN, NOTES_BTN, TRASH_BTN]
    ]
    return ReplyKeyboardMarkup(keyboard, resize_keyboard=True)


def get_confirmation_keyboard() -> InlineKeyboardMarkup:
    """Создает клавиатуру для подтверждения выбора списка"""
    keyboard = [
        [
            InlineKeyboardButton(YES_BTN, callback_data=f"confirm_yes"),
            InlineKeyboardButton(NO_BTN, callback_data=f"confirm_no")
        ]
    ]
    return InlineKeyboardMarkup(keyboard)


def get_task_types_keyboard() -> InlineKeyboardMarkup:
    """Создает клавиатуру со всеми возможными типами задач"""
    keyboard = []
    
    # Создаем кнопки для каждого типа задач
    for task_type in TaskType:
        button_text = TASK_TYPE_TO_READABLE.get(task_type, str(task_type))
        keyboard.append([
            InlineKeyboardButton(button_text, callback_data=f"type_{task_type.value}")
        ])
    
    return InlineKeyboardMarkup(keyboard)
