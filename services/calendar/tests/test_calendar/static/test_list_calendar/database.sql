DO $$
DECLARE
    frozen_creation_time TIMESTAMPTZ := '2025-04-19T10:00:00+00:00';

BEGIN

-- Задачи для user_id = 101 (Добавляем тестовые кейсы с новыми статусами)
INSERT INTO calendar.calendar (id, user_id, name, description, status, created_at, updated_at) VALUES
    ('00000000-0000-0000-0000-000000000001', 101, 'Задача 1', 'Задача 1 (User 101): Активная', 'active', frozen_creation_time, frozen_creation_time),
    ('00000000-0000-0000-0000-000000000002', 101, 'Задача 2', 'Задача 2 (User 101): Активная', 'active', frozen_creation_time, frozen_creation_time),
    ('00000000-0000-0000-0000-000000000003', 101, 'Задача 3', 'Задача 3 (User 101): Активная', 'active', frozen_creation_time, frozen_creation_time),
    ('00000000-0000-0000-0000-000000000006', 101, 'Задача 6', 'Задача 6 (User 101): Активная', 'active', frozen_creation_time, frozen_creation_time),
    ('00000000-0000-0000-0000-000000000007', 101, 'Задача 7', 'Задача 7 (User 101): Активная', 'active', frozen_creation_time, frozen_creation_time),
    ('00000000-0000-0000-0000-000000000009', 101, 'Задача 9', 'Задача 9 (User 101): Активная', 'active', frozen_creation_time, frozen_creation_time),
    ('00000000-0000-0000-0000-000000000011', 101, 'Задача 11 - Длинное имя', 'Задача 11 (User 101): Активная - Длинное описание для проверки поля TEXT, содержит разные символы !@#$%^&*()_+=-`~[]{}|\\;:",./<>?', 'active', frozen_creation_time, frozen_creation_time),
    ('00000000-0000-0000-0000-000000000013', 101, 'Задача 13', 'Задача 13 (User 101): Активная', 'active', frozen_creation_time, frozen_creation_time),
    ('00000000-0000-0000-0000-000000000021', 101, '!@#$%^&*()', 'Задача со спецсимволами в имени', 'active', frozen_creation_time, frozen_creation_time),
    ('00000000-0000-0000-0000-000000000017', 101, 'Задача 17', 'Задача 17 (User 101): Удалена', 'deleted', frozen_creation_time, frozen_creation_time);

-- Задачи для user_id = 102 (Обновлено: добавлено имя, убраны project_id и priority)
INSERT INTO calendar.calendar (id, user_id, name, description, status, created_at, updated_at) VALUES
    ('00000000-0000-0000-0000-000000000014', 102, 'Задача 14', 'Задача 14 (User 102): Активная', 'active', frozen_creation_time, frozen_creation_time),
    ('00000000-0000-0000-0000-000000000019', 102, 'Задача 19', 'Задача 19 (User 102): Удалена', 'deleted', frozen_creation_time, frozen_creation_time);

END $$;
