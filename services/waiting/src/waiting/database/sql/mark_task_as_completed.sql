-- kMarkTaskAsCompleted
-- $1 - task_id

UPDATE waiting.tasks
SET status = 'completed'
WHERE id = $1::UUID
