from typing import Optional
from aiogram import Router, F, Bot
from aiogram.types import Message, CallbackQuery
from aiogram.fsm.context import FSMContext
from telegram_bot.clients.api_client import api_client
from telegram_bot.keyboards.inline_keyboards import get_tasks_list_keyboard, get_task_view_keyboard, get_confirm_delete_keyboard
from telegram_bot.utils.callback_data import TaskAction, Pagination
from telegram_bot.internal.states import CommonStates
import logging
import asyncio # Для "Выполнено"

logger = logging.getLogger(__name__)
router = Router()

ITEM_TYPE_MAP = {
    "Задачи": "current_actions",
    "Не сейчас": "sometime_later",
    "Ожидания": "waiting",
    "Заметки": "notes",
    "Выполнено": "completed" # Специальный тип для агрегации
}
ITEM_NAME_MAP = {
    "current_actions": "активных задач",
    "sometime_later": "отложенных задач",
    "waiting": "ожиданий",
    "notes": "заметок",
    "completed": "выполненных задач"
}

SERVICE_LIST_METHODS = {
    "current_actions": api_client.list_current_actions,
    "sometime_later": api_client.list_sometime_later_tasks,
    "waiting": api_client.list_waiting_tasks,
    "notes": api_client.list_notes,
}
SERVICE_ENTITY_NAMES = { # Для сообщений
    "current_actions": ("Задачи", "задач"),
    "sometime_later": ("Отложенные", "отложенных задач"),
    "waiting": ("Ожидания", "ожиданий"),
    "notes": ("Заметки", "заметок"),
    "completed": ("Выполненные", "выполненных задач")

}

async def show_item_list(message_or_query: Message | CallbackQuery, item_type: str, user_id: int, cursor: Optional[str] = None, bot: Optional[Bot] = None):
    """Общая функция для отображения списка элементов."""
    is_callback = isinstance(message_or_query, CallbackQuery)
    target_message = message_or_query.message if is_callback else message_or_query
    
    if not target_message: # На случай если query.message будет None
        logger.error(f"Target message is None for item_type {item_type}")
        if is_callback: await message_or_query.answer("Ошибка отображения списка.")
        return

    title, empty_list_msg_part = SERVICE_ENTITY_NAMES.get(item_type, ("Элементы", "элементов"))

    if item_type == "completed":
        await show_completed_items_list(message_or_query, user_id, cursor, bot)
        return

    list_method = SERVICE_LIST_METHODS.get(item_type)
    if not list_method:
        await target_message.answer(f"Неизвестный тип списка: {item_type}")
        return
        
    status_filter = "active"
    if item_type == "sometime_later": status_filter = "pending"
    elif item_type == "notes": status_filter = "active" # notes API ожидает 'active' or 'deleted'

    items_data, status_code = await list_method(user_id=user_id, status=status_filter, cursor=cursor)

    if status_code == 200 and items_data:
        items = items_data.get( "tasks" if item_type != "notes" else "notes", [])
        next_cursor = items_data.get("cursor")
        
        # Для кнопки "Назад" в пагинации нам нужен предыдущий курсор.
        # API не возвращает его, так что мы запоминаем текущий как "предыдущий" для следующего шага.
        # Это упрощенная логика, не идеальна для глубокой пагинации назад.
        # Чтобы сделать полноценную "назад", нужно хранить историю курсоров или API должен поддерживать prev_cursor/offset.
        # Для данной ВКР, отображение "влево" будет возвращать на первую страницу (cursor=None).
        prev_page_cursor = None # Этот курсор вернет на первую страницу
        if cursor: # Если мы не на первой странице, то кнопка "влево" будет без курсора
             pass # Оставляем prev_page_cursor = None для возврата на первую страницу

        if items:
            keyboard = get_tasks_list_keyboard(items, item_type, current_cursor=cursor, next_cursor=next_cursor, prev_cursor=prev_page_cursor)
            text = f"📋 Ваш список '{title}':"
            if is_callback:
                await target_message.edit_text(text, reply_markup=keyboard)
            else:
                await target_message.answer(text, reply_markup=keyboard)
        else:
            text = f"📭 Ваш список '{title}' пуст."
            if is_callback:
                 # Если список стал пуст после действия, лучше отправить новое сообщение
                await target_message.answer(text)
                await target_message.delete() # Удаляем старое сообщение со списком
            else:
                await target_message.answer(text)
    else:
        error_msg = items_data.get("error", "Не удалось загрузить список") if items_data else "Сервис недоступен"
        await target_message.answer(f"⚠️ Ошибка: {error_msg} (код: {status_code})")
    
    if is_callback:
        await message_or_query.answer()


@router.message(F.text.in_(ITEM_TYPE_MAP.keys()))
async def handle_menu_button_press(message: Message, state: FSMContext, bot: Bot):
    await state.clear() # Очищаем состояние при выборе из главного меню
    item_type_key = message.text
    item_type_val = ITEM_TYPE_MAP.get(item_type_key)
    if item_type_val:
        await show_item_list(message, item_type_val, message.from_user.id, bot=bot)
    else:
        await message.answer("Неизвестная команда меню.")


@router.callback_query(Pagination.filter())
async def handle_pagination(query: CallbackQuery, callback_data: Pagination, bot: Bot):
    user_id = query.from_user.id
    await show_item_list(query, callback_data.item_type, user_id, callback_data.cursor, bot=bot)
    # await query.answer() # Answer делается внутри show_item_list


async def show_completed_items_list(message_or_query: Message | CallbackQuery, user_id: int, cursor: Optional[str] = None, bot: Optional[Bot] = None):
    is_callback = isinstance(message_or_query, CallbackQuery)
    target_message = message_or_query.message if is_callback else message_or_query

    if not target_message:
        if is_callback: await message_or_query.answer("Ошибка отображения списка.")
        return

    await target_message.answer("⏳ Загружаю выполненные задачи...")

    all_completed_tasks = []
    # Предположим, что пагинация для "Выполнено" будет сложной, если каждый сервис имеет свой курсор.
    # Для MVP, "Выполнено" может показывать ограниченное количество последних задач без сложной пагинации
    # или пагинацию только по одному из сервисов (например, current_actions).
    # В текущей реализации API пагинация по "Выполнено" не очень очевидно работает, если смешивать.
    # Здесь я сделаю сбор без общей пагинации, просто первые страницы каждого.
    # TODO: Уточнить, как должна работать пагинация для "Выполнено" на уровне API.
    # Пока что курсор будет игнорироваться для "Выполнено".

    services_to_query = {
        "current_actions": api_client.list_current_actions,
        "sometime_later": api_client.list_sometime_later_tasks,
        "waiting": api_client.list_waiting_tasks,
    }
    
    # Для ВКРБ: можно просто показать N последних из каждой категории.
    # Или реализовать пагинацию по одной из категорий, а остальные подгружать фиксированно.
    # Пока что сделаем простой сбор без пагинации через callback.
    # Или, если ваш API поддерживает глобальный курсор для выполненных - отлично.
    # Мой пример API не имеет такого, поэтому сбор будет суммарный.

    tasks_futures = [
        s_method(user_id=user_id, status="completed") #, cursor=cursor) # cursor для выполненных пока не используем
        for s_method in services_to_query.values()
    ]
    results = await asyncio.gather(*tasks_futures, return_exceptions=True)

    has_errors = False
    for res_data, status_code in results:
        if isinstance(res_data, Exception):
            logger.error(f"Error fetching completed tasks: {res_data}")
            has_errors = True
            continue
        if status_code == 200 and res_data:
            tasks = res_data.get("tasks", [])
            for task in tasks:
                 # Добавляем источник, чтобы знать, куда слать запросы на удаление/изменение.
                 # Это должно приходить от API, но если нет, определяем по эндпоинту.
                 # В данном случае, так как мы агрегируем, нужно как-то это хранить/определять
                 # Проще всего если API отдаст source_service_type в самой задаче.
                 # Для упрощения, если этого нет, мы будем показывать только имя.
                 # Действия с выполненными сложны без явного source_service.
                 # В ТЗ сказано "делает запрос в сервис, из которого пришла задача"
                 # Это значит, что Task должен содержать информацию об источнике.
                 # Если `task` объект из `ListTasksResponse` не содержит `source_type`,
                 # то управление ими будет затруднено.
                 # Предположим, что для "Выполнено" мы можем иметь такой source_type.
                 # Если нет, то только "Удалить" (логическое, т.е. completed -> deleted).
                all_completed_tasks.append(task) # Нужен source_type в task!
        elif status_code != 200:
            logger.warning(f"Partial error fetching completed tasks: {status_code} from a service.")
            has_errors = True
            
    # Сортировка по `completed_at` если это поле есть и оно надежно
    # all_completed_tasks.sort(key=lambda t: t.get('completed_at', t.get('updated_at')), reverse=True)

    # Пагинация для выполненных задач (очень упрощенная):
    # Можно сделать пагинацию "виртуальную" на стороне бота, но это неэффективно при больших объемах.
    # Здесь, для простоты, если общий список большой, можно обрезать.
    # Либо, как и выше, пагинация по факту делается отдельно для каждого типа при вызове.
    # Сейчас мы не используем курсор для "Выполненого" и просто показываем всё.

    if all_completed_tasks:
        # Для `get_tasks_list_keyboard` нужен item_type, чтобы callback_data правильно формировались.
        # Так как это агрегированный список, мы используем "completed".
        # Но это означает, что `TaskAction` должен будет знать, в какой реальный сервис обратиться.
        # Это решается либо добавлением `source_service` в `task_id` (плохо), либо в `item` как поле.
        # Если `task['id']` глобально уникальны, то можно хранить маппинг `task_id -> source_service`.
        # Для демонстрации, обработчик `TaskAction` для `item_type="completed"` будет сложнее.
        keyboard = get_tasks_list_keyboard(all_completed_tasks, "completed", current_cursor=None, next_cursor=None, prev_cursor=None) # Упрощенная пагинация
        text = "✅ Ваш список выполненных задач:"
        if is_callback and target_message:
            await target_message.edit_text(text, reply_markup=keyboard)
        elif target_message:
            await target_message.answer(text, reply_markup=keyboard)
    else:
        text = "🎉 Все выполненные задачи обработаны или их еще нет."
        if has_errors:
            text += "\n(Возникли проблемы при загрузке части данных)"
        if is_callback and target_message:
            await target_message.edit_text(text)
            # await target_message.answer(text) # Новое сообщение, если старое было со списком
            # await target_message.delete()
        elif target_message:
            await target_message.answer(text)
    
    if is_callback:
        await message_or_query.answer()

