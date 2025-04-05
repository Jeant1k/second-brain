import uvicorn
from fastapi import FastAPI, Request
from fastapi.responses import JSONResponse
from fastapi.middleware.cors import CORSMiddleware
from fastapi.exceptions import RequestValidationError

from ai_proxy.api.init import router
from ai_proxy.internal.exceptions import BadRequestError, UnprocessableEntityError, InternalServerError
from ai_proxy.internal.utils import load_environment, get_env_var
from ai_proxy.internal.logger import logger


# Load environment variables
load_environment()

# Create FastAPI app
app = FastAPI(
    title="AI Proxy API",
    description="AI proxy service for LLM models",
    version="1.0.0",
)

# Add CORS middleware
app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

# Add API routes
app.include_router(router)

# Exception handlers
@app.exception_handler(BadRequestError)
@app.exception_handler(UnprocessableEntityError)
@app.exception_handler(InternalServerError)
async def http_exception_handler(request: Request, exc):
    return JSONResponse(
        status_code=exc.status_code,
        content=exc.detail,
    )

@app.exception_handler(RequestValidationError)
async def validation_exception_handler(request: Request, exc: RequestValidationError):
    formatted_errors = []
    for error in exc.errors():
        loc = " -> ".join([str(loc_item) for loc_item in error["loc"]])
        formatted_errors.append(f"{loc}: {error['msg']}")
    
    error_message = "; ".join(formatted_errors)
    
    error = BadRequestError(
        message=f"Validation error: {error_message}",
        code="validation_error"
    )
    
    return JSONResponse(
        status_code=error.status_code,
        content=error.detail,
    )

# Health check endpoint
@app.get("/health")
async def health_check():
    logger.info("Health check requested")
    return {"status": "ok"}

if __name__ == "__main__":
    host = get_env_var("HOST", "0.0.0.0")
    port = int(get_env_var("PORT", "8000"))
    debug = get_env_var("DEBUG", "false").lower() == "true"
    
    uvicorn.run(
        "ai_proxy.main:app",
        host=host,
        port=port,
        reload=debug,
    )
