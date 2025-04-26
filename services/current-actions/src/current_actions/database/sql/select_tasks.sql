-- kSelectTasks
-- $1 - user_id
-- $2 - updated_at
-- $3 - id
-- $4 - status
-- $5 - limit

WITH selected_tasks AS (
    SELECT
        t.id,
        t.updated_at
    FROM current_actions.tasks t
    WHERE
        -- 1. Фильтр по user_id (обязательный)
        t.user_id = $1

        -- 2. Фильтр по статусу (опциональный)
        -- Если $4 (status) IS NULL, условие всегда истинно
        AND ($4::current_actions.task_status IS NULL OR t.status = $4)

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
    t.description,
    t.status,
    t.project_id,
    t.priority,
    t.created_at,
    t.updated_at,
    t.completed_at,
    COALESCE(
        array_agg((tg.id, tg.name, tg.created_at)) FILTER (WHERE tg.id IS NOT NULL),
         ARRAY[]::record[]
    ) AS tags
FROM
    current_actions.tasks t
JOIN
    selected_tasks s ON t.id = s.id
LEFT JOIN
    current_actions.tags tg ON t.id = tg.task_id
GROUP BY
    t.id
ORDER BY
    t.updated_at ASC,
    t.id ASC;
