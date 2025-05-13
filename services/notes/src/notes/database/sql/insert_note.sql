-- InsertNote
-- $1 - user_id
-- $2 - name
-- $3 - description

WITH inserted_note AS (
    INSERT INTO notes.notes (
        user_id,
        name,
        description
    ) 
    VALUES (
        $1,
        $2,
        $3
    )
    RETURNING id
)
SELECT id FROM inserted_note;
