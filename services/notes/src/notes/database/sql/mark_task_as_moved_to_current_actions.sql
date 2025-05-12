-- kMarkTaskAsMovedToCurrentActions
-- $1 - task id

UPDATE notes.tasks
SET status = 'moved_to_current_actions'
WHERE id = $1::UUID
