import asyncio
import logging
import os

from aiogram import Bot, Dispatcher, F
from aiogram.fsm.storage.memory import MemoryStorage
from aiogram.types import BotCommand, BotCommandScopeDefault
from dotenv import load_dotenv

# Импортируем роутеры
from telegram_bot.handlers import common_handlers, free_text_handler, task_list_handlers, task_action_handlers
from telegram_bot.clients.api_client import api_client # для инициализации при старте, если нужно

# Загрузка переменных окружения
load_dotenv()

TELEGRAM_BOT_TOKEN = os.getenv("TELEGRAM_BOT_TOKEN")
if not TELEGRAM_BOT_TOKEN:
    exit("Error: no TELEGRAM_BOT_TOKEN provided")

# Настройка логирования
logging.basicConfig(
    level=logging.INFO,
    format="%(asctime)s - %(levelname)s - %(name)s - %(message)s",
)
logger = logging.getLogger(__name__)


async def set_commands(bot: Bot):
    commands = [
        BotCommand(command="start", description="🚀 Запустить/Перезапустить бота"),
        BotCommand(command="help", description="ℹ️ Помощь по командам"),
        BotCommand(command="support", description="🛠️ Поддержка"),
        BotCommand(command="cancel", description="❌ Отменить текущее действие")
    ]
    await bot.set_my_commands(commands, BotCommandScopeDefault())


async def main():
    bot = Bot(token=TELEGRAM_BOT_TOKEN)
    storage = MemoryStorage() # Используем MemoryStorage для FSM
    dp = Dispatcher(storage=storage)

    # Регистрация роутеров
    dp.include_router(common_handlers.router)
    dp.include_router(task_list_handlers.router) # Должен идти до free_text, если кнопки совпадают с текстом
    dp.include_router(task_action_handlers.router) # Callback-хендлеры
    dp.include_router(free_text_handler.router)   # Хендлер для свободного текста должен быть последним из Message хендлеров

    # Установка команд меню
    await set_commands(bot)
    
    logger.info("Bot is starting...")
    # Запуск polling
    try:
        await dp.start_polling(bot, allowed_updates=dp.resolve_used_update_types())
    finally:
        await bot.session.close()
        logger.info("Bot has been stopped.")


if __name__ == "__main__":
    try:
        asyncio.run(main())
    except (KeyboardInterrupt, SystemExit):
        logger.info("Bot stopped by user.")
