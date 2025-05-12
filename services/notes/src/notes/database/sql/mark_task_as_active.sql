-- kMarkTaskAsActive
-- $1 - task_id

UPDATE notes.tasks
SET status = 'active'
WHERE id = $1::UUID
