-- kUpdateNameOrDescription
-- $1 - task id
-- $2 - task name
-- $3 - task description

UPDATE notes.tasks
SET
    name = COALESCE($2, name),
    description = COALESCE($3, description)
WHERE id = $1
RETURNING id;
