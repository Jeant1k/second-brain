INSERT INTO waiting.tasks (
    id,
    user_id,
    name,
    description,
    status,
    created_at,
    updated_at,
    completed_at
)
VALUES (
    'a8098c1a-f86e-11da-bd1a-001124440000'::uuid,
    123456,
    'Some old task name',
    'Some old task description',
    'pending',
    '2025-03-19T10:00:00+00:00',
    '2025-03-20T10:00:00+00:00',
    '2025-03-21T10:00:00+00:00'
);
