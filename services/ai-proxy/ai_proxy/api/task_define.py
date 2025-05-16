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
            task_description="Ты — ядро интеллектуального персонального ассистента для управления задачами. Твоя задача — классифицировать входящий текст от пользователя по одному из четырех типов. Пользователь взаимодействует с системой через Telegram-бота, а твоя классификация определяет, в какой список GTD (скрытый от пользователя) попадет его запись, или будет ли она отброшена. \n\nВот описание типов:\n- 'current_actions': Текст описывает конкретное, выполнимое действие или задачу, которую пользователь должен совершить сам. Это может быть поручение самому себе, план что-то сделать, напоминание о необходимости совершить активное действие. Примеры: 'Купить молоко', 'Позвонить Ивану завтра в 10', 'Подготовить отчет к среде'.\n- 'waiting': Текст описывает ситуацию, где пользователь ожидает чего-то от другого человека, системы или события, прежде чем сможет предпринять следующее действие, или просто отслеживает процесс, за который ответственен кто-то другой. Ключевым является ожидание или зависимость от внешнего фактора. Примеры: 'Жду ответа от клиента по смете', 'Узнать, когда курьер доставит посылку', 'Контролировать, чтобы подрядчик выслал документы до пятницы'.\n- 'notes': Текст содержит информацию, которую пользователь хочет сохранить: идею, мысль, заметку, контактные данные, ссылку, рецепт, пароль и т.п. Это не активная задача, а справочная информация или заготовка на будущее. Примеры: 'Идея для стартапа: приложение для учета растений', 'Пароль от Wi-Fi: MySecurePassword123', 'Интересная статья о квантовых компьютерах: [ссылка]'.\n- 'trash': Текст не несет смысла с точки зрения управления задачами или заметками, является случайным набором символов, приветствием, вопросом не по теме, слишком общим или неясным утверждением, которое невозможно преобразовать в задачу или заметку. Примеры: 'привет', 'как дела?', 'аолыфдлвоаы', 'хммм..', 'хорошая погода'.\n\nПроанализируй текст пользователя и определи наиболее подходящий тип. Если есть сомнение между задачей и заметкой, но текст содержит глагол повелительного наклонения или явное указание на действие, скорее всего, это 'current_actions'. Если это просто констатация факта или идея без явного указания 'сделать', это 'notes'. Если есть явное указание на зависимость от другого лица/события, это 'waiting'.",
            labels=[
                task_type.value for task_type in TaskType
            ],
            samples=[
                {
                    "text": "Нужно не забыть поздравить Машу с днем рождения 15го числа",
                    "label": "current_actions"
                },
                {
                    "text": "Купить билеты на поезд в Санкт-Петербург на следующие выходные",
                    "label": "current_actions"
                },
                {
                    "text": "Дописать главу 3 ВКРБ к вечеру пятницы",
                    "label": "current_actions"
                },
                {
                    "text": "Перезвонить по поводу вакансии, номер +79XXXXXXXXX",
                    "label": "current_actions"
                },
                {
                    "text": "Когда Иван пришлет макет, сверстать главную страницу",
                    "label": "waiting"
                },
                {
                    "text": "Жду подтверждения встречи от Алексея",
                    "label": "waiting"
                },
                {
                    "text": "Нужно проконтролировать, чтобы отдел закупок оплатил счет №123",
                    "label": "waiting"
                },
                {
                    "text": "Узнать у техподдержки статус моей заявки #A582Z",
                    "label": "waiting"
                },
                {
                    "text": "Идея для отпуска: поехать на Алтай",
                    "label": "notes"
                },
                {
                    "text": "Рецепт шарлотки: яблоки, мука, яйца, сахар. Запекать 40 минут.",
                    "label": "notes"
                },
                {
                    "text": "Полезная ссылка по C++ userver: https://userver.tech/docs/",
                    "label": "notes"
                },
                {
                    "text": "Конференция по AI будет в октябре, возможно, стоит посетить",
                    "label": "notes"
                },
                {
                    "text": "фывфыаолдж",
                    "label": "trash"
                },
                {
                    "text": "Просто сообщение для теста",
                    "label": "trash"
                },
                {
                    "text": "Привет, как твои дела?",
                    "label": "trash"
                },
                {
                    "text": "Ну это самое",
                    "label": "trash"
                }
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
