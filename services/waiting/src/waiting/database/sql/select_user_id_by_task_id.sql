-- kSelectUserIdByTaskId
-- $1 - task id

SELECT user_id
FROM waiting.tasks
WHERE id = $1::UUID
