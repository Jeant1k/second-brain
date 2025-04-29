-- kMarkTaskAsDeleted
-- $1 - task_id

UPDATE current_actions.tasks
SET status = 'deleted'
WHERE id = $1::UUID
