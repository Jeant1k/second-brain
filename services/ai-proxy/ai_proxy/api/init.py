from fastapi import APIRouter

from ai_proxy.api.task_define import router as task_define_router

router = APIRouter()
router.include_router(task_define_router)
