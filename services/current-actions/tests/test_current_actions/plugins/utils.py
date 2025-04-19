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
                ARRAY_REMOVE(ARRAY_AGG(tags.name), NULL) AS tags
            FROM current_actions.tasks AS tasks
            LEFT JOIN current_actions.tags AS tags ON tasks.id = tags.task_id
            WHERE tasks.user_id = {user_id}
            GROUP BY tasks.id
        """
    )

    result = []
    
    for row in cursor.fetchall():
        task_dict = {
            'user_id': row['user_id'],
            'description': row['description'],
            'status': row['status'],
            'project_id': row['project_id'],
            'priority': row['priority'],
            'tags': row['tags']
        }
        result.append(task_dict)
    
    return result
