-- kMarkTaskAsActive
-- $1 - task_id

UPDATE waiting.tasks
SET status = 'active'
WHERE id = $1::UUID
