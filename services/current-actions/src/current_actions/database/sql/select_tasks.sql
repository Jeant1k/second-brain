-- kSelectTasks
-- $1 - user_id
-- $2 - updated_at
-- $3 - id
-- $4 - status
-- $5 - limit


WITH tasks AS (
    SELECT 
        t.id,
        t.user_id,
        t.description,
        t.project_id,
        t.priority,
        t.status,
        t.created_at,
        t.updated_at,
        t.completed_at
    FROM 
        current_actions.tasks t
    WHERE 
        t.user_id = $1
        AND ($4::current_actions.task_status IS NULL OR t.status = $4)
        AND ($2::TIMESTAMPTZ IS NULL OR t.updated_at <= $2 OR (t.updated_at = $2 AND t.id < $3))
    ORDER BY 
        t.updated_at DESC, t.id DESC
    LIMIT $5
),
task_tags AS (
    SELECT 
        tg.task_id,
        array_agg(tg.name) AS tags
    FROM 
        current_actions.tags tg
    WHERE 
        tg.task_id IN (SELECT id FROM tasks)
    GROUP BY 
        tg.task_id
)
SELECT 
    t.id,
    t.user_id,
    t.description,
    t.project_id,
    t.priority,
    COALESCE(tt.tags, ARRAY[]::VARCHAR[]) AS tags,
    t.status,
    t.created_at,
    t.updated_at,
    t.completed_at
FROM 
    tasks t
LEFT JOIN 
    task_tags tt ON t.id = tt.task_id
ORDER BY 
    t.updated_at DESC, t.id DESC
