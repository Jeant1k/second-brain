def select_tasks_and_tags(pgsql, user_id):
    cursor = pgsql['current_actions'].dict_cursor()
    cursor.execute(
        f"""
            SELECT
                tasks.id,
                tasks.user_id,
                tasks.description,
                tasks.status,
                tasks.project_id, 
                tasks.priority,
                ARRAY_AGG(tags.name) AS tags
            FROM current_actions.tasks AS tasks
            LEFT JOIN current_actions.tags AS tags ON tasks.id = tags.task_id
            WHERE tasks.user_id = {user_id}
            GROUP BY tasks.id
        """
    )

    return cursor.fetchall()
