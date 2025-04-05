from fastapi import HTTPException, status


class BadRequestError(HTTPException):
    def __init__(self, message: str, code: str = "bad_request"):
        super().__init__(
            status_code=status.HTTP_400_BAD_REQUEST,
            detail={"message": message, "code": code}
        )


class UnprocessableEntityError(HTTPException):
    def __init__(self, message: str, code: str = "unprocessable_entity"):
        super().__init__(
            status_code=status.HTTP_422_UNPROCESSABLE_ENTITY,
            detail={"message": message, "code": code}
        )


class InternalServerError(HTTPException):
    def __init__(self, message: str = "Internal server error", code: str = "internal_server_error"):
        super().__init__(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail={"message": message, "code": code}
        )
