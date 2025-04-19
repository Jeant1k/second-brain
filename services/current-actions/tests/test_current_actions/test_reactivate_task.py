import pytest

from testsuite.databases import pgsql
from .plugins.utils import select_task
from .plugins.constants import TASK_ID


@pytest.mark.parametrize(
    'testcase',
    [
        pytest.param('task_id_not_found'),
        pytest.param('task_not_found'),
        pytest.param('task_id_wrong_format'),
        pytest.param('happy_path'),
    ],
)
@pytest.mark.pgsql('current_actions', files=['database.sql'])
async def test_reactivate_task(
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
        '/current-actions/v1/task/reactivate',
        json=initial_data['request_body'],
    )

    # assert
    assert response.status == expected_data['status_code']
    assert response.text == expected_data['response']

    assert select_task(pgsql, TASK_ID) == expected_data['database_data']
