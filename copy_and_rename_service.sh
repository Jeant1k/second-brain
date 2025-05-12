#!/bin/bash

# --- НАСТРОЙКИ ---
# Укажите полный путь к директории ПОСЛЕ ПЕРВОГО ЭТАПА (где все "waiting")
SOURCE_DIR="/home/jean/second_brain/services/waiting"
# Укажите полный путь к НОВОЙ директории, куда будет скопирован и изменен сервис
DEST_DIR="/home/jean/second_brain/services/notes"
# -----------------

# Проверка на существование исходной директории
if [ ! -d "$SOURCE_DIR" ]; then
    echo "Ошибка: Исходная директория '$SOURCE_DIR' (результат предыдущего шага) не найдена!"
    exit 1
fi

# Проверка, не совпадает ли исходная и целевая директория
if [ "$SOURCE_DIR" == "$DEST_DIR" ]; then
    echo "Ошибка: Исходная и целевая директории совпадают. Это опасно!"
    echo "Скрипт предназначен для копирования и модификации в новой директории."
    exit 1
fi

# Шаг 0: Предупреждение и запрос подтверждения
echo "ВНИМАНИЕ!"
echo "Этот скрипт скопирует '$SOURCE_DIR' в '$DEST_DIR'"
echo "Затем он произведет следующие замены ВНУТРИ '$DEST_DIR':"
echo "  - 'waiting' -> 'notes' (в контенте)"
echo "  - 'Waiting' -> 'Notes' (в контенте)"
echo "  - имена файлов/директорий 'waiting...' -> 'notes...'"
echo ""
echo "УБЕДИТЕСЬ, ЧТО У ВАС ЕСТЬ БЭКАП '$SOURCE_DIR' или исходной директории!"
read -p "Продолжить? (yes/no): " confirmation
if [ "$confirmation" != "yes" ]; then
    echo "Операция отменена."
    exit 0
fi

# --- Шаг 1: Копирование директории сервиса ---
echo ""
echo "Шаг 1: Копирование директории..."
if [ -d "$DEST_DIR" ]; then
    echo "Предупреждение: Целевая директория '$DEST_DIR' уже существует."
    read -p "Удалить существующую '$DEST_DIR' и продолжить? (yes/no): " overwrite_confirm
    if [ "$overwrite_confirm" == "yes" ]; then
        rm -rf "$DEST_DIR"
        echo "Существующая директория '$DEST_DIR' удалена."
    else
        echo "Операция отменена. Целевая директория не была изменена."
        exit 1
    fi
fi

cp -a "$SOURCE_DIR" "$DEST_DIR"
if [ $? -ne 0 ]; then
    echo "Ошибка при копировании директории."
    exit 1
fi
echo "Директория успешно скопирована в '$DEST_DIR'."


# --- Шаг 2: Замена вхождений в файлах ---
echo ""
echo "Шаг 2: Замена вхождений в файлах внутри '$DEST_DIR'..."

# Находим все файлы (кроме тех, что в .git, если есть) и производим замены
find "$DEST_DIR" -type f -not -path '*/.git/*' -print0 | while IFS= read -r -d $'\0' file; do
    # Проверка, что файл текстовый
    if file -b --mime-encoding "$file" | grep -q "us-ascii\|utf-8\|iso-8859"; then
        echo "Обработка файла: $file"
        # Используем perl для замен
        perl -i -pe '
            s/waiting/notes/g;
            s/Waiting/Notes/g;
        ' "$file"
        if [ $? -ne 0 ]; then
            echo "Предупреждение: Возможна ошибка при обработке файла $file с помощью perl."
        fi
    else
        echo "Пропуск (предположительно бинарный): $file"
    fi
done
echo "Замена вхождений в файлах завершена."


# --- Шаг 3: Переименование файлов и директорий ---
echo ""
echo "Шаг 3: Переименование файлов и директорий в '$DEST_DIR'..."
echo "Переименование будет производиться от самых глубоко вложенных к родительским."

# Паттерн для поиска в именах файлов/директорий
rename_pattern="waiting"
target_name="notes"

echo "Ищем и переименовываем элементы, содержащие '$rename_pattern'..."
# Находим все файлы и директории, имя которых содержит паттерн
# Используем -depth для правильного порядка переименования
find "$DEST_DIR" -depth -name "*${rename_pattern}*" -print0 | while IFS= read -r -d $'\0' old_path; do
    # Проверяем, существует ли еще этот путь
    if [ ! -e "$old_path" ]; then
        continue
    fi

    old_basename=$(basename "$old_path")
    dir_path=$(dirname "$old_path")

    # Заменяем первое вхождение паттерна в имени
    # ВНИМАНИЕ: Это заменит 'waiting' на 'notes' даже если 'waiting' является частью другого слова,
    # например 'awaiting_file' станет 'anotes_file'. Если такое поведение нежелательно,
    # может потребоваться более сложная логика (например, с использованием утилиты rename с поддержкой regex).
    new_basename=$(echo "$old_basename" | sed "s/${rename_pattern}/${target_name}/")

    # Полный новый путь
    new_path="$dir_path/$new_basename"

    # Переименовываем, только если имя действительно изменилось и новый путь не совпадает со старым
    if [ "$old_path" != "$new_path" ]; then
        # Проверка на случай, если файл/директория с таким именем уже существует
        if [ -e "$new_path" ]; then
            echo "Предупреждение: Целевой путь '$new_path' уже существует. Пропуск переименования '$old_path'."
        else
            echo "Переименование: '$old_path' -> '$new_path'"
            mv -T "$old_path" "$new_path"
            if [ $? -ne 0 ]; then
                echo "Ошибка при переименовании '$old_path' в '$new_path'."
            fi
        fi
    fi
done
echo "Переименование файлов и директорий завершено."

echo ""
echo "--- Все операции завершены! ---"
echo "Новый сервис находится в: $DEST_DIR"
echo ""
echo "Не забудьте:"
echo "1. Проверить изменения в '$DEST_DIR' (особенно CMakeLists.txt, .yaml, исходный код)."
echo "2. Пересобрать проект userver в новой директории."
echo "3. Протестировать работоспособность нового сервиса."
echo "4. Проверить историю Git, если она важна."


