-- kMarkTaskAsMovedToCurrentActions
-- $1 - task id

UPDATE sometime_later.tasks
SET status = 'moved_to_current_actions'
WHERE id = $1
    AND status <> 'moved_to_current_actions'
RETURNING
    id,
    user_id,
    name,
    description,
    status,
    created_at,
    updated_at,
    completed_at;
