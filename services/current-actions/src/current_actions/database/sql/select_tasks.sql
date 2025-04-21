-- kSelectTasks
-- $1 - user_id
-- $2 - id
-- $3 - status
-- $4 - limit


-- TODO: Испрваить запрос, добавить апдейтед ат в аргументы
WITH task_tags AS (
    SELECT task_id, array_agg(name) as tags
    FROM current_actions.tags
    GROUP BY task_id
)
SELECT 
    t.id,
    t.user_id, 
    t.description, 
    t.project_id, 
    t.priority, 
    COALESCE(tt.tags, ARRAY[]::VARCHAR[]) as tags,
    t.status, 
    t.created_at, 
    t.updated_at, 
    t.completed_at
FROM current_actions.tasks t
LEFT JOIN task_tags tt ON t.id = tt.task_id
WHERE t.user_id = $1
AND ($3::current_actions.task_status IS NULL OR t.status = $3)
AND ($2::UUID IS NULL OR 
        (t.updated_at, t.id) < (
            (SELECT updated_at FROM current_actions.tasks WHERE id = $2),
            (SELECT id FROM current_actions.tasks WHERE id = $2)
        ))
ORDER BY t.updated_at DESC, t.id DESC
LIMIT $4