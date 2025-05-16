from aiogram.types import InlineKeyboardMarkup, InlineKeyboardButton
from aiogram.utils.keyboard import InlineKeyboardBuilder
from typing import List, Dict, Optional
from telegram_bot.utils.callback_data import TaskAction, Pagination, FreeTextAction

MAX_TASK_NAME_LENGTH = 30 # Для отображения на кнопках

def get_tasks_list_keyboard(items: List[Dict], item_type: str, current_cursor: Optional[str] = None, next_cursor: Optional[str] = None, prev_cursor: Optional[str] = None) -> InlineKeyboardMarkup:
    builder = InlineKeyboardBuilder()
    for item in items:
        name = item.get('name', 'Без названия')
        display_name = (name[:MAX_TASK_NAME_LENGTH] + '...') if len(name) > MAX_TASK_NAME_LENGTH else name
        builder.row(
            InlineKeyboardButton(
                text=display_name,
                callback_data=TaskAction(action="view", item_id=item['id'], item_type=item_type, cursor=current_cursor).pack()
            )
        )

    pagination_buttons = []
    if prev_cursor:
        pagination_buttons.append(
            InlineKeyboardButton(text="⬅️ Влево", callback_data=Pagination(action="prev", item_type=item_type, cursor=prev_cursor).pack())
        )
    if next_cursor:
        pagination_buttons.append(
            InlineKeyboardButton(text="Вправо ➡️", callback_data=Pagination(action="next", item_type=item_type, cursor=next_cursor).pack())
        )
    if pagination_buttons:
        builder.row(*pagination_buttons)
    
    return builder.as_markup()

def get_task_view_keyboard(item_id: str, item_name: str, item_type: str, current_list_cursor: Optional[str] = None) -> InlineKeyboardMarkup:
    builder = InlineKeyboardBuilder()
    display_name = (item_name[:MAX_TASK_NAME_LENGTH] + '...') if len(item_name) > MAX_TASK_NAME_LENGTH else item_name
    
    # Кнопка для редактирования названия
    builder.row(InlineKeyboardButton(
        text=f"📝 {display_name}",
        callback_data=TaskAction(action="edit", item_id=item_id, item_type=item_type, cursor=current_list_cursor).pack()
    ))

    action_buttons = [
        InlineKeyboardButton(text="⬅️ Назад", callback_data=TaskAction(action="back", item_id="0", item_type=item_type, cursor=current_list_cursor).pack())
    ]

    if item_type == "current_actions":
        action_buttons.extend([
            InlineKeyboardButton(text="✅ Выполнить", callback_data=TaskAction(action="complete", item_id=item_id, item_type=item_type, cursor=current_list_cursor).pack()),
            InlineKeyboardButton(text="⏳ Отложить", callback_data=TaskAction(action="move_to_sometime", item_id=item_id, item_type=item_type, cursor=current_list_cursor).pack()),
            InlineKeyboardButton(text="🗑️ Удалить", callback_data=TaskAction(action="delete_prompt", item_id=item_id, item_type=item_type, cursor=current_list_cursor).pack())
        ])
    elif item_type == "sometime_later":
        action_buttons.extend([
            InlineKeyboardButton(text="✅ Выполнить", callback_data=TaskAction(action="complete", item_id=item_id, item_type=item_type, cursor=current_list_cursor).pack()),
            InlineKeyboardButton(text="⚡️ В работу", callback_data=TaskAction(action="move_to_current", item_id=item_id, item_type=item_type, cursor=current_list_cursor).pack()),
            InlineKeyboardButton(text="🗑️ Удалить", callback_data=TaskAction(action="delete_prompt", item_id=item_id, item_type=item_type, cursor=current_list_cursor).pack())
        ])
    elif item_type == "waiting":
        action_buttons.extend([
            InlineKeyboardButton(text="✅ Выполнить", callback_data=TaskAction(action="complete", item_id=item_id, item_type=item_type, cursor=current_list_cursor).pack()),
            InlineKeyboardButton(text="🗑️ Удалить", callback_data=TaskAction(action="delete_prompt", item_id=item_id, item_type=item_type, cursor=current_list_cursor).pack())
        ])
    elif item_type == "notes":
         action_buttons.extend([
            InlineKeyboardButton(text="🗑️ Удалить", callback_data=TaskAction(action="delete_prompt", item_id=item_id, item_type=item_type, cursor=current_list_cursor).pack())
        ])
    elif item_type == "completed": # Для выполненных задач
        action_buttons.extend([
            InlineKeyboardButton(text="🗑️ Удалить навсегда", callback_data=TaskAction(action="delete_prompt", item_id=item_id, item_type=item_type, cursor=current_list_cursor).pack())
        ])


    # Группируем кнопки по 2-3 в ряд для лучшего вида
    if len(action_buttons) <= 3:
        builder.row(*action_buttons)
    else: # 4 кнопки, 1+3
        builder.row(action_buttons[0])
        builder.row(*action_buttons[1:])


    return builder.as_markup()


def get_confirm_delete_keyboard(item_id: str, item_type: str, current_list_cursor: Optional[str] = None, original_message_id: int | None = None) -> InlineKeyboardMarkup:
    builder = InlineKeyboardBuilder()
    if item_type == "trash_suggestion": # Особый случай для свободного ввода
         builder.row(
            InlineKeyboardButton(text="🗑️ Да, это мусор", callback_data=FreeTextAction(action="confirm_delete", original_message_id=original_message_id).pack()),
            InlineKeyboardButton(text="✍️ Нет, ввести снова", callback_data=FreeTextAction(action="retry_input", original_message_id=original_message_id).pack())
        )
    else:
        builder.row(
            InlineKeyboardButton(
                text="✅ Да, удалить",
                callback_data=TaskAction(action="delete_confirm", item_id=item_id, item_type=item_type, cursor=current_list_cursor).pack()
            ),
            InlineKeyboardButton(
                text="❌ Нет, отмена",
                callback_data=TaskAction(action="view", item_id=item_id, item_type=item_type, cursor=current_list_cursor).pack() # Вернуться к просмотру задачи
            )
        )
    return builder.as_markup()
