INSERT INTO current_actions.tasks (
    id,
    user_id,
    description,
    status,
    project_id
)
VALUES (
    'a8098c1a-f86e-11da-bd1a-001124440000'::uuid,
    123456,
    'Some task description',
    'completed'::current_actions.task_status,
    '123e4567-e89b-12d3-a456-426655440000'::uuid
);
