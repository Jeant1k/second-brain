INSERT INTO sometime_later.tasks (
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
    'Some task name',
    'Some task description',
    'moved_to_current_actions',
    '2025-04-19T10:00:00+00:00',
    '2025-04-19T10:00:00+00:00',
    '2025-04-20T10:00:00+00:00'
);
