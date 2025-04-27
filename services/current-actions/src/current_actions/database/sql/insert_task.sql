-- InsertTask
-- $1 - user_id
-- $2 - name
-- $3 - description

WITH inserted_task AS (
    INSERT INTO current_actions.tasks (
        user_id,
        name,
        description
    ) 
    VALUES (
        $1,
        $2,
        $3
    )
    RETURNING id
)
SELECT id FROM inserted_task;
