import logging
from logging.config import dictConfig

from ai_proxy.internal.utils import get_env_var


# Configure logging
dictConfig({
    "version": 1,
    "disable_existing_loggers": False,
    "formatters": {
        "default": {
            "()": "uvicorn.logging.DefaultFormatter",
            "fmt": "%(levelprefix)s %(message)s",
        },
    },
    "handlers": {
        "default": {
            "formatter": "default",
            "class": "logging.StreamHandler",
            "stream": "ext://sys.stdout",
        },
    },
    "loggers": {
        "ai_proxy": {
            "handlers": ["default"],
            "level": get_env_var("LOG_LEVEL", "INFO"),
        },
    },
})

logger = logging.getLogger("ai_proxy")
