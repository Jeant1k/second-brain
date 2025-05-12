-- kMarkTaskAsPending
-- $1 - task_id

UPDATE waiting.tasks
SET status = 'pending'
WHERE id = $1::UUID
