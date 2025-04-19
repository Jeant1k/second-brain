import pytest

from testsuite.databases import pgsql
from .plugins.utils import select_user_tasks_and_tags


@pytest.mark.parametrize(
    'testcase',
    [
        pytest.param('user_id_not_found'),
        pytest.param('description_not_found'),
        pytest.param('project_id_wrong_format'),
        pytest.param('happy_path'),
        pytest.param('full_happy_path'),
    ],
)
async def test_create_task(
    service_client,
    load_json,
    pgsql,
    testcase,
):
    # arrange
    test_data = load_json(f'testcases/{testcase}.json')
    initial_data = test_data['initial_data']
    expected_data = test_data['expected_result']

    # act
    response = await service_client.post(
        '/current-actions/v1/task',
        json=initial_data['request_body'],
    )

    # assert
    assert response.status == expected_data['status_code']
    assert response.text == expected_data['response']

    assert select_user_tasks_and_tags(pgsql, initial_data['request_body'].get(
        'user_id', 0)) == expected_data['database_data']
