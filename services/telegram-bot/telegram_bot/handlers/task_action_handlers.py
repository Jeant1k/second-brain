from aiogram import Router, F, Bot
from aiogram.types import Message, CallbackQuery
from aiogram.fsm.context import FSMContext
from telegram_bot.clients.api_client import api_client
from telegram_bot.keyboards.inline_keyboards import get_task_view_keyboard, get_confirm_delete_keyboard
from telegram_bot.utils.callback_data import TaskAction
from telegram_bot.internal.states import CommonStates # для переименования
from telegram_bot.handlers.task_list_handlers import show_item_list # для возврата к списку
import logging

logger = logging.getLogger(__name__)
router = Router()

SERVICE_ACTION_CLIENTS = {
    "current_actions": {
        "get_one": None, # API не имеет get one, получаем из списка
        "update": api_client.update_current_action_name,
        "complete": api_client.complete_current_action,
        "remove": api_client.remove_current_action,
        "move_to_sometime": api_client.move_current_action_to_sometime_later,
    },
    "sometime_later": {
        "update": api_client.update_sometime_later_task_name,
        "complete": api_client.complete_sometime_later_task,
        "remove": api_client.remove_sometime_later_task,
        "move_to_current": api_client.move_sometime_later_to_current_actions,
    },
    "waiting": {
        "update": api_client.update_waiting_task_name,
        "complete": api_client.complete_waiting_task,
        "remove": api_client.remove_waiting_task,
    },
    "notes": {
        "update": api_client.update_note_name,
        "remove": api_client.remove_note,
    },
    "completed": { # Особая логика для выполненных, т.к. они из разных источников
        # "update": ..., # Зависит от того, как мы храним source_type
        # "remove": ..., # "Удалить навсегда"
    }
}
# В use case для Выполнено: "делает запрос в сервис, из которого пришла задача"
# Это значит, что ID задачи должен быть глобально уникален, ИЛИ мы должны знать источник.
# Если ID глобально уникальны, то по ID можно определить сервис.
# Если нет, task должен нести поле source_service.
# Для MVP предположим, что удаление "Выполнено" - это вызов remove на всех сервисах
# или что ID глобально уникален и мы где-то храним маппинг ID -> сервис.
# Здесь для простоты я предположу, что 'id' задачи в 'completed' списке уже содержит
# или позволяет определить источник. Или что мы просто не даем редактировать выполненные.

# --- Просмотр элемента ---
@router.callback_query(TaskAction.filter(F.action == "view"))
async def handle_view_item_action(query: CallbackQuery, callback_data: TaskAction, state: FSMContext, bot: Bot):
    user_id = query.from_user.id
    item_id = callback_data.item_id
    item_type = callback_data.item_type
    current_list_cursor = callback_data.cursor

    if not query.message:
        await query.answer("Ошибка: сообщение не найдено.")
        return

    # Чтобы получить имя задачи (item_name), нам нужно его либо передавать в callback_data (не очень хорошо для длинных имен)
    # Либо снова запросить список и найти элемент по ID, либо иметь эндпоинт "get_task_by_id".
    # В API нет "get_task_by_id". Проще всего при отображении запомнить имя, но если его нет, то
    # просто показываем заглушку или снова фетчим весь список (что плохо).
    # Для упрощения, если нажимаем "Назад" после удаления/редактирования, мы перерисовываем список.
    # Если идем к просмотру с кнопки списка, имя можно было бы передать.
    # Здесь для "Назад" и "Отмена" при удалении, мы просто перерисовываем карточку с тем же ID.
    # Предположим, нам нужно запросить элемент заново, чтобы получить актуальное имя.
    # Так как нет GET /item/{id}, мы не можем получить имя напрямую.
    # Будем использовать "Загрузка..." и затем пользователь сам нажмет на кнопку имени для редактирования.
    # TODO: Для отображения имени задачи в get_task_view_keyboard нужен item_name.
    # Его нужно либо хранить в state, либо передавать, либо запрашивать (если есть API).
    # Сейчас в get_task_view_keyboard имя задачи будет захардкожено или просто "Задача/Заметка"
    # Реальный item_name будет подставлен после редактирования или если мы его где-то сохранили.

    # Заглушка для имени, т.к. у нас нет get by id
    # В идеале, при нажатии на задачу из списка, ее имя передается в callback_data,
    # либо мы делаем get_by_id. Здесь я упрощу и буду перерисовывать список при "назад".
    # Для простоты, на кнопке будет ID, а не имя, пока не будет запроса на редактирование.
    item_name_display = f"{item_type.replace('_', ' ').capitalize()} ID: {item_id[:8]}"

    # Если у нас имя есть (например, из предыдущего шага FSM или CallbackData)
    fsm_data = await state.get_data()
    cached_item_name = fsm_data.get(f"item_name_{item_id}")
    if cached_item_name:
        item_name_display = cached_item_name
    else: # Пытаемся найти в сообщении, если это inline кнопка с текстом задачи
        if query.message.reply_markup:
            for row in query.message.reply_markup.inline_keyboard:
                for button in row:
                    if button.callback_data and item_id in button.callback_data:
                        item_name_display = button.text.replace("📝 ","") # Убираем эмодзи если есть
                        await state.update_data({f"item_name_{item_id}": item_name_display}) # Кешируем
                        break
    
    keyboard = get_task_view_keyboard(item_id, item_name_display, item_type, current_list_cursor)
    await query.message.edit_text(f"Действия для элемента (ID: {item_id[:8]}...):", reply_markup=keyboard)
    await query.answer()


# --- Возврат к списку ---
@router.callback_query(TaskAction.filter(F.action == "back"))
async def handle_back_to_list_action(query: CallbackQuery, callback_data: TaskAction, state: FSMContext, bot: Bot):
    user_id = query.from_user.id
    item_type = callback_data.item_type
    current_list_cursor = callback_data.cursor # Курсор списка, к которому возвращаемся
    await state.clear() # Очищаем состояние, если оно было (например, от переименования)
    await show_item_list(query, item_type, user_id, current_list_cursor, bot=bot)
    # await query.answer() # Вызывается в show_item_list


# --- Редактирование имени (начало) ---
@router.callback_query(TaskAction.filter(F.action == "edit"))
async def handle_edit_item_action_prompt(query: CallbackQuery, callback_data: TaskAction, state: FSMContext):
    if not query.message:
        await query.answer("Ошибка.")
        return
    await state.set_state(CommonStates.waiting_for_new_item_name)
    await state.update_data(
        item_id_to_edit=callback_data.item_id,
        item_type_to_edit=callback_data.item_type,
        original_message_id=query.message.message_id, # Сохраняем ID, чтобы отредактировать карточку задачи
        original_message_chat_id=query.message.chat.id,
        current_list_cursor=callback_data.cursor # Сохраняем курсор для возврата
    )
    await query.message.edit_text("Введите новое название задачи/заметки:")
    await query.answer()

# --- Редактирование имени (получение нового имени) ---
@router.message(CommonStates.waiting_for_new_item_name, F.text)
async def handle_process_new_item_name(message: Message, state: FSMContext, bot: Bot):
    new_name = message.text
    data = await state.get_data()
    item_id = data.get("item_id_to_edit")
    item_type = data.get("item_type_to_edit")
    original_message_id = data.get("original_message_id")
    original_message_chat_id = data.get("original_message_chat_id")
    current_list_cursor = data.get("current_list_cursor")

    await state.clear() # Очищаем состояние

    if not all([item_id, item_type, original_message_id, original_message_chat_id]):
        await message.reply("Произошла ошибка состояния. Попробуйте снова.")
        return

    update_method = SERVICE_ACTION_CLIENTS.get(item_type, {}).get("update")
    if not update_method:
        await message.reply(f"Действие 'обновить' не поддерживается для типа '{item_type}'.")
        # Восстанавливаем предыдущее сообщение, если возможно
        if original_message_id and original_message_chat_id:
             keyboard = get_task_view_keyboard(item_id, "Ошибка имени", item_type, current_list_cursor) # Имя нужно получить
             await bot.edit_message_text("Ошибка обновления.", chat_id=original_message_chat_id, message_id=original_message_id, reply_markup=keyboard)
        return

    response_data, status_code = await update_method(item_id, new_name)

    # Восстанавливаем карточку задачи
    keyboard = get_task_view_keyboard(item_id, new_name if status_code == 202 else "Ошибка обновления", item_type, current_list_cursor)
    text_to_show = f"Название обновлено на '{new_name}'." if status_code == 202 else f"Ошибка обновления: {response_data.get('error', status_code)}"
    
    try:
        await bot.edit_message_text(
            text=text_to_show,
            chat_id=original_message_chat_id,
            message_id=original_message_id,
            reply_markup=keyboard
        )
        await message.delete() # Удаляем сообщение пользователя с новым именем
    except Exception as e: # Если исходное сообщение было удалено или что-то пошло не так
        logger.error(f"Error updating message after rename: {e}")
        await message.answer(text_to_show, reply_markup=keyboard) # Отправляем новым сообщением

    await state.update_data({f"item_name_{item_id}": new_name}) # Обновляем кешированное имя

# --- Действия с элементами: complete, move_to_sometime, move_to_current ---
async def _handle_simple_item_modification(query: CallbackQuery, callback_data: TaskAction, action_name_rus: str, bot: Bot, state: FSMContext):
    if not query.message:
        await query.answer("Ошибка.")
        return

    item_id = callback_data.item_id
    item_type = callback_data.item_type
    api_action_key = callback_data.action # "complete", "move_to_sometime", etc.

    action_method = SERVICE_ACTION_CLIENTS.get(item_type, {}).get(api_action_key)
    
    source_service_for_completed = None
    if item_type == "completed": # Особая логика для "Выполнено"
        # Здесь нужно определить исходный сервис задачи.
        # Предположим, что ID содержит префикс или мы где-то храним эту информацию.
        # Это сложная часть, если API не дает source_type для выполненных задач.
        # Для MVP: если это "completed", то complete/move не имеет смысла. Только удаление.
        # Если API отдает Task с полем source_type (e.g. 'current_actions'), то используем его
        # task_details = await api_client.get_task_details(item_id) # Если бы был такой метод
        # source_service_for_completed = task_details.get('source_type') 
        # action_method = SERVICE_ACTION_CLIENTS.get(source_service_for_completed, {}).get(api_action_key)
        await query.answer("Действие не применимо к выполненным задачам из этого меню.", show_alert=True)
        return


    if not action_method:
        await query.answer(f"Действие '{action_name_rus}' не поддерживается для '{item_type}'.", show_alert=True)
        return

    resp_data, status_code = await action_method(item_id)

    if status_code == 200:
        await query.answer(f"✅ {action_name_rus}!", show_alert=False)
        # Обновляем список, откуда элемент ушел. Пользователь вернется туда через "Назад" или выберет заново.
        # Можно сразу перерисовать список на экране
        await query.message.edit_text(f"✅ Элемент '{item_id[:8]}...' был '{action_name_rus}'.\nОбновите список или вернитесь назад.")
        # Или лучше сразу показать обновленный список
        # await show_item_list(query, item_type, query.from_user.id, callback_data.cursor, bot=bot)
    else:
        error_message = resp_data.get("error", "Неизвестная ошибка") if resp_data else "Сервис недоступен"
        await query.answer(f"⚠️ Ошибка: {error_message} ({status_code})", show_alert=True)


@router.callback_query(TaskAction.filter(F.action == "complete"))
async def handle_complete_item_action(query: CallbackQuery, callback_data: TaskAction, bot: Bot, state: FSMContext):
    await _handle_simple_item_modification(query, callback_data, "Выполнено", bot, state)

@router.callback_query(TaskAction.filter(F.action == "move_to_sometime"))
async def handle_move_to_sometime_action(query: CallbackQuery, callback_data: TaskAction, bot: Bot, state: FSMContext):
    await _handle_simple_item_modification(query, callback_data, "Перемещено в 'Не сейчас'", bot, state)

@router.callback_query(TaskAction.filter(F.action == "move_to_current"))
async def handle_move_to_current_action(query: CallbackQuery, callback_data: TaskAction, bot: Bot, state: FSMContext):
    await _handle_simple_item_modification(query, callback_data, "Перемещено в 'Задачи'", bot, state)


# --- Удаление (запрос подтверждения) ---
@router.callback_query(TaskAction.filter(F.action == "delete_prompt"))
async def handle_delete_item_prompt_action(query: CallbackQuery, callback_data: TaskAction, bot: Bot, state: FSMContext):
    if not query.message:
        await query.answer("Ошибка.")
        return
        
    item_id = callback_data.item_id
    item_type = callback_data.item_type
    confirm_keyboard = get_confirm_delete_keyboard(item_id, item_type, callback_data.cursor)
    entity_name = "задачу" if item_type != "notes" else "заметку"
    if item_type == "completed":
        entity_name = "выполненную задачу (навсегда)"

    await query.message.edit_text(f"Вы уверены, что хотите удалить {entity_name} (ID: {item_id[:8]}...)?", reply_markup=confirm_keyboard)
    await query.answer()

# --- Удаление (подтверждение) ---
@router.callback_query(TaskAction.filter(F.action == "delete_confirm"))
async def handle_delete_item_confirm_action(query: CallbackQuery, callback_data: TaskAction, bot: Bot, state: FSMContext):
    if not query.message:
        await query.answer("Ошибка.")
        return

    item_id = callback_data.item_id
    original_item_type = callback_data.item_type # Тип списка, из которого удаляем (current_actions, notes, completed, etc.)
    item_type_for_api_call = original_item_type # По умолчанию совпадает

    # Особая логика для "completed"
    # Use case: "telegram-bot отправляет запрос в сервис, из которого пришла задача, чтобы пометить задачу удаленной"
    # Это значит, что мы должны знать `source_service` для выполненной задачи.
    # Предположим, что это уже есть в `item_id` или мы можем его получить.
    # Для MVP, представим что "удалить выполненную" - это вызов `/remove` на всех источниках.
    # Или, если `item_id` уникален глобально, то мы можем как-то по нему определить сервис.
    # Это нужно продумать на уровне архитектуры данных.
    # Вариант 1: ID задачи имеет префикс сервиса (ca_uuid, sl_uuid, wa_uuid).
    # Вариант 2: Task объект из API для "completed" содержит source_service.
    # Вариант 3 (упрощенный для MVP): Удаление "completed" вызывает /remove на всех трех сервисах задач.
    # Этот вариант не очень хорош, если ID не уникальны между сервисами.
    # Для ВКРБ проще всего, если ID глобально уникален и сервис `completed` может сказать источник.
    # Если нет, то `TaskAction` для `item_type=completed` должен нести `source_service_type`.
    # Либо, как здесь, мы пытаемся угадать, или это просто логическое удаление (completed -> deleted), которое
    # не зовет реальный /remove, а просто меняет статус в каком-то агрегирующем хранилище.
    # ТЗ говорит "пометить задачу удаленной" - это значит вызов /remove.
    
    # Для текущей реализации, если это 'completed', мы полагаемся, что ID уникален
    # и бэкенд сам разберется (что маловероятно без дополнительных данных).
    # Самый простой путь для ТЗ:
    # Если callback_data.item_type == "completed", то нам нужен еще один параметр (например, task_real_source_type)
    # Либо мы должны были его сохранить в FSM или он должен быть частью item_id

    # Упрощение: Предположим, что "completed" задачи хранятся только в своих оригинальных сервисах.
    # И вызов "remove" для них - это тот же remove, что и для активных задач.
    # НО! API для current_actions, etc., уже имеют status (active, completed, deleted).
    # Поэтому "удалить" выполненную задачу может означать переход completed -> deleted.

    remove_method = None
    if original_item_type == "current_actions":
        remove_method = SERVICE_ACTION_CLIENTS["current_actions"]["remove"]
    elif original_item_type == "sometime_later":
        remove_method = SERVICE_ACTION_CLIENTS["sometime_later"]["remove"]
    elif original_item_type == "waiting":
        remove_method = SERVICE_ACTION_CLIENTS["waiting"]["remove"]
    elif original_item_type == "notes":
        remove_method = SERVICE_ACTION_CLIENTS["notes"]["remove"]
    elif original_item_type == "completed":
        # Это самый сложный случай. Какому сервису слать remove?
        # Если ID глобальны - то надо определить сервис по ID.
        # Если нет - нужен `source_service` в `callback_data` или в информации о задаче.
        # Допустим, мы просто используем `current_actions` как fallback, если не знаем источник.
        # Это надо четко прописать в ВКР.
        # "делает запрос в сервис, из которого пришла задача"
        # Для этого нам нужно знать этот сервис.
        # ПРОПУСТИМ эту логику для `completed` в `remove_method` пока,
        # так как нет информации об источнике задачи в `callback_data`.
        # Можно добавить `source_type` в `TaskAction` для `item_type == 'completed'`
        # Или, более вероятно, `completed` задачи должны возвращаться API с полем `source_service`.
        # **Заглушка**:
        await query.message.edit_text(f"⚠️ Удаление выполненных задач из агрегированного списка требует знания исходного сервиса. Эта функция пока не полностью реализована для 'Выполнено'.")
        await query.answer("Функция в разработке для 'Выполнено'", show_alert=True)
        return

    if not remove_method:
        await query.answer(f"Действие 'удалить' не поддерживается для типа '{original_item_type}'.", show_alert=True)
        return

    resp_data, status_code = await remove_method(item_id)

    if status_code == 200:
        await query.answer("🗑️ Удалено!", show_alert=False)
        # После удаления лучше всего перерисовать список (или уведомить пользователя)
        await query.message.edit_text(f"🗑️ Элемент '{item_id[:8]}...' удален.\nОбновите список или вернитесь назад.")
        # await show_item_list(query, original_item_type, query.from_user.id, callback_data.cursor, bot=bot)
    else:
        error_message = resp_data.get("error", "Неизвестная ошибка") if resp_data else "Сервис недоступен"
        await query.answer(f"⚠️ Ошибка при удалении: {error_message} ({status_code})", show_alert=True)

