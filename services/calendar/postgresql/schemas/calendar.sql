CREATE SCHEMA IF NOT EXISTS calendar;

-- Перечисление для статуса задачи
CREATE TYPE calendar.note_status AS ENUM ('active', 'deleted');

-- Таблица с когда-нибудь потом задачами
CREATE TABLE IF NOT EXISTS calendar.calendar (
    id              UUID                        PRIMARY KEY DEFAULT gen_random_uuid(),
    user_id         BIGINT                      NOT NULL,
    name            TEXT                        NOT NULL,
    description     TEXT                        NOT NULL,
    status          calendar.note_status           NOT NULL DEFAULT 'active',
    created_at      TIMESTAMPTZ                 NOT NULL DEFAULT now(),
    updated_at      TIMESTAMPTZ                 NOT NULL DEFAULT now()
);

-- Создание индексов для оптимизации запросов
CREATE INDEX idx_calendar_user_id ON calendar.calendar(user_id);
CREATE INDEX idx_calendar_user_id_status ON calendar.calendar(user_id, status);

-- Функция для автоматического обновления поля updated_at
CREATE OR REPLACE FUNCTION calendar.set_updated_at()
RETURNS TRIGGER AS $$
BEGIN
    IF NEW.updated_at IS NULL THEN
        NEW.updated_at = now();
    END IF;
    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

-- Триггер для автоматического обновления поля updated_at при изменении задачи
CREATE TRIGGER trg_calendar_updated_at
BEFORE UPDATE ON calendar.calendar
FOR EACH ROW
EXECUTE FUNCTION calendar.set_updated_at();
