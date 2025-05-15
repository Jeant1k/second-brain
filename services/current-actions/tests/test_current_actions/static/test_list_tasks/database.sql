DO $$
DECLARE
    frozen_creation_time TIMESTAMPTZ := '2025-04-19T10:00:00+00:00';
    static_completed_time TIMESTAMPTZ := '2025-04-26T10:00:00+00:00';

BEGIN

-- Задачи для user_id = 101 (Добавляем тестовые кейсы с новыми статусами)
INSERT INTO current_actions.tasks (id, user_id, name, description, status, completed_at, created_at, updated_at) VALUES
    ('00000000-0000-0000-0000-000000000001', 101, 'Задача 1', 'Задача 1 (User 101): Активная', 'active', NULL, frozen_creation_time, frozen_creation_time),
    ('00000000-0000-0000-0000-000000000002', 101, 'Задача 2', 'Задача 2 (User 101): Активная', 'active', NULL, frozen_creation_time, frozen_creation_time),
    ('00000000-0000-0000-0000-000000000003', 101, 'Задача 3', 'Задача 3 (User 101): Активная', 'active', NULL, frozen_creation_time, frozen_creation_time),
    ('00000000-0000-0000-0000-000000000004', 101, 'Задача 4', 'Задача 4 (User 101): Завершена', 'completed', static_completed_time, frozen_creation_time, frozen_creation_time),
    ('00000000-0000-0000-0000-000000000005', 101, 'Задача 5', 'Задача 5 (User 101): Завершена', 'completed', static_completed_time, frozen_creation_time, frozen_creation_time),
    ('00000000-0000-0000-0000-000000000006', 101, 'Задача 6', 'Задача 6 (User 101): Активная', 'active', NULL, frozen_creation_time, frozen_creation_time),
    ('00000000-0000-0000-0000-000000000007', 101, 'Задача 7', 'Задача 7 (User 101): Активная', 'active', NULL, frozen_creation_time, frozen_creation_time),
    ('00000000-0000-0000-0000-000000000008', 101, 'Задача 8', 'Задача 8 (User 101): Завершена', 'completed', static_completed_time, frozen_creation_time, frozen_creation_time),
    ('00000000-0000-0000-0000-000000000009', 101, 'Задача 9', 'Задача 9 (User 101): Активная', 'active', NULL, frozen_creation_time, frozen_creation_time),
    ('00000000-0000-0000-0000-000000000010', 101, 'Задача 10', 'Задача 10 (User 101): Завершена', 'completed', static_completed_time, frozen_creation_time, frozen_creation_time),
    ('00000000-0000-0000-0000-000000000011', 101, 'Задача 11 - Длинное имя', 'Задача 11 (User 101): Активная - Длинное описание для проверки поля TEXT, содержит разные символы !@#$%^&*()_+=-`~[]{}|\\;:",./<>?', 'active', NULL, frozen_creation_time, frozen_creation_time),
    ('00000000-0000-0000-0000-000000000012', 101, 'Задача 12', 'Задача 12 (User 101): Завершена', 'completed', static_completed_time, frozen_creation_time, frozen_creation_time),
    ('00000000-0000-0000-0000-000000000013', 101, 'Задача 13', 'Задача 13 (User 101): Активная', 'active', NULL, frozen_creation_time, frozen_creation_time),
    ('00000000-0000-0000-0000-000000000021', 101, '!@#$%^&*()', 'Задача со спецсимволами в имени', 'active', NULL, frozen_creation_time, frozen_creation_time),
    ('00000000-0000-0000-0000-000000000016', 101, 'Задача 16', 'Задача 16 (User 101): Отложена на потом', 'moved_to_sometime_later', NULL, frozen_creation_time, frozen_creation_time),
    ('00000000-0000-0000-0000-000000000017', 101, 'Задача 17', 'Задача 17 (User 101): Удалена', 'deleted', NULL, frozen_creation_time, frozen_creation_time);

-- Задачи для user_id = 102 (Обновлено: добавлено имя, убраны project_id и priority)
INSERT INTO current_actions.tasks (id, user_id, name, description, status, completed_at, created_at, updated_at) VALUES
    ('00000000-0000-0000-0000-000000000014', 102, 'Задача 14', 'Задача 14 (User 102): Активная', 'active', NULL, frozen_creation_time, frozen_creation_time),
    ('00000000-0000-0000-0000-000000000015', 102, 'Задача 15', 'Задача 15 (User 102): Завершена', 'completed', static_completed_time, frozen_creation_time, frozen_creation_time),
    ('00000000-0000-0000-0000-000000000018', 102, 'Задача 18', 'Задача 18 (User 102): Отложена', 'moved_to_sometime_later', NULL, frozen_creation_time, frozen_creation_time),
    ('00000000-0000-0000-0000-000000000019', 102, 'Задача 19', 'Задача 19 (User 102): Удалена', 'deleted', NULL, frozen_creation_time, frozen_creation_time);

END $$;
