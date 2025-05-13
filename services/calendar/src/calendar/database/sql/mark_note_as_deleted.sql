-- kMarkNoteAsDeleted
-- $1 - note_id

UPDATE calendar.calendar
SET status = 'deleted'
WHERE id = $1::UUID
