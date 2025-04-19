-- kMarkTaskAsActive
-- $1 - task_id

UPDATE current_actions.tasks
SET status = 'active'
WHERE id = $1::UUID
