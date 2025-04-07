from telegram_bot.generated.ai_proxy_api import TaskType

# Кнопки для основного меню
CURRENT_ACTIONS_BTN = "Текущие действия"
PROJECTS_BTN = "Проекты"
WAITING_BTN = "Ожидание"
COMPLETED_BTN = "Выполнено"
SOMEDAY_BTN = "Когда-нибудь потом"
NOTES_BTN = "Заметки"
TRASH_BTN = "Мусор"

# Кнопки для подтверждения
YES_BTN = "Да"
NO_BTN = "Нет"

# Пути для API
AI_PROXY_TASK_DEFINE_PATH = "/ai-proxy/v1/task/define"
TASK_STORAGE_PATH = "/tasks"
TASK_STORAGE_BY_TYPE_PATH = "/tasks/by-type"

# Маппинг типов задач на человеко-читаемые названия
TASK_TYPE_TO_READABLE = {
    TaskType.trash: "Мусор",
    TaskType.someday_maybe: "Когда-нибудь потом",
    TaskType.notes: "Заметки",
    TaskType.do_it: "Выполнить сразу",
    TaskType.delegate: "Ожидание",
    TaskType.current_actions: "Текущие действия",
    TaskType.calendar: "Календарь",
    TaskType.project: "Проекты",
    TaskType.completed: "Выполнено"
}

EN_READABLE_TO_TASK_TYPE = {
    "current_actions": TaskType.current_actions,
    "project": TaskType.project,
    "delegate": TaskType.delegate,
    "completed": TaskType.completed,
    "someday_maybe": TaskType.someday_maybe,
    "notes": TaskType.notes,
    "trash": TaskType.trash,
    "do_it": TaskType.do_it,
    "calendar": TaskType.calendar
}

# Маппинг кнопок на типы задач
BUTTON_TO_TASK_TYPE = {
    CURRENT_ACTIONS_BTN: TaskType.current_actions,
    PROJECTS_BTN: TaskType.project,
    WAITING_BTN: TaskType.delegate,
    COMPLETED_BTN: TaskType.completed,
    SOMEDAY_BTN: TaskType.someday_maybe,
    NOTES_BTN: TaskType.notes,
    TRASH_BTN: TaskType.trash
}

# Ответы бота
SUCCESSFUL_TASK_SAVE = "Задача успешно сохранена в список '{}'."
TASK_SAVE_ERROR = "Не удалось сохранить задачу. Пожалуйста, попробуйте позже."
TASK_DEFINE_ERROR = "Не удалось определить тип задачи. Пожалуйста, выберите тип вручную."
TASK_DEFINE_QUESTION = "Задача будет добавлена в список '{}'. Всё верно?"
EMPTY_LIST_MESSAGE = "Список '{}' пуст."
UNKNOWN_ERROR = "Произошла неизвестная ошибка. Пожалуйста, попробуйте позже."
WELCOME_MESSAGE = """
Привет! Я твой личный ассистент SECOND BRAIN.
Просто отправь мне свои задачи, и я помогу организовать их по спискам GTD.
Используй кнопки внизу для просмотра существующих списков.
"""
