-- kSelectNoteById
-- $1 - note id

SELECT
    id,
    user_id,
    name,
    description,
    status,
    created_at,
    updated_at
FROM
    calendar.calendar
WHERE
    id = $1;