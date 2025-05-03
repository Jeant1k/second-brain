-- kMarkTaskAsDeleted
-- $1 - task_id

UPDATE sometime_later.tasks
SET status = 'deleted'
WHERE id = $1::UUID
