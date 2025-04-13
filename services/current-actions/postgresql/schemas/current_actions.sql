CREATE SCHEMA IF NOT EXISTS current_actions;

-- Перечисление для статуса задачи
CREATE TYPE current_actions.task_status AS ENUM ('active', 'completed');

-- Перечисление для приоритета задачи
CREATE TYPE current_actions.task_priority AS ENUM ('high', 'medium', 'low');

-- Таблица с текущими действиями
CREATE TABLE current_actions.tasks (
    id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    user_id BIGINT NOT NULL,
    description TEXT NOT NULL,
    status current_actions.task_status NOT NULL DEFAULT 'active',
    project_id UUID NULL,
    priority current_actions.task_priority NOT NULL DEFAULT 'medium',
    created_at TIMESTAMPTZ NOT NULL DEFAULT now(),
    updated_at TIMESTAMPTZ NOT NULL DEFAULT now(),
    completed_at TIMESTAMPTZ NULL
);

COMMENT ON TABLE current_actions.tasks IS 'Таблица с текущими действиями пользователей';
COMMENT ON COLUMN current_actions.tasks.id IS 'Уникальный идентификатор задачи';
COMMENT ON COLUMN current_actions.tasks.user_id IS 'Идентификатор пользователя';
COMMENT ON COLUMN current_actions.tasks.description IS 'Описание задачи';
COMMENT ON COLUMN current_actions.tasks.status IS 'Статус задачи (активная или выполненная)';
COMMENT ON COLUMN current_actions.tasks.project_id IS 'Идентификатор проекта, к которому относится задача';
COMMENT ON COLUMN current_actions.tasks.priority IS 'Приоритет задачи';
COMMENT ON COLUMN current_actions.tasks.created_at IS 'Время создания задачи';
COMMENT ON COLUMN current_actions.tasks.updated_at IS 'Время последнего обновления задачи';
COMMENT ON COLUMN current_actions.tasks.completed_at IS 'Время выполнения задачи';

-- Таблица с тегами для задач
CREATE TABLE current_actions.tags (
    id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    task_id UUID NOT NULL REFERENCES current_actions.tasks(id) ON DELETE CASCADE,
    name VARCHAR(100) NOT NULL,
    created_at TIMESTAMPTZ NOT NULL DEFAULT now()
);

COMMENT ON TABLE current_actions.tags IS 'Таблица с тегами для задач';
COMMENT ON COLUMN current_actions.tags.id IS 'Уникальный идентификатор тега';
COMMENT ON COLUMN current_actions.tags.task_id IS 'Идентификатор задачи, к которой относится тег';
COMMENT ON COLUMN current_actions.tags.name IS 'Название тега';
COMMENT ON COLUMN current_actions.tags.created_at IS 'Время создания тега';

-- Создание индексов для оптимизации запросов
CREATE INDEX idx_tasks_user_id ON current_actions.tasks(user_id);
CREATE INDEX idx_tasks_project_id ON current_actions.tasks(project_id);
CREATE INDEX idx_tasks_user_id_status ON current_actions.tasks(user_id, status);
CREATE INDEX idx_tags_task_id ON current_actions.tags(task_id);
CREATE INDEX idx_tags_name ON current_actions.tags(name);

-- Функция для автоматического обновления поля updated_at
CREATE OR REPLACE FUNCTION current_actions.set_updated_at()
RETURNS TRIGGER AS $$
BEGIN
    NEW.updated_at = now();
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
    IF NEW.status = 'completed' AND OLD.status = 'active' THEN
        NEW.completed_at = now();
    ELSIF NEW.status = 'active' AND OLD.status = 'completed' THEN
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
