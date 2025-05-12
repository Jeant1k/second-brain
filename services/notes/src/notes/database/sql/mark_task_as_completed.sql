-- kMarkTaskAsCompleted
-- $1 - task_id

UPDATE notes.tasks
SET status = 'completed'
WHERE id = $1::UUID
