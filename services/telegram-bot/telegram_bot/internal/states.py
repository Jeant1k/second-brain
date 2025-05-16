from aiogram.fsm.state import State, StatesGroup

class CommonStates(StatesGroup):
    waiting_for_new_item_name = State() # Для переименования задачи/заметки
    waiting_for_retry_task_input = State() # Для повторного ввода после "мусора"
