-- kMarkTaskAsPending
-- $1 - task_id

UPDATE sometime_later.tasks
SET status = 'pending'
WHERE id = $1::UUID
