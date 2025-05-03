-- kMarkTaskAsCompleted
-- $1 - task_id

UPDATE sometime_later.tasks
SET status = 'completed'
WHERE id = $1::UUID
