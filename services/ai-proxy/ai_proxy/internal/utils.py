import os
from typing import Optional

from dotenv import load_dotenv


def get_env_var(key: str, default: Optional[str] = None) -> str:
    """Get environment variable or return default value"""
    value = os.getenv(key, default)
    if value is None:
        raise ValueError(f"Environment variable {key} not set")
    return value


def load_environment():
    """Load environment variables from .env file"""
    load_dotenv()
