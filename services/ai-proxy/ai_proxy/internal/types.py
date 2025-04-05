from typing import Dict, List, Optional, Any
from pydantic import BaseModel


class ClassificationResult(BaseModel):
    label: str
    score: float
