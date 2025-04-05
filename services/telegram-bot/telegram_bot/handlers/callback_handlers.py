from telegram import Update
from telegram.ext import CallbackContext
from telegram_bot.internal.task_manager import TaskManager
from telegram_bot.internal.constants import (
    SUCCESSFUL_TASK_SAVE,
    TASK_SAVE_ERROR,
    UNKNOWN_ERROR
)
from telegram_bot.keyboards.reply_keyboards import (
    get_main_keyboard,
    get_task_types_keyboard
)
from telegram_bot.internal.exceptions import TaskStorageError
from telegram_bot.internal.logger import get_logger

logger = get_logger()
task_manager = TaskManager()


async def handle_confirmation_callback(update: Update, context: CallbackContext) -> None:
    """Обрабатывает callbacks от кнопок подтверждения"""
    query = update.callback_query
    user_id = str(update.effective_user.id)
    
    await query.answer()
    
    # Получаем действие из callback_data
    action = query.data.split("_")[1]  # confirm_yes или confirm_no
    
    # Проверяем, есть ли текущая задача в контексте
    if "current_task" not in context.user_data:
        await query.edit_message_text(
            "Произошла ошибка. Пожалуйста, введите задачу заново."
        )
        return
    
    current_task = context.user_data["current_task"]
    
    if action == "yes":
        logger.info(f"User {user_id} confirmed task type: {current_task['task_type']}")
        
        try:
            # Сохраняем задачу
            success = await task_manager.save_task(
                user_id=user_id,
                task_description=current_task["description"],
                task_type=current_task["task_type"]
            )
            
            if success:
                message = SUCCESSFUL_TASK_SAVE.format(current_task["readable_name"])
                await query.edit_message_text(message)
            else:
                await query.edit_message_text(TASK_SAVE_ERROR)
                
        except TaskStorageError as e:
            logger.error(f"Task storage error for user {user_id}: {e}")
            await query.edit_message_text(TASK_SAVE_ERROR)
        except Exception as e:
            logger.error(f"Unexpected error for user {user_id}: {e}")
            await query.edit_message_text(UNKNOWN_ERROR)
            
    elif action == "no":
        logger.info(f"User {user_id} declined task type, showing all types")
        await query.edit_message_text(
            "Пожалуйста, выберите подходящий список для задачи:",
            reply_markup=get_task_types_keyboard()
        )
        
    # Удаляем текущую задачу из контекста
    if action == "yes":
        del context.user_data["current_task"]


async def handle_task_type_selection(update: Update, context: CallbackContext) -> None:
    """Обрабатывает выбор типа задачи"""
    query = update.callback_query
    user_id = str(update.effective_user.id)
    
    await query.answer()
    
    # Получаем выбранный тип задачи из callback_data
    selected_type = query.data.split("_")[1]  # type_task_type
    
    # Проверяем, есть ли текущая задача в контексте
    if "current_task" not in context.user_data:
        await query.edit_message_text(
            "Произошла ошибка. Пожалуйста, введите задачу заново."
        )
        return
    
    current_task = context.user_data["current_task"]
    current_task["task_type"] = selected_type
    
    logger.info(f"User {user_id} manually selected task type: {selected_type}")
    
    try:
        # Сохраняем задачу с выбранным типом
        success = await task_manager.save_task(
            user_id=user_id,
            task_description=current_task["description"],
            task_type=selected_type
        )
        
        if success:
            message = SUCCESSFUL_TASK_SAVE.format(selected_type)
            await query.edit_message_text(message)
        else:
            await query.edit_message_text(TASK_SAVE_ERROR)
            
    except TaskStorageError as e:
        logger.error(f"Task storage error for user {user_id}: {e}")
        await query.edit_message_text(TASK_SAVE_ERROR)
    except Exception as e:
        logger.error(f"Unexpected error for user {user_id}: {e}")
        await query.edit_message_text(UNKNOWN_ERROR)
    
    # Удаляем текущую задачу из контекста
    del context.user_data["current_task"]
