-- kSelectUserIdByTaskId
-- $1 - task id

SELECT user_id
FROM current_actions.tasks
WHERE id = $1::UUID
