-- kSelectTaskById
-- $1 - task id

SELECT
    id,
    user_id,
    name,
    description,
    status,
    created_at,
    updated_at,
    completed_at
FROM
    current_actions.tasks
WHERE
    id = $1;