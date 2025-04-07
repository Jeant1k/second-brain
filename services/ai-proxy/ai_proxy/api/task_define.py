from fastapi import APIRouter, Depends
from yandex_cloud_ml_sdk import YCloudML
from datetime import datetime

from ai_proxy.generated.models import V1TaskDefineRequest, V1TaskDefineResponse, TaskType
from ai_proxy.internal.exceptions import UnprocessableEntityError, InternalServerError
from ai_proxy.internal.types import ClassificationResult
from ai_proxy.internal.logger import logger
from ai_proxy.internal.utils import get_env_var


router = APIRouter()


def get_ml_client() -> YCloudML:
    """Get Yandex Cloud ML client"""
    folder_id = get_env_var("FOLDER_ID")
    yc_api_key = get_env_var("YC_API_KEY")
    return YCloudML(folder_id=folder_id, auth=yc_api_key)


@router.post("/ai-proxy/v1/task/define", response_model=V1TaskDefineResponse)
async def define_task(
    request: V1TaskDefineRequest,
    ml_client: YCloudML = Depends(get_ml_client)
) -> V1TaskDefineResponse:
    """Define task type based on task description"""
    logger.debug("Define task requested")

    try:
        # Create and configure classification model
        now = datetime.now().strftime("%A, %d %B %Y, %H:%M")
        model = ml_client.models.text_classifiers("yandexgpt").configure(
            task_description=f"Текущее дата и время: {now}. Классифицируй входящую задачу в соответствии с методологией тайм-менеджмента GTD (Getting Things Done). Определи, к какой из 9 категорий относится задача на основе её содержания, сроков, сложности выполнения и необходимых ресурсов. Учитывай контекст, явные и неявные временные рамки, количество необходимых действий для завершения, и требуемое участие других людей.",
            labels=[
                task_type.value for task_type in TaskType
            ],
            samples=[
                {"text": "Dfsjkdhfkjsd sdjfhksdjf", "label": "trash"},
                {"text": "Посетить Японию и увидеть цветение сакуры", "label": "someday_maybe"},
                {"text": "Научиться играть на гитаре", "label": "someday_maybe"},
                # {"text": "Летом следующего года организовать поездку в Грузию с друзьями", "label": "trigger_someday_maybe"},
                # {"text": "Записаться на курсы по программированию в следующем месяце", "label": "trigger_someday_maybe"},
                {"text": "Идея для будущей презентации: сравнить результаты продаж за последние 3 квартала и выделить ключевые моменты роста", "label": "notes"},
                {"text": "Рецепт борща от бабушки: 500г говядины, 3 картофелины, 1 свекла, 1 морковь, 1 луковица, 200г капусты, 2 ст.л. томатной пасты, соль, перец, лавровый лист", "label": "notes"},
                {"text": "Позвонить в службу поддержки и уточнить статус заказа", "label": "do_it"},
                {"text": "Отправить деньги на свой вклад", "label": "do_it"},
                {"text": "Попросить системного администратора настроить доступ к новому серверу", "label": "delegate"},
                {"text": "Починить полку", "label": "delegate"},
                {"text": "Заполнить заявление на отпуск и отправить в HR", "label": "current_actions"},
                {"text": "Составить список необходимых покупок для ремонта", "label": "current_actions"},
                {"text": "Провести презентацию проекта для руководства 25 числа следующего месяца", "label": "calendar"},
                {"text": "Встретиться с другом в театре в 14:00 15 марта", "label": "calendar"},
                {"text": "Организовать корпоративное мероприятие к годовщине компании", "label": "project"},
                {"text": "Переехать в новую квартиру до конца месяца", "label": "project"},
            ]
        )

        # Run classification
        results = model.run(request.task)
        logger.debug(f"Obtained result from model = {results}")

        # Convert results to typed objects
        typed_results = []
        for prediction in results:
            label = prediction.get('label')
            score = float(prediction.get('confidence'))
            typed_results.append(ClassificationResult(label=label, score=score))

        # Sort by score in descending order
        typed_results.sort(key=lambda x: x.score, reverse=True)
        logger.debug(f"Sorted result = {typed_results}")
        
        # Get task type with highest score
        best_result = typed_results[0]
        
        return V1TaskDefineResponse(task_type=TaskType(best_result.label))
    
    except ValueError as e:
        raise UnprocessableEntityError(
            message=f"Failed to classify task: {str(e)}",
            code="classification_error"
        )
    except Exception as e:
        raise InternalServerError(
            message=f"Unexpected error during task classification: {str(e)}",
            code="internal_error"
        )
