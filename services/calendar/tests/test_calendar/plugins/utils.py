import psycopg2


def select_user_calendar(pgsql, user_id):
    cursor = pgsql['calendar'].dict_cursor()
    cursor.execute(
        f"""
            SELECT
                calendar.user_id,
                calendar.name,
                calendar.description,
                calendar.status
            FROM calendar.calendar AS calendar
            WHERE calendar.user_id = {user_id}
        """
    )

    result = []

    for row in cursor.fetchall():
        result.append(dict(row))

    return result


def select_user_full_calendar(pgsql, user_id):
    cursor = pgsql['calendar'].dict_cursor()
    cursor.execute(
        f"""
            SELECT
                calendar.id,
                calendar.user_id,
                calendar.name,
                calendar.description,
                calendar.status,
                calendar.created_at,
                calendar.updated_at
            FROM calendar.calendar AS calendar
            WHERE calendar.user_id = {user_id}
        """
    )

    result = []

    for row in cursor.fetchall():
        row['created_at'] = row['created_at'].isoformat()
        row['updated_at'] = row['updated_at'].isoformat()
        result.append(dict(row))

    return result


def select_note(pgsql, note_id):
    cursor = pgsql['calendar'].dict_cursor()

    try:
        cursor.execute(
            f"""
                SELECT
                    calendar.user_id,
                    calendar.name,
                    calendar.description,
                    calendar.status
                FROM calendar.calendar AS calendar
                WHERE calendar.id = '{note_id}'::uuid
            """
        )
    except psycopg2.errors.InvalidTextRepresentation:
        return {}

    rows = cursor.fetchall()
    if len(rows) == 0:
        return {}
    row = rows[0]

    return dict(row)
