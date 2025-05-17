from aiogram import Router, F, Bot
from aiogram.types import Message, CallbackQuery
from aiogram.fsm.context import FSMContext
from telegram_bot.clients.api_client import api_client
from telegram_bot.internal.states import CommonStates
from telegram_bot.keyboards.inline_keyboards import get_confirm_delete_keyboard
from telegram_bot.utils.callback_data import FreeTextAction
import logging

logger = logging.getLogger(__name__)
router = Router()

async def add_item_based_on_type(user_id: int, item_name: str, item_type: str, message: Message):
    response_data, status_code = None, None
    type_rus = ""

    if item_type == "curr_act":
        response_data, status_code = await api_client.create_current_action(user_id, item_name)
        type_rus = "задача"
    elif item_type == "waiting":
        response_data, status_code = await api_client.create_waiting_task(user_id, item_name)
        type_rus = "ожидание"
    elif item_type == "notes":
        response_data, status_code = await api_client.create_note(user_id, item_name)
        type_rus = "заметка"
    # smt_ltr не создается через свободный ввод, только через перенос из curr_act
    
    if status_code in [201, 200]: # 200 может быть, если API так настроен
        await message.reply(f"✅ {type_rus.capitalize()} '{item_name}' успешно добавлена.")
    else:
        error_msg = response_data.get("error", "Неизвестная ошибка") if response_data else "Сервис недоступен"
        await message.reply(f"⚠️ Не удалось добавить {type_rus}: {error_msg} (код: {status_code})")

@router.message(F.text & ~F.text.startswith('/')) # Любой текст, не являющийся командой
async def handle_free_text(message: Message, state: FSMContext, bot: Bot):
    user_id = message.from_user.id
    task_text = message.text

    await message.reply("🧠 Распознаю вашу задачу...")
    
    ai_response, ai_status = await api_client.ai_define_task(task_text)

    if ai_status == 200 and ai_response:
        task_type = ai_response.get("task_type")
        logger.info(f"AI defined task '{task_text}' as type: {task_type}")

        if task_type == "trash":
            # Сохраняем ID сообщения, чтобы его потом удалить или отредактировать
            sent_message = await message.reply(
                "🗑️ Похоже, это не похоже на задачу. Удалить этот ввод?",
                reply_markup=get_confirm_delete_keyboard(item_id="0", item_type="trash_suggestion", original_message_id=message.message_id)
            )
            await state.update_data(original_task_text=task_text, last_bot_message_id=sent_message.message_id)
        elif task_type in ["current_actions", "waiting", "notes"]:
            if task_type == "current_actions":
                task_type = "curr_act"
            await add_item_based_on_type(user_id, task_text, task_type, message)
        else:
            await message.reply(f"😕 Неизвестный тип задачи от AI: {task_type}. Попробуйте переформулировать.")
    else:
        error_msg = ai_response.get("error", "AI сервис недоступен") if ai_response else "AI сервис недоступен"
        await message.reply(f"⚠️ Ошибка при обращении к AI: {error_msg} (код: {ai_status}).\nПопробуйте позже или введите задачу вручную через кнопки.")

# Обработка кнопок "Да (мусор)" / "Нет (ввести снова)"
@router.callback_query(FreeTextAction.filter(F.action == "confirm_delete"))
async def handle_freetext_confirm_delete(query: CallbackQuery, callback_data: FreeTextAction, state: FSMContext, bot: Bot):
    await query.answer("Действие учтено.")
    if callback_data.original_message_id and query.message:
        try:
            # Удаляем сообщение "Похоже, это не похоже на задачу..."
            await bot.delete_message(chat_id=query.message.chat.id, message_id=query.message.message_id)
            # Можно также удалить исходное сообщение пользователя, если это нужно
            # await bot.delete_message(chat_id=query.message.chat.id, message_id=callback_data.original_message_id)
            await query.message.answer("🗑️ Ввод проигнорирован.")
        except Exception as e:
            logger.error(f"Error deleting message for freetext confirm_delete: {e}")
            await query.message.edit_text("🗑️ Ввод проигнорирован.") # Fallback
    await state.clear()


@router.callback_query(FreeTextAction.filter(F.action == "retry_input"))
async def handle_freetext_retry_input_prompt(query: CallbackQuery, callback_data: FreeTextAction, state: FSMContext, bot: Bot):
    await query.answer()
    if query.message :
        await query.message.edit_text("✍️ Пожалуйста, введите текст задачи еще раз:")
    await state.set_state(CommonStates.waiting_for_retry_task_input)
    # Сохраняем ID сообщения с кнопками, чтобы его потом удалить
    await state.update_data(message_to_delete_id=query.message.message_id if query.message else None)


@router.message(CommonStates.waiting_for_retry_task_input, F.text & ~F.text.startswith('/'))
async def handle_freetext_retry_input_process(message: Message, state: FSMContext, bot: Bot):
    user_id = message.from_user.id
    task_text = message.text
    
    context_data = await state.get_data()
    message_to_delete_id = context_data.get("message_to_delete_id")

    if message_to_delete_id:
        try:
            await bot.delete_message(chat_id=message.chat.id, message_id=message_to_delete_id)
        except Exception as e:
            logger.error(f"Error deleting prompt message for retry input: {e}")
            
    await state.clear() # Очищаем состояние перед рекурсивным вызовом

    # Повторно вызываем основной обработчик свободного текста
    # Это немного рекурсивно, но для простоты пойдет.
    # В более сложных сценариях стоит вынести логику определения и добавления в отдельную функцию.
    await handle_free_text(message, state, bot) # Передаем новый message, state и bot

