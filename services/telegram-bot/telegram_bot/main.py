import asyncio
from telegram.ext import ApplicationBuilder
from telegram_bot.handlers import register_handlers
from telegram_bot.internal.utils import get_env_var
from telegram_bot.internal.logger import get_logger

logger = get_logger()


async def main():
    """Основная функция запуска бота"""
    # Получаем токен бота из переменных окружения
    bot_token = get_env_var("TELEGRAM_BOT_TOKEN")
    
    if not bot_token:
        logger.error("TELEGRAM_BOT_TOKEN not found in environment variables")
        exit(1)
    
    logger.info("Starting telegram bot")
    
    # Создаем объект приложения
    application = ApplicationBuilder().token(bot_token).build()
    
    # Регистрируем обработчики
    register_handlers(application)
    
    # Запускаем бота
    await application.initialize()
    await application.start()
    await application.updater.start_polling()
    
    logger.info("Bot started")
    
    # Держим бота запущенным, пока не получим сигнал остановки
    try:
        await application.updater.stop_polling()
        await application.stop()
    except (KeyboardInterrupt, SystemExit):
        logger.info("Bot stopped")


if __name__ == "__main__":
    asyncio.run(main())
