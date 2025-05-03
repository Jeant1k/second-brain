-- kSelectUserIdByTaskId
-- $1 - task id

SELECT user_id
FROM sometime_later.tasks
WHERE id = $1::UUID
