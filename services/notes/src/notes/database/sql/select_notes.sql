-- kSelectNotes
-- $1 - user_id
-- $2 - updated_at
-- $3 - id
-- $4 - status
-- $5 - limit

WITH selected_notes AS (
    SELECT
        t.id,
        t.updated_at
    FROM notes.notes t
    WHERE
        -- 1. Фильтр по user_id (обязательный)
        t.user_id = $1

        -- 2. Фильтр по статусу (опциональный)
        -- Если $4 (status) IS NULL, условие всегда истинно
        AND ($4::notes.note_status IS NULL OR t.status = $4)

        -- 3. Keyset Pagination (курсор)
        -- Условие срабатывает, если курсор ($2, $3) не передан (IS NULL)
        -- ИЛИ если запись строго "после" курсора по (updated_at ASC, id ASC)
        AND (
            ($2::TIMESTAMPTZ IS NULL AND $3::UUID IS NULL)
            OR
            (t.updated_at >= $2 AND t.id >= $3)
        )
    ORDER BY
        t.updated_at ASC,
        t.id ASC
    LIMIT $5
)
SELECT
    t.id,
    t.user_id,
    t.name,
    t.description,
    t.status,
    t.created_at,
    t.updated_at
FROM
    notes.notes t
JOIN
    selected_notes s ON t.id = s.id
ORDER BY
    t.updated_at ASC,
    t.id ASC;
