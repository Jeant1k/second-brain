-- kSelectUserIdByNoteId
-- $1 - note id

SELECT user_id
FROM notes.notes
WHERE id = $1::UUID
