from aiogram import Router, F
from aiogram.types import Message
from aiogram.filters import CommandStart, Command
from aiogram.fsm.context import FSMContext
from telegram_bot.keyboards.reply_keyboards import get_main_menu_keyboard
from aiogram.enums import ParseMode

router = Router()

@router.message(CommandStart())
async def handle_start(message: Message, state: FSMContext):
    await state.clear()
    await message.answer(
        f"Привет, {message.from_user.full_name}!\n"
        "Я ваш персональный AI-ассистент для управления задачами.\n"
        "Просто напишите мне задачу, и я помогу ее классифицировать и сохранить.\n"
        "Используйте кнопки ниже для навигации или введите /help для списка команд.",
        reply_markup=get_main_menu_keyboard()
    )

@router.message(Command("help"))
async def handle_help(message: Message, state: FSMContext):
    # await state.clear() # Не всегда нужно очищать состояние по /help
    help_text = (
        "<b>Основные команды:</b>\n"
        "/start - Перезапустить бота и показать главное меню\n"
        "/help - Показать это сообщение\n"
        "/support - Информация о поддержке\n\n"
        "<b>Управление задачами:</b>\n"
        "Просто напишите текст вашей задачи, и AI попытается ее определить.\n"
        "Используйте кнопки на клавиатуре для доступа к спискам:\n"
        "  <b>Задачи</b> - ваши текущие активные задачи.\n"
        "  <b>Не сейчас</b> - отложенные задачи.\n"
        "  <b>Ожидания</b> - задачи, выполнения которых вы ждете от других.\n"
        "  <b>Заметки</b> - простые текстовые заметки.\n"
        "  <b>Выполнено</b> - архив выполненных задач.\n\n"
        "<i>Подсказка:</i> Внутри списков вы можете нажимать на задачи для их изменения, выполнения или удаления."
    )
    await message.answer(help_text, parse_mode=ParseMode.HTML)

@router.message(Command("support"))
async def handle_support(message: Message, state: FSMContext):
    # await state.clear()
    await message.answer(
        "Если у вас возникли проблемы или есть предложения, пожалуйста, свяжитесь с @your_support_contact."
        "\nЭто учебный проект, так что поддержка может быть ограничена :)"
    )

# Хендлер для отмены любого состояния FSM
@router.message(Command("cancel"))
@router.message(F.text.casefold() == "отмена")
async def cancel_handler(message: Message, state: FSMContext) -> None:
    current_state = await state.get_state()
    if current_state is None:
        await message.answer("Нет активных действий для отмены.", reply_markup=get_main_menu_keyboard())
        return

    await state.clear()
    await message.answer("Действие отменено.", reply_markup=get_main_menu_keyboard())

