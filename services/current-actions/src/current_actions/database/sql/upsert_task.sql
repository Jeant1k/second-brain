-- kUpsertTask
-- $1 - id
-- $2 - user_id
-- $3 - name
-- $4 - description
-- $5 - status
-- $6 - created_at
-- $7 - updated_at
-- $8 - completed_at

INSERT INTO current_actions.tasks (
    id, user_id, name, description, status, created_at, updated_at, completed_at
) VALUES (
    $1, $2, $3, $4, $5, $6, $7, $8
) ON CONFLICT (id) DO UPDATE SET
    user_id = EXCLUDED.user_id,
    name = EXCLUDED.name,
    description = EXCLUDED.description,
    status = EXCLUDED.status,
    created_at = EXCLUDED.created_at,
    updated_at = EXCLUDED.updated_at,
    completed_at = EXCLUDED.completed_at
RETURNING xmax = 0 AS inserted;
