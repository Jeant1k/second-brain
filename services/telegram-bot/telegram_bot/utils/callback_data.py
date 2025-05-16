from aiogram.filters.callback_data import CallbackData

class TaskAction(CallbackData, prefix="task"):
    action: str  # view, edit, complete, mv_smtm, mv_curr, del_prompt, del_conf, back
    item_id: str
    item_type: str # curr_act, smt_ltr, waiting, notes, completed
    # для пагинации, если возвращаемся к списку после действия
    # или если действие затрагивает список
    cursor: str | None = None

class Pagination(CallbackData, prefix="page"):
    action: str # prev, next
    item_type: str # curr_act, smt_ltr, waiting, notes, completed
    cursor: str

class FreeTextAction(CallbackData, prefix="freetext"):
    action: str # confirm_delete, retry_input
    original_message_id: int | None = None # Для удаления сообщения "Задача не распознана"
