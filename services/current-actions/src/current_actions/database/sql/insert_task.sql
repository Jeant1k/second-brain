-- InsertTask
-- $1 - user_id
-- $2 - description
-- $3 - project_id
-- $4 - priority
-- $5 - массив имен тегов

WITH inserted_task AS (
    INSERT INTO current_actions.tasks (
        user_id, 
        description, 
        project_id, 
        priority
    ) 
    VALUES (
        $1,
        $2,
        CASE 
            WHEN $3::TEXT IS NOT NULL THEN $3::UUID 
            ELSE NULL 
        END,
        CASE 
            WHEN $4 = 0 THEN 'high'::current_actions.task_priority
            WHEN $4 = 1 THEN 'medium'::current_actions.task_priority
            WHEN $4 = 2 THEN 'low'::current_actions.task_priority
        END
    )
    RETURNING id
),
tag_insert AS (
    INSERT INTO current_actions.tags (
        task_id,
        name
    )
    SELECT 
        (SELECT id FROM inserted_task),
        unnest($5::VARCHAR[])
    RETURNING 1
)
SELECT id FROM inserted_task;
