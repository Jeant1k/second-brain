-- kUpdateNameOrDescription
-- $1 - note id
-- $2 - note name
-- $3 - note description

UPDATE calendar.calendar
SET
    name = COALESCE($2, name),
    description = COALESCE($3, description)
WHERE id = $1
RETURNING id;
