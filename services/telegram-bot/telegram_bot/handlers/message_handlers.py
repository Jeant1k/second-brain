from telegram import Update
from telegram.ext import CallbackContext
from telegram_bot.internal.task_manager import TaskManager
from telegram_bot.internal.constants import (
    BUTTON_TO_TASK_TYPE,
    TASK_DEFINE_QUESTION,
    TASK_DEFINE_ERROR,
    UNKNOWN_ERROR,
    EMPTY_LIST_MESSAGE
)
from telegram_bot.keyboards.reply_keyboards import (
    get_confirmation_keyboard,
    get_main_keyboard
)
from telegram_bot.internal.utils import format_tasks_list, get_task_type_readable_name
from telegram_bot.internal.exceptions import AIProxyError, TaskStorageError
from telegram_bot.internal.logger import get_logger

logger = get_logger()
task_manager = TaskManager()


async def handle_text_message(update: Update, context: CallbackContext) -> None:
    """Обрабатывает текстовые сообщения как задачи"""
    user_id = str(update.effective_user.id)
    task_description = update.message.text.strip()
    
    logger.info(f"User {user_id} sent task: {task_description}")
    
    try:
        # Определяем тип задачи с помощью AI
        task_type, readable_name = await task_manager.define_task_type(task_description)
        
        # Сохраняем в контексте для использования в обработчике callback
        context.user_data["current_task"] = {
            "description": task_description,
            "task_type": task_type,
            "readable_name": readable_name
        }
        
        # Спрашиваем подтверждение у пользователя
        message = TASK_DEFINE_QUESTION.format(readable_name)
        await update.message.reply_text(
            message,
            reply_markup=get_confirmation_keyboard()
        )
        
    except AIProxyError as e:
        logger.error(f"AI Proxy error for user {user_id}: {e}")
        await update.message.reply_text(
            TASK_DEFINE_ERROR,
            reply_markup=get_main_keyboard()
        )
    except Exception as e:
        logger.error(f"Unexpected error for user {user_id}: {e}")
        await update.message.reply_text(
            UNKNOWN_ERROR,
            reply_markup=get_main_keyboard()
        )


async def handle_list_button(update: Update, context: CallbackContext) -> None:
    """Обрабатывает нажатие на кнопки со списками задач"""
    user_id = str(update.effective_user.id)
    button_text = update.message.text
    
    logger.info(f"User {user_id} pressed button: {button_text}")
    
    # Получаем тип задачи соответствующий кнопке
    if button_text not in BUTTON_TO_TASK_TYPE:
        await update.message.reply_text(
            "Неизвестная кнопка. Используйте кнопки на клавиатуре.",
            reply_markup=get_main_keyboard()
        )
        return
    
    task_type = BUTTON_TO_TASK_TYPE[button_text]
    
    try:
        # Получаем задачи из хранилища
        tasks = await task_manager.get_tasks_by_type(user_id, task_type)
        
        # Форматируем и отправляем пользователю
        if not tasks:
            await update.message.reply_text(
                EMPTY_LIST_MESSAGE.format(button_text),
                reply_markup=get_main_keyboard()
            )
        else:
            message = format_tasks_list(tasks, button_text)
            await update.message.reply_text(
                message,
                reply_markup=get_main_keyboard()
            )
            
    except TaskStorageError as e:
        logger.error(f"Task storage error for user {user_id}: {e}")
        await update.message.reply_text(
            f"Не удалось получить список '{button_text}'. Пожалуйста, попробуйте позже.",
            reply_markup=get_main_keyboard()
        )
    except Exception as e:
        logger.error(f"Unexpected error for user {user_id}: {e}")
        await update.message.reply_text(
            UNKNOWN_ERROR,
            reply_markup=get_main_keyboard()
        )
