-- kMarkTaskAsDeleted
-- $1 - task_id

UPDATE notes.tasks
SET status = 'deleted'
WHERE id = $1::UUID
