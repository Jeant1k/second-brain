-- kMarkTaskAsMovedToCurrentActions
-- $1 - task id

UPDATE sometime_later.tasks
SET status = 'moved_to_current_actions'
WHERE id = $1::UUID
