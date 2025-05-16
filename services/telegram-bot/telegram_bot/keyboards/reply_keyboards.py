from aiogram.types import ReplyKeyboardMarkup, KeyboardButton

def get_main_menu_keyboard() -> ReplyKeyboardMarkup:
    buttons = [
        [KeyboardButton(text="Задачи"), KeyboardButton(text="Не сейчас")],
        [KeyboardButton(text="Ожидания"), KeyboardButton(text="Заметки")],
        [KeyboardButton(text="Выполнено")]
    ]
    keyboard = ReplyKeyboardMarkup(keyboard=buttons, resize_keyboard=True)
    return keyboard
