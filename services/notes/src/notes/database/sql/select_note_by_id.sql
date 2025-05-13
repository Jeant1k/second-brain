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
    notes.notes
WHERE
    id = $1;