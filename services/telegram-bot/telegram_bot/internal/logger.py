from loguru import logger
import sys

# Настройка логгера
logger.remove()
logger.add(
    sys.stderr,
    format="{time} {level} {message}",
    level="DEBUG",
    serialize=False,
)


def get_logger():
    return logger
