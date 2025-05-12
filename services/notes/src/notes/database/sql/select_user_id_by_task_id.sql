-- kSelectUserIdByTaskId
-- $1 - task id

SELECT user_id
FROM notes.tasks
WHERE id = $1::UUID
