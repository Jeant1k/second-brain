-- kMarkTaskAsMovedToSometimeLater
-- $1 - task id

UPDATE current_actions.tasks
SET status = 'moved_to_sometime_later'
WHERE id = $1::UUID