-- kMarkNoteAsDeleted
-- $1 - note_id

UPDATE notes.notes
SET status = 'deleted'
WHERE id = $1::UUID
