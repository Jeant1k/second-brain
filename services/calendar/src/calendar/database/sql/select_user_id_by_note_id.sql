-- kSelectUserIdByNoteId
-- $1 - note id

SELECT user_id
FROM calendar.calendar
WHERE id = $1::UUID
