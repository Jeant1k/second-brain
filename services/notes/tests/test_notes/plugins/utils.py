import psycopg2


def select_user_notes(pgsql, user_id):
    cursor = pgsql['notes'].dict_cursor()
    cursor.execute(
        f"""
            SELECT
                notes.user_id,
                notes.name,
                notes.description,
                notes.status
            FROM notes.notes AS notes
            WHERE notes.user_id = {user_id}
        """
    )

    result = []

    for row in cursor.fetchall():
        result.append(dict(row))

    return result


def select_user_full_notes(pgsql, user_id):
    cursor = pgsql['notes'].dict_cursor()
    cursor.execute(
        f"""
            SELECT
                notes.id,
                notes.user_id,
                notes.name,
                notes.description,
                notes.status,
                notes.created_at,
                notes.updated_at
            FROM notes.notes AS notes
            WHERE notes.user_id = {user_id}
        """
    )

    result = []

    for row in cursor.fetchall():
        row['created_at'] = row['created_at'].isoformat()
        row['updated_at'] = row['updated_at'].isoformat()
        result.append(dict(row))

    return result


def select_note(pgsql, note_id):
    cursor = pgsql['notes'].dict_cursor()

    try:
        cursor.execute(
            f"""
                SELECT
                    notes.user_id,
                    notes.name,
                    notes.description,
                    notes.status
                FROM notes.notes AS notes
                WHERE notes.id = '{note_id}'::uuid
            """
        )
    except psycopg2.errors.InvalidTextRepresentation:
        return {}

    rows = cursor.fetchall()
    if len(rows) == 0:
        return {}
    row = rows[0]

    return dict(row)
