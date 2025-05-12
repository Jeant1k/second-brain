-- kMarkTaskAsMovedToCurrentActions
-- $1 - task id

UPDATE waiting.tasks
SET status = 'moved_to_current_actions'
WHERE id = $1::UUID
