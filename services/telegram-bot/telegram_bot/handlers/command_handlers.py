from telegram import Update
from telegram.ext import CallbackContext
from telegram_bot.keyboards.reply_keyboards import get_main_keyboard
from telegram_bot.internal.constants import WELCOME_MESSAGE
from telegram_bot.internal.logger import get_logger

logger = get_logger()


async def start_command(update: Update, context: CallbackContext) -> None:
    """Обработчик команды /start"""
    logger.info(f"User {update.effective_user.id} started the bot")
    
    # Отправляем приветственное сообщение и клавиатуру
    await update.message.reply_text(
        WELCOME_MESSAGE,
        reply_markup=get_main_keyboard()
    )


async def help_command(update: Update, context: CallbackContext) -> None:
    """Обработчик команды /help"""
    logger.info(f"User {update.effective_user.id} requested help")
    
    # Отправляем сообщение с помощью
    help_text = (
        "Я твой личный ассистент SECOND BRAIN. Вот что я умею:\n\n"
        "1. Отправь мне любую задачу текстом, и я определю, в какой список её поместить\n"
        "2. Используй кнопки для просмотра задач в каждом списке\n"
        "3. Все твои задачи автоматически сохраняются и организуются\n\n"
        "Команды:\n"
        "/start - Запустить бота\n"
        "/help - Показать справку"
    )
    
    await update.message.reply_text(
        help_text,
        reply_markup=get_main_keyboard()
    )
