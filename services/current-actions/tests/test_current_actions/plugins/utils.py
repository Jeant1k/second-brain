import psycopg2


def select_user_tasks(pgsql, user_id):
    cursor = pgsql['current_actions'].dict_cursor()
    cursor.execute(
        f"""
            SELECT
                tasks.user_id,
                tasks.name,
                tasks.description,
                tasks.status
            FROM current_actions.tasks AS tasks
            WHERE tasks.user_id = {user_id}
        """
    )

    result = []

    for row in cursor.fetchall():
        result.append(dict(row))

    return result


def select_task(pgsql, task_id):
    cursor = pgsql['current_actions'].dict_cursor()

    try:
        cursor.execute(
            f"""
                SELECT
                    tasks.user_id,
                    tasks.name,
                    tasks.description,
                    tasks.status
                FROM current_actions.tasks AS tasks
                WHERE tasks.id = '{task_id}'::uuid
            """
        )
    except psycopg2.errors.InvalidTextRepresentation:
        return {}

    rows = cursor.fetchall()
    if len(rows) == 0:
        return {}
    row = rows[0]

    return dict(row)
