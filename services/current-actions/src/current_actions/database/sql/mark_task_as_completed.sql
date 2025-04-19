-- kMarkTaskAsCompleted
-- $1 - task_id

UPDATE current_actions.tasks
SET status = 'completed'
WHERE id = $1::UUID
