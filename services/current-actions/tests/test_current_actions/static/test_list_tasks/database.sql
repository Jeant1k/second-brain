DO $$
DECLARE
    frozen_creation_time TIMESTAMPTZ := '2025-04-19T10:00:00+00:00';

    project1_uuid UUID := '11111111-1111-1111-1111-111111111111';
    project2_uuid UUID := '22222222-2222-2222-2222-222222222222';
    static_completed_time TIMESTAMPTZ := '2025-04-26T10:00:00+00:00';
BEGIN

-- Задачи для user_id = 101 (13 записей)
INSERT INTO current_actions.tasks (id, user_id, description, status, project_id, priority, completed_at, created_at, updated_at) VALUES
    ('00000000-0000-0000-0000-000000000001', 101, 'Задача 1 (User 101): Активная, Высокий приоритет, Проект 1', 'active', project1_uuid, 'high', NULL, frozen_creation_time, frozen_creation_time),
    ('00000000-0000-0000-0000-000000000002', 101, 'Задача 2 (User 101): Активная, Средний приоритет, Проект 1', 'active', project1_uuid, 'medium', NULL, frozen_creation_time, frozen_creation_time),
    ('00000000-0000-0000-0000-000000000003', 101, 'Задача 3 (User 101): Активная, Низкий приоритет, Без проекта', 'active', NULL, 'low', NULL, frozen_creation_time, frozen_creation_time),
    ('00000000-0000-0000-0000-000000000004', 101, 'Задача 4 (User 101): Завершена, Средний приоритет, Проект 2', 'completed', project2_uuid, 'medium', static_completed_time, frozen_creation_time, frozen_creation_time),
    ('00000000-0000-0000-0000-000000000005', 101, 'Задача 5 (User 101): Завершена, Высокий приоритет, Без проекта', 'completed', NULL, 'high', static_completed_time, frozen_creation_time, frozen_creation_time),
    ('00000000-0000-0000-0000-000000000006', 101, 'Задача 6 (User 101): Активная, Средний приоритет, Проект 2', 'active', project2_uuid, 'medium', NULL, frozen_creation_time, frozen_creation_time),
    ('00000000-0000-0000-0000-000000000007', 101, 'Задача 7 (User 101): Активная, Низкий приоритет, Проект 1', 'active', project1_uuid, 'low', NULL, frozen_creation_time, frozen_creation_time),
    ('00000000-0000-0000-0000-000000000008', 101, 'Задача 8 (User 101): Завершена, Низкий приоритет, Проект 1', 'completed', project1_uuid, 'low', static_completed_time, frozen_creation_time, frozen_creation_time),
    ('00000000-0000-0000-0000-000000000009', 101, 'Задача 9 (User 101): Активная, Высокий приоритет, Без проекта', 'active', NULL, 'high', NULL, frozen_creation_time, frozen_creation_time),
    ('00000000-0000-0000-0000-000000000010', 101, 'Задача 10 (User 101): Завершена, Средний приоритет, Без проекта', 'completed', NULL, 'medium', static_completed_time, frozen_creation_time, frozen_creation_time),
    ('00000000-0000-0000-0000-000000000011', 101, 'Задача 11 (User 101): Активная, Средний приоритет, Проект 1 - Длинное описание для проверки поля TEXT, содержит разные символы !@#$%^&*()_+=-`~[]{}|\\;:",./<>?', 'active', project1_uuid, 'medium', NULL, frozen_creation_time, frozen_creation_time),
    ('00000000-0000-0000-0000-000000000012', 101, 'Задача 12 (User 101): Завершена, Высокий приоритет, Проект 2', 'completed', project2_uuid, 'high', static_completed_time, frozen_creation_time, frozen_creation_time),
    ('00000000-0000-0000-0000-000000000013', 101, 'Задача 13 (User 101): Активная, Низкий приоритет, Проект 2', 'active', project2_uuid, 'low', NULL, frozen_creation_time, frozen_creation_time);

-- Задачи для user_id = 102 (2 записи)
INSERT INTO current_actions.tasks (id, user_id, description, status, project_id, priority, completed_at, created_at, updated_at) VALUES
    ('00000000-0000-0000-0000-000000000014', 102, 'Задача 14 (User 102): Активная, Высокий приоритет, Проект 1', 'active', project1_uuid, 'high', NULL, frozen_creation_time, frozen_creation_time),
    ('00000000-0000-0000-0000-000000000015', 102, 'Задача 15 (User 102): Завершена, Средний приоритет, Без проекта', 'completed', NULL, 'medium', static_completed_time, frozen_creation_time, frozen_creation_time);

INSERT INTO current_actions.tags (id, task_id, name, created_at) VALUES
    -- Теги для Задачи 1 (User 101)
    ('eeeeeeee-eeee-eeee-eeee-eeeeeeeeeee1', '00000000-0000-0000-0000-000000000001', 'срочно', frozen_creation_time),
    ('eeeeeeee-eeee-eeee-eeee-eeeeeeeeeee2', '00000000-0000-0000-0000-000000000001', 'frontend', frozen_creation_time),
    -- Тег для Задачи 4 (User 101)
    ('eeeeeeee-eeee-eeee-eeee-eeeeeeeeeee3', '00000000-0000-0000-0000-000000000004', 'backend', frozen_creation_time),
    -- Теги для Задачи 9 (User 101)
    ('eeeeeeee-eeee-eeee-eeee-eeeeeeeeeee4', '00000000-0000-0000-0000-000000000009', 'рефакторинг', frozen_creation_time),
    ('eeeeeeee-eeee-eeee-eeee-eeeeeeeeeee5', '00000000-0000-0000-0000-000000000009', 'срочно', frozen_creation_time),
    -- Тег для Задачи 14 (User 102)
    ('eeeeeeee-eeee-eeee-eeee-eeeeeeeeeee6', '00000000-0000-0000-0000-000000000014', 'аналитика', frozen_creation_time),
    -- Тег для Задачи 15 (User 102)
    ('eeeeeeee-eeee-eeee-eeee-eeeeeeeeeee7', '00000000-0000-0000-0000-000000000015', 'отчет', frozen_creation_time);

END $$;
