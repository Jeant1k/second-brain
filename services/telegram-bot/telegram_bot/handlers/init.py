from telegram.ext import CommandHandler, MessageHandler, CallbackQueryHandler, filters
from .command_handlers import start_command, help_command
from .message_handlers import handle_text_message, handle_list_button
from .callback_handlers import handle_confirmation_callback, handle_task_type_selection
from telegram_bot.internal.constants import (
    CURRENT_ACTIONS_BTN,
    PROJECTS_BTN,
    WAITING_BTN,
    COMPLETED_BTN,
    SOMEDAY_BTN,
    NOTES_BTN,
    TRASH_BTN
)


def register_handlers(application):
    """Регистрирует обработчики сообщений и команд"""
    
    # Обработчики команд
    application.add_handler(CommandHandler("start", start_command))
    application.add_handler(CommandHandler("help", help_command))
    
    # Обработчики кнопок списков
    list_buttons = filters.Regex(f"^({CURRENT_ACTIONS_BTN}|{PROJECTS_BTN}|{WAITING_BTN}|{COMPLETED_BTN}|{SOMEDAY_BTN}|{NOTES_BTN}|{TRASH_BTN})$")
    application.add_handler(MessageHandler(list_buttons, handle_list_button))
    
    # Обработчики callback-запросов
    application.add_handler(CallbackQueryHandler(handle_confirmation_callback, pattern="^confirm_"))
    application.add_handler(CallbackQueryHandler(handle_task_type_selection, pattern="^type_"))
    
    # Обработчик текстовых сообщений (задач)
    application.add_handler(MessageHandler(filters.TEXT & ~list_buttons, handle_text_message))
