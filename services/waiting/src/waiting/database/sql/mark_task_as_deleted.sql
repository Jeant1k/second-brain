-- kMarkTaskAsDeleted
-- $1 - task_id

UPDATE waiting.tasks
SET status = 'deleted'
WHERE id = $1::UUID
