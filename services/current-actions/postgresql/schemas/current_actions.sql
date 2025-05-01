CREATE SCHEMA IF NOT EXISTS current_actions;

-- Перечисление для статуса задачи
CREATE TYPE current_actions.task_status AS ENUM ('active', 'completed', 'moved_to_someday_later', 'deleted');

-- Перечисление для приоритета задачи
CREATE TYPE current_actions.task_priority AS ENUM ('high', 'medium', 'low');

-- Таблица с текущими действиями
CREATE TABLE IF NOT EXISTS current_actions.tasks (
    id              UUID                            PRIMARY KEY DEFAULT gen_random_uuid(),
    user_id         BIGINT                          NOT NULL,
    name            TEXT                            NOT NULL,
    description     TEXT                            NOT NULL,
    status          current_actions.task_status     NOT NULL DEFAULT 'active',
    created_at      TIMESTAMPTZ                     NOT NULL DEFAULT now(),
    updated_at      TIMESTAMPTZ                     NOT NULL DEFAULT now(),
    completed_at    TIMESTAMPTZ                     NULL
);

-- Создание индексов для оптимизации запросов
CREATE INDEX idx_tasks_user_id ON current_actions.tasks(user_id);
CREATE INDEX idx_tasks_user_id_status ON current_actions.tasks(user_id, status);

-- Функция для автоматического обновления поля updated_at
CREATE OR REPLACE FUNCTION current_actions.set_updated_at()
RETURNS TRIGGER AS $$
BEGIN
    IF NEW.updated_at IS NULL THEN
        NEW.updated_at = now();
    END IF;
    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

-- Триггер для автоматического обновления поля updated_at при изменении задачи
CREATE TRIGGER trg_tasks_updated_at
BEFORE UPDATE ON current_actions.tasks
FOR EACH ROW
EXECUTE FUNCTION current_actions.set_updated_at();

-- Функция для автоматического заполнения поля completed_at при изменении статуса на completed
CREATE OR REPLACE FUNCTION current_actions.set_completed_at()
RETURNS TRIGGER AS $$
BEGIN
    IF NEW.status = 'completed' AND OLD.status <> 'completed' THEN
        IF NEW.completed_at IS NULL OR NEW.completed_at = OLD.completed_at THEN
            NEW.completed_at = now();
        END IF;
    ELSIF NEW.status <> 'completed' THEN
        NEW.completed_at = NULL;
    END IF;
    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

-- Триггер для автоматического заполнения поля completed_at при изменении статуса
CREATE TRIGGER trg_tasks_completed_at
BEFORE UPDATE ON current_actions.tasks
FOR EACH ROW
WHEN (NEW.status IS DISTINCT FROM OLD.status)
EXECUTE FUNCTION current_actions.set_completed_at();
